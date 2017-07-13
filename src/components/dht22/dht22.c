#include "dht22.h"

#define DHT_BIT_STREAM_DATA_LENGTH          40
#define DHT_PULSE_IN_NS_THREDHOLD_BIT_0     64000     // 64us
#define DHT_PULSE_IN_NS_THREDHOLD_BIT_1     28000     // 28us

dht_ret_code_t DHT_BitStream_Decode(const uint32_t *p_pulseDuration_ns, uint8_t length, dht_data_t *p_sensorData)
{
  dht_ret_code_t ret = DHT_RET_OK;
  uint64_t temporarySerializedData;
  uint32_t pulseLen;
  uint8_t pulseIdx;
  uint8_t checksum;

  //=============== Sanity check input arguments ===============
  if (length != DHT_BIT_STREAM_DATA_LENGTH)
  {
    ret = DHT_BIT_STREAM_ERROR;
    goto exit;
  }

  if ((p_pulseDuration_ns == NULL) || (p_sensorData == NULL))
  {
    ret = DHT_OTHER_ERROR;
    goto exit;
  }
  //============================================================

  temporarySerializedData = 0;
  pulseIdx = 0;
  checksum = 0;

  for (int loop = 0; loop < DHT_BIT_STREAM_DATA_LENGTH; loop++)
  {
    temporarySerializedData <<= 1;

    pulseLen = p_pulseDuration_ns[pulseIdx];
    pulseIdx++;

    if (pulseLen < DHT_PULSE_IN_NS_THREDHOLD_BIT_0)
    {
        temporarySerializedData &= ~1ULL;
    }
    else if (pulseLen > DHT_PULSE_IN_NS_THREDHOLD_BIT_1)
    {
        temporarySerializedData |= 1ULL;

        checksum += ((uint8_t)128 >> (loop & 0x07));
    }
    else
    {
        ret = DHT_BIT_STREAM_ERROR;
        goto exit;
    }
  }

  p_sensorData->serializedData = temporarySerializedData;
  checksum >>= 1;   // Divide by 2 before checking

  if (checksum != p_sensorData->checksum)
  {
    // printf("FAIL: checksum %d --- actual checksum %d\n", sensor_data->checksum, checksum);
    ret = DHT_CHECKSUM_FAIL;
  }

exit:
  return ret;
}
