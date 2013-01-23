//Cabeçalho RFID

//#ifndef ns_rfidPacket_h
//#define ns_rfidPacket_h

#include "agent.h"
#include "tclcl.h"
#include "packet.h"
#include "address.h"
#include "ip.h"

struct hdr_rfidPacket {
	int id_; //Do leitor
	int tagEPC_; //Da tag
	int tipo_; //0 para tag-leitor e 1 para leitor-tag
	int singularization_; // 0 - sem 1 - Slotted Aloha
	int service_; // Tipo de serviço - Para QoS
	int ack_;
	// Header access methods
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() { return offset_; }
	inline static hdr_rfidPacket* access(const Packet* p) {
		return (hdr_rfidPacket*) p->access(offset_);
	}
};

//#endif // ns_rfidPacket_h
