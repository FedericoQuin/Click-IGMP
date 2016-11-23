#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "checkigmpheader.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

CLICK_DECLS

CheckIGMPHeader::CheckIGMPHeader()
{}

CheckIGMPHeader::~ CheckIGMPHeader()
{}

int CheckIGMPHeader::configure(Vector<String> &conf, ErrorHandler *errh) { 
	return 0;
}

void CheckIGMPHeader::push(int, Packet *p){
	const click_ip *ipheader = p->ip_header();
	if (!p->has_network_header() or ipheader->ip_p != 2){
		output(1).push(p);
		return;
	}

	IGMP_query* igmphq = (IGMP_query*)(p->data()+p->ip_header_length());
	bool succesQ = false;
	if (igmphq != 0){
		if (igmphq->Type == 17){
			succesQ = checkQuery(p);
		}
	}

	IGMP_report* igmphr = (IGMP_report*)(p->data()+p->ip_header_length());
	bool succesR = false;
	if (igmphr != 0){
		if (igmphq->Type == 0x22){
			succesR = checkReport(p);
		}
	}
	if(succesR or succesQ){
		output(0).push(p);
	}
	else{
		output(1).push(p);
	}
}

bool CheckIGMPHeader::checkQuery(Packet *p){
	uint32_t hlength = p->ip_header_length();
	IGMP_query* igmph = (IGMP_query*)(p->data()+hlength);
	if(p->length()-hlength >sizeof(IGMP_query)){
		return false;
	}

	unsigned csum =click_in_cksum((unsigned char *)igmph, p->length() - hlength) & 0xFFFF;
	if(csum != 0){
		return false;
	}

	return true;
}

bool CheckIGMPHeader::checkReport(Packet *p){
	uint32_t hlength = p->ip_header_length();
	IGMP_report* igmph = (IGMP_report*)(p->data()+hlength);
	uint16_t number = ntohs(igmph->Number_of_Group_Records);
	if(p->length() - hlength>sizeof(IGMP_report)+number*sizeof(IGMP_grouprecord)){
		return false;
	}

	unsigned csum =click_in_cksum((unsigned char *)igmph, p->length() - hlength) & 0xFFFF;
	if(csum != 0){
		return false;
	}
	return true;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CheckIGMPHeader)
