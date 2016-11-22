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
	IGMP_query* igmphq = (IGMP_query*)(p->data());
	bool succesQ = false;
	if (igmphq != 0){
		if (igmphq->Type == 17){
			succesQ = checkQuery(p);
		}
	}

	IGMP_report* igmphr = (IGMP_report*)(p->data());
	bool succesR = false;
	if (igmphq != 0){
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
	IGMP_query* igmph = (IGMP_query*)(p->data());
	if(p->length() >sizeof(IGMP_query)){
		return false;
	}

	unsigned csum =click_in_cksum((unsigned char *)igmph, p->length()) & 0xFFFF;
	if(csum != 0){
		return false;
	}

	return true;
}

bool CheckIGMPHeader::checkReport(Packet *p){
	IGMP_report* igmph = (IGMP_report*)(p->data());
	if(p->length() > sizeof(IGMP_report)){
		return false;
	}

	unsigned csum =click_in_cksum((unsigned char *)igmph, p->length()) & 0xFFFF;
	if(csum != 0){
		return false;
	}
	return true;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CheckIGMPHeader)
