#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpquerygenerator.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/packet_anno.hh>

#include <time.h>
#include <stdlib.h>
#include <iostream>

CLICK_DECLS

IGMPQueryGenerator::IGMPQueryGenerator(){}

IGMPQueryGenerator::~IGMPQueryGenerator() {}

int IGMPQueryGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
	return 0;
}

void IGMPQueryGenerator::push(uint8_t QRV, unsigned int MRT ,unsigned int QQIT,IPAddress GroupIP, uint8_t interface){
	if(Packet* something = make_packet(QRV,MRT,QQIT,GroupIP,interface)){
		output(0).push(something);
	}
}

Packet* IGMPQueryGenerator::make_packet(uint8_t QRV,unsigned int MRT, unsigned int QQIT ,IPAddress GroupIP, uint8_t interface) {
	int headroom = sizeof(click_ether) + sizeof(click_ip);
	WritablePacket* q = Packet::make(headroom, 0, sizeof(struct IGMP_query), 0);
	if (!q)
		return 0;
	
	//nog MRC EN QQIC
	uint8_t MRC = uint8_t(MRT);
	uint8_t QQIC = uint8_t(QQIT);

	memset(q->data(), '\0', q->length());
	IGMP_query* igmph = (IGMP_query*)(q->data());

	igmph->Type = IGMP_QUERY_TYPE;
	igmph->Max_Resp_Code = MRC;
	igmph->Group_Address = GroupIP;
	igmph->Resv = 0;
	igmph->S = 0;
	igmph->QRV = QRV;
	igmph->QQIC = QQIC;
	igmph->Number_of_Sources = 0;
	igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeof(IGMP_query));
	q->set_anno_u8(PAINT_ANNO_OFFSET, interface);
	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPQueryGenerator)
