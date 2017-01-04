#ifndef CLICK_IGMPPACKETS_H
#define CLICK_IGMPPACKETS_H

#define IGMP_QUERY_TYPE 0x11
/*
The IGMP_query header
*/
struct IGMP_query {
    uint8_t Type;
    uint8_t Max_Resp_Code;
    uint16_t Checksum;
    uint32_t Group_Address;

    uint8_t QRV : 3;
    uint8_t S : 1;
    uint8_t Resv : 4;

    uint8_t QQIC;
    uint16_t Number_of_Sources;
};

#define IGMP_REPORT_TYPE 0x22
/*
The IGMP_report header
*/
struct IGMP_report {
    uint8_t Type;
    uint8_t Reserved1;
    uint16_t Checksum;
    uint16_t Reserved2;
    uint16_t Number_of_Group_Records;
};

#define RECORD_TYPE_MODE_IN 1
#define RECORD_TYPE_MODE_EX 2
#define RECORD_TYPE_EX_TO_IN 3
#define RECORD_TYPE_IN_TO_EX 4

/*
The IGMP_grouprecord
*/
struct IGMP_grouprecord {
    uint8_t Record_Type;
    uint8_t Aux_Data_Len;
    uint16_t Number_of_Sources;
    uint32_t Multicast_Address;
};

/*
Enum used in report generators
*/
enum ReportType {General, Group, StateChange};

#endif