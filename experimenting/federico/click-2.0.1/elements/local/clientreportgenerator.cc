#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "clientreportgenerator.hh"
#include "igmppackets.h"
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include <time.h>
#include <stdlib.h>
#include <iostream>

CLICK_DECLS

ClientReportGenerator::ClientReportGenerator() : clientState(0) {}
ClientReportGenerator::~ClientReportGenerator() {}

int ClientReportGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
    if (cp_va_kparse(conf, this, errh, "STATE", cpkM+cpkP, cpElementCast,"ClientInfoBase", &clientState, cpEnd) < 0) return -1;
    if (clientState == 0) return errh->error("Wrong element given as argument, should be a ClientInfoBase element.");

    srand(time(NULL));
    return 0;
}


void ClientReportGenerator::sendGeneralReport(int maxRespTime) {
    float rng = (float) rand() / (float) RAND_MAX;
    int sendTime = (int) (rng * maxRespTime);

    /// general report already planned before chosen delay?
    if (f_generalTimers.size() != 0) {
        int timeLeft = (f_generalTimers.at(0)->expiry_steady() - Timestamp::now_steady()).msecval();
        if (timeLeft < sendTime) {
            return;
        }
    }
    /// clear all the general reports and send a new one after the chosen time
    for (int i = 0; i < f_generalTimers.size(); i++) {
        f_generalTimers.at(i)->clear();
    }
    f_generalTimers = Vector<Timer*>();

    TimerReportData* data = new TimerReportData();
    data->me = this;
    data->submissionsLeft = clientState->getQRV();
    data->timeInterval = maxRespTime;
    data->type = General;

    Timer* t = new Timer(&ClientReportGenerator::handleExpiry, data);
    t->initialize(this);
    t->schedule_after_msec(sendTime);
    f_generalTimers.push_back(t);
}


void ClientReportGenerator::sendGroupSpecificReport(IPAddress ipAddr, int maxRespTime) {
    float rng = (float) rand() / (float) RAND_MAX;
    int sendTime = (int) (rng * maxRespTime);
    
    /// general report already planned before chosen delay?
    if (f_generalTimers.size() != 0) {
        int timeLeft = (f_generalTimers.at(0)->expiry_steady() - Timestamp::now_steady()).msecval();
        if (timeLeft < sendTime) {
            return;
        }
    }

    /// Group specific report already scheduled?
    if (f_groupTimers[ipAddr]) {
        Timer* currentTimer = f_groupTimers[ipAddr];
        int timeLeft = (currentTimer->expiry_steady() - Timestamp::now_steady()).msecval();
        if (timeLeft > sendTime) {
            /// reschedule the timer to run out sooner since the chosen time is sooner than the scheduled one
            const Timestamp sooner = Timestamp::now() + Timestamp((double) sendTime / 1000.0);
            currentTimer->reschedule_at(sooner);
        }
        return;
    }


    TimerReportData* data = new TimerReportData();
    data->me = this;
    data->submissionsLeft = clientState->getQRV();
    data->timeInterval = maxRespTime;
    data->type = Group;
    data->groupAddr = ipAddr;

    Timer* t = new Timer(&ClientReportGenerator::handleExpiry, data);
    t->initialize(this);
    t->schedule_after_msec(sendTime);
    f_groupTimers[ipAddr] = t;
}


int ClientReportGenerator::handleJoin(const String& conf, Element* e, void* thunk, ErrorHandler* errh) {
    ClientReportGenerator* thisElement = (ClientReportGenerator*) e;
    IPAddress input_ipaddr;

    if (cp_va_kparse(conf, thisElement, errh, 
        "ADDR", cpkM+cpkP, cpIPAddress, &input_ipaddr,
        cpEnd)
    < 0 ) return -1;

    if (thisElement->clientState->hasAddress(input_ipaddr) == true) {
        return 0;
    }

    if (thisElement->f_stateChangeTimers[input_ipaddr]) {
        thisElement->f_stateChangeTimers[input_ipaddr]->clear();
        thisElement->f_stateChangeTimers[input_ipaddr] = 0;
    }

    /// add the element to the clientState and send an IGMP report to the network
    thisElement->clientState->addAddress(input_ipaddr);
    if (Packet* q = thisElement->make_packet(RECORD_TYPE_IN_TO_EX, input_ipaddr)) {
        int URI = thisElement->clientState->getUnsolicitedReportInterval();

        TimerReportData* data = new TimerReportData();
        data->me = thisElement;
        data->submissionsLeft = thisElement->clientState->getQRV();
        data->timeInterval = URI;
        data->packetToSend = q;
        data->type = StateChange;
        data->groupAddr = input_ipaddr;

        Timer* t = new Timer(&ClientReportGenerator::handleExpiry, data);
        t->initialize(thisElement);
        thisElement->f_stateChangeTimers[input_ipaddr] = t;
        float rng = (float) rand() / (float) RAND_MAX;
        t->schedule_after_msec((int) (rng * URI));
    }

    return 0;
}

