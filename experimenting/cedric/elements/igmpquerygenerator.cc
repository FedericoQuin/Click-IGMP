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

IGMPQueryGenerator::IGMPQueryGenerator() : f_mrc(0), f_qqic(0) {}

IGMPQueryGenerator::~IGMPQueryGenerator() {}

int IGMPQueryGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
	if ( cp_va_kparse(conf, this, errh,
		"MRC", cpkM + cpkP, cpUnsigned, &f_mrc,
		"QQIC", 0, cpUnsigned, &f_qqic,
	cpEnd) < 0 ) return -1;
	return 0;
}

void IGMPQueryGenerator::push(uint8_t QRV, IPAddress GroupIP){
	if(Packet* something = make_packet(QRV,GroupIP)){
		output(0).push(something);
	}
}

Packet* IGMPQueryGenerator::make_packet(uint8_t QRV,IPAddress GroupIP) {
	int headroom = sizeof(click_ether) + sizeof(click_ip);
	WritablePacket* q = Packet::make(headroom, 0, sizeof(struct IGMP_query), 0);
	if (!q)
		return 0;
	
	// memset(q->data(), '\0', sizeof(struct IGMP_query));
	memset(q->data(), '\0', q->length());
	IGMP_query* igmph = (IGMP_query*)(q->data());

	igmph->Type = 0x11;
	igmph->Max_Resp_Code = f_mrc;
	igmph->Group_Address = GroupIP; //IPAddress("226.1.1.1");
	igmph->Resv = 0;
	igmph->S = 0;
	igmph->QRV = QRV;
	igmph->QQIC = f_qqic;
	igmph->Number_of_Sources = 0;
	igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeof(IGMP_query));

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPQueryGenerator)
