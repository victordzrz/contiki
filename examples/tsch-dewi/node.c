/*
 * Copyright (c) 2016, Inria.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         An example of Rime/TSCH
 * \author
 *         Simon Duquennoy <simon.duquennoy@inria.fr>
 *
 */

#include <stdio.h>
#include "contiki-conf.h"
#include "net/netstack.h"
#include "net/rime/rime.h"
#include "net/mac/tsch/tsch.h"
#include "net/mac/tsch/tsch-schedule.h"
#include "net/mac/channel-stats.h"

const int number_of_nodes=3;
const linkaddr_t coordinator_addr =    { { 0xF5, 0x1D } };
const linkaddr_t node1_addr =    { { 0xF5, 0x3B } };
const linkaddr_t node2_addr =    { { 0x84, 0xB0 } };
const linkaddr_t node3_addr =    { { 0x85, 0xD2 } };


/*---------------------------------------------------------------------------*/
PROCESS(unicast_test_process, "Rime Node");
PROCESS(print_channel_stats, "Print stats");
AUTOSTART_PROCESSES(&unicast_test_process, &print_channel_stats);


/*---------------------------------------------------------------------------*/
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  // printf("App: unicast message received from %u.%u\n",
  //  from->u8[0], from->u8[1]);
}
/*---------------------------------------------------------------------------*/
static void
sent_uc(struct unicast_conn *ptr, int status, int num_tx)
{
  // printf("App: unicast message sent, status %u, num_tx %u\n",
  //  status, num_tx);
}

static const struct unicast_callbacks unicast_callbacks = { recv_uc, sent_uc };
static struct unicast_conn uc;

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(unicast_test_process, ev, data)
{
  PROCESS_BEGIN();

  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));

  NETSTACK_MAC.on();

  unicast_open(&uc, 146, &unicast_callbacks);

  while(!tsch_is_associated){
    static struct etimer et;

    etimer_set(&et, CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    printf("Waiting for association\n");
  }

  struct tsch_slotframe * current_slotframe =  tsch_schedule_get_slotframe_by_handle(0);
  for(int i=1;i< TSCH_SCHEDULE_CONF_DEFAULT_LENGTH ; i++){
    if(!linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr)){
      if(i%number_of_nodes==0 && linkaddr_cmp(&node1_addr, &linkaddr_node_addr)){
        tsch_schedule_add_link(current_slotframe,
          LINK_OPTION_TX ,
          LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
          i, 0);
        }
      if(i%number_of_nodes==1 && linkaddr_cmp(&node2_addr, &linkaddr_node_addr)){
        tsch_schedule_add_link(current_slotframe,
          LINK_OPTION_TX ,
          LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
          i, 0);
        }
      if(i%number_of_nodes==2 && linkaddr_cmp(&node3_addr, &linkaddr_node_addr)){
        tsch_schedule_add_link(current_slotframe,
          LINK_OPTION_TX ,
          LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
          i, 0);
        }
      }
      else{
        tsch_schedule_add_link(current_slotframe,
          LINK_OPTION_RX ,
          LINK_TYPE_ADVERTISING, &tsch_broadcast_address,
          i, 0);
        }
      }

  while(1) {
    static struct etimer et;

    etimer_set(&et, 1);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    if(tsch_is_associated){
      packetbuf_copyfrom("Hello", 5);

      if(!linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr) &&  tsch_queue_packet_count(&coordinator_addr)<4) {
        //printf("App: sending unicast message to %u.%u\n", destination_addr.u8[0], destination_addr.u8[1]);
        unicast_send(&uc, &coordinator_addr);
      }
    }
  }

  PROCESS_END();
}

PROCESS_THREAD(print_channel_stats, ev, data){
   PROCESS_BEGIN();
   static struct etimer et2;
   static struct channel_stats_t channel_stats_list[16];
   static struct channel_info_t channel_dr[16];
   static uint8_t channel_hopping_sequence[32];

   init_channel_stats();

   /* Delay 1 second */
   etimer_set(&et2, CLOCK_SECOND);

   while(1) {
     PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));
     if(tsch_is_associated){
       /* Reset the etimer to trig again in 1 second */
       channel_stats_get_rssi(&channel_dr);
       channel_stats_get_stats(&channel_stats_list);
       for(int i=0;i<16;i++){
         printf("[%d] DR:%d RSSI:%d\n", channel_stats_list[i].channel,calculate_dalivery_rate(channel_stats_list[i]),(int)channel_dr[i].value);
       }
       calculate_channel_hopping_sequence(&channel_hopping_sequence);
       printf("[");
       for(int i=0;i<32;i++){
         printf("%d,",channel_hopping_sequence[i]);
       }
       printf("]\n");
       /* ... */
     }
     etimer_reset(&et2);
   }
   PROCESS_END();
}

int calculate_dalivery_rate(struct channel_stats_t stats){
  if(stats.tx_total==0 || stats.tx_ok==0){
    return 0;
  }else{
    return (int)((((double)stats.tx_ok)/stats.tx_total)*1000);
  }
}
/*---------------------------------------------------------------------------*/
