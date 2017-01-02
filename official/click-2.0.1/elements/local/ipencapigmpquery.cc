#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "ipencapigmpquery.hh"
#include "igmppackets.h"

#include <iostream>

CLICK_DECLS

IPEncapIGMPQuery::IPEncapIGMPQuery() : f_src(IPAddress(0)) {}
IPEncapIGMPQuery::~IPEncapIGMPQuery() {}


int IPEncapIGMPQuery::configure(Vector<String>& conf, ErrorHandler* errh) {
    if (cp_va_kparse(conf, this, errh, 
        "SRC", cpkM+cpkP, cpIPAddress, &f_src,
    cpEnd) < 0) return -1;
    return 0;
}

void IPEncapIGMPQuery::push(int, Packet* p) {
    WritablePacket* q = p->uniqueify();

    if (!q) {
        return;
    }

    IGMP_query* igmph = (IGMP_query*)q->data();
    IPAddress dst = IPAddress(igmph->Group_Address);

    q = p->push(sizeof(click_ip));

    click_ip* iph = reinterpret_cast<click_ip*>(q->data());    
    memset(iph, 0, sizeof(click_ip));

    iph->ip_v = 4;
    iph->ip_hl = sizeof(click_ip) >> 2;
    iph->ip_src = f_src;
    iph->ip_ttl = 1;
    iph->ip_dst = dst.empty() == true ? IPAddress("224.0.0.1").in_addr() : dst.in_addr();
    iph->ip_p = IP_PROTO_IGMP;
    iph->ip_tos = 0;
    iph->ip_len = htons(q->length());
    
    q->set_dst_ip_anno(dst.empty() == true ? IPAddress("224.0.0.1") : dst);
    q->set_ip_header(iph, sizeof(click_ip));
    iph->ip_sum = click_in_cksum( (const unsigned char*)iph, sizeof(click_ip));

    output(0).push(p);
}


CLICK_ENDDECLS

EXPORT_ELEMENT(IPEncapIGMPQuery)