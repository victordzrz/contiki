#ifndef __TSCH_CHANNEL_STATS_H__
#define __TSCH_CHANNEL_STATS_H__

#include "contiki.h"

void init_channel_stats(void);

void notify_rx_ok(int src, uint8_t channel, int16_t rssi, uint16_t lqi);
void notify_rx_slot(int src,uint8_t channel);

struct channel_stats_t
{
  uint8_t channel;
  double average_rssi;
  double average_lqi;
  uint16_t tx;
  uint16_t rx_total;
  uint16_t rx;
};

#endif
