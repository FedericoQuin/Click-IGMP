#ifndef CLICK_IGMPREPORT_HH
#define CLICK_IGMPREPORT_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

//temporarily just a dummy packet creater
class IGMPReport : public Element {
public:
    IGMPReport();
    ~IGMPReport();

    const char* class_name() const { return "IGMPReport"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    void run_timer(Timer*);


private:
    Packet* make_packet();

    Vector<IPAddress> f_listenAddresses;

};

CLICK_ENDDECLS
#endif