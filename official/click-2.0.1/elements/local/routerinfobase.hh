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
		adds IP to Group, also an answer to queries
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
		void suppressQueries();
		/*
		Suppresses the sending of queries, as a result of querier election.
		*/
		void allowQueries();
		/*
		Allows the sending of queries, after querier election
		*/
		void setQQIT(unsigned int);
		void setMRT(unsigned int);
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
		static int handleSetQI(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		/*
		Handler to set the interval in which a general query will be sent
		*/
		static int handleSetQRI(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		/*
		Handler to set the time to wait before the actual delete
		*/
		static int handleSetLMQI(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		/*
		Handler to set the amount of specific queriers  to send
		*/
		static int handleSetLMQC(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		/*
		Handler to set the amount of between 2 specific queries
		*/


		void add_handlers();
		

	private:
		struct SpecificDeleteData{
			RouterInfoBase* element;
			uint8_t interface;
			IPAddress IP;
			unsigned int toSend;
		};
		static void sendQuery(Timer*,void*);
		static void sendQuerySpecific(Timer*,void*);
		static void deleteInterfacesFromGroupGeneral(Timer*,void*);
		static void deleteInterfaceFromGroupSpecific(Timer*,void*);
		static void sendStartUp(Timer*,void*);
		static void sendSpecificQuery(Timer*,void*);
		IGMPQueryGenerator *_querier;
		HashTable<uint8_t,HashTable<IPAddress, bool> > maydeletegeneral;
		HashTable<uint8_t,HashTable<IPAddress, bool> > maydeletespecific;
		HashTable<uint8_t,HashTable<IPAddress, Timer*> > deleteTimers;
		Timer * queryTimer;
		HashTable<uint8_t,Vector<IPAddress> > table;
		uint8_t QRV;
		unsigned int QI;
		unsigned int QRI;
		unsigned int GMI;
		unsigned int SQI;
		unsigned int SQC;
		unsigned int LMQI;
		unsigned int LMQC;
		unsigned int LMQT;
		unsigned int queriesToSend;
		bool queriesSuppressed;
};

bool vector_contains(const Vector<IPAddress>& v, const IPAddress& target);

unsigned int toTime(uint8_t);


CLICK_ENDDECLS
#endif
