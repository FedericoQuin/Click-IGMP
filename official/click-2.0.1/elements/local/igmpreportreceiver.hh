#ifndef CLICK_IGMPREPORTRECEIVER_HH
#define CLICK_IGMPREPORTRECEIVER_HH
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
IGMPReportReceiver()

=s local

parses and handles a IGMPReport packet inside of a IP packet

=d

Expects IP packets as input. 

*/

class IGMPReportReceiver : public Element { 
	public:
		IGMPReportReceiver();
		~IGMPReportReceiver();
		
		const char *class_name() const	{ return "IGMPReportReceiver"; }
		const char *port_count() const	{ return "1/0"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	private:
		bool checkReport(Packet *);
		RouterInfoBase* infoBase;

};

CLICK_ENDDECLS
#endif
