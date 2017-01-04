#ifndef CLICK_ROUTERREPORTGENERATOR_HH
#define CLICK_ROUTERREPORTGENERATOR_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <click/ipaddress.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "routerinfobase.hh"
#include "igmppackets.h"

CLICK_DECLS


class RouterReportGenerator : public Element {
public:
    RouterReportGenerator();
    ~RouterReportGenerator();

    const char* class_name() const { return "RouterReportGenerator"; }
    const char* port_count() const { return "0/1"; }
    const char* processing() const { return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    void sendGeneralReport(int maxRespTime, uint8_t interface);
    void sendGroupSpecificReport(IPAddress ipAddr, int maxRespTime, uint8_t interface);


private:
    /**
     * Makes a packet according to the ip address given as argument (general query with no ip given - interface state change)
     */
    Packet* make_packet(uint8_t interface, int groupRecordProto = 1, IPAddress changedIP = IPAddress());

    /**
     * Struct used for the timers
     */
    struct TimerReportData {
        RouterReportGenerator* me;
        int submissionsLeft;
        int timeInterval;
        ReportType type;
        IPAddress groupAddr;
        uint8_t interface;
    };
    static void handleExpiry(Timer* timer, void* data);
    void expire(TimerReportData* data);


    RouterInfoBase* routerState;
    Vector<Timer*> f_generalTimers;
    HashTable<IPAddress, Timer*> f_groupTimers;
};

CLICK_ENDDECLS
#endif