#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "igmpreportgenerator.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <time.h>
#include <stdlib.h>
#include <iostream>

CLICK_DECLS

IGMPReportGenerator::IGMPReportGenerator() : clientState(0) {}
IGMPReportGenerator::~IGMPReportGenerator() {}

int IGMPReportGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
    if (cp_va_kparse(conf, this, errh, "STATE", cpkM+cpkP, cpElement, &clientState, cpEnd) < 0) return -1;
    if (clientState == 0) return errh->error("Wrong element given as argument, should be a ClientInfoBase element.");


    Timer* timer = new Timer(this);
    timer->initialize(this);
    timer->schedule_after_msec(1000);

    return 0;
}

void IGMPReportGenerator::run_timer(Timer* timer) {
    return; 

    // left the timer here for later potential debugging reasons
    static int temp = 0;
    int new_addr = 16909056+temp++;
    // f_listenAddresses.push_back(IPAddress(htonl(new_addr)));
    // if (Packet*q = this->make_packet(3, f_listenAddresses.back())) {
    if (Packet*q = this->make_packet(RECORD_TYPE_MODE_EX)) {
        output(0).push(q);
        timer->schedule_after_msec(1000);
    }
}

void IGMPReportGenerator::sendGroupSpecificReport(IPAddress ipAddr) {
    bool isListenedTo = clientState->hasAddress(ipAddr);

    if (Packet* q = this->make_packet((isListenedTo == true ? RECORD_TYPE_MODE_EX : RECORD_TYPE_MODE_IN), ipAddr)) {
        output(0).push(q);
    }
}

void IGMPReportGenerator::sendGeneralReport() {
    if (Packet* q = this->make_packet(RECORD_TYPE_MODE_EX)) {
        output(0).push(q);
    }
}


int IGMPReportGenerator::handleJoin(const String& conf, Element* e, void* thunk, ErrorHandler* errh) {
    IGMPReportGenerator* thisElement = (IGMPReportGenerator*) e;
    IPAddress input_ipaddr;

    if (cp_va_kparse(conf, thisElement, errh, 
        "ADDR", cpkM+cpkP, cpIPAddress, &input_ipaddr,
        cpEnd)
    < 0 ) return -1;

    if (thisElement->clientState->hasAddress(input_ipaddr) == true) {
        return 0;
    }

    /// add the element to the clientState and sent an IGMP report to the network
    thisElement->clientState->addAddress(input_ipaddr);
    if (Packet* q = thisElement->make_packet(RECORD_TYPE_IN_TO_EX, input_ipaddr)) {
        thisElement->output(0).push(q);
    }

    return 0;
}

int IGMPReportGenerator::handleLeave(const String& conf, Element* e, void* thunk, ErrorHandler* errh) {
    IGMPReportGenerator* thisElement = (IGMPReportGenerator*) e;
    IPAddress input_ipaddr;

    if (cp_va_kparse(conf, thisElement, errh, 
        "ADDR", cpkM+cpkP, cpIPAddress, &input_ipaddr,
        cpEnd)
    < 0 ) return -1;

    if (thisElement->clientState->hasAddress(input_ipaddr) == false) {
        return 0;
    }

    thisElement->clientState->deleteAddress(input_ipaddr);
    if (Packet* q = thisElement->make_packet(RECORD_TYPE_EX_TO_IN, input_ipaddr)) {
        thisElement->output(0).push(q);
    }

    return 0;
}

void IGMPReportGenerator::add_handlers() {
    add_write_handler("join_group", &handleJoin, (void*)0);
    add_write_handler("leave_group", &handleLeave, (void*)0);
}


Packet* IGMPReportGenerator::make_packet(int groupRecordProto, IPAddress changedIP) {

    Vector<IPAddress> listenAddresses = clientState->getAllAddresses();

    bool isGeneralReport = false;
    if (changedIP.empty() == true)
        isGeneralReport = true;

    // click_chatter(isGeneralReport == true ? "general" : "not general");

    int amtGroupRecords = 0;

    if (isGeneralReport == true)
        amtGroupRecords = listenAddresses.size();
    else
        amtGroupRecords = 1;

    // is dependant on the amount of multicast addresses being listened to
    int sizeIGMPheader = sizeof(struct IGMP_report) + amtGroupRecords * sizeof(struct IGMP_grouprecord);
    int headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket* q = Packet::make(headroom, 0, sizeIGMPheader, 0);

    if (!q)
        return 0; //in case there was no memory (or other reasons) for the packet not being able to be created;

    memset(q->data(), '\0', sizeIGMPheader);

    IGMP_report* igmph = (IGMP_report*)(q->data());

    igmph->Type = 0x22;
    igmph->Reserved1 = 0;
    igmph->Reserved2 = 0;
    igmph->Number_of_Group_Records = htons(amtGroupRecords);

    IGMP_grouprecord* record = (IGMP_grouprecord*)(igmph + 1);

    if (isGeneralReport == true) {
        for (int i = 0; i < amtGroupRecords; i++) {
            record->Record_Type = groupRecordProto; // uses the type specified in the argument 
            record->Aux_Data_Len = 0; // not used in IGMPv3
            record->Number_of_Sources = 0; // does not need to be implemented in our version
            record->Multicast_Address = listenAddresses.at(i); // multicast_address i

            record = record + 1;
        }
    }
    else {
        record->Record_Type = groupRecordProto;
        record->Aux_Data_Len = 0;
        record->Number_of_Sources = 0;
        record->Multicast_Address = changedIP;
    }

    igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeIGMPheader );
    return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPReportGenerator)