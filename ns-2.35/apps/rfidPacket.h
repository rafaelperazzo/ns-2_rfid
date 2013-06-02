//Cabeçalho RFID

//#ifndef ns_rfidPacket_h
//#define ns_rfidPacket_h

#include "agent.h"
#include "tclcl.h"
#include "packet.h"
#include "address.h"
#include "ip.h"

struct hdr_rfidPacket {
	int id_; //reader id
	int tagEPC_; //tag EPC
	int tipo_; //0 -  tag-reader,  1 reader-tag e 2 ACK reader-tag
	int singularization_; // 0 DEPRECATED
	int service_; /** Tipo de serviço
	0 REPLY ALL READER QUERIES
	1 REPLY JUST DIFFERENT READER QUERIES	*/
	int ack_;
	float rng16_; //random number generated by tag
	int qValue_; //Q value from Q algorithm
	int command_; //Command type: QUERY,QUERYADJUST,...
	int slotCounter_;
	int colCounter_; //Collision slot counter
	int sucCounter_; //Success slot counter
	int idlCounter_; //Idle slot counter
	int slotNumber_;
	int reply_;
	int session_;
	int trace_; //Include debug traces
	int mechanism_; //QoS Mechanism: 0 off - 1 on
	// Header access methods
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() { return offset_; }
	inline static hdr_rfidPacket* access(const Packet* p) {
		return (hdr_rfidPacket*) p->access(offset_);
	}
};

//#endif // ns_rfidPacket_h
