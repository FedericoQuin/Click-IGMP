#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/error.hh>
#include <click/confparse.hh>
#include <click/config.h>
#include "igmpquery.hh"
#include "igmppackets.h"
CLICK_DECLS



IGMPquery::IGMPquery() {}

IGMPquery::~IGMPquery() {}


int IGMPquery::configure(Vector<String>& conf, ErrorHandler* errh) {
    Timer* timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);

    return 0;
}


void IGMPquery::run_timer(Timer* timer) {
    if (Packet* q = make_packet()) {
        output(0).push(q);
        timer->reschedule_after_msec(1000);
    }
}

Packet* IGMPquery::make_packet() {
    int headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket* q = Packet::make(headroom, 0, sizeof(struct IGMP_query), 0);

    if (!q)
        return 0; //in case there was no memory (or other reasons) for the packet not being able to be created;

    memset(q->data(), '\0', sizeof(struct IGMP_query));

    IGMP_query* igmph = (IGMP_query*)(q->data());

    igmph->Type = 17;
    igmph->Max_Resp_Code = 127;
    igmph->Group_Address = 0;
    igmph->Resv = 0;
    igmph->S = 0;
    igmph->QRV = 0;
    igmph->QQIC = 0;
    igmph->Number_of_Sources = 0;
    igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeof(IGMP_query) );

    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPquery)
