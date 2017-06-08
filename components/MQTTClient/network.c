#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "network.h"

static int nw_connect(network_t *net)
{
  int rc;

  if ((rc = connect(net->socket, net->addri->ai_addr, net->addri->ai_addrlen)) != 0)
  {
    printf("%s\n", strerror(errno));
  }

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
  int rc;
  rc = write(net->socket, buf, bufLen);

  if (rc == -1)
  {
    printf("%s\n", strerror(errno));
  }

  return rc;
}

int Network_Init(network_t *net, char *hostString, int port)
{
  int rc;
  char buf[10];

  struct in_addr *addr;
  // struct addrinfo *result = NULL;
  struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
  // sa_family_t family = AF_INET;

  itoa(port, buf, sizeof(buf)/sizeof(buf[0]));
  rc = getaddrinfo(hostString, buf, &hints, &net->addri);

  if (rc != 0)
  {
    printf("Error code: %d\n", rc);
    goto exit;
  }

  net->socket = socket(net->addri->ai_family, net->addri->ai_socktype, 0);

  if (net->socket == -1)
  {
    printf("Failed to create socket\n");
    printf("%s\n", strerror(errno));
    goto exit;
  }

  nw_connect(net);

  printf("Socket created with id : %d\n", net->socket);

  addr = &((struct sockaddr_in *)net->addri->ai_addr)->sin_addr;
  printf("Address : %x\n", addr->s_addr);

  // net->hostString = *p_hostString;
  // net->port = port;
  net->socket = 0;
  net->nconnect = nw_connect;
  net->nread = nw_read;
  net->nwrite = nw_write;

exit:
  return rc;
}

int Network_Deinit(network_t *net)
{
  int rc;

  // net->hostString = NULL;
  // net->port = 0;
  net->nconnect = NULL;
  net->nread = NULL;
  net->nwrite = NULL;

  rc = shutdown(net->socket, SHUT_WR);
  rc = recv(net->socket, NULL, (size_t)0, 0);
  rc = close(net->socket);

  return rc;
}