int ClientReportGenerator::handleLeave(const String& conf, Element* e, void* thunk, ErrorHandler* errh) {
    ClientReportGenerator* thisElement = (ClientReportGenerator*) e;
    IPAddress input_ipaddr;

    if (cp_va_kparse(conf, thisElement, errh, 
        "ADDR", cpkM+cpkP, cpIPAddress, &input_ipaddr,
        cpEnd)
    < 0 ) return -1;

    if (thisElement->clientState->hasAddress(input_ipaddr) == false) {
        return 0;
    }

    if (thisElement->f_stateChangeTimers[input_ipaddr]) {
        thisElement->f_stateChangeTimers[input_ipaddr]->clear();
        thisElement->f_stateChangeTimers[input_ipaddr] = 0;
    }

    thisElement->clientState->deleteAddress(input_ipaddr);
    if (Packet* q = thisElement->make_packet(RECORD_TYPE_EX_TO_IN, input_ipaddr)) {
        int URI = thisElement->clientState->getUnsolicitedReportInterval();

        TimerReportData* data = new TimerReportData();
        data->me = thisElement;
        data->submissionsLeft = thisElement->clientState->getQRV();
        data->timeInterval = URI;
        data->packetToSend = q;
        data->type = StateChange;
        data->groupAddr = input_ipaddr;
        Timer* t = new Timer(&ClientReportGenerator::handleExpiry, data);
        t->initialize(thisElement);
        thisElement->f_stateChangeTimers[input_ipaddr] = t;
        float rng = (float) rand() / (float) RAND_MAX;
        t->schedule_after_msec((int) (rng * URI));
    }

    return 0;
}

void ClientReportGenerator::add_handlers() {
    add_write_handler("join_group", &handleJoin, (void*)0);
    add_write_handler("leave_group", &handleLeave, (void*)0);
}


Packet* ClientReportGenerator::make_packet(int groupRecordProto, IPAddress changedIP) {

    Vector<IPAddress> listenAddresses = clientState->getAllAddresses();

    bool isGeneralReport = false;
    if (changedIP.empty() == true)
        isGeneralReport = true;

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

    igmph->Type = IGMP_REPORT_TYPE;
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

Packet* ClientReportGenerator::make_packet_combined(int groupRecordProto, Vector<IPAddress> includeAddresses) {
    int amtGroupRecords = includeAddresses.size();

    int sizeIGMPheader = sizeof(struct IGMP_report) + amtGroupRecords * sizeof(struct IGMP_grouprecord);
    int headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket* q = Packet::make(headroom, 0, sizeIGMPheader, 0);

    if (!q)
        return 0;

    memset(q->data(), '\0', sizeIGMPheader);
    IGMP_report* igmph = (IGMP_report*)(q->data());

    igmph->Type = IGMP_REPORT_TYPE;
    igmph->Reserved1 = 0;
    igmph->Reserved2 = 0;
    igmph->Number_of_Group_Records = htons(amtGroupRecords);

    IGMP_grouprecord* record = (IGMP_grouprecord*)(igmph + 1);

    for (int i = 0; i < amtGroupRecords; i++) {
        if (clientState->hasAddress(includeAddresses.at(i)) == false) {
            continue;
        }
        record->Record_Type = groupRecordProto; // uses the type specified in the argument 
        record->Aux_Data_Len = 0; // not used in IGMPv3
        record->Number_of_Sources = 0; // does not need to be implemented in our version
        record->Multicast_Address = includeAddresses.at(i);

        record = record + 1;
    }

    igmph->Checksum = click_in_cksum( (const unsigned char*)igmph, sizeIGMPheader );
    return q;
}


void ClientReportGenerator::handleExpiry(Timer* timer, void* data) {
    TimerReportData* reportData = (TimerReportData*) data;
    assert(reportData);

    Packet* p = 0;

    ReportType type = reportData->type;
    if (type == General) {
        p = reportData->me->make_packet(RECORD_TYPE_MODE_EX);
    } else if (type == Group) {
        bool isListenedTo = reportData->me->clientState->hasAddress(reportData->groupAddr);
        p = reportData->me->make_packet((isListenedTo == true ? RECORD_TYPE_MODE_EX : RECORD_TYPE_MODE_IN), reportData->groupAddr);
    } else if (type == StateChange) {
        p = reportData->packetToSend->clone()->uniqueify();
    }

    reportData->me->output(0).push(p);

    reportData->submissionsLeft = reportData->submissionsLeft - 1;
    if (reportData->submissionsLeft != 0) {
        float rng = (float) rand() / (float) RAND_MAX;
        timer->reschedule_after_msec((int) (rng * reportData->timeInterval));
    } else {
        reportData->me->expire(reportData);
    }
}

void ClientReportGenerator::expire(TimerReportData* data) {
    switch (data->type) {
        case (General): 
            if (f_generalTimers.empty() == false) {
                f_generalTimers.erase(f_generalTimers.begin());
            }
            break;
        case (Group):
            f_groupTimers[data->groupAddr] = 0;
            break;
        case (StateChange):
            f_stateChangeTimers[data->groupAddr] = 0;
            break;
    }
}



CLICK_ENDDECLS
EXPORT_ELEMENT(ClientReportGenerator)