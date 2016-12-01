#ifndef CLICK_IGMPQUERYHANDLER_HH
#define CLICK_IGMPQUERYHANDLER_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "clientinfobase.hh"
#include "igmpreport.hh"

CLICK_DECLS

class IGMPQueryHandler : public Element { 
	public:
		IGMPQueryHandler();
		~IGMPQueryHandler();
		
		const char *class_name() const	{ return "IGMPQueryHandler"; }
		const char *port_count() const	{ return "1/0"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

        void push(int, Packet*);
		
	private:
		void handleQuery(Packet* p);
		ClientInfoBase* clientState;
        IGMPReport* reportGenerator;

};

CLICK_ENDDECLS
#endif
