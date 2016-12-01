
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "clientinfobase.hh"
#include "igmpqueryhandler.hh"
#include "igmppackets.h"

CLICK_DECLS

IGMPQueryHandler::IGMPQueryHandler() : clientState(0), reportGenerator(0) {}

IGMPQueryHandler::~IGMPQueryHandler() {}

int IGMPQueryHandler::configure(Vector<String>& conf, ErrorHandler* errh) {
    if (cp_va_kparse(conf, this, errh, 
        "STATE", cpkM+cpkP, cpElement, &clientState,
        "RPT", cpkM+cpkP, cpElement, &reportGenerator,
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

    IPAddress groupAddr = igmph->Group_Address;

    // click_chatter(groupAddr.unparse().c_str());

    if (groupAddr.empty() == true) {
        /// if the address is equal to zero --> general query
        reportGenerator->sendGeneralReport();
    }
    else {
        reportGenerator->sendGroupSpecificReport(groupAddr);
    }
}


CLICK_ENDDECLS

EXPORT_ELEMENT(IGMPQueryHandler)