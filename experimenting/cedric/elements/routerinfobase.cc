#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "routerinfobase.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <iostream>

CLICK_DECLS

RouterInfoBase::RouterInfoBase()
{
	QRV = 2;
}

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

void RouterInfoBase::setQRV(uint8_t value){
	if (value > 7){
		value = 0;
	}
	QRV = value;
}

int RouterInfoBase::handleSetQRV(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	uint8_t value = 2;

	if (cp_va_kparse(conf, thisElement, errh, 
        "QRV", cpkM+cpkP, cpByte, &value,
        cpEnd)
    < 0 ) return -1;

	thisElement->setQRV(value);
	return 0;
}

uint8_t RouterInfoBase::getQRV(){
	return QRV;
}

void RouterInfoBase::add_handlers(){
	add_write_handler("set_qrv", &handleSetQRV, (void*)0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouterInfoBase)
