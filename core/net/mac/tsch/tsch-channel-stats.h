#ifndef __TSCH_CHANNEL_STATS_H__
#define __TSCH_CHANNEL_STATS_H__

#include "contiki.h"

void init_channel_stats(void);

void notify_rx(uint8_t channel, int16_t rssi, uint16_t lqi);

struct channel_stats_t
{
  uint8_t channel;
  double average_rssi;
  double average_lqi;
  long int tx;
  long int rx;
};

#endif
