#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "routerqueryhandler.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/packet_anno.hh>
#include <click/timer.hh>
#include <iostream>

CLICK_DECLS

RouterQueryHandler::RouterQueryHandler() : routerState(0), reportGenerator(0) {}

RouterQueryHandler::~RouterQueryHandler() {}

int RouterQueryHandler::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh, 
        "STATE", cpkM+cpkP, cpElementCast,"RouterInfoBase", &routerState,
        "RPTGEN", cpkM+cpkP, cpElementCast,"RouterReportGenerator", &reportGenerator, 
    cpEnd) < 0) return -1;
    if (routerState == 0) return errh->error("Wrong element given as first argument, should be a RouterInfoBase element.");
    if (reportGenerator == 0) return errh->error("Wrong element given as second argument, should be a RouterReportGenerator element.");

	return 0;
}

void RouterQueryHandler::push(int, Packet* p){
    this->handleQuery(p);
}

void RouterQueryHandler::handleQuery(Packet* p) {
    IGMP_query* igmph = (IGMP_query*)(p->data() + p->ip_header_length());

    // just in case
    const int type = igmph->Type;
    if (type != IGMP_QUERY_TYPE) {
        return;
    }
    uint8_t interface = p->anno_u8(PAINT_ANNO_OFFSET);

    /// Handling of the Querier Robustness Variable
    const uint8_t qrv = igmph->QRV;
    if (qrv != 0) {
        routerState->setQRV(qrv);
    }

    int maxResCode = igmph->Max_Resp_Code;
    int maxRespTime = 0;

    if (maxResCode < 128) {
        maxRespTime = maxResCode;
    } else {
        int mant = maxResCode & 15;
        int exp = (maxResCode & 112) >> 4;
        maxRespTime = (mant | 0x10) << (exp + 3);
    }
    maxRespTime *= 100; // The maxRespTime is represented in 1/10 of seconds

    IPAddress groupAddr = igmph->Group_Address;
    if (groupAddr.empty() == true) {
        reportGenerator->sendGeneralReport(maxRespTime, interface);
    }
    else {
        reportGenerator->sendGroupSpecificReport(groupAddr, maxRespTime, interface);
    }
}


CLICK_ENDDECLS
EXPORT_ELEMENT(RouterQueryHandler)
