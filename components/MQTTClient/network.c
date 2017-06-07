#include <stdio.h>

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "network.h"

static int nw_connect(network *net)
{
  int rc = 0;

  return rc;
}

static int nw_read(network *net, unsigned char *buf, int bufLen)
{
  int rc = 0;
  rc = read(net->socket, buf, bufLen);
  return rc;
}

static int nw_write(network *net, unsigned char *buf, int bufLen)
{
  int rc = 0;
  rc = write(net->socket, buf, bufLen);
  return rc;
}

int Network_Init(network *net, unsigned char **p_hostString, int port)
{
  int rc;

  int type = SOCK_STREAM;
  struct sockaddr_in address;
  struct addrinfo *result = NULL;
  struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
  sa_family_t family = AF_INET;

  

  net->hostString = *p_hostString;
  net->port = port;
  net->socket = 0;
  net->connect = nw_connect;
  net->read = nw_read;
  net->write = nw_write;

  rc = 

  return rc;
}

int Network_Deinit(network *net)
{
  int rc;

  net->hostString = NULL;
  net->port = 0;
  net->connect = NULL;
  net->read = NULL;
  net->write = NULL;

  rc = shutdown(net->socket, SHUT_WR);
  rc = recv(net->socket, NULL, (size_t)0, 0);
  rc = close(net->socket);

  return 0;
}
