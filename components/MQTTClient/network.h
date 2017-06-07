#ifndef __NETWORK_H__
#define __NETWORK_H__

typedef struct
{
  unsigned char *hostString;
  int port;
  int socket;
  
  int (*connect)(network *net);
  int (*read)(network *net, unsigned char *buf, int bufLen);
  int (*write)(network *net, unsigned char *buf, int bufLen);
}network;

int Network_Init(network *net, unsigned char **p_hostString, int port);
int Network_Deinit(network *net);

#endif /* __NETWORK_H__ */