#ifndef CLICK_IGMPQUERY_HH
#define CLICK_IGMPQUERY_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

class IGMPquery : public Element {
public:
    IGMPquery();
    ~IGMPquery();

    const char* class_name() const { return "IGMPquery"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }

    int configure(Vector<String>&, ErrorHandler*);
    void run_timer(Timer*); /// temporary for testing, afterwards with handler

private:
    Packet* make_packet();

    /// temporary testing without class variables and constant values in methods.

};

CLICK_ENDDECLS
#endif
