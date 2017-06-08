#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "mqttclient_mq135.h"
#include "event_center.h"
#include "MQTTPacket.h"
#include "network.h"

#define MQTT_SERVER "iot.eclipse.org"
#define MQTT_SERVER_IP "192.168.1.104"
#define MQTT_PORT 1883
#define MQTT_PORT_STRING "1883"

const struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};

int read_sock = 0;

static int read_socket_wrapper(unsigned char *buf, int count)
{
  return read(read_sock, buf, count);
}

void mqttclient_mq135_task(void *vparams)
{
  // const struct addrinfo hints = {
  //   .ai_family = AF_INET,
  //   .ai_socktype = SOCK_STREAM,
  // };
  
  // struct addrinfo *res;
  // struct in_addr *addr;


  // uint32_t len = 0;
  // // int rc = 0;
  // unsigned char buf[200];
  // char* payload = "My Testing Payload - TLE";
  // MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  // MQTTString topicString = MQTTString_initializer;
  
  // int sock;
  // int rc = 0;
  
  // int payloadlen = strlen(payload);
  // int buflen = sizeof(buf);
  // // int r;
  
  xEventGroupWaitBits(EC_EventGroup, EC_EVENT_GOT_IP_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

  // getaddrinfo(MQTT_SERVER, MQTT_PORT_STRING, &hints, &res);
  // addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;

  // printf("Address : %x\n", addr->s_addr);
  // sock = socket(res->ai_family, res->ai_socktype, 0);

  // // res->ai_addr = (struct sockaddr)htonl(MQTT_SERVER_IP);
  // inet_pton(AF_INET, MQTT_SERVER_IP, &((struct sockaddr_in *)res->ai_addr)->sin_addr);
  
  // if (connect(sock, res->ai_addr, res->ai_addrlen) != 0)
  // {
  //   printf("Something wrong\n");
  //   return;
  // }
  // else
  // {
  //   printf("Connected to iot.eclipse.org:1883\n");
  // }

  // printf("Device is connected\n");

  // data.clientID.cstring = "me";
  // data.keepAliveInterval = 15;
  // data.cleansession = 1;
  // len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
  // topicString.cstring = "topic";

  // // len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */

  // rc = write(sock, buf, len);
  // if (rc == -1)
  // {
  //   printf("%s\n", strerror(errno));
  // }

  // len = 0;

  // // len = MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, (unsigned char *) payload, payloadlen); /* 2 */
  // len = MQTTSerialize_pingreq(buf + len, buflen - len);

  network_t test_network;

  Network_Init(&test_network, MQTT_SERVER, MQTT_PORT);

  while(1)
  {
    vTaskDelay(10000/portTICK_PERIOD_MS);
    printf("In mqttclient_mq135_task\n");
    
    // data.clientID.cstring = "me";
    // data.keepAliveInterval = 60;
    // data.cleansession = 0;
    // len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
    
    // topicString.cstring = "mytopic";
    // len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
    // len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, (unsigned char *) payload, payloadlen); /* 2 */
    // len += MQTTSerialize_disconnect(buf + len, buflen - len); /* 3 */

    //rc = Socket_new("127.0.0.1", 1883, &mysock);
    // rc = write(sock, buf, len);
    // if (rc == -1)
    // {
    //   printf("%s\n", strerror(errno));
    // }

    // len = 0;
    // printf("Write RC : %d\n", rc);
    // close(sock);
  }
}

void mqttclient_mq135_sub_task(void *vparams)
{
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  MQTTString topicString = MQTTString_initializer;
  // MQTTTransport read_transport;

  struct addrinfo *res;
  // struct in_addr *addr;
  // uint32_t sock;

  uint32_t len;
  unsigned char buf[200];
  int req_qos = 0;

  xEventGroupWaitBits(EC_EventGroup, EC_EVENT_GOT_IP_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

  getaddrinfo(MQTT_SERVER, MQTT_PORT_STRING, &hints, &res);
  // addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;

  read_sock = socket(res->ai_family, res->ai_socktype, 0);
  
  // write(read_sock, buf, len);

  if (connect(read_sock, res->ai_addr, res->ai_addrlen) != 0)
  {
    printf("Cannot connect to host\n");
    return;
  }
  else
  {
    printf("Connected to Host\n");  
  }

  data.clientID.cstring = "mqtt_sub_task";
  data.keepAliveInterval = 60;
  data.cleansession = 1;

  // read_transport.sck = &read_sock;
  // read_transport.getfn = read_socket_wrapper;
  // read_transport.state = 0;

  len = MQTTSerialize_connect((unsigned char *)buf, sizeof(buf), &data);
  write(read_sock, buf, len);

  if (MQTTPacket_read(buf, sizeof(buf), read_socket_wrapper) == CONNACK)
  {
    unsigned char sessionPresent, connack_rc;

    if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, sizeof(buf)) != 1 || connack_rc != 0)
    {
      printf("Unable to connect, return code %d\n", connack_rc);
      // goto exit;
    }

    printf("sessionPresent : %d\n", sessionPresent);
  }

  topicString.cstring = "substopic";
  len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, 1, 1, &topicString, &req_qos);
  write(read_sock, buf, len);

  do {
    int frc;
    if ((frc=MQTTPacket_read(buf, sizeof(buf), read_socket_wrapper)) == SUBACK) /* wait for suback */
    {
      unsigned short submsgid;
      int subcount;
      int granted_qos;

      MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, sizeof(buf));
      if (granted_qos != 0)
      {
        printf("granted qos != 0, %d\n", granted_qos);
        // goto exit;
      }
      break;
    }
    else if (frc == -1)
      // goto exit;
      break;
    } while (1); /* handle timeouts here */


  while(1)
  {
    if (MQTTPacket_read(buf, sizeof(buf), read_socket_wrapper) == PUBLISH)
    {
      unsigned char dup;
      int qos;
      unsigned char retained;
      unsigned short msgid;
      int payloadlen_in;
      unsigned char* payload_in;
      // int rc;
      MQTTString receivedTopic;

      MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
          &payload_in, &payloadlen_in, buf, sizeof(buf));
      printf("message arrived %.*s\n", payloadlen_in, payload_in);
    }
  }
}
