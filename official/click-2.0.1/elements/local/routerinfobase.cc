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
	QI = 125;
	QRI = 100;
	GMI = QRV*QI+QRI;
	SQI = 1/4* QI;
	SQC = QRV;
	LMQI = 10;
	LMQC = QRV;
	LMQT = LMQI*LMQC;

	queriesSuppressed = false;
}

RouterInfoBase::~RouterInfoBase()
{}

int RouterInfoBase::configure(Vector<String> &conf, ErrorHandler *errh) {
	uint8_t qrv = 0;
	unsigned int qi = 0;
	unsigned int qri = 0;
	//gmi word bepaald
	unsigned int sqi = 0; //kan bepaald worden
	unsigned int sqc = 0; //kan bepaald worden
	unsigned int lmqi = 0;
	unsigned int lmqc = 0; //kan bepaald worden
	//lmqt word bepaald
	if ( cp_va_kparse(conf, this, errh,
		"QUERIER", cpkM + cpkP, cpElement, &_querier,
		"QRV", 0, cpByte, &qrv,
		"QI", 0, cpUnsigned, &qi,
		"QRI", 0, cpUnsigned, &qri,
		"SQI", 0, cpUnsigned, &sqi,
		"SQC", 0, cpUnsigned, &sqc,
		"LMQI", 0, cpUnsigned, &lmqi,
		"LMQC", 0, cpUnsigned, &lmqc,
	cpEnd) < 0 ) return -1;
	
	if (qrv != 0){
		QRV = qrv;
	}
	if (qi != 0){
		QI = qi;
	}
	if (qri != 0){
		QRI = qri;
	}

	GMI = QRV*toTime(QI)+toTime(QRI)/10;


	SQI = QI/4;
	if(sqi != 0){
		SQI = sqi;
	}
	SQC = QRV;
	if(sqc != 0){
		SQC = sqc;
	}
	if(lmqi != 0){
		LMQI = lmqi;
	}
	LMQC = QRV;
	if(lmqc != 0){
		LMQC = lmqc;
	}

	LMQT = toTime(LMQI)/10 * toTime(LMQC);
	queriesToSend = SQC;

	queryTimer = new Timer(&sendStartUp,this);
	queryTimer->initialize(this);
	queryTimer->schedule_after_sec(toTime(SQI));
	return 0;
}

void RouterInfoBase::sendStartUp(Timer* timer, void* userdata){
	RouterInfoBase *thisElement = (RouterInfoBase*) userdata;
	thisElement->queriesToSend --;
	if(thisElement->queriesToSend != 0){
		thisElement->_querier->push(thisElement->QRV,thisElement->QRI,thisElement->SQI,0,0);
		thisElement->queryTimer->reschedule_after_sec(toTime(thisElement->SQI));
	}
	else{
		thisElement->_querier->push(thisElement->QRV,thisElement->QRI,thisElement->QI,0,0);
		thisElement->queryTimer->assign(sendQuery,userdata);
		thisElement->queryTimer->schedule_after_sec(toTime(thisElement->QI));
	}
}

void RouterInfoBase::deleteInterfacesFromGroupGeneral(Timer* timer, void* userdata){
	RouterInfoBase *thisElement = (RouterInfoBase*) userdata;

	for(HashTable<uint8_t,HashTable<IPAddress, bool> >::iterator i = thisElement->maydeletegeneral.begin(); i != thisElement->maydeletegeneral.end(); i++){
		for(HashTable<IPAddress,bool>::iterator j = i->second.begin(); j != i->second.end(); j++){
			if(j->second){
				thisElement->deleteIPFromGroup(j->first,i->first);
			}
			j->second = true;
		}
	}
	delete timer;
}

void RouterInfoBase::sendQuery(Timer* timer, void* userdata){
	RouterInfoBase *thisElement = (RouterInfoBase*) userdata;
	if(thisElement->queriesToSend == 0){
		thisElement->queriesToSend = thisElement->QRV;
		Timer* deleteTimer = new Timer(deleteInterfacesFromGroupGeneral,userdata);
		deleteTimer->initialize(thisElement);
		deleteTimer->schedule_after_sec(thisElement->GMI);
	}
	thisElement->queriesToSend --;
	thisElement->_querier->push(thisElement->QRV,thisElement->QRI,thisElement->QI,0,0);
	thisElement->queryTimer->schedule_after_sec(toTime(thisElement->QI));
}

