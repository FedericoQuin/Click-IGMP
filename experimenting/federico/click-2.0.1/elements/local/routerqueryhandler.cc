#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "routerqueryhandler.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <iostream>

CLICK_DECLS

RouterQueryHandler::RouterQueryHandler() {}

RouterQueryHandler::~RouterQueryHandler() {}

int RouterQueryHandler::configure(Vector<String> &conf, ErrorHandler *errh) { 
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




    /// Handling of the Querier Robustness Variable
    // const int qrv = igmph->QRV;
    // if (qrv != 0) {
    //     clientState->setQRV(qrv);
    // }

    // int maxResCode = igmph->Max_Resp_Code;
    // int maxRespTime = 0;

    // if (maxResCode < 128) {
    //     maxRespTime = maxResCode;
    // } else {
    //     int mant = maxResCode & 15;
    //     int exp = (maxResCode & 112) >> 4;
    //     std::cout << "mant: " << mant << ", exp: " << exp << std::endl;
    // }

    // maxRespTime *= 100; // The maxRespTime is represented in 1/10 of seconds

    // IPAddress groupAddr = igmph->Group_Address;
    // // click_chatter(groupAddr.unparse().c_str());
    // if (groupAddr.empty() == true) {
    //     reportGenerator->sendGeneralReport(maxRespTime);
    // }
    // else {
    //     reportGenerator->sendGroupSpecificReport(groupAddr, maxRespTime);
    // }
}


CLICK_ENDDECLS
EXPORT_ELEMENT(RouterQueryHandler)
