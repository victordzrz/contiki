#include "tsch-channel-stats.h"
#include "contiki.h"


#define CHANNEL_NUMBER 16


static struct channel_stats_t channel_stats_list[CHANNEL_NUMBER];

void init_channel_stats(void){
  for(int i=0;i<CHANNEL_NUMBER;i++){
    channel_stats_list[i].channel=i;
  }
}

//Art of Computer Programming, Volume 2: Seminumerical Algorithms, Donald E. Knuth
void notify_rx_ok(int src, uint8_t channel, int16_t rssi, uint16_t lqi){
  struct channel_stats_t* current_channel_stats=&(channel_stats_list[channel-11]);//normalize to 0-15
  if(rssi==0 || lqi==0){
    printf("\nZERO\n");
  }
  else{
    if(current_channel_stats->rx==0){
      current_channel_stats->average_rssi=rssi;
      current_channel_stats->average_lqi=lqi;
      current_channel_stats->rx++;
    }
    else{
      current_channel_stats->rx++;

      current_channel_stats->average_rssi= current_channel_stats->average_rssi
      + (rssi-current_channel_stats->average_rssi)/current_channel_stats->rx;

      current_channel_stats->average_lqi= current_channel_stats->average_lqi
      + (lqi-current_channel_stats->average_lqi)/current_channel_stats->rx;
    }
    int a_rssi=(int)current_channel_stats->average_rssi;
    int a_lqi=(int)current_channel_stats->average_lqi;
    printf("%d\t%d\t%d\t%ld\t%ld\n",
    channel,
    a_rssi,
    a_lqi,
    current_channel_stats->rx,
    current_channel_stats->rx_total);
  }
}

void notify_rx_slot(int src,uint8_t channel){
  (channel_stats_list[channel-11].rx_total)++;
}
