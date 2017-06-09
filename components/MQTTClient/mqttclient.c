#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "MQTTPacket.h"
#include "mqttClient.h"

#define MQTTCLIENT_TAG "MQTT CLIENT"
#define MQTTCLIENT_LOGD(format, ...)  ESP_LOGD(MQTTCLIENT_TAG, format, ##__VA_ARGS__)
#define MQTTCLIENT_LOGI(format, ...)  ESP_LOGI(MQTTCLIENT_TAG, format, ##__VA_ARGS__)
#define MQTTCLIENT_LOGW(format, ...)  ESP_LOGW(MQTTCLIENT_TAG, format, ##__VA_ARGS__)
#define MQTTCLIENT_LOGE(format, ...)  ESP_LOGE(MQTTCLIENT_TAG, format, ##__VA_ARGS__)

static int32_t MQTTClient_Connect(mqttClient_t *client);

int32_t MQTTClient_Init(struct mqttClient_t *client, char *host, int32_t port)
{
  int32_t rc = -1;
  char buf[10];
  struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

  itoa(port, buf, sizeof(buf)/sizeof(buf[0]));
  rc = getaddrinfo(host, buf, &hints, &client->addri);

  if (rc != 0)
  {
    printf("Error code: %d\n", rc);
    goto exit;
  }

  client->sock = socket(client->addri->ai_family, client->addri->ai_socktype, 0);

  if (client->sock == -1)
  {
    MQTTCLIENT_LOGE("Failed to create socket\n");
    goto exit;
  }

  MQTTCLIENT_LOGI("Socket created with id : %d\n", client->sock);

  rc = MQTTClient_Connect(client);
  
  struct in_addr *addr;
  addr = &((struct sockaddr_in *)client->addri->ai_addr)->sin_addr;
  printf("Address : %x\n", addr->s_addr);

exit:
  return rc;
}

int32_t MQTTClient_Publish(struct mqttClient_t *client, uint8_t *topic, uint8_t *msgBuf, uint32_t msgLen)
{
  int32_t rc = -1;

  return rc;
}

int32_t MQTTClient_Subscribe(struct mqttClient_t *client, uint8_t *topic, mqttClientMsgHandler_t callback)
{
  int32_t rc = -1;

  return rc;
}

int32_t MQTTClient_Loop(struct mqttClient_t *client)
{
  int32_t rc = -1;

  return rc;
}

static int32_t MQTTClient_Connect(mqttClient_t *client)
{
  int32_t rc = -1;

  return rc;
}
