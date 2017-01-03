#ifndef CLICK_IGMPQUERYGENERATOR_HH
#define CLICK_IGMPQUERYGENERATOR_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

CLICK_DECLS
/*
=c
IGMPQUERYGenerator()

=s local

Creates a IGMPqueryGenerator packet.

=d

Not yet completly done

*/

class IGMPQueryGenerator : public Element {
public:
	IGMPQueryGenerator();
	~IGMPQueryGenerator();

	const char* class_name() const { return "IGMPQueryGenerator"; }
	const char* port_count() const { return "0/1"; }
	const char* processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);
	Packet* make_packet(uint8_t,unsigned int, unsigned int,IPAddress,uint8_t);
	void push(uint8_t,unsigned int , unsigned int, IPAddress,uint8_t);

};

CLICK_ENDDECLS
#endif
