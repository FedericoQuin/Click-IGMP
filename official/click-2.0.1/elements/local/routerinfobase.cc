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

void RouterInfoBase::addIPToGroup(IPAddress groupIP, IPAddress joinIP){

	//checks wether it is already in there
	for (Vector<IPAddress>::iterator i = table[groupIP].begin(); i != table[groupIP].end(); i++){
		if (*i == joinIP){
			return;
		}
	}

	//just adds the IP address
	Vector<IPAddress> current = table[groupIP];
	current.push_back(joinIP);
	table[groupIP] = current;

	// table[groupIP].push_back(joinIP);
}

void RouterInfoBase::deleteIPFromGroup(IPAddress groupIP, IPAddress joinIP){
	Vector<IPAddress> IPs = table[groupIP];

	for (Vector<IPAddress>::iterator i = table[groupIP].begin(); i != table[groupIP].end(); i++){
		if (*i == joinIP){
			table[groupIP].erase(i);
			return;
		}
	}
	return;
}

Vector<IPAddress> RouterInfoBase::getGroup(IPAddress groupIP){
	return table[groupIP];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouterInfoBase)
