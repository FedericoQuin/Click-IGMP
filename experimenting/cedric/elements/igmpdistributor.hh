#ifndef CLICK_IGMPDISTRIBUTOR_HH
#define CLICK_IGMPDISTRIBUTOR_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "routerinfobase.hh"

CLICK_DECLS
/*
=c
IGMPDistributor()

=s local

Distributes multicast message over subnets

=d

Expects IP packets as input. 

*/

class IGMPDistributor : public Element { 
	public:
		IGMPDistributor();
		~IGMPDistributor();
		
		const char *class_name() const	{ return "IGMPDistributor"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	private:
		RouterInfoBase* infoBase;

};

CLICK_ENDDECLS
#endif
