#ifndef CLICK_ROUTERQUERYHANDLER_HH
#define CLICK_ROUTERQUERYHANDLER_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "routerreportgenerator.hh"

CLICK_DECLS


/**
 * Class that sorts IGMP packet by their type: query or report
 * 
 * Query packets are pushed on output 0.
 * Report packets are pushed on output 1.
 */

class RouterQueryHandler : public Element { 
	public:
		RouterQueryHandler();
		~RouterQueryHandler();
		
		const char *class_name() const	{ return "RouterQueryHandler"; }
		const char *port_count() const	{ return "1/0"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		void push(int, Packet*);

    private:
        void handleQuery(Packet* p);

        Vector<RouterReportGenerator> f_generators;
};

CLICK_ENDDECLS
#endif
