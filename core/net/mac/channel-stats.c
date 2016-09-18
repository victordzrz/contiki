#include "channel-stats.h"
#include "contiki.h"


#define CHANNEL_NUMBER 16


static struct channel_stats_t channel_stats_list[CHANNEL_NUMBER];

void add_record(struct channel_stats_t * channel_stats, int16_t rssi, uint16_t lqi);

void init_channel_stats(void){
  for(int i=0;i<CHANNEL_NUMBER;i++){
    channel_stats_list[i].channel=i;
  }
}

void channel_stats_rx(int src, uint8_t channel, int16_t rssi, uint16_t lqi){
  struct channel_stats_t* current_channel_stats=&(channel_stats_list[channel-11]);//normalize to 0-15
  current_channel_stats->rx++;
  add_record(current_channel_stats,rssi,lqi);
  printf("RX %x\t%d\t%d\t%d\t%ld\n",
  src,
  channel,
  rssi,
  lqi,
  current_channel_stats->rx);
}

void channel_stats_tx(int dest,uint8_t channel, int16_t rssi, uint16_t lqi, int tx_status){
  struct channel_stats_t* current_channel_stats=&(channel_stats_list[channel-11]);//normalize to 0-15
  current_channel_stats->tx_total++;
  if(tx_status==TX_OK){
    current_channel_stats->tx_ok++;
    add_record(current_channel_stats,rssi,lqi);
  }
  printf("TX %x\t%d\t%d\t%d\t%ld\t%ld\n",
  dest,
  channel,
  rssi,
  lqi,
  current_channel_stats->tx_ok,
  current_channel_stats->tx_total);
}

//Art of Computer Programming, Volume 2: Seminumerical Algorithms, Donald E. Knuth
void add_record(struct channel_stats_t * channel_stats, int16_t rssi, uint16_t lqi){
  if(rssi==0 || lqi==0){
    printf("ZERO\n");
  }
  else{
    if(channel_stats->record_number==0){
      channel_stats->average_rssi=rssi;
      channel_stats->average_lqi=lqi;
      channel_stats->record_number++;
    }
    else{
      channel_stats->record_number++;

      channel_stats->average_rssi= channel_stats->average_rssi
      + (rssi-channel_stats->average_rssi)/channel_stats->record_number;

      channel_stats->average_lqi= channel_stats->average_lqi
      + (lqi-channel_stats->average_lqi)/channel_stats->record_number;
    }
  }
}
