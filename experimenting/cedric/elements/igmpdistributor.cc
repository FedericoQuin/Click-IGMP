#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpdistributor.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/vector.hh>

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

void IGMPDistributor::push(int, Packet *p){
	const click_ip *ipheader = p->ip_header();
	IPAddress toCheck = ipheader->ip_dst;
	Vector<IPAddress> vector = infoBase->getGroup(toCheck);
	if(vector.empty()){
		output(0).push(p);
		return;
	}
	for(Vector<IPAddress>::iterator i = vector.begin(); i != vector.end(); i++){
		WritablePacket *q = p->uniqueify();
		click_ip *header = (click_ip*)(q->data());
		header->ip_src = toCheck;
		header->ip_dst = i->in_addr();
		q->set_anno_u32(0,i->addr());
		header->ip_sum = click_in_cksum((unsigned char *)(header),sizeof(click_ip));
		output(0).push(q);
	}


}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPDistributor)
