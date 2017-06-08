#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

typedef struct network_t
{
  // unsigned char *hostString;
  // int port;
  struct addrinfo *addri;
  int socket;
  
  int (*nconnect)(struct network_t *net);
  int (*nread)(struct network_t *net, unsigned char *buf, int bufLen);
  int (*nwrite)(struct network_t *net, unsigned char *buf, int bufLen);
}network_t;

int Network_Init(network_t *net, char *hostString, int port);
int Network_Deinit(network_t *net);

#endif /* __NETWORK_H__ */
