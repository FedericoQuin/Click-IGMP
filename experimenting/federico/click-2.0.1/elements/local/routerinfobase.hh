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
#include "igmpquerygenerator.hh"

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
		/*
		Constructor
		*/
		~RouterInfoBase();
		/*
		Destructor
		*/
		
		const char *class_name() const	{ return "RouterInfoBase"; }
		int configure(Vector<String>&, ErrorHandler*);
		void addIPToGroup(IPAddress,uint8_t);
		/*
		Adds the second IPAddress to the group with the first IPAddress
		*/
		void deleteIPFromGroup(IPAddress,uint8_t);
		/*
		Deletes the second IPAddress from the group with the first IPAddress
		*/
		Vector<IPAddress> getGroups(uint8_t);
		/*
		Returns a vector with the IPAddresses from those who are joined to the group with the input IPAddress 
		*/
		Vector<IPAddress> getAllGroups();
		/*
		Returns a vector with the IPAddresses which are being listened to on at least one interface
		*/
		bool hasAddress(IPAddress);
		/*
		Checks if the router has any interface listening to the given IPAddress
		*/
		uint8_t getQRV();
		/*
		Get the QRV value. Default value is 2.
		*/
		void setQRV(uint8_t);
		/*
		Sets the QRV value to the given value. if it is bigger than 7, 0 will be used
		*/
		void sendQuery(IPAddress,unsigned int);
		/// --------
    	/// HANDLERS
    	/// --------
		static int handleSetQRV(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		/*
		Handler to set the QRV value to the given value. if it is bigger than 7, 0 will be used
		*/
		static int handleSetInterval(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		/*
		Handler to set the interval in which a query will be sent
		*/
		static int handleSetWait(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		/*
		Handler to set the time to wait before the actual delete
		*/

		void add_handlers();
		

	private:
		static void sendQuery(Timer*,void*);
		static void deleteInterfaceFromGroup(Timer*,void*);

		IGMPQueryGenerator *_querier;
		HashTable<uint8_t,HashTable<IPAddress, Timer*> > deletetimers;
		Timer * queryTimer;
		HashTable<uint8_t,Vector<IPAddress> > table;
		uint8_t QRV;
		unsigned int QQIT;
		unsigned int MRT;
		
};

bool vector_contains(const Vector<IPAddress>& v, const IPAddress& target);

CLICK_ENDDECLS
#endif
