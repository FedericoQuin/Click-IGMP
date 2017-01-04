#ifndef CLICK_CLIENTINFOBASE_HH
#define CLICK_CLIENTINFOBASE_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/vector.hh>

CLICK_DECLS

class ClientInfoBase : public Element { 
	public:
		ClientInfoBase();
		~ClientInfoBase();

		const char *class_name() const	{ return "ClientInfoBase"; }
		int configure(Vector<String>&, ErrorHandler*);
		void* cast(const char* n);
		
		void addAddress(IPAddress);
		void deleteAddress(IPAddress);
		bool hasAddress(IPAddress) const;
		Vector<IPAddress> getAllAddresses() const;
		void setQRV(const int qrv);
		int getQRV() const;
		int getUnsolicitedReportInterval() const;
		void setUnsolicitedReportInterval(int value);
		
		/** 
		 * Sets the Unsolicited report interval with a handler, expressed in amount of milliseconds
		 */
		static int handleSetUnsolicited(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
		void add_handlers();


	private:
		int f_qrv; /// The Querier Robustness Variable
		int f_unsolRepInterval; /// The Unsolicited Report Interval, expressed in milliseconds
		Vector<IPAddress> f_listenAddresses;
		
};

CLICK_ENDDECLS
#endif
