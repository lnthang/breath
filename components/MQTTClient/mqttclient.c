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

  /* Find host information for connection */
  itoa(port, buf, sizeof(buf)/sizeof(buf[0]));
  rc = getaddrinfo(host, buf, &hints, &client->addri);
  if (rc != 0)
  {
    /* Something wrong exit with return code */
    MQTTCLIENT_LOGE("Cannot find host info.\n");
    goto exit;
  }

  /* Create a socket */
  client->sock = socket(client->addri->ai_family, client->addri->ai_socktype, 0);
  if (client->sock == -1)
  {
    MQTTCLIENT_LOGE("Failed to create socket\n");
    goto exit;
  }
  // MQTTCLIENT_LOGI("Socket created with id : %d\n", client->sock);

  /* Connect to host */
  rc = MQTTClient_Connect(client);
  if (rc != 0)
  {
    MQTTCLIENT_LOGE("Failed to connect to %s.\n", host);
    goto exit;
  }
  MQTTCLIENT_LOGI("Successfully connected to host %s.\n", host);

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
  return connect(client->sock, client->addri->ai_addr, client->addri->ai_addrlen);
}
