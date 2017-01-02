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
	MRT = 100;
	QQIT = 125;
}

RouterInfoBase::~RouterInfoBase()
{}

int RouterInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
	if ( cp_va_kparse(conf, this, errh,
		"QUERIER", cpkM + cpkP, cpElement, &_querier,
		"QRV", 0, cpByte, &QRV,
		"QQIT", 0, cpUnsigned, &QQIT,
		"MRT", 0, cpUnsigned, &MRT,
	cpEnd) < 0 ) return -1;
	if(MRT/10 >= QQIT){
		return -1;
	}
	queryTimer = new Timer(&sendQuery,this);
	queryTimer->initialize(this);
	queryTimer->schedule_after_sec(QQIT);
	return 0;
}

void RouterInfoBase::deleteInterfaceFromGroup(Timer* timer, void* userdata){
	RouterInfoBase *thisElement = (RouterInfoBase*) userdata;
	uint8_t interface;
	IPAddress IP;
	for(HashTable<uint8_t,HashTable<IPAddress, Timer*> >::iterator i = thisElement->deletetimers.begin(); i != thisElement->deletetimers.end(); i++){
		HashTable<IPAddress,Timer*> secondary = i.value();
		for(HashTable<IPAddress,Timer*>::iterator j = secondary.begin(); j != secondary.end(); j++){
			if(j->second == timer){
				interface = i->first;
				IP = j->first;
			}
		}
	}

	thisElement->deleteIPFromGroup(IP,interface);
}

void RouterInfoBase::sendQuery(Timer* timer, void* userdata){
	RouterInfoBase *thisElement = (RouterInfoBase*) userdata;
	thisElement->_querier->push(thisElement->QRV,thisElement->MRT,thisElement->QQIT,0);
	for(HashTable<uint8_t,Vector<IPAddress> >::iterator i = thisElement->table.begin(); i != thisElement->table.end(); i++){
		for(Vector<IPAddress>::iterator j = i->second.begin(); j < i->second.end(); j++){
			if(not thisElement->deletetimers[i->first][*j]){
				thisElement->deletetimers[i->first][*j] = new Timer(&deleteInterfaceFromGroup,userdata);
				thisElement->deletetimers[i->first][*j]->initialize(thisElement);
				thisElement->deletetimers[i->first][*j]->schedule_after_msec(100*thisElement->MRT);
			}
		}
	}
	thisElement->queryTimer->reschedule_after_sec(thisElement->QQIT);
}

void RouterInfoBase::sendQuery(IPAddress IP, unsigned int interface){
	if(deletetimers[interface][IP]){
		return;
	}
	_querier->push(QRV,MRT,QQIT,IP);
	deletetimers[interface][IP] = new Timer(&deleteInterfaceFromGroup,this);
	deletetimers[interface][IP]->initialize(this);
	deletetimers[interface][IP]->schedule_after_msec(100*MRT);
}

void RouterInfoBase::addIPToGroup(IPAddress groupIP, uint8_t joinInterface){
	if (deletetimers[joinInterface][groupIP]){
		deletetimers[joinInterface].erase(groupIP);
	}
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
	add_write_handler("set_qqic", &handleSetInterval, (void*)0);
	add_write_handler("set_qrt",&handleSetWait,(void*)0);
}


int RouterInfoBase::handleSetInterval(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "QQIT", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	if(value<=thisElement->MRT/10){
		return -1;
	}
	thisElement->QQIT = value;
	return 0;
}

int RouterInfoBase::handleSetWait(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "MRT", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	if(value/10 >= thisElement->QQIT){
		return -1;
	}
	thisElement->MRT = value;
	return 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouterInfoBase)
