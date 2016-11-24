#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "routermulticastfilter.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/packet_anno.hh>
#include <click/vector.hh>

CLICK_DECLS

RouterMulticastFilter::RouterMulticastFilter()
{}

RouterMulticastFilter::~RouterMulticastFilter()
{}

int RouterMulticastFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (cp_va_kparse(conf, this, errh,"INTERFACE",cpkM+cpkP,cpByte,&interface, "INFOBASE", cpkM+cpkP, cpElement, &infoBase, cpEnd) < 0) return -1;
	if (infoBase == 0) return errh->error("INFOBASE can't be a nullptr");
	return 0;
}

void RouterMulticastFilter::push(int, Packet *p){
	const click_ip *ipheader = p->ip_header();
	IPAddress groupIP= ipheader->ip_dst;
	Vector<IPAddress> addresses = infoBase->getGroups(interface);

	for(Vector<IPAddress>::iterator i = addresses.begin(); i!=addresses.end(); i++){
		if(*i == groupIP){
			output(0).push(p);
			return;
		}
	}
	output(1).push(p);
	return;
	
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouterMulticastFilter)
