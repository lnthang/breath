#ifndef _DHT22_H
#define _DHT22_H

#include <stdio.h>
#include <stdint.h>

typedef enum
{
  DHT_RET_OK              =  0,
  DHT_CHECKSUM_FAIL       = -1,
  DHT_TIMEOUT             = -2,
  DHT_BIT_STREAM_ERROR    = -3,
  DHT_OTHER_ERROR         = -4
}dht_ret_code_t;

typedef union 
{
  uint64_t serializedData;

  struct {
    uint64_t checksum           :   8;
    uint64_t temperatureFrac    :   8;
    uint64_t temperatureInt     :   8;
    uint64_t rhumidityFrac      :   8;
    uint64_t rhumidityInt       :   8;
    uint64_t reserved           :   24;
  };
}dht_data_t;

dht_ret_code_t DHT_BitStream_Decode(const uint32_t *p_pulseDuration_ns, uint8_t length, dht_data_t *p_sensorData);

#endif // _DHT22_H
