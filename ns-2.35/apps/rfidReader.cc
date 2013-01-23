
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

static class RfidReaderClass : public TclClass {
public:
	RfidReaderClass() : TclClass("Agent/RfidReader") {}
	TclObject* create(int, const char*const*) {
		return (new RfidReaderAgent());
	}
} class_rfidReader;


RfidReaderAgent::RfidReaderAgent() : Agent(PT_RFIDPACKET)
{
	bind("packetSize_", &size_);
	bind("tagEPC_",&tagEPC_);
	bind("id_",&id_);
	bind("singularization_",&singularization_);
	bind("service_",&service_);
}

int RfidReaderAgent::command(int argc, const char*const* argv)
{
  if (argc == 2) {
    if (strcmp(argv[1], "query-tags") == 0) {
      Packet* pkt = allocpkt();
      hdr_ip* iph = HDR_IP(pkt);
      hdr_rfidPacket *ph = hdr_rfidPacket::access(pkt);
      ph->id_ = id_;
      ph->tipo_ = 1;
      ph->singularization_ = singularization_;
      if (service_==1) { ph->service_=1; }
      iph->daddr() = IP_BROADCAST;
      iph->dport() = iph->sport();
      //printf("Antes de enviar pacote de requisição...\n");
      send(pkt, (Handler*) 0);
      //printf("Requisição enviada com sucesso...\n");
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
  if ((hdr->tipo_==0)&&(hdr->id_==id_)) {
  	//printf("[LEITOR] (%d) recebeu RESPOSTA de (%i)\n",hdr->id_,hdr->tagEPC_);
	if (hdr->ack_==1) {
		//Enviar ACK de confirmação
		Packet* pktret = allocpkt();
	        hdr_rfidPacket* hdrTag = hdr_rfidPacket::access(pktret);
        	hdr_ip* hdrIp = hdr_ip::access(pktret);
	        hdrTag->tagEPC_ = hdr->tagEPC_;
        	hdrTag->id_ = hdr->id_;
	        hdrTag->tipo_ = 2;
        	//hdrIp->daddr() = IP_BROADCAST;
	        hdrIp->daddr() = hdrip->saddr();
        	hdrIp->dport() = hdrip->sport();
		send(pktret,0);
		//printf("[LEITOR-ACK]Enviado ACK para %d\n",hdr->tagEPC_);
	}
  }
  else if(hdr->tipo_==0){
	printf("Leitor (NÃO IDENTIFICADO) recebeu RESPOSTA de (%i)\n",hdr->tagEPC_);
  }
  Packet::free(pkt);
  return;
}
