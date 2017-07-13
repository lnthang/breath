#include "unity.h"
#include "dht22.h"
#include <stdint.h>

void setUp(void)
{
}

void tearDown(void)
{
}

// void test_dht22_NeedToImplement(void)
// {
//   TEST_IGNORE_MESSAGE("Need to Implement dht22");
// }

void test_whenInputWrongValueTo__DHT_BitStream_Decode__thenErrorCodeShouldReturned(void)
{
  uint32_t tmpArray[40];
  dht_data_t dhtValue;

  // All Arguments are invalid
  TEST_ASSERT_EQUAL(DHT_BIT_STREAM_ERROR, DHT_BitStream_Decode(0, 0, 0));

  // Argument Combinations are invalid
  TEST_ASSERT_EQUAL(DHT_BIT_STREAM_ERROR, DHT_BitStream_Decode(0, 0, &dhtValue));
  TEST_ASSERT_EQUAL(DHT_OTHER_ERROR, DHT_BitStream_Decode(0, 40, 0));
  TEST_ASSERT_EQUAL(DHT_OTHER_ERROR, DHT_BitStream_Decode(0, 40, &dhtValue));
  TEST_ASSERT_EQUAL(DHT_BIT_STREAM_ERROR, DHT_BitStream_Decode(tmpArray, 0, 0));
  TEST_ASSERT_EQUAL(DHT_BIT_STREAM_ERROR, DHT_BitStream_Decode(tmpArray, 0, &dhtValue));
  TEST_ASSERT_EQUAL(DHT_OTHER_ERROR, DHT_BitStream_Decode(tmpArray, 40, 0));

  // Length other than 40 is also an error
  TEST_ASSERT_EQUAL(DHT_BIT_STREAM_ERROR, DHT_BitStream_Decode(tmpArray, 20, &dhtValue));
}
