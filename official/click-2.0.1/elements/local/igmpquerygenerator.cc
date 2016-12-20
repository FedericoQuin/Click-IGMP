#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpquerygenerator.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

#include <time.h>
#include <stdlib.h>
#include <iostream>

CLICK_DECLS

IGMPQueryGenerator::IGMPQueryGenerator() : f_mrc(0), f_qrv(0), f_qqic(0) {}

IGMPQueryGenerator::~IGMPQueryGenerator() {}

int IGMPQueryGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {

	if ( cp_va_kparse(conf, this, errh,
		"MRC", cpkM + cpkP, cpUnsigned, &f_mrc,
		"QRV", 0, cpUnsigned, &f_qrv,
		"QQIC", 0, cpUnsigned, &f_qqic,
	cpEnd) < 0 ) return -1;

	Timer* timer = new Timer(this);
	timer->initialize(this);
	timer->schedule_after_msec(125 * 1000);

	return 0;
}

void IGMPQueryGenerator::sendGroupSpecificQuery(IPAddress addr) {
	if (Packet* q = this->make_packet(addr)) {
		output(0).push(q);
	}
}


void IGMPQueryGenerator::run_timer(Timer* timer) {
	if (Packet* q = this->make_packet()) {
		output(0).push(q);
		timer->reschedule_after_msec(125 * 1000); // TODO still hardcoded, make this adjustable according to a variable in infobase
	}
}

/**
 * Makes by default a general query, or a group specific query when specifying an IP address.
 */
Packet* IGMPQueryGenerator::make_packet(IPAddress groupAddr) {
	int headroom = sizeof(click_ether) + sizeof(click_ip);
	WritablePacket* q = Packet::make(headroom, 0, sizeof(struct IGMP_query), 0);
	if (!q)
		return 0;
	
	// memset(q->data(), '\0', sizeof(struct IGMP_query));
	memset(q->data(), '\0', q->length());
	IGMP_query* igmph = (IGMP_query*)(q->data());

	igmph->Type = 0x11;
	igmph->Max_Resp_Code = f_mrc;
	igmph->Group_Address = groupAddr;
	igmph->Resv = 0;
	igmph->S = 0;
	igmph->QRV = f_qrv;
	igmph->QQIC = f_qqic;
	igmph->Number_of_Sources = 0;
	igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeof(IGMP_query));

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPQueryGenerator)
