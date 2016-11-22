#ifndef CLICK_CHECKIGMPHEADER_HH
#define CLICK_CHECKIGMPHEADER_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

CLICK_DECLS
/*
=c
CheckIGMPHeader()

=s local

checks IGMP header on IGMP packets

=d

Expects ICMP packets as input. Checks that the packet's length is sensible, 
that the type field is correct and that its checksum field is valid. 
Pushes valid packets out on output 0 and invalid packets out on output 1.

*/

class CheckIGMPHeader : public Element { 
	public:
		CheckIGMPHeader();
		~CheckIGMPHeader();
		
		const char *class_name() const	{ return "CheckIGMPHeader"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet *);

	private:
		bool checkQuery(Packet *);
		bool checkReport(Packet *);
};

CLICK_ENDDECLS
#endif
