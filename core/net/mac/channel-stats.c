#include "net/mac/channel-stats.h"
#include "contiki.h"
#include "net/mac/tsch/tsch-conf.h"


#define CHANNEL_NUMBER 16
#define CHANNEL_HOPPING_SEQUENCE_LENGTH 32
#define ALPHA 0.2f


static struct channel_stats_t channel_stats_list[CHANNEL_NUMBER];

void add_record(struct channel_stats_t * channel_stats, int16_t rssi, uint16_t lqi);

void init_channel_stats(void){
  for(int i=0;i<CHANNEL_NUMBER;i++){
    channel_stats_list[i].channel=i+11;
  }
}

void channel_stats_rx(int src, uint8_t channel, int16_t rssi, uint16_t lqi){
  struct channel_stats_t* current_channel_stats=&(channel_stats_list[channel-11]);//normalize to 0-15
  current_channel_stats->rx++;
  add_record(current_channel_stats,rssi,lqi);
  // printf("RX %x\t%d\t%d\t%d\t%ld\n",
  // src,
  // channel,
  // rssi,
  // lqi,
  // current_channel_stats->rx);
}

void channel_stats_tx(int dest,uint8_t channel, int16_t rssi, uint16_t lqi, int tx_status){
  struct channel_stats_t* current_channel_stats=&(channel_stats_list[channel-11]);//normalize to 0-15
  current_channel_stats->tx_total++;
  if(tx_status==TX_OK){
    current_channel_stats->tx_ok++;
    add_record(current_channel_stats,rssi,lqi);
    current_channel_stats->average_dr=1*ALPHA+(1-ALPHA)*current_channel_stats->average_dr;
  }
  else{
    current_channel_stats->average_dr=0*ALPHA+(1-ALPHA)*current_channel_stats->average_dr;
  }
  // printf("TX %x\t%d\t%d\t%d\t%ld\t%ld\n",
  // dest,
  // channel,
  // rssi,
  // lqi,
  // current_channel_stats->tx_ok,
  // current_channel_stats->tx_total);
}


//Exponential Moving Average
void add_record(struct channel_stats_t * channel_stats, int16_t rssi, uint16_t lqi){
  if(rssi==0 || lqi==0){
    printf("ZERO\n");
  }
  else{
    if(channel_stats->sample_number==0){
      channel_stats->average_rssi=rssi;
      channel_stats->average_lqi=lqi;
      channel_stats->sample_number++;
    }
    else{
      channel_stats->sample_number++;

      channel_stats->average_rssi=((double)rssi*ALPHA) + (1.0 - ALPHA) * channel_stats->average_rssi;
      channel_stats->average_lqi=((double)lqi*ALPHA) + (1.0 - ALPHA) * channel_stats->average_lqi;
    }
  }
}

void channel_stats_get_lqi(struct channel_info_t * buffer){
  for(int i=0;i<CHANNEL_NUMBER;i++){
    buffer[i].value=channel_stats_list[i].average_lqi;
    buffer[i].channel=channel_stats_list[i].channel;
  }
}

void channel_stats_get_rssi(struct channel_info_t * buffer){
  for(int i=0;i<CHANNEL_NUMBER;i++){
    buffer[i].value=channel_stats_list[i].average_rssi;
    buffer[i].channel=channel_stats_list[i].channel;
  }
}

void channel_stats_get_delivery_rate(struct channel_info_t * buffer){
  for(int i=0;i<CHANNEL_NUMBER;i++){
    // if(channel_stats_list[i].tx_total==0){
    //   buffer[i].value=0;
    // }
    // else{
    //   buffer[i].value=((double)channel_stats_list[i].tx_ok)/channel_stats_list[i].tx_total;
    // }
    buffer[i].value=channel_stats_list[i].average_dr;
    buffer[i].channel=channel_stats_list[i].channel;
  }
}

void channel_stats_get_stats(struct channel_stats_t * channel_stats_buffer){
  for(int i=0;i<CHANNEL_NUMBER;i++){
      channel_stats_buffer[i].channel=channel_stats_list[i].channel;
      channel_stats_buffer[i].average_rssi=channel_stats_list[i].average_rssi;
      channel_stats_buffer[i].average_lqi=channel_stats_list[i].average_lqi;
      channel_stats_buffer[i].sample_number=channel_stats_list[i].sample_number;
      channel_stats_buffer[i].tx_total=channel_stats_list[i].tx_total;
      channel_stats_buffer[i].tx_ok=channel_stats_list[i].tx_ok;
      channel_stats_buffer[i].rx=channel_stats_list[i].rx;
  }
}

void calculate_channel_hopping_sequence(uint8_t * channel_hopping_sequence){
  struct channel_info_t buffer[CHANNEL_NUMBER];
  channel_stats_get_delivery_rate(&buffer);

  double total_allocation=0;

  for(int i=0;i<CHANNEL_NUMBER;i++){
    total_allocation+=buffer[i].value;
  }

  double allocation_step=total_allocation/CHANNEL_HOPPING_SEQUENCE_LENGTH;

  int channels_allocated=0;

  for(channels_allocated=0;channels_allocated<CHANNEL_NUMBER;channels_allocated++){
    channel_hopping_sequence[channels_allocated]=TSCH_DEFAULT_HOPPING_SEQUENCE[channels_allocated];
    for(int i=0;i<CHANNEL_NUMBER;i++){
      if(buffer[i].channel==channel_hopping_sequence[channels_allocated]){
        buffer[i].value-=allocation_step;
      }
    }
  }

  for(;channels_allocated<CHANNEL_HOPPING_SEQUENCE_LENGTH;channels_allocated++){
    uint8_t best_channel=0;
    double best_channel_rate=0;

    for(int i=0;i<CHANNEL_NUMBER;i++){
      if(buffer[i].value>best_channel_rate){
        best_channel=buffer[i].channel;
        best_channel_rate=buffer[i].value;
      }
    }
    channel_hopping_sequence[channels_allocated]=best_channel;
    for(int i=0;i<CHANNEL_NUMBER;i++){
      if(buffer[i].channel==channel_hopping_sequence[channels_allocated]){
        buffer[i].value-=allocation_step;
      }
    }
  }

}
