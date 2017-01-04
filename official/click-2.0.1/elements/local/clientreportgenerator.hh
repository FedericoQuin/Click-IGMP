#ifndef CLICK_CLIENTREPORTGENERATOR_HH
#define CLICK_CLIENTREPORTGENERATOR_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "clientinfobase.hh"
#include "igmppackets.h"

CLICK_DECLS

/*
=c
ClientReportGenerator()

=s local

Creates a ClientReportGenerator packet.

=d

It pushes an ClientReportGenerator packet out without an IP-header.
It will create an EXLUDE package when the join_group handler is called
It will create an INCLUDE package when the leave_group handler is called

*/


class ClientReportGenerator : public Element {
public:
    ClientReportGenerator();
    ~ClientReportGenerator();

    const char* class_name() const { return "ClientReportGenerator"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    void sendGeneralReport(int maxRespTime);
    void sendGroupSpecificReport(IPAddress ipAddr, int maxRespTime);

    /// --------
    /// HANDLERS
    /// --------

    /**
	 * Join a group
     */
    static int handleJoin(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    
    /**
     * Leave a group
     */
    static int handleLeave(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    void add_handlers();


private:
    /**
     * Makes a packet according to the ip address given as argument (general query with no ip given - interface state change)
     */
    Packet* make_packet(int groupRecordProto = 1, IPAddress changedIP = IPAddress());
    /**
     * Method used to make a combined packet for group specific queries
     */
    Packet* make_packet_combined(int groupRecordProto, Vector<IPAddress> includeAddresses);

    /**
     * Struct used for the timers
     */
    struct TimerReportData {
        ClientReportGenerator* me;
        int submissionsLeft;
        int timeInterval;
        Packet* packetToSend;
        ReportType type;
        IPAddress groupAddr;
    };
    static void handleExpiry(Timer* timer, void* data);
    void expire(TimerReportData* data);


    ClientInfoBase* clientState;
    Vector<Timer*> f_generalTimers;
    HashTable<IPAddress, Timer*> f_groupTimers;
    HashTable<IPAddress, Timer*> f_stateChangeTimers;
};

CLICK_ENDDECLS
#endif