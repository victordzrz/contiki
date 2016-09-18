#ifndef __TSCH_CHANNEL_STATS_H__
#define __TSCH_CHANNEL_STATS_H__

#include "contiki.h"

#define TX_OK 1
#define TX_FAIL 0

void init_channel_stats(void);

void channel_stats_rx(int src, uint8_t channel, int16_t rssi, uint16_t lqi);
void channel_stats_tx(int dest,uint8_t channel, int16_t rssi, uint16_t lqi, int tx_status);

struct channel_stats_t
{
  uint8_t channel;
  double average_rssi;
  double average_lqi;
  uint32_t record_number;
  uint32_t tx_total;
  uint32_t tx_ok;
  uint32_t rx;
};

#endif
