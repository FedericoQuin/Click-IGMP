#ifndef CLICK_QUERIERELECTION_HH
#define CLICK_QUERIERELECTION_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "routerinfobase.hh"

CLICK_DECLS


/**
 * Class that is used for the election (reception of a query on the same subnet)
 */

class QuerierElection : public Element { 
	public:
		QuerierElection();
		~QuerierElection();
		
		const char *class_name() const	{ return "QuerierElection"; }
		const char *port_count() const	{ return "1/0"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet*);
		void run_timer(Timer*);

    private:
        void handleQuery(Packet* p);
        RouterInfoBase* routerState;
        IPAddress f_ipAddress;
		Timer* timeoutTimer;
};

CLICK_ENDDECLS
#endif
