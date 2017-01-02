#ifndef CLICK_CLIENTMULTICASTFILTER_HH
#define CLICK_CLIENTMULTICASTFILTER_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include "clientinfobase.hh"

CLICK_DECLS

/*
=c
ClientMulticastFilter()

=s local

Filters multicast packets according to desired packets.

=d

This element will filter out packets that it wants to output 0, and push packets it doesn't want to output 1

*/

class ClientMulticastFilter : public Element {
public:

    ClientMulticastFilter();
    ~ClientMulticastFilter();

    const char* class_name() const { return "ClientMulticastFilter"; }
    const char* port_count() const { return "1/2"; }
    const char* processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    void push(int, Packet*);

private:
    bool isPacketWanted(IPAddress multicastAddr) const;
    ClientInfoBase* clientState;
};

CLICK_ENDDECLS
#endif