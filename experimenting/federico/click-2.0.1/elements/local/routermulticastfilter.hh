#ifndef CLICK_ROUTERMULTICASTFILTER_HH
#define CLICK_ROUTERMULTICASTFILTER_HH
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
RouterMulticastFilter()

=s local

classifies an IP packet based on the infobase

=d

Expects IP packets as input, will give the same packet on either output 0 or output 1. 

*/

class RouterMulticastFilter : public Element { 
	public:
		RouterMulticastFilter();
		~RouterMulticastFilter();
		
		const char *class_name() const	{ return "RouterMulticastFilter"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	private:
		RouterInfoBase* infoBase;
		uint8_t interface;

};

CLICK_ENDDECLS
#endif
