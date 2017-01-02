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

IGMPQueryGenerator::IGMPQueryGenerator(){}

IGMPQueryGenerator::~IGMPQueryGenerator() {}

int IGMPQueryGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
	return 0;
}

void IGMPQueryGenerator::push(uint8_t QRV, unsigned int MRT ,unsigned int QQIT,IPAddress GroupIP){
	if(Packet* something = make_packet(QRV,MRT,QQIT,GroupIP)){
		output(0).push(something);
	}
}

Packet* IGMPQueryGenerator::make_packet(uint8_t QRV,unsigned int MRT, unsigned int QQIT ,IPAddress GroupIP) {
	int headroom = sizeof(click_ether) + sizeof(click_ip);
	WritablePacket* q = Packet::make(headroom, 0, sizeof(struct IGMP_query), 0);
	if (!q)
		return 0;
	
	//nog MRC EN QQIC
	uint8_t MRC = 0;
	uint8_t QQIC = 0;

	if(MRT < 128){
		MRC = uint8_t(MRT);
	}
	else{
		//NOG TE DOEN
	}

	if (QQIT < 128){
		QQIC = uint8_t(QQIT);
	}
	else{
		//OOK NOG TE DOEN
		uint8_t firstBit = 1;

	}
	// memset(q->data(), '\0', sizeof(struct IGMP_query));

	memset(q->data(), '\0', q->length());
	IGMP_query* igmph = (IGMP_query*)(q->data());

	igmph->Type = 0x11;
	igmph->Max_Resp_Code = MRC;
	igmph->Group_Address = GroupIP; //IPAddress("226.1.1.1");
	igmph->Resv = 0;
	igmph->S = 0;
	igmph->QRV = QRV;
	igmph->QQIC = QQIC;
	igmph->Number_of_Sources = 0;
	igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeof(IGMP_query));

	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPQueryGenerator)
