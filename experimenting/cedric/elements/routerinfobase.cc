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
	time = 125;
	waitingTime = 100;
}

RouterInfoBase::~RouterInfoBase()
{}

int RouterInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
	if ( cp_va_kparse(conf, this, errh,
		"QUERIER", cpkM + cpkP, cpElement, &_querier,
		"QRV", 0, cpByte, &QRV,
		"INTERVAL", 0, cpUnsigned, &time,
		"WAIT", 0, cpUnsigned, &waitingTime,
	cpEnd) < 0 ) return -1;
	if(waitingTime >= time){
		return -1;
	}
	return 0;
}

void RouterInfoBase::deleteInterfaceFromGroup(Timer* timer, void* userdata){
	RouterInfoBase *thisElement = (RouterInfoBase*) userdata;
	uint8_t interface;
	IPAddress IP;
	for(HashTable<uint8_t,HashTable<IPAddress, Timer*> >::iterator i = thisElement->timers.begin(); i != thisElement->timers.end(); i++){
		HashTable<IPAddress,Timer*> secondary = i.value();
		for(HashTable<IPAddress,Timer*>::iterator j = secondary.begin(); j != secondary.end(); j++){
			if(j->second == timer){
				interface = i->first;
				IP = j->first;
			}
		}
	}

	thisElement->deleteIPFromGroup(IP,interface);
	thisElement->timers[interface].erase(IP);
}

void RouterInfoBase::sendQuery(Timer* timer, void* userdata){
	RouterInfoBase *thisElement = (RouterInfoBase*) userdata;
	for(HashTable<uint8_t,HashTable<IPAddress,Timer*> >::iterator i = thisElement->timers.begin(); i != thisElement->timers.end(); i++){
		for(HashTable<IPAddress,Timer*>::iterator j = i.value().begin(); j != i.value().end(); j++){
			if(j->second == timer){
				thisElement->_querier->push(thisElement->QRV,j->first);
				if(j->second){
					delete j->second;
				}
				j->second = new Timer(&deleteInterfaceFromGroup,userdata);
				j->second->initialize(thisElement);
				j->second->schedule_after_sec(thisElement->waitingTime);
				return;
			}
		}
	}
}

void RouterInfoBase::sendQuery(IPAddress IP, unsigned int interface){
	_querier->push(QRV,IP);
	if(timers[interface][IP]){
		delete timers[interface][IP];
	}
	timers[interface][IP] = new Timer(&deleteInterfaceFromGroup,this);
	timers[interface][IP]->initialize(this);
	timers[interface][IP]->schedule_after_sec(waitingTime);
}

void RouterInfoBase::addIPToGroup(IPAddress groupIP, uint8_t joinInterface){
	if (timers[joinInterface][groupIP]){
		delete timers[joinInterface][groupIP];
	}
	timers[joinInterface][groupIP] = new Timer(&sendQuery,this);
	timers[joinInterface][groupIP]->initialize(this);
	timers[joinInterface][groupIP]->schedule_after_sec(time);
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
			std::cout<<"DELETED"<<std::endl;
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
	add_write_handler("set_interval", &handleSetInterval, (void*)0);
	add_write_handler("set_wait",&handleSetWait,(void*)0);
}


int RouterInfoBase::handleSetInterval(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "INTERVAL", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	if(value<=thisElement->waitingTime){
		return -1;
	}
	thisElement->time = value;
	return 0;
}

int RouterInfoBase::handleSetWait(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "WAIT", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	if(value >= thisElement->time){
		return -1;
	}
	thisElement->waitingTime = value;
	return 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouterInfoBase)
