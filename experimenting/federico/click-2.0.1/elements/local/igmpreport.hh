#ifndef CLICK_IGMPREPORT_HH
#define CLICK_IGMPREPORT_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

/*
=c
IGMPReport()

=s local

Creates a IGMPReport packet.

=d

It pushes an IGMPReport packet out without an IP-header.
It will create an empty EXLUDE package when the join_group handler is called
It will create an empty INCLUDE package when the leave_group handler is called

*/

class IGMPReport : public Element {
public:
    IGMPReport();
    ~IGMPReport();

    const char* class_name() const { return "IGMPReport"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    // timer still here for debugging reasons
    void run_timer(Timer*);

    /// --------
    /// HANDLERS
    /// --------
    static int handleJoin(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    /*
	Join a group
    */
    static int handleLeave(const String& conf, Element* e, void* thunk, ErrorHandler* errh);
    /*
    Leave a group
    */
    void add_handlers();


private:
    Packet* make_packet(int groupRecordProto = 1, IPAddress changedIP = IPAddress());
    Vector<IPAddress> f_listenAddresses;

};

CLICK_ENDDECLS
#endif