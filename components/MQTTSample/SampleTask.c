#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "freertos/FreeRTOS.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#include "esp_system.h"
#include "esp_log.h"

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include "MQTTClient.h"
#include "event_center.h"

/* Constants that aren't configurable in menuconfig */
// #define MQTT_SERVER "mqtt.thingspeak.com"
#define MQTT_SERVER       "192.168.10.158"
#define MQTT_SECURE_PORT  443
#define MQTT_PORT         1883
#define MQTT_BUF_SIZE     1000
#define MQTT_WEBSOCKET    1  // 0=no 1=yes

static unsigned char mqtt_sendBuf[MQTT_BUF_SIZE];
static unsigned char mqtt_readBuf[MQTT_BUF_SIZE];

static const char *TAG = "MQTASK";

extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_cert_pem_end");


void mqtt_task(void *pvParameters)
{
  int ret;
  Network network;
  MQTTClient client;

  xEventGroupWaitBits(EC_EventGroup, EC_EVENT_GOT_IP_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

  ESP_LOGI(TAG, "Connected to wifi\n");

  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_config conf;
  mbedtls_entropy_context entropy;
  mbedtls_net_context server_fd;


  mbedtls_ssl_init(&ssl);
  mbedtls_x509_crt_init(&cacert);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_ssl_config_init(&conf);
  mbedtls_entropy_init(&entropy);

  ESP_LOGI(TAG, "Seeding the random number generator");
  ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
  if(ret != 0)
  {
      ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
      abort();
  }

  ESP_LOGI(TAG, "Loading the CA root certificate...");
  ret = mbedtls_x509_crt_parse(&cacert, server_root_cert_pem_start, server_root_cert_pem_end - server_root_cert_pem_start);
  if(ret < 0)
  {
      ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
      abort();
  }

  ESP_LOGI(TAG, "Setting hostname for TLS session...");
  /* Hostname set here should match CN in server certificate */
  // if((ret = mbedtls_ssl_set_hostname(&ssl, WEB_SERVER)) != 0)
  {
    // ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
    // abort();
  }


  NetworkInit(&network);

  while(1) 
  {
    // /* Wait for the callback to set the CONNECTED_BIT in the
    //          event group.
    //       */
    //       xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
    //                           false, true, portMAX_DELAY);
    // ESP_LOGI(TAG, "Connected to AP");
    // ESP_LOGI(TAG, "Start MQTT Task ...");

    
    // NetworkInit(&network);
    // network.websocket = MQTT_WEBSOCKET;

//     // ESP_LOGI(TAG,"NetworkConnect %s:%d ...",MQTT_SERVER,MQTT_PORT);
    NetworkConnect(&network, MQTT_SERVER, MQTT_PORT);
//       // ESP_LOGI(TAG,"MQTTClientInit  ...");
    MQTTClientInit(&client, &network,
        2000,             // command_timeout_ms
        mqtt_sendBuf,     // sendbuf,
        MQTT_BUF_SIZE,    // sendbuf_size,
        mqtt_readBuf,     // readbuf,
        MQTT_BUF_SIZE     // readbuf_size
      );

    MQTTString clientId = MQTTString_initializer;
    clientId.cstring = "ESP32MQTT";

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID          = clientId;
    data.willFlag          = 0;
    data.MQTTVersion       = 4; // 3 = 3.1 4 = 3.1.1
    data.keepAliveInterval = 60;
    data.cleansession      = 1;

//     ESP_LOGI(TAG,"MQTTConnect  ...");
    ret = MQTTConnect(&client, &data);
    if (ret != SUCCESS) 
    {
      ESP_LOGI(TAG, "MQTTConnect not SUCCESS: %d", ret);
      goto exit;
    }

    char msgbuf[100];
    
    for (int i=0;i<5;i++) 
    {
      MQTTMessage message;
      sprintf(msgbuf, "field1=%d&field2=%lf",(uint8_t)(esp_random()&0xFF),(double)((esp_random()&0xFFFF)/10));

      ESP_LOGI(TAG, "MQTTPublish  ... %s",msgbuf);
      message.qos = QOS0;
      message.retained = false;
      message.dup = false;
      message.payload = (void*)msgbuf;
      message.payloadlen = strlen(msgbuf)+1;

      // ret = MQTTPublish(&client, "channels/<your chanel id>/publish/<your write API key>", &message);
      ret = MQTTPublish(&client, "channels", &message);
      if (ret != SUCCESS) {
        ESP_LOGI(TAG, "MQTTPublish not SUCCESS: %d", ret);
        goto exit;
      }
      for(int countdown = 30; countdown >= 0; countdown--)
      {
        if (countdown%10 == 0)
        {
          ESP_LOGI(TAG, "%d...", countdown);
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
      }
    }

exit:
    MQTTDisconnect(&client);

    // ESP_LOGI(TAG, "Connected to MQTT server\nDelay 2s\n");
    // vTaskDelay(2000 / portTICK_RATE_MS);
    // ESP_LOGI(TAG, "Disconnecting...\n");
    NetworkDisconnect(&network);
//     for(int countdown = 60; countdown >= 0; countdown--) 
//     {
//       if(countdown%10==0) 
//       {
//         ESP_LOGI(TAG, "%d...", countdown);
//       }
//       vTaskDelay(1000 / portTICK_RATE_MS);
//     }
    ESP_LOGI(TAG, "Starting again!");

    vTaskDelay(2000 / portTICK_RATE_MS);
  }
 }
