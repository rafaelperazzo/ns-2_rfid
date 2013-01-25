/*
 * rfidTag.cc
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
 * File: Code for a new 'RfidTag' Agent Class for the ns
 *       network simulator
 * Author: Rafael Perazzo Barbosa Mota (perazzo@ime.usp.br), Setembro 2012
 *
 */

#include "rfidTag.h"
#include "rfidPacket.h"
#include <stdlib.h>
#include <time.h>
#include <random.h>

static class RfidTagClass : public TclClass {
public:
	RfidTagClass() : TclClass("Agent/RfidTag") {}
	TclObject* create(int, const char*const*) {
		return (new RfidTagAgent());
	}
} class_rfidTag;

RfidTagAgent::RfidTagAgent() : Agent(PT_RFIDPACKET), tagEPC_(0), id_(0)
{
	bind("packetSize_", &size_);
	bind("id_",&id_);
	bind("tagEPC_",&tagEPC_);
	bind("service_",&service_);
	bind("kill_",&kill_);
	bind("time_",&time_);
	bind("slot_",&slot_);
	bind("memory_",&memory_);
	bind("rng16_",&rng16_);
}

int RfidTagAgent::command(int argc, const char*const* argv)
{
  //The tag do not start communication.
  // If the command hasn't been processed by RfidReaderAgent()::command,
  // call the command() function for the base class
  //A tag nunca inicia a comunicação - Tag passiva
   return (Agent::command(argc, argv));
}


float RfidTagAgent::RandomFloat(float min, float max)
{
    // this  function assumes max > min, you may want 
    // more robust error checking for a non-debug build
    assert(max > min); 
    float random = ((float) rand()) / (float) RAND_MAX;

    // generate (in your case) a float between 0 and (4.5-.78)
    // then add .78, giving you a float between .78 and 4.5
    float range = max - min;  
    return (random*range) + min;
}


void RfidTagAgent::recv(Packet* pkt, Handler*)
{
  // Lê o conteúdo do cabeçalho IP
  hdr_ip* hdrip = hdr_ip::access(pkt);
  // Lê o conteúdo do cabeçalho do pacote RFID
  hdr_rfidPacket* hdr = hdr_rfidPacket::access(pkt);
  if (hdr->tipo_==FLOW_RT) { //Responde apenas a requisições de leitores
	  //printf("Tag (%d) recebeu REQUISICAO do leitor (%d)\n",tagEPC_, hdr->id_);
  	  //criar pacote de resposta
  	  Packet* pktret = allocpkt();
  	  hdr_rfidPacket* rfidHeader = hdr_rfidPacket::access(pktret);
  	  hdr_ip* ipHeader = hdr_ip::access(pktret);
  	  rfidHeader->tagEPC_ = tagEPC_;
	  rfidHeader->id_ = hdr->id_;
  	  rfidHeader->tipo_ = FLOW_TR;
	  rfidHeader->service_=hdr->service_;
	  ipHeader->daddr() = hdrip->saddr();
  	  ipHeader->dport() = hdrip->sport();
	  if (hdr->service_==SERVICE_TRACKING) {
	  	rfidHeader->ack_=1;
		if (hdr->singularization_==SING_NOSINGULARIZATION) { //Se não for solicitada singularizaçao
                	if (hdr->id_!=id_) { 
                        	send(pktret,0);
                	}
          	}
          	else { //caso seja solicitada singularização
                	if (hdr->id_!=id_) {
                        	double tempo = Random::uniform(0,time_);
				//float tempo = RandomFloat(0,0.4);
				//printf("Tempo: %f\n",tempo);
                        	Scheduler& sch = Scheduler::instance();
                        	sch.schedule(target_,pktret,tempo);
                	}
          	}
	  }
	  else if (hdr->service_==SERVICE_STANDARD) {
		//criar pacote de resposta
	        Packet* pktret = allocpkt();
          	hdr_rfidPacket* rfidHeader = hdr_rfidPacket::access(pktret);
          	hdr_ip* ipHeader = hdr_ip::access(pktret);
          	rfidHeader->tagEPC_ = tagEPC_;
          	rfidHeader->id_ = hdr->id_;
          	rfidHeader->tipo_ = FLOW_TR;
          	ipHeader->daddr() = hdrip->saddr();
          	ipHeader->dport() = hdrip->sport();
		send(pktret,0);
	  }
  }
  else if (hdr->tipo_==FLOW_RT_ACK) { //Tag recebe um ACK
	id_=hdr->id_; //Grava o ID do leitor que confirmou o recebimento
	//printf("[TAG]Recebido ACK(%d)\n",hdr->tagEPC_);
  }
  else { //Descarta o pacote caso a origem não tenha sido um leitor
	Packet::free(pkt);
  }
  Packet::free(pkt);
  return;
}
