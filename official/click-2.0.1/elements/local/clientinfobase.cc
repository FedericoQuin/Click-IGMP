#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/vector.hh>
#include "clientinfobase.hh"
#include <iostream>

CLICK_DECLS

ClientInfoBase::ClientInfoBase() : f_qrv(2), f_unsolRepInterval(1000) {
    f_listenAddresses = Vector<IPAddress>();
}
ClientInfoBase::~ClientInfoBase() {}

void* ClientInfoBase::cast(const char* n) {
    if (strcmp(n, "ClientInfoBase") == 0)
        return (ClientInfoBase*)this;
    return 0;
}


int ClientInfoBase::configure(Vector<String>& conf, ErrorHandler* errh) {
    return 0;
}


void ClientInfoBase::addAddress(IPAddress newAddr) {
    if (hasAddress(newAddr) == true) {
        return;
    }
    f_listenAddresses.push_back(newAddr);
}

void ClientInfoBase::deleteAddress(IPAddress deleteAddr) {
    if (hasAddress(deleteAddr) == false) {
        return;
    }
    
    for (Vector<IPAddress>::iterator it = f_listenAddresses.begin(); it != f_listenAddresses.end(); it++) {
        if (*it == deleteAddr) {
            f_listenAddresses.erase(it);
            return;
        }
    }
}

bool ClientInfoBase::hasAddress(IPAddress compAddr) const {
    for (Vector<IPAddress>::const_iterator it = f_listenAddresses.begin(); it != f_listenAddresses.end(); it++) {
        if (*it == compAddr) {
            return true;
        }
    }
    return false;
}


Vector<IPAddress> ClientInfoBase::getAllAddresses() const {
    return f_listenAddresses;
}

void ClientInfoBase::setQRV(const int qrv) {
    if (qrv == 0) {
        return;
    }   
    f_qrv = qrv;
}

int ClientInfoBase::getQRV() const {
    return f_qrv;
}

int ClientInfoBase::getUnsolicitedReportInterval() const {
    return f_unsolRepInterval;
}

void ClientInfoBase::setUnsolicitedReportInterval(int value) {
    f_unsolRepInterval = value;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(ClientInfoBase)