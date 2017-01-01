#ifndef CLICK_IGMPREPORTGENERATOR_HH
#define CLICK_IGMPREPORTGENERATOR_HH
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

CLICK_DECLS

/*
=c
IGMPReportGenerator()

=s local

Creates a IGMPReportGenerator packet.

=d

It pushes an IGMPReportGenerator packet out without an IP-header.
It will create an empty EXLUDE package when the join_group handler is called
It will create an empty INCLUDE package when the leave_group handler is called

*/

enum ReportType {General, Group, StateChange};

class IGMPReportGenerator : public Element {
public:
    IGMPReportGenerator();
    ~IGMPReportGenerator();

    const char* class_name() const { return "IGMPReportGenerator"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    void sendGroupSpecificReport(IPAddress ipAddr, int maxRespTime);
    void sendGeneralReport(int maxRespTime);

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
        IGMPReportGenerator* me;
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