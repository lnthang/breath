#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "esp_system.h"
#include "esp_log.h"

#include "NetworkESP32.h"

static const char *TAG = "Network";

static int NetworkRead(struct Network *net, unsigned char *buf, int bufLen, int timeout);
static int NetworkWrite(struct Network *net, unsigned char *buf, int bufLen, int timeout);

void NetworkInit(Network *net)
{
  net->socket = -1;
  net->isConnect = -1;

  net->mqttread = NetworkRead;
  net->mqttwrite = NetworkWrite;
}

int NetworkConnect(Network *net, char *host, int port)
{
  int rc = -1;
  char buf[10];
  struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
  struct addrinfo *servinfo;

  /* Find host information for connection */
  (void)itoa(port, buf, sizeof(buf)/sizeof(buf[0]));
  rc = getaddrinfo(host, buf, &hints, &servinfo);

  if (rc != 0)
  {
    /* Something wrong exit with return code */
    ESP_LOGI(TAG, "Cannot find host info");
    goto exit;
  }

  /* Create a socket */
  net->socket = socket(servinfo->ai_family, servinfo->ai_socktype, 0);
  if (net->socket == -1)
  {
    ESP_LOGI(TAG, "Cannot create socket");
    goto exit;
  }

  ESP_LOGI(TAG, "Socket created : %d", net->socket);

  rc = connect(net->socket, servinfo->ai_addr, servinfo->ai_addrlen);

  if (rc != 0)
  {
    ESP_LOGI(TAG, "Network connection failed");
    goto exit;
  }

  ESP_LOGI(TAG, "Network connection OK");

exit:
  freeaddrinfo(servinfo);
  return rc;
}

void NetworkDisconnect(Network *net)
{
  if (net->socket != -1)
  {
    close(net->socket);
    ESP_LOGI(TAG, "Socket %d is closed", net->socket);
    net->socket = -1;
  }
}

int NetworkSub(Network *net)
{
  return 0;
}

int NetworkYield(Network *net)
{
  return 0;
}

static int NetworkRead(struct Network *net, unsigned char *buf, int bufLen, int timeout)
{
  int rc = -1;

  //@TODO: Handle timeout if necessary
  if (net->socket != -1)
  {
    rc = read(net->socket, buf, bufLen);
  }

  return rc;
}

static int NetworkWrite(struct Network *net, unsigned char *buf, int bufLen, int timeout)
{
  int rc = -1;

  //@TODO: Handle timeout if necessary
  if (net->socket != -1)
  {
    rc = write(net->socket, buf, bufLen);
  }

  return rc;
}
