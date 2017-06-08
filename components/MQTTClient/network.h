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
  
  int (*connect)(struct network_t *net);
  int (*read)(struct network_t *net, unsigned char *buf, int bufLen);
  int (*write)(struct network_t *net, unsigned char *buf, int bufLen);
}network_t;

int Network_Init(network_t *net, char *hostString, int port);
int Network_Deinit(network_t *net);

#endif /* __NETWORK_H__ */