void RouterInfoBase::sendQuery(IPAddress IP, unsigned int interface){
	if(deleteTimers[interface][IP]){
		return;
	}
	maydeletespecific[interface][IP] = true;
	if (queriesSuppressed == false) {
		_querier->push(QRV,LMQI,LMQI/10,IP,interface);
	}

	SpecificDeleteData* data = new SpecificDeleteData();
	data->element = this;
	data->interface = interface;
	data->IP = IP;
	data->toSend = LMQC-1;
	Timer * queryTimer = new Timer(&sendQuerySpecific,data);
	queryTimer->initialize(this);
	queryTimer->schedule_after_msec(100*LMQT);

	deleteTimers[interface][IP] = new Timer(&deleteInterfaceFromGroupSpecific,data);
	deleteTimers[interface][IP]->initialize(this);
	deleteTimers[interface][IP]->schedule_after_sec(LMQT);

}
void RouterInfoBase::deleteInterfaceFromGroupSpecific(Timer* timer, void* userdata){
	SpecificDeleteData *data = (SpecificDeleteData*) userdata;
	if(data->element->maydeletespecific[data->interface][data->IP]){
		data->element->deleteIPFromGroup(data->IP,data->interface);
	}
	delete timer;
	data->element->deleteTimers[data->interface][data->IP] = NULL;
}

void RouterInfoBase::sendQuerySpecific(Timer* timer,void* userdata){	
	SpecificDeleteData *data = (SpecificDeleteData*) userdata;
	if(data->toSend == 0){
		delete timer;
		return;
	}
	data->toSend --;
	if (data->element->queriesSuppressed == false) {
		data->element->_querier->push(data->element->QRV,data->element->LMQI,data->element->LMQI/10,data->IP,data->interface);
	}
	timer->reschedule_after_msec(100*data->element->LMQT);
}

void RouterInfoBase::addIPToGroup(IPAddress groupIP, uint8_t joinInterface){
	maydeletespecific[joinInterface][groupIP] = false;
	maydeletegeneral[joinInterface][groupIP] = false;
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

Vector<IPAddress> RouterInfoBase::getAllGroups() {
	Vector<IPAddress> addresses = Vector<IPAddress>();
	for (HashTable<uint8_t,Vector<IPAddress> >::iterator i = table.begin(); i != table.end(); i++) {
		for (Vector<IPAddress>::iterator j = i->second.begin(); j < i->second.end(); j++) {
			if (vector_contains(addresses, *j) == false) {
				addresses.push_back(*j);
			}
		}
	}
	return addresses;
}

bool RouterInfoBase::hasAddress(IPAddress addr) {
	Vector<IPAddress> addresses = this->getAllGroups();
	return vector_contains(addresses, addr);
}

void RouterInfoBase::suppressQueries() {
	queriesSuppressed = true;
}

void RouterInfoBase::allowQueries() {
	queriesSuppressed = false;
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
	add_write_handler("set_qi", &handleSetQI, (void*)0);
	add_write_handler("set_qri",&handleSetQRI,(void*)0);
	add_write_handler("set_lmqi",&handleSetLMQI,(void*)0);
	add_write_handler("set_lmqc",&handleSetLMQC,(void*)0);
}


int RouterInfoBase::handleSetQI(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "QI", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	thisElement->QI = value;
	thisElement->GMI = thisElement->QRV*toTime(thisElement->QI)+toTime(thisElement->QRI)/10;

	return 0;
}

int RouterInfoBase::handleSetQRI(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "QRI", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	thisElement->QRI = value;
	thisElement->GMI = thisElement->QRV*toTime(thisElement->QI)+toTime(thisElement->QRI)/10;
	return 0;
}

int RouterInfoBase::handleSetLMQI(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "LMQI", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	thisElement->LMQI = value;
	thisElement->LMQT = toTime(thisElement->LMQI)/10 * toTime(thisElement->LMQC);
	return 0;
}

int RouterInfoBase::handleSetLMQC(const String& conf, Element* e, void* thunk, ErrorHandler* errh){
	RouterInfoBase* thisElement = (RouterInfoBase*) e;
	unsigned int value = 0;
	if (cp_va_kparse(conf, thisElement, errh, 
        "LMQC", cpkM+cpkP, cpUnsigned, &value,
        cpEnd)
    < 0 ) return -1;

	thisElement->LMQC = value;
	thisElement->LMQT = toTime(thisElement->LMQI)/10 * toTime(thisElement->LMQC);
	return 0;
}

void RouterInfoBase::setQQIT(unsigned int qqit){
	QI = qqit;
	GMI = QRV*toTime(QI)+toTime(QRI)/10;
}

void RouterInfoBase::setMRT(unsigned int mrt){
	QRI = mrt;
	GMI = QRV*QI+QRI/10;
}

bool vector_contains(const Vector<IPAddress>& v, const IPAddress& target) {
	for (Vector<IPAddress>::const_iterator i = v.begin(); i != v.end(); i++) {
		if (*i == target) {
			return true;
		}
	}
	return false;
}

unsigned int toTime(uint8_t code){
	if(code > 127){
		uint8_t temp = (uint8_t) code;
		int mant = temp & 15;
        int exp = (temp & 112) >> 4;
        return (mant | 0x10) << (exp + 3);
	}
	return code;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RouterInfoBase)
