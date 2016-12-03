#ifndef CLICK_IGMPREPORTGENERATOR_HH
#define CLICK_IGMPREPORTGENERATOR_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>
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

class IGMPReportGenerator : public Element {
public:
    IGMPReportGenerator();
    ~IGMPReportGenerator();

    const char* class_name() const { return "IGMPReportGenerator"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    // timer still here for debugging reasons
    void run_timer(Timer*);

    void sendGroupSpecificReport(IPAddress ipAddr);
    void sendGeneralReport();

    /// --------
    /// HANDLERS
    /// --------

    /*
	Join a group
    */
    static int handleJoin(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    /*
    Leave a group
    */
    static int handleLeave(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    void add_handlers();


private:
    Packet* make_packet(int groupRecordProto = 1, IPAddress changedIP = IPAddress());
    ClientInfoBase* clientState;

};

CLICK_ENDDECLS
#endif