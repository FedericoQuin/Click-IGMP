#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpreport.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

CLICK_DECLS

IGMPReport::IGMPReport() {}
IGMPReport::~IGMPReport() {}

int IGMPReport::configure(Vector<String>& conf, ErrorHandler* errh) {
    Timer* timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);

    return 0;
}

void IGMPReport::run_timer(Timer* timer) {
    static int temp = 0;
    int new_addr = 16909056+temp++;
    f_listenAddresses.push_back(IPAddress(htonl(new_addr)));

    if (Packet*q = this->make_packet()) {
        output(0).push(q);
        timer->schedule_after_msec(1000);
    }
}

Packet* IGMPReport::make_packet() {
    int amtGroupRecords = f_listenAddresses.size();
    // is dependant on the amount of multicast addresses being listened to
    int sizeIGMPheader = sizeof(struct IGMP_report) + amtGroupRecords * sizeof(struct IGMP_grouprecord);
    int headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket* q = Packet::make(headroom, 0, sizeIGMPheader, 0);

    if (!q)
        return 0; //in case there was no memory (or other reasons) for the packet not being able to be created;

    memset(q->data(), '\0', sizeIGMPheader);

    IGMP_report* igmph = (IGMP_report*)(q->data());

    igmph->Type = 0x22;
    igmph->Reserved1 = 0;
    igmph->Reserved2 = 0;
    igmph->Number_of_Group_Records = htons(amtGroupRecords);

    IGMP_grouprecord* record = (IGMP_grouprecord*)(igmph + 1);
    for (int i = 0; i < amtGroupRecords; i++) {
        record->Record_Type = RECORD_TYPE_EX_TO_IN; // TODO make more concrete, dependant on change from include to exclude or vice versa 
        record->Aux_Data_Len = 0; // not used in IGMPv3
        record->Number_of_Sources = 0; // does not need to be implemented in our version
        record->Multicast_Address = f_listenAddresses.at(i); // multicast_address i

        record = record + 1;
    }

    igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeIGMPheader );
    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPReport)