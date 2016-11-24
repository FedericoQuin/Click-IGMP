#ifndef CLICK_IGMPQUERY_HH
#define CLICK_IGMPQUERY_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

CLICK_DECLS
/*
=c
IGMPQUERY()

=s local

Creates a IGMPquery packet.

=d

Not yet completly done

*/

class IGMPQuery : public Element {
public:
	IGMPQuery();
	~IGMPQuery();

	const char* class_name() const { return "IGMPQuery"; }
	const char* port_count() const { return "0/1"; }
	const char* processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void run_timer(Timer*);

private:
	Packet* make_packet();
	unsigned int f_mrc;
	unsigned int f_qrv;
	unsigned int f_qqic;
};

CLICK_ENDDECLS
#endif
