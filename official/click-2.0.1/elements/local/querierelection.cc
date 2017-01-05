#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "querierelection.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/packet_anno.hh>
#include <click/timer.hh>
#include <iostream>

CLICK_DECLS

QuerierElection::QuerierElection() : routerState(0), timeoutTimer(0) {}

QuerierElection::~QuerierElection() {}

int QuerierElection::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh, 
        "STATE", cpkM+cpkP, cpElementCast,"RouterInfoBase", &routerState,
        "IPADDR", cpkM+cpkP, cpIPAddress, &f_ipAddress,
    cpEnd) < 0) return -1;
    if (routerState == 0) return errh->error("Wrong element given as first argument, should be a RouterInfoBase element.");

	return 0;
}

void QuerierElection::push(int, Packet* p){
    this->handleQuery(p);
}

void QuerierElection::run_timer(Timer* timer) {
    routerState->allowQueries();
    timer->clear();
    timeoutTimer = 0;
}

void QuerierElection::handleQuery(Packet* p) {
    IGMP_query* igmph = (IGMP_query*)(p->data() + p->ip_header_length());

    // just in case
    const int type = igmph->Type;
    if (type != IGMP_QUERY_TYPE) {
        return;
    }
    WritablePacket* q = p->push(sizeof(click_ip));
    click_ip* iph = reinterpret_cast<click_ip*>(q->data());
    IPAddress otherRouter = iph->ip_src;

    if (otherRouter.addr() > f_ipAddress.addr()) {
        /// don't cease to send if the other router has a higher ip address than yourself
        return;
    }

    // TODO unsure what to do with s flag
    // TODO unsure to take over MRC

    routerState->suppressQueries();
    const uint8_t qrv = igmph->QRV;
    if (qrv != 0) {
        routerState->setQRV(qrv);
    }

    int qqic = igmph->QQIC;
    int qqi = 0;
    if (qqic < 128) {
        qqi = qqic;
    } else {
        int mant = qqic & 15;
        int exp = (qqic & 112) >> 4;
        qqi = (mant | 0x10) << (exp + 3);
    }
    if (qqi != 0) {
        routerState->setQQIT(qqic);
    }

    int maxRespCode = igmph->Max_Resp_Code;
    int maxRespTime = 0;
    if (maxRespCode < 128) {
        maxRespTime = maxRespCode;
    } else {
        int mant = maxRespCode & 15;
        int exp = (maxRespCode & 112) >> 4;
        maxRespTime = (mant | 0x10) << (exp + 3);
    }
    if (maxRespTime != 0) {
        routerState->setMRT(maxRespCode);
    }


    if (timeoutTimer != 0) {
        timeoutTimer->clear();
        timeoutTimer = 0;
    }
    timeoutTimer = new Timer();
    timeoutTimer->initialize(this);
    int timeoutTime = routerState->getOtherQuerierPresentInterval();
    timeoutTimer->schedule_after_msec(timeoutTime);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(QuerierElection)
