
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "clientinfobase.hh"
#include "igmpqueryhandler.hh"
#include "igmppackets.h"
#include <iostream>

CLICK_DECLS

IGMPQueryHandler::IGMPQueryHandler() : clientState(0), reportGenerator(0) {}

IGMPQueryHandler::~IGMPQueryHandler() {}

int IGMPQueryHandler::configure(Vector<String>& conf, ErrorHandler* errh) {
    if (cp_va_kparse(conf, this, errh, 
        "STATE", cpkM+cpkP, cpElementCast, "ClientInfoBase", &clientState,
        "RPT", cpkM+cpkP, cpElementCast, "IGMPReportGenerator", &reportGenerator,
     cpEnd) < 0) return -1;
    
    if (clientState == 0) return errh->error("Wrong element given as first argument, should be a ClientInfoBase element.");
    if (reportGenerator == 0) return errh->error("Wrong element given as second argument, should be a IGMPReport element.");

    return 0;
}

void IGMPQueryHandler::push(int, Packet* p) {
    this->handleQuery(p);
}

void IGMPQueryHandler::handleQuery(Packet* p) {
    IGMP_query* igmph = (IGMP_query*)(p->data() + p->ip_header_length());

    /// Handling of the Querier Robustness Variable
    const int qrv = igmph->QRV;
    if (qrv != 0) {
        clientState->setQRV(qrv);
    }

    int maxResCode = igmph->Max_Resp_Code;
    int maxRespTime = 0;

    if (maxResCode < 128) {
        maxRespTime = maxResCode;
    } else {
        int mant = maxResCode & 15;
        int exp = (maxResCode & 112) >> 4;
        std::cout << "mant: " << mant << ", exp: " << exp << std::endl;
    }

    maxRespTime *= 100; // The maxRespTime is represented in 1/10 of seconds

    IPAddress groupAddr = igmph->Group_Address;
    // click_chatter(groupAddr.unparse().c_str());
    if (groupAddr.empty() == true) {
        reportGenerator->sendGeneralReport(maxRespTime);
    }
    else {
        reportGenerator->sendGroupSpecificReport(groupAddr, maxRespTime);
    }
}


CLICK_ENDDECLS

EXPORT_ELEMENT(IGMPQueryHandler)