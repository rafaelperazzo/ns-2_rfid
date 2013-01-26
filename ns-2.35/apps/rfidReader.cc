
/*
 * rfidReader.cc
 * Copyright (C) 2000 by the University of Southern California
 * $Id: ping.cc,v 1.8 2005/08/25 18:58:01 johnh Exp $
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * The copyright of this module includes the following
 * linking-with-specific-other-licenses addition:
 *
 * In addition, as a special exception, the copyright holders of
 * this module give you permission to combine (via static or
 * dynamic linking) this module with free software programs or
 * libraries that are released under the GNU LGPL and with code
 * included in the standard release of ns-2 under the Apache 2.0
 * license or under otherwise-compatible licenses with advertising
 * requirements (or modified versions of such code, with unchanged
 * license).  You may copy and distribute such a system following the
 * terms of the GNU GPL for this module and the licenses of the
 * other code concerned, provided that you include the source code of
 * that other code when and as the GNU GPL requires distribution of
 * source code.
 *
 * Note that people who make modified versions of this module
 * are not obligated to grant this special exception for their
 * modified versions; it is their choice whether to do so.  The GNU
 * General Public License gives permission to release a modified
 * version without this exception; this exception also makes it
 * possible to release a modified version which carries forward this
 * exception.
 *
 */


/*
 * File: Code for a new 'RfidReader' Agent Class for the ns
 *       network simulator
 * Author: Rafael Perazzo Barbosa Mota (perazzo@ime.usp.br), Setembro 2012
 *
 */

#include "rfidReader.h"
#include "rfidPacket.h"
#include <time.h>
static class RfidReaderClass : public TclClass {
public:
	RfidReaderClass() : TclClass("Agent/RfidReader") {}
	TclObject* create(int, const char*const*) {
		return (new RfidReaderAgent());
	}
} class_rfidReader;


RfidReaderAgent::RfidReaderAgent() : Agent(PT_RFIDPACKET), rs_timer_(this)
{
	bind("packetSize_", &size_);
	bind("tagEPC_",&tagEPC_);
	bind("id_",&id_);
	bind("singularization_",&singularization_);
	bind("service_",&service_);
	bind("memory_",&memory_);
	bind("qValue_",&qValue_);
}

int RfidReaderAgent::command(int argc, const char*const* argv)
{
  if (argc == 2) {
    if (strcmp(argv[1], "query-tags") == 0) {
      //Scheduler& sch = Scheduler::instance();
      //printf("%f\n",sch.clock());
      //Criando Pacote
      /*Packet* pkt = allocpkt(); 
      //Criando cabeçado da camada de Rede
      hdr_ip* iph = HDR_IP(pkt);
      //Criando cabeçalho RFID
      hdr_rfidPacket *ph = hdr_rfidPacket::access(pkt);
      //Preparando cabeçalho
      ph->id_ = id_; //ID do leitor
      ph->tipo_ = FLOW_RT; //Tipo de fluxo: LEITOR-TAG
      ph->singularization_ = singularization_; //Responde ou não imediatamente
      //if (service_==1) { ph->service_=1; } //Tipo de serviço: Responde apenas requisições de leitores diferentes
      ph->service_=service_;
      iph->daddr() = IP_BROADCAST; //Destino: broadcast
      iph->dport() = iph->sport();
      //printf("Antes de enviar pacote de requisição...\n");
      send(pkt, (Handler*) 0);*/
      //printf("Requisição enviada com sucesso...\n");
      //rs_timer_.resched(3);
      resend();
      return (TCL_OK);
    }
    else if (strcmp(argv[1], "standard-query-tags") == 0) {
	/*Packet* pkt = allocpkt(); 
	//Create network header
	hdr_ip* ipHeader = HDR_IP(pkt);
	//Create RFID header
	hdr_rfidPacket *rfidHeader = hdr_rfidPacket::access(pkt);
	//Prepating headers
      	rfidHeader->id_ = id_; //Reader ID
      	rfidHeader->tipo_ = FLOW_RT; //flow direction
      	rfidHeader->singularization_ = singularization_; //imediatly reply or random time reply
      	rfidHeader->service_=service_;
      	ipHeader->daddr() = IP_BROADCAST; //Destination: broadcast
      	ipHeader->dport() = ipHeader->sport();
      	//Sends the packet
	send(pkt, (Handler*) 0);*/
	resend();
      	return (TCL_OK);
    }
  }

  // If the command hasn't been processed by RfidReaderAgent()::command,
  // call the command() function for the base class
  return (Agent::command(argc, argv));
}


void RfidReaderAgent::recv(Packet* pkt, Handler*)
{
  // Access the IP header for the received packet:
  hdr_ip* hdrip = hdr_ip::access(pkt);
  // Access the RfidReader header for the received packet:
  hdr_rfidPacket* hdr = hdr_rfidPacket::access(pkt);
  //printf("Tipo: %d - Id: %d - Service: %d\n",hdr->tipo_,hdr->id_,hdr->service_);
  if ((hdr->tipo_==FLOW_TR)&&(hdr->id_==id_)&&(hdr->service_==SERVICE_TRACKING)) { //Se o pacote é do tipo TAG-LEITOR e for endereçado a este leitor
  	//printf("%i\n",hdr->tagEPC_);
	if (hdr->ack_==1) {
		//Enviar ACK de confirmação
		Packet* pktret = allocpkt();
	        hdr_rfidPacket* rfidHeader = hdr_rfidPacket::access(pktret);
        	hdr_ip* ipHeader = hdr_ip::access(pktret);
	        rfidHeader->tagEPC_ = hdr->tagEPC_;
        	rfidHeader->id_ = hdr->id_;
	        rfidHeader->tipo_ = FLOW_RT_ACK;
		rfidHeader->service_=service_;
		rfidHeader->singularization_=singularization_;
        	//hdrIp->daddr() = IP_BROADCAST;
	        ipHeader->daddr() = hdrip->saddr();
        	ipHeader->dport() = hdrip->sport();
		send(pktret,0);
		//printf("Enviou Ack de confirmacao\n");
	}
  }
  else if ((hdr->tipo_==FLOW_TR)&&(hdr->id_==id_)&&(hdr->service_==SERVICE_STANDARD)) {
	drop(pkt,"NEG");
  }
  else if(hdr->tipo_==FLOW_RT){
	printf("Leitor (NÃO IDENTIFICADO) recebeu RESPOSTA de (%i)\n",hdr->tagEPC_);
  }
  Packet::free(pkt);
  return;
}
void RfidReaderAgent::resend() {
	Packet* pkt = allocpkt(); 
        //Create network header
        hdr_ip* ipHeader = HDR_IP(pkt);
        //Create RFID header
        hdr_rfidPacket *rfidHeader = hdr_rfidPacket::access(pkt);
        //Prepating headers
        rfidHeader->id_ = id_; //Reader ID
        rfidHeader->tipo_ = FLOW_RT; //flow direction
        rfidHeader->singularization_ = singularization_; //imediatly reply or random time reply
        rfidHeader->service_=service_;
        ipHeader->daddr() = IP_BROADCAST; //Destination: broadcast
        ipHeader->dport() = ipHeader->sport();
        ipHeader->saddr() = here_.addr_; //Destination: broadcast
        ipHeader->sport() = here_.port_;
	//Sends the packet
        send(pkt, (Handler*) 0);
}

void RetransmitTimer::expire(Event *e) {
    a_->resend();
}
