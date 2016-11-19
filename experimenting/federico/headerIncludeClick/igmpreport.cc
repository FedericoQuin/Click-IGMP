#include <click/timer.hh>
#include <click/element.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/error.hh>
#include <click/confparse.hh>
#include <click/config.h>
#include <clicknet/igmppackets.h>
#include "igmpreport.hh"


CLICK_DECLS

IGMPReport::IGMPReport() {}
IGMPReport::~IGMPReport() {}

int IGMPReport::configure(Vector<String>& conf, Errorhandler* errh) {
    Timer* timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);

    return 0;
}

void IGMPReport::run_timer(Timer* timer) {
    if (Packet*q = this->make_packet()) {
        output(0).push(q);
        timer->schedule_after_msec(1000);
    }
}

Packet* IGMPReport::make_packet() {
    int headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket* q = Packet::make(headroom, 0, sizeof(struct IGMP_report), 0);

    if (!q)
        return 0; //in case there was no memory (or other reasons) for the packet not being able to be created;

    memset(q->data(), '\0', sizeof(struct IGMP_report));

    IGMP_report* igmph = (IGMP_report*)(q->data());

    igmph->Type = 0x22;
    igmph->Reserved1 = 0;
    igmph->Reserved2 = 0;
    igmph->Number_of_Group_Records = 0;

    igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeof(IGMP_report) );

    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPReport)