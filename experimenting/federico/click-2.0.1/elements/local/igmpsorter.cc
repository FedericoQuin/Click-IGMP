#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpsorter.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <iostream>

CLICK_DECLS

IGMPSorter::IGMPSorter() {}

IGMPSorter::~IGMPSorter() {}

int IGMPSorter::configure(Vector<String> &conf, ErrorHandler *errh) { 
	return 0;
}

void IGMPSorter::push(int, Packet* p){
    IGMP_query* igmph = (IGMP_query*)(p->data() + p->ip_header_length());

    const int type = igmph->Type;
    if (type == IGMP_QUERY_TYPE) {
        output(0).push(p);
    } else if (type == IGMP_REPORT_TYPE) {
        output(1).push(p);
    }
}


CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPSorter)
