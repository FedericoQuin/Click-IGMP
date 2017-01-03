#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "clientinfobase.hh"
#include "clientmulticastfilter.hh"

CLICK_DECLS

ClientMulticastFilter::ClientMulticastFilter() : clientState(0) {}
ClientMulticastFilter::~ClientMulticastFilter() {}

int ClientMulticastFilter::configure(Vector<String>& conf, ErrorHandler* errh) {
    if (cp_va_kparse(conf, this, errh, "STATE", cpkM+cpkP, cpElementCast, "ClientInfoBase", &clientState, cpEnd) < 0) return -1;
    
    if (clientState == 0) return errh->error("Wrong element given as argument, should be a ClientInfoBase element.");
    return 0;
}

void ClientMulticastFilter::push(int port, Packet* p) {
	const click_ip* iph = (click_ip*)p->data();
	IPAddress multicastAddr = iph->ip_dst;

    if (isPacketWanted(multicastAddr) == true) {
        output(0).push(p);
    }
    else {
        output(1).push(p);
    }
}

bool ClientMulticastFilter::isPacketWanted(IPAddress multicastAddr) const {
    return clientState->hasAddress(multicastAddr);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ClientMulticastFilter)