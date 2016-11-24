#ifndef CLICK_ROUTERINFOBASE_HH
#define CLICK_ROUTERINFOBASE_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>

CLICK_DECLS
/*
=c
RouterInfoBase()

=s local

A infobase for the router. It contains a table with the IP from a group and the corresponding group

=d

It does not have any in or outputs

*/

class RouterInfoBase : public Element { 
	public:
		RouterInfoBase();
		~RouterInfoBase();
		
		const char *class_name() const	{ return "RouterInfoBase"; }
		int configure(Vector<String>&, ErrorHandler*);
		void addIPToGroup(IPAddress,IPAddress);
		void deleteIPFromGroup(IPAddress,IPAddress);
		Vector<IPAddress> getGroup(IPAddress);
		

	private:
		HashTable<IPAddress,Vector<IPAddress> > table;
		
};

CLICK_ENDDECLS
#endif
