#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "mqttclient_mq135.h"
#include "event_center.h"
#include "MQTTPacket.h"

#define MQTT_SERVER "iot.eclipse.org"
#define MQTT_PORT 1883
#define MQTT_PORT_STRING "1883"

void mqttclient_mq135_task(void *vparams)
{

  const struct addrinfo hints = {
    .ai_family = AF_INET,
    .ai_socktype = SOCK_STREAM,
  };
  
  struct addrinfo *res;
  struct in_addr *addr;


  uint32_t len = 0;
  int rc = 0;
  unsigned char buf[200];
  char* payload = "My Testing Payload - TLE";
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  MQTTString topicString = MQTTString_initializer;
  
  int sock;
  
  int payloadlen = strlen(payload);
  int buflen = sizeof(buf);
  int r;
  
	xEventGroupWaitBits(EC_EventGroup, EC_EVENT_GOT_IP_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

	printf("Device is connected\n");
	while(1)
	{
		vTaskDelay(2000/portTICK_PERIOD_MS);
		printf("In mqttclient_mq135_task\n");
    
    data.clientID.cstring = "me";
    data.keepAliveInterval = 20;
    data.cleansession = 1;
    len = MQTTSerialize_connect(buf, buflen, &data); /* 1 */
    
    topicString.cstring = "mytopic";
    len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, (unsigned char *) payload, payloadlen); /* 2 */
    len += MQTTSerialize_disconnect(buf + len, buflen - len); /* 3 */
    


    r = getaddrinfo(MQTT_SERVER, MQTT_PORT_STRING, &hints, &res);
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;

    printf("Address : %x\n", addr->s_addr);
    sock = socket(res->ai_family, res->ai_socktype, 0);
    
    if (connect(sock, res->ai_addr, res->ai_addrlen) != 0)
    {
      printf("Something wrong\n");
    }
    else
    {
      printf("Connected to iot.eclipse.org:1883\n");
    }

    //rc = Socket_new("127.0.0.1", 1883, &mysock);
    rc = write(sock, buf, len);
    rc = close(sock);
	}
}
