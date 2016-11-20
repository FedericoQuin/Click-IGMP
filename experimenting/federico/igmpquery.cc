#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpquery.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

#include <time.h>
#include <stdlib.h>
#include <iostream>

CLICK_DECLS

IGMPQuery::IGMPQuery() : f_mrc(0), f_qrv(0), f_qqiv(0) {}

IGMPQuery::~IGMPQuery() {}

int IGMPQuery::configure(Vector<String>& conf, ErrorHandler* errh) {

	if ( cp_va_kparse(conf, this, errh,
		"MRC", cpkM + cpkP, cpUnsigned, &f_mrc,
		"QRV", 0, cpUnsigned, &f_qrv,
		"QQIV", 0, cpUnsigned, &f_qqiv,
	cpEnd) < 0 ) return -1;

	Timer* timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(1000);

	return 0;
}

void IGMPQuery::run_timer(Timer* timer) {
	if (Packet* q = this->make_packet()) {
		output(0).push(q);
		timer->reschedule_after_msec(1000);
	}
}

Packet* IGMPQuery::make_packet() {
	int headroom = sizeof(click_ether) + sizeof(click_ip);
	WritablePacket* q = Packet::make(headroom, 0, sizeof(struct IGMP_query), 0);
	if (!q)
		return 0;
	
	memset(q->data(), '\0', sizeof(struct IGMP_query));
	IGMP_query* igmph = (IGMP_query*)(q->data());

	igmph->Type = 0x11;
	igmph->Max_Resp_Code = 127;
	igmph->Group_Address = 0;
	igmph->Resv = 0;
	igmph->S = 0;
	igmph->QRV = 0;
	igmph->QQIC = 0;
	igmph->Number_of_Sources = 0;
	igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeof(IGMP_query));

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPQuery)
