#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpreportreceiver.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/packet_anno.hh>

CLICK_DECLS

IGMPReportReceiver::IGMPReportReceiver()
{}

IGMPReportReceiver::~IGMPReportReceiver()
{}

int IGMPReportReceiver::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (cp_va_kparse(conf, this, errh, "INFOBASE", cpkM, cpElement, &infoBase, cpEnd) < 0) return -1;
	if (infoBase == 0) return errh->error("INFOBASE can't be a nullptr");
	return 0;
}

void IGMPReportReceiver::push(int, Packet *p){
	if (!checkReport(p)){
		return;
	}
	const click_ip *ipheader = p->ip_header();

	uint8_t sourceInterface = p->anno_u8(PAINT_ANNO_OFFSET);

	IGMP_report* igmp = (IGMP_report*)(p->data()+p->ip_header_length());


	uint16_t number = ntohs(igmp->Number_of_Group_Records);



	for(uint16_t i = 0; i < number; i++){
		IGMP_grouprecord* record = (IGMP_grouprecord*)(p->data()+p->ip_header_length()+sizeof(IGMP_report)+i*sizeof(IGMP_grouprecord));
		IPAddress groupAddess = record->Multicast_Address;
		uint8_t type = record->Record_Type;
		if(type == RECORD_TYPE_IN_TO_EX){
			infoBase->addIPToGroup(groupAddess,sourceInterface);
		}
		else if(type == RECORD_TYPE_EX_TO_IN){
			//STEL NOG EEN QUERY
			infoBase->sendQuery(groupAddess,sourceInterface);
		}
		else if(type == RECORD_TYPE_MODE_EX){
			infoBase->addIPToGroup(groupAddess,sourceInterface);
		}
		else if(type == RECORD_TYPE_MODE_IN){
			//WIL NIET MEER LUISTEREN
		}

	}
	
}

bool IGMPReportReceiver::checkReport(Packet* p){
	IGMP_report* igmph = (IGMP_report*)(p->data()+p->ip_header_length());
	if (igmph->Type == 0x22){
		return true;
	}
	return false;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPReportReceiver)
