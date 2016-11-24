#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpdistributor.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/vector.hh>
#include <iostream>

CLICK_DECLS

IGMPDistributor::IGMPDistributor()
{}

IGMPDistributor::~IGMPDistributor()
{}

int IGMPDistributor::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (cp_va_kparse(conf, this, errh, "INFOBASE", cpkM, cpElement, &infoBase, cpEnd) < 0) return -1;
	if (infoBase == 0) return errh->error("INFOBASE can't be a nullptr");
	return 0;
}

// void IGMPDistributor::push(int, Packet *p){
// 	const click_ip *ipheader = p->ip_header();
// 	IPAddress toCheck = ipheader->ip_dst;
// 	Vector<IPAddress> vector = infoBase->getGroup(toCheck);
// 	if(vector.empty()){
// 		output(0).push(p);
// 		return;
// 	}

// 	std::cout << "Size of vector: " << vector.size() << std::endl;
// 	for (int i = 0; i < vector.size(); i++) {
// 		String ipaddress = vector[i].unparse();
// 		std::cout << "\t" << ipaddress << std::endl;
// 	}
// 	for(Vector<IPAddress>::iterator i = vector.begin(); i != vector.end(); i++){

// 		WritablePacket *q = p->uniqueify();
// 		click_ip *header = (click_ip*)(q->data());
// 		header->ip_src = toCheck;
// 		header->ip_dst = i->in_addr();
// 		q->set_dst_ip_anno(*i);
// 		// q->set_anno_u32(0,i->addr());
// 		header->ip_sum = click_in_cksum((unsigned char *)(header),sizeof(click_ip));
// 		output(0).push(q);
// 	}
// }

void IGMPDistributor::push(int port, Packet* p) {
	const click_ip* IPHeader = (click_ip*)p->data();

	IPAddress destinationAddr = IPHeader->ip_dst;
	Vector<IPAddress> IGMPlisteners = infoBase->getGroup(destinationAddr);

	if (IGMPlisteners.size() == 0) {
		output(0).push(p);
		return;
	}

	// one or more sources are listening to this address, distribute the packet to all of them
	for (int i = 0; i < IGMPlisteners.size(); i++) {
		IPAddress newDestination = IGMPlisteners[i];

		WritablePacket* newPkt = p->uniqueify();

		click_ip* newIPHeader = (click_ip*)newPkt->data();
		newIPHeader->ip_src = destinationAddr;
		newIPHeader->ip_dst = newDestination;
		newPkt->set_dst_ip_anno(newDestination);

		newIPHeader->ip_sum = click_in_cksum((unsigned char*)newIPHeader, sizeof(click_ip));

		output(0).push(newPkt);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPDistributor)
