#ifndef CLICK_IPENCAPIGMPQUERY_HH
#define CLICK_IPENCAPIGMPQUERY_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>

CLICK_DECLS


/**
 * Encapsulates an IGMP query packet with the correct IP header, specifically:
 *
 *  - Correct DST address (different between general queries and group specific queries)
 *  - TTL 1
 *  - Protocol 2 (IGMP)
 */
class IPEncapIGMPQuery : public Element {
public:
	IPEncapIGMPQuery();
	~IPEncapIGMPQuery();

	const char* class_name() const { return "IPEncapIGMPQuery"; }
	const char* port_count() const { return "1/1"; }
	const char* processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

    void push(int, Packet*);
private:
    IPAddress f_src;

};


CLICK_ENDDECLS
#endif
