#ifndef CLICK_IGMPQUERY_HH
#define CLICK_IGMPQUERY_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/ipaddress.hh>

CLICK_DECLS


class IGMPQuery : public Element {
public:
	IGMPQuery();
	~IGMPQuery();

	const char* class_name() const { return "IGMPQuery"; }
	const char* port_count() const { return "0/1"; }
	const char* processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void run_timer(Timer*);

private:
	Packet* make_packet();
	unsigned int f_mrc;
	unsigned int f_qrv;
	unsigned int f_qqiv;
};


struct IGMP_query {
	uint8_t Type;
	uint8_t Max_Resp_Code;
	uint16_t Checksum;
	uint32_t Group_Address;

	uint8_t Resv : 4;
	uint8_t S : 1;
	uint8_t QRV : 3;

	uint8_t QQIC;
	uint16_t Number_of_Sources;
} CLICK_SIZE_PACKED_ATTRIBUTE;


CLICK_ENDDECLS
#endif