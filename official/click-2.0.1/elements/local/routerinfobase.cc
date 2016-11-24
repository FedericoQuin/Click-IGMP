#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "routerinfobase.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

CLICK_DECLS

RouterInfoBase::RouterInfoBase()
{}

RouterInfoBase::~RouterInfoBase()
{}

int RouterInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) { 
	return 0;
}

void RouterInfoBase::addIPToGroup(IPAddress groupIP, uint8_t joinInterface){

	for (Vector<IPAddress>::iterator i = table[joinInterface].begin(); i != table[joinInterface].end(); i++){
		if (*i == groupIP){
			return;
		}
	}
	table[joinInterface].push_back(groupIP);
}

void RouterInfoBase::deleteIPFromGroup(IPAddress groupIP, uint8_t UnjoinInterface){
	for (Vector<IPAddress>::iterator i = table[UnjoinInterface].begin(); i != table[UnjoinInterface].end(); i++){
		if (*i == groupIP){
			table[UnjoinInterface].erase(i);
			return;
		}
	}
	return;
}

Vector<IPAddress> RouterInfoBase::getGroups(uint8_t interface){
	return table[interface];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouterInfoBase)
