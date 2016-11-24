#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/vector.hh>
#include "clientinfobase.hh"

CLICK_DECLS

ClientInfoBase::ClientInfoBase() {}
ClientInfoBase::~ClientInfoBase() {}

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



CLICK_ENDDECLS
EXPORT_ELEMENT(ClientInfoBase)