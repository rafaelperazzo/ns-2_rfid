/*
 * rfidTag.cc
 * Copyright (C) 2000 by the University of Southern California
 * $Id: rfidTag.cc,v 1.8 2005/08/25 18:58:01 johnh Exp $
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
#include <math.h>

static class RfidTagClass : public TclClass {
public:
	RfidTagClass() : TclClass("Agent/RfidTag") {}
	TclObject* create(int, const char*const*) {
		return (new RfidTagAgent());
	}
} class_rfidTag;

RfidTagAgent::RfidTagAgent() : Agent(PT_RFIDPACKET), id_(0), tagEPC_(0), slot_(0), rng16_(0),state_(0)
{
	bind("packetSize_", &size_);
	bind("id_",&id_);
	bind("tagEPC_",&tagEPC_);
	bind("service_",&service_);
	bind("kill_",&kill_);
	bind("time_",&time_);
	bind("memory_",&memory_);
}

int RfidTagAgent::command(int argc, const char*const* argv)
{
  //The tag do not start communication.
  // If the command hasn't been processed by RfidReaderAgent()::command,
  // call the command() function for the base class
  //A tag nunca inicia a comunicação - Tag passiva
   return (Agent::command(argc, argv));
}


int RfidTagAgent::RandomInt(int min, int max) {
	unsigned first = time(0);
	srand(first);
	int range;
	range=max-min+1;
	int r=rand()/100%range+min;
	return(r);

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


//
// Generate a random number between 0 and 1
// return a uniform number in [0,1].
double RfidTagAgent::unifRand()
{
    return (rand() / double(RAND_MAX));
}
//
// Generate a random number in a real interval.
// param a one end point of the interval
// param b the other end of the interval
// return a inform rand numberin [a,b].
double RfidTagAgent::unifRand(double a, double b)
{
    return ((b-a)*(unifRand()) + a);
}
//
// Generate a random integer between 1 and a given value.
// param n the largest value 
// return a uniform random value in [1,...,n]
long RfidTagAgent::unifRand(long n)
{
    
    if (n < 0) n = -n;
    if (n==0) return 0;
    /* There is a slight error in that this code can produce a return value of n+1
    **
    **  return long(unifRand()*n) + 1;
    */
    //Fixed code
    long guard = (long) (unifRand() * n) +1;
    return (guard > n)? n : guard;
}
//
// Reset the random number generator with the system clock.
void RfidTagAgent::seed()
{
	srand(tagEPC_*(unsigned)time(0)+Scheduler::instance().clock()*memory_);
}

// Reset the random number generator with the system clock.
void RfidTagAgent::seed(double s)
{
        srand(s);
}


void RfidTagAgent::recv(Packet* pkt, Handler*)
{
  // Lê o conteúdo do cabeçalho IP
  hdr_ip* hdrip = hdr_ip::access(pkt);
  // Lê o conteúdo do cabeçalho do pacote RFID
  hdr_rfidPacket* hdr = hdr_rfidPacket::access(pkt);
  if (hdr->tipo_==FLOW_RT) { //Responde apenas a requisições de leitores
	  if (hdr->service_==SERVICE_TRACKING) {
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
		rfidHeader->ack_=1;
		if (hdr->singularization_==SING_NOSINGULARIZATION) { //Se não for solicitada singularizaçao
                	if (hdr->id_!=id_) { 
                        	send(pktret,0);
                	}
          	}
          	else { //caso seja solicitada singularização
                	if (hdr->id_!=id_) {
                        	double tempo = Random::uniform(0,time_);
                        	Scheduler& sch = Scheduler::instance();
                        	sch.schedule(target_,pktret,tempo);
                	}
          	}
	  }
	  else if (hdr->service_==SERVICE_STANDARD) { 
		/*
		*EPCGLOBAL Radio-Frequency Identity Protocols
 		*Class-1 Generation-2 UHF RFID
		*Protocol for Communications at 860 MHz – 960 MHz
		*Version 1.2.0
		*/
		//criar pacote de resposta
		memory_=hdr->qValue_;
	        if ((hdr->command_==RC_QUERY)&&(state_!=T_ACKNOWLEDGED)) {
			updateSlot();
			if (slot_==0) {
	                        state_=T_READY;
        	                sendPacket(pkt,RC_QUERY);
                        }
                        else {
                	 	state_=T_ARBITRATE;
                        }

		}
		else if ((hdr->command_==RC_QUERYADJUST)&&(state_!=T_ACKNOWLEDGED)) {
		     	updateSlot();
			if (slot_==0) {
		                state_=T_READY;
		                sendPacket(pkt,RC_QUERYADJUST);
       			}
       			else {
               			state_=T_ARBITRATE;
		        }
		}
		else if ((hdr->command_==RC_QUERYREPLY)&&(hdr->tagEPC_==tagEPC_)) {
			state_=T_ACKNOWLEDGED;
			slot_--;
		        printf("(TAG IDENTIFICADA) [%d] estado(%d): valor do slot : %d\n",here_.addr_,state_,slot_);
			sendPacket(pkt,TC_REPLY);
	  	}
		else if ((hdr->command_==RC_QUERYREPLY)&&(hdr->tagEPC_==IP_BROADCAST)&&(slot_>0)) {
			if (state_!=T_ACKNOWLEDGED) {
				slot_=slot_-1;
			        printf("tag [%d] diminui o slot para (%d)\n",here_.addr_,slot_);
				if (slot_==0) {
                	                state_=T_READY;
                        	        sendPacket(pkt,RC_QUERYREPLY);
                        	}
                        	else {
                                	state_=T_ARBITRATE;
                        	}
			}
		}
	}
  }
  else if (hdr->tipo_==FLOW_RT_ACK) { //Tag recebe um ACK
	id_=hdr->id_; //Grava o ID do leitor que confirmou o recebimento
  }
  else { //Descarta o pacote caso a origem não tenha sido um leitor
	Packet::free(pkt);
  }
  Packet::free(pkt);
  return;
}

void RfidTagAgent::updateSlot() {
	seed();
	rng16_=Random::uniform(0,pow(2,memory_)-1);
        if (state_!=T_ACKNOWLEDGED) {
		slot_=round(rng16_);
	}
        printf("tag [%d] de estado (%d) atualizou o slot para:  %d\n",here_.addr_,state_,slot_);
}

void RfidTagAgent::sendPacket(Packet* pkt, int command) {
	// Lê o conteúdo do cabeçalho IP
	hdr_ip* hdrip = hdr_ip::access(pkt);
	// Lê o conteúdo do cabeçalho do pacote RFID
	hdr_rfidPacket* hdr = hdr_rfidPacket::access(pkt);
	Packet* pktret = allocpkt();
        hdr_rfidPacket* rfidHeader = hdr_rfidPacket::access(pktret);
        hdr_ip* ipHeader = hdr_ip::access(pktret);
        rfidHeader->tagEPC_ = tagEPC_;
        rfidHeader->id_ = hdr->id_;
        rfidHeader->tipo_ = FLOW_TR;
	rfidHeader->service_ = hdr->service_;
	rfidHeader->singularization_ = hdr->singularization_
	rfidHeader->command_=command;
	rfidHeader->rng16_=rng16_;
        ipHeader->daddr() = hdrip->saddr();
        ipHeader->dport() = hdrip->sport();
	ipHeader->saddr() = here_.addr_;
	ipHeader->sport() = here_.port_;
	send(pktret,0);
}
