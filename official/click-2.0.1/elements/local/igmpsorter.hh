#ifndef CLICK_IGMPSORTER_HH
#define CLICK_IGMPSORTER_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

CLICK_DECLS


/**
 * Class that sorts IGMP packet by their type: query or report
 * 
 * Query packets are pushed on output 0.
 * Report packets are pushed on output 1.
 */

class IGMPSorter : public Element { 
	public:
		IGMPSorter();
		~IGMPSorter();
		
		const char *class_name() const	{ return "IGMPSorter"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet*);
};

CLICK_ENDDECLS
#endif
