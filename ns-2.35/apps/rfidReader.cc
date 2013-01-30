
/*
 * rfidReader.cc
 * Copyright (C) 2000 by the University of Southern California
 * $Id: rfidReader.cc,v 1.8 2005/08/25 18:58:01 johnh Exp $
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
 * Last update: 30/JAN/2012
 */

#include "rfidReader.h"
#include "rfidPacket.h"
#include <time.h>
#include <math.h>
static class RfidReaderClass : public TclClass {
public:
	RfidReaderClass() : TclClass("Agent/RfidReader") {}
	TclObject* create(int, const char*const*) {
		return (new RfidReaderAgent());
	}
} class_rfidReader;


RfidReaderAgent::RfidReaderAgent() : Agent(PT_RFIDPACKET), state_(0), command_(0),Qfp_(4),counter_(0),rs_timer_(this), rs_timer_restart_(this)
{
	bind("packetSize_", &size_);
	bind("tagEPC_",&tagEPC_);
	bind("id_",&id_);
	bind("singularization_",&singularization_);
	bind("service_",&service_);
	bind("memory_",&memory_);
	bind("qValue_",&qValue_);
	bind("c_",&c_);
	bind("t2_",&t2_);
}

int RfidReaderAgent::command(int argc, const char*const* argv)
{
  if (argc == 2) {
    if (strcmp(argv[1], "query-tags") == 0) {
      resend();
      return (TCL_OK);
    }
    else if (strcmp(argv[1], "standard-query-tags") == 0) {
	send_query();
	rs_timer_.resched(t2_); //Wait for tags responses
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
	if (hdr->command_!=TC_REPLY) { //SINGULARIZATION
		counter_++;
	}
	tagEPC_=hdr->tagEPC_;
	if (hdr->command_==TC_REPLY) { //UNIQUE TAG RESPONSE
		printf("Tag [%d] identificada\n",hdr->tagEPC_);
		counter_=0;
	}

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
        ipHeader->saddr() = here_.addr_; //Source: reader ip
        ipHeader->sport() = here_.port_;
	//Sends the packet
        send(pkt, (Handler*) 0);
}

void RfidReaderAgent::send_query() {

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
	rfidHeader->command_=RC_QUERY;
	rfidHeader->qValue_=qValue_;
	rfidHeader->tagEPC_=IP_BROADCAST;
        ipHeader->daddr() = IP_BROADCAST; //Destination: broadcast
        ipHeader->saddr() = here_.addr_; //Source: reader ip
        ipHeader->sport() = here_.port_;
        //Sends the packet
        send(pkt, (Handler*) 0);
	//Packet::free(pkt);

}

void RfidReaderAgent::send_query_ajust() {

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
	rfidHeader->command_=RC_QUERYADJUST;
        rfidHeader->qValue_=qValue_;
	rfidHeader->tagEPC_=IP_BROADCAST;
        printf("Novo qValue=%i\n",rfidHeader->qValue_);
	ipHeader->daddr() = IP_BROADCAST; //Destination: broadcast
        ipHeader->dport() = ipHeader->sport();
        ipHeader->saddr() = here_.addr_; //Source: reader ip
        ipHeader->sport() = here_.port_;
        //Sends the packet
        send(pkt, (Handler*) 0);
}

void RfidReaderAgent::send_query_reply() {

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
	rfidHeader->command_=RC_QUERYREPLY;
        rfidHeader->qValue_=qValue_;
	rfidHeader->tagEPC_=tagEPC_;
        ipHeader->daddr() = IP_BROADCAST; //Destination: broadcast
        ipHeader->saddr() = here_.addr_; //Source: reader ip
        //Sends the packet
	send(pkt, (Handler*) 0);
}

void RfidReaderAgent::send_query_reply_update_slot() {

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
        rfidHeader->command_=RC_QUERYREPLY;
        rfidHeader->qValue_=qValue_;
        rfidHeader->tagEPC_=IP_BROADCAST;
        ipHeader->daddr() = IP_BROADCAST; //Destination: broadcast
        ipHeader->saddr() = here_.addr_; //Source: reader ip
        //Sends the packet
        send(pkt, (Handler*) 0); 
}


int RfidReaderAgent::getCounter() {
	return (counter_);
}

int RfidReaderAgent::getIP() {
	return (here_.addr_);
}

void RfidReaderAgent::start_sing() {

	 if (counter_==0) {
                printf("NENHUMA TAG RESPONDEU!!\n");
                Qfp_=fmax(0,Qfp_ - c_);
                if (Qfp_>15) {
                        Qfp_=15;
                }
                qValue_=round(Qfp_);
                printf("Qfp=%1f - Q=%d\n",Qfp_,qValue_);
                send_query_ajust();
		if (qValue_==0) return;
		else
			rs_timer_.resched(t2_);
        }
        if (counter_==1) {
                printf("APENAS UMA TAG RESPONDEU!!\n");
		counter_=0;
		send_query_reply();
		rs_timer_.resched(t2_);
		send_query_reply_update_slot();
		rs_timer_restart_.resched(t2_);
        }
	if (counter_>1) {
                printf("COLISÃO - MAIS DE UMA TAG RESPONDEU(%d)!!\n",counter_);
                Qfp_=fmin(15,Qfp_ + c_);
                if (Qfp_<0) {
                        Qfp_=0;
                }
                qValue_=round(Qfp_);
                printf("Qfp=%1f - Q=%d\n",Qfp_,qValue_);
                counter_=0;
		send_query_ajust();
		if (qValue_==15) return;
		else
			rs_timer_.resched(t2_);
        }
}

void RetransmitTimer::expire(Event *e) {
	a_->start_sing();
}

void RestartTimer::expire(Event *e) {
        a_->send_query();
	a_->rs_timer_.resched(a_->t2_);
}

