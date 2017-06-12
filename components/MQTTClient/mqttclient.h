#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include <stdint.h>

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

// #define CONFIG_MQTT_MAX_CLIENT                5
// #define CONFIG_MQTT_MAX_SUB_TOPIC_PER_CLIENT  5

#define MQTT_MAX_CLIENT                   CONFIG_MQTT_MAX_CLIENT
#define MQTT_MAX_SUB_TOPIC_PER_CLIENT     CONFIG_MQTT_MAX_SUB_TOPIC_PER_CLIENT

typedef int32_t (*mqttClientMsgHandler_t)(char *topic, char *msgBuf, uint32_t msgLen);

int32_t MQTTClient_Init(void);
int32_t MQTTClient_Create(char *host, int32_t port);
int32_t MQTTClient_Release(int32_t clientId);

int32_t MQTTClient_Publish(uint8_t clientId, char *topic, char *msgBuf, uint32_t msgLen);
int32_t MQTTClient_Subscribe(uint8_t clientId, char *topic, mqttClientMsgHandler_t callback);
int32_t MQTTClient_Loop(void);

#endif /* __MQTT_CLIENT_H__ */
