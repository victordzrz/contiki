#ifndef __TSCH_CHANNEL_STATS_H__
#define __TSCH_CHANNEL_STATS_H__

#include "contiki.h"

#define TX_OK 1
#define TX_FAIL 0

struct channel_stats_t
{
  uint8_t channel;
  double average_rssi;
  double average_lqi;
  double average_dr;
  uint32_t sample_number;
  uint32_t tx_total;
  uint32_t tx_ok;
  uint32_t rx;
};

struct channel_info_t
{
  uint8_t channel;
  double value;
};

void init_channel_stats(void);

void channel_stats_rx(int src, uint8_t channel, int16_t rssi, uint16_t lqi);
void channel_stats_tx(int dest,uint8_t channel, int16_t rssi, uint16_t lqi, int tx_status);

void channel_stats_get_lqi(struct channel_info_t * buffer);
void channel_stats_get_rssi(struct channel_info_t * buffer);
void channel_stats_get_delivery_rate(struct channel_info_t * buffer);
void channel_stats_get_stats(struct channel_stats_t * channel_stats_buffer);
void calculate_channel_hopping_sequence(uint8_t * channel_hopping_sequence);



#endif
