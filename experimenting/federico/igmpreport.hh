#ifndef CLICK_IGMPREPORT_HH
#define CLICK_IGMPREPORT_HH

#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

//temporarily just a dummy packet creater
class IGMPReport {
public:
    IGMPReport();
    ~IGMPReport();

    const char* class_name() const { return "IGMPReport"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }

    int configure(Vector<String>&, ErrorHandler*);
    void run_timer(Timer*);


private:
    Packet* make_packet();

};





CLICK_ENDDECLS
#endif