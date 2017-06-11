#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include <stdint.h>

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#define MQTT_CLIENT_MAX_SUB_TOPIC   5

typedef int32_t (*mqttClientMsgHandler_t)(uint8_t *topic, uint8_t *msgBuf, uint32_t msgLen);

typedef struct mqttClient_t {
  struct addrinfo *addri;
  mqttClientMsgHandler_t pHandler[MQTT_CLIENT_MAX_SUB_TOPIC];
  char *subTopic[MQTT_CLIENT_MAX_SUB_TOPIC];
  uint8_t numSubTopic;

  int sock;
}mqttClient_t;

// typedef struct mqttClient_t mqttClient_t;

int32_t MQTTClient_Init(/*struct mqttClient_t *client, char *host, int32_t port */);
int32_t MQTTClient_Create(char *host, int32_t port);
int32_t MQTTClient_Publish(struct mqttClient_t *client, uint8_t *topic, uint8_t *msgBuf, uint32_t msgLen);
int32_t MQTTClient_Subscribe(struct mqttClient_t *client, uint8_t *topic, mqttClientMsgHandler_t callback);
int32_t MQTTClient_Loop(struct mqttClient_t *client);

#endif /* __MQTT_CLIENT_H__ */
