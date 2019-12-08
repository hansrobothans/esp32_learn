#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/IRRemote.h"
#include "driver/rmt.h"

#define CLK_DIV           100
#define TICK_10_US        (80000000 / CLK_DIV / 100000)
static RingbufHandle_t ringBuf;

#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

uint8_t IRrecv_Pin;
uint8_t IRrecv_Chanel;

void IRrecvInit(uint8_t pin, uint8_t port)
{
  IRrecv_Pin = pin;
  IRrecv_Chanel = port;

  rmt_config_t config;
  config.rmt_mode = RMT_MODE_RX;
  config.clk_div = CLK_DIV;
  config.channel = (rmt_channel_t)IRrecv_Chanel;
  config.gpio_num = (gpio_num_t)IRrecv_Pin;
  config.mem_block_num = 2;
  config.rx_config.filter_en = 1;
  config.rx_config.filter_ticks_thresh = 100;
  config.rx_config.idle_threshold = TICK_10_US * 100 * 20;

  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(config.channel, 5000, 0));

  rmt_get_ringbuf_handle(config.channel, &ringBuf);

  rmt_rx_start(config.channel, 1);

  return;
}

bool IRrecvIsInRange(rmt_item32_t item, int lowDuration, int highDuration, int tolerance)
{
  uint32_t lowValue = item.duration0 * 10 / TICK_10_US;
  uint32_t highValue = item.duration1 * 10 / TICK_10_US;

  if (lowValue<(lowDuration-tolerance)||lowValue>(lowDuration+tolerance)||(highValue!=0&&(highValue<(highDuration-tolerance)||highValue>(highDuration+tolerance))))
  {
    return false;
  }
  return true;
}

bool IRrecvIS0(rmt_item32_t item)
{
  return IRrecvIsInRange(item, NEC_BIT_MARK, NEC_BIT_MARK, 100);
}

bool IRrecvIS1(rmt_item32_t item)
{
  return IRrecvIsInRange(item, NEC_BIT_MARK, NEC_ONE_SPACE, 100);
}

uint8_t IRrecvDecode(rmt_item32_t *data, int numItems)
{
  if (!IRrecvIsInRange(data[0], NEC_HDR_MARK, NEC_HDR_SPACE, 200))
  {
    return 0;
  }
  int i;
  uint8_t address = 0, notAddress = 0, command = 0, notCommand = 0;
  int accumCounter = 0;
  uint8_t accumValue = 0;
  for (i=1; i<numItems; i++)
  {
    if (IRrecvIS0(data[i]))
    {
      accumValue = accumValue >> 1;
    }
    else if (IRrecvIS1(data[i]))
    {
      accumValue = (accumValue >> 1) | 0x80;
    }

    if (accumCounter == 7)
    {
      accumCounter = 0;
      if (i==8)
      {
        address = accumValue;
      }
      else if (i==16)
      {
        notAddress = accumValue;
      }
      else if (i==24)
      {
        command = accumValue;
      }
      else if (i==32)
      {
        notCommand = accumValue;
      }
      accumValue = 0;
    }
    else
    {
      accumCounter++;
    }
  }

  if (address != (notAddress ^ 0xff) || command != (notCommand ^ 0xff))
  {
    return 0;
  }

  return command;
}

uint8_t IRrecvReadIR(void)
{
  size_t itemSize;
  uint8_t command = 0;

  rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive((RingbufHandle_t)ringBuf, (size_t *)&itemSize, (TickType_t)portMAX_DELAY);

  int numItems = itemSize / sizeof(rmt_item32_t);
  int i;
  rmt_item32_t *p = item;
  for (i=0; i<numItems; i++) {
    p++;
  }
  command=IRrecvDecode(item, numItems);
  vRingbufferReturnItem(ringBuf, (void*) item);

  return command;
}
