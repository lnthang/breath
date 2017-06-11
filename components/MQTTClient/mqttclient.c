#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "esp_log.h"

#include "MQTTPacket.h"
#include "mqttClient.h"

#define MQTTCLIENT_TAG "MQTT CLIENT"
#define MQTTCLIENT_LOGD(format, ...)  ESP_LOGD(MQTTCLIENT_TAG, format, ##__VA_ARGS__)
#define MQTTCLIENT_LOGI(format, ...)  ESP_LOGI(MQTTCLIENT_TAG, format, ##__VA_ARGS__)
#define MQTTCLIENT_LOGW(format, ...)  ESP_LOGW(MQTTCLIENT_TAG, format, ##__VA_ARGS__)
#define MQTTCLIENT_LOGE(format, ...)  ESP_LOGE(MQTTCLIENT_TAG, format, ##__VA_ARGS__)

#define MAX_SUPPORTED_CLIENT_NUM    5


static struct {
  // struct{
  //   mqttClient_t client;
  //   uint8_t isFree;
  // }clientList[MAX_SUPPORTED_CLIENT_NUM];
  mqttClient_t client[MAX_SUPPORTED_CLIENT_NUM];
  uint8_t numFreeSlot;
}clientList;


static int32_t MQTTClient_Connect(mqttClient_t *client);
static int32_t MQTTClient_ReadSock(void *sock, unsigned char *buf, int32_t len);

int32_t MQTTClient_Init(/*struct mqttClient_t *client, char *host, int32_t port */)
{
  /* Initial global variable */
  memset(&clientList.client, 0, sizeof(mqttClient_t) * MAX_SUPPORTED_CLIENT_NUM);
  clientList.numFreeSlot = MAX_SUPPORTED_CLIENT_NUM; 


#if 0
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
  if (rc == -1)
  {
    MQTTCLIENT_LOGE("Failed to connect to %s.\n", host);
    goto exit;
  }
  MQTTCLIENT_LOGI("Successfully connected to host %s.\n", host);

exit:
#endif

  return 0;
}

int32_t MQTTClient_Create(char *host, int32_t port)
{
  int32_t clientIdx = -1;
  int32_t rc = -1;
  mqttClient_t *pClient = NULL;

  char buf[10];
  struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

  for (int idx = 0; idx < MAX_SUPPORTED_CLIENT_NUM; idx++)
  {
    if (clientList.client[idx].addri == NULL)
    {
      /* If NULL -> mean not allocated yet - Available for new connection */
      clientIdx = idx;
      pClient = &clientList.client[idx];
      MQTTCLIENT_LOGI("Find a free client slot at index %d", idx);
      break;
    }

    if (idx == (MAX_SUPPORTED_CLIENT_NUM - 1))
    {
      /* No available slot then exit with error code */
      MQTTCLIENT_LOGE("Cannot find a free client slot to allocate\n");
      goto exit;
    }
  }

  /* Find host information for connection */
  itoa(port, buf, sizeof(buf)/sizeof(buf[0]));
  rc = getaddrinfo(host, buf, &hints, &pClient->addri);
  if (rc != 0)
  {
    /* Something wrong exit with return code */
    MQTTCLIENT_LOGE("Cannot find host info\n");
    goto exit;
  }

  /* Create a socket */
  pClient->sock = socket(pClient->addri->ai_family, pClient->addri->ai_socktype, 0);
  if (pClient->sock == -1)
  {
    MQTTCLIENT_LOGE("Failed to create socket\n");
    goto exit;
  }

  rc = MQTTClient_Connect(pClient);

exit:
  return clientIdx;
}

int32_t MQTTClient_Publish(struct mqttClient_t *client, uint8_t *topic, uint8_t *msgBuf, uint32_t msgLen)
{
  int32_t rc = -1;
  int32_t len;
  uint8_t buf[200];

  // len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */



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
  int32_t rc;
  int32_t len;
  unsigned char buf1[200];
  char buf2[20];
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  MQTTTransport transport;

  rc = connect(client->sock, client->addri->ai_addr, client->addri->ai_addrlen);

  if (rc != 0)
  {
    MQTTCLIENT_LOGE("Network connection failed\n");
    goto exit;
  }

  MQTTCLIENT_LOGI("Network connection OK\n");

  itoa(esp_random(), buf2, 10);
  data.clientID.cstring = buf2;
  data.keepAliveInterval = 15;
  data.cleansession = 1;
  len = MQTTSerialize_connect(buf1, sizeof(buf1), &data);


  rc = write(client->sock, buf1, len);
  if (rc == -1)
  {
    MQTTCLIENT_LOGE("Write socket failed, return code %d\n", rc);
    goto exit;
  }

  transport.sck = &client->sock;
  transport.getfn = MQTTClient_ReadSock;
  transport.state = 0;

  //@TODO: should have timeout here
  while ((rc = MQTTPacket_readnb(buf1, sizeof(buf1), &transport)) != CONNACK)
  {
    MQTTCLIENT_LOGI("Wait CONNACK: %d\n", rc);
  }

  uint8_t sessionPresent, connAckRc;
  
  if (MQTTDeserialize_connack(&sessionPresent, &connAckRc, buf1, sizeof(buf1)) != 1 || connAckRc != 0)
  {
    MQTTCLIENT_LOGE("Unable to connect, return code %d\n", connAckRc);
    goto exit;
  }

  MQTTCLIENT_LOGI("Successfully connected to MQTT Broker\n");

exit:
  return rc;
}

static int32_t MQTTClient_ReadSock(void *sock, unsigned char *buf, int32_t len)
{
  return read(*((int32_t*)sock), buf, len);
}
