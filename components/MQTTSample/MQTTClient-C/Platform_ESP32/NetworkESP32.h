#ifndef __NETWORK_ESP32_H__
#define __NETWORK_ESP32_H__

#include <stdint.h>
#include <stdio.h>

struct Network
{
  int socket;
  int isConnect;

  int (*mqttread)(struct Network*, unsigned char*, int, int);
  int (*mqttwrite)(struct Network*, unsigned char*, int, int);
};

typedef struct Network Network;

void NetworkInit(Network*);
int NetworkConnect(Network*, char*, int);
void NetworkDisconnect(Network*);
int NetworkSub(Network*);
int NetworkYield(Network*);

#endif
