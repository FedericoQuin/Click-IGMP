#ifndef CLICK_CLIENTQUERYHANDLER_HH
#define CLICK_CLIENTQUERYHANDLER_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "clientinfobase.hh"
#include "clientreportgenerator.hh"

CLICK_DECLS

class ClientQueryHandler : public Element { 
	public:
		ClientQueryHandler();
		~ClientQueryHandler();
		
		const char *class_name() const	{ return "ClientQueryHandler"; }
		const char *port_count() const	{ return "1/0"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

        void push(int, Packet*);
		
	private:
		void handleQuery(Packet* p);
		ClientInfoBase* clientState;
        ClientReportGenerator* reportGenerator;

};

CLICK_ENDDECLS
#endif
