#include <stdio.h>

#include "network.h"

static int nw_connect(network_t *net)
{
  int rc = 0;

  return rc;
}

static int nw_read(network_t *net, unsigned char *buf, int bufLen)
{
  int rc = 0;
  rc = read(net->socket, buf, bufLen);
  return rc;
}

static int nw_write(network_t *net, unsigned char *buf, int bufLen)
{
  int rc = 0;
  rc = write(net->socket, buf, bufLen);
  return rc;
}

int Network_Init(network_t *net, char *hostString, int port)
{
  int rc;
  char buf[10];

  int type = SOCK_STREAM;
  struct in_addr *addr;
  // struct addrinfo *result = NULL;
  struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
  sa_family_t family = AF_INET;

  itoa(port, buf, sizeof(buf)/sizeof(buf[0]));

  rc = getaddrinfo(hostString, buf, &hints, &net->addri);

  if (rc != 0)
  {
    printf("Error code: %d\n", rc);
    goto exit;
  }

  addr = &((struct sockaddr_in *)net->addri->ai_addr)->sin_addr;
  printf("Address : %x\n", addr->s_addr);

  // net->hostString = *p_hostString;
  // net->port = port;
  net->socket = 0;
  net->connect = nw_connect;
  net->read = nw_read;
  net->write = nw_write;

exit:
  return rc;
}

int Network_Deinit(network_t *net)
{
  int rc;

  // net->hostString = NULL;
  // net->port = 0;
  net->connect = NULL;
  net->read = NULL;
  net->write = NULL;

  rc = shutdown(net->socket, SHUT_WR);
  rc = recv(net->socket, NULL, (size_t)0, 0);
  rc = close(net->socket);

  return 0;
}
