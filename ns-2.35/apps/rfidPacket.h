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
	int tipo_; //0 para tag-leitor,  1 para leitor-tag e 2 para ACK leitor-tag
	int singularization_; // 0 - Responde imediatamente 1 - Responde após esperar tempo aleatório
	int service_; /** Tipo de serviço
	0 para responder todas as requisições do leitor
	1 para responder requisições de leitores diferentes	*/
	int ack_;
	float rng16_;
	int qValue_;
	int command_;
	// Header access methods
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() { return offset_; }
	inline static hdr_rfidPacket* access(const Packet* p) {
		return (hdr_rfidPacket*) p->access(offset_);
	}
};

//#endif // ns_rfidPacket_h
