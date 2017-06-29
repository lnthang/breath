#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <driver/gpio.h>
#include <driver/adc.h>
#include <time.h>
#include "sensors/include/DHT.h"
#include "sensors/include/DHT_U.h"
#include "sensors/include/MQ135.h"



#include <string.h>
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/base64.h"
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

MQ135 gasSensorMQ135_0 = MQ135(ADC1_CHANNEL_0);

#define DHTPIN2            17         // Pin which is connected to the DHT sensor.
#define DHTTYPE            DHT22     // DHT 22 (AM2302)

#ifndef HIGH
    #define HIGH 1
#endif
#ifndef LOW
    #define LOW 0
#endif

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview
DHT dht2(DHTPIN2, DHTTYPE);

const char *SENSORTAG = "SENSORS";

typedef struct DHT_data_t
{
	float temperature;
	float humidity;
	float dewPoint;
	float heatIndex;
	heatIndexClassification_t heatIndexClassification;
	
}DHT_Data;

void printDHT(DHT_Data data)
{
	printf("-- DHT %f %f %f %f %d\n", data.temperature, data.humidity, data.heatIndex, data.dewPoint, (int) data.heatIndexClassification);
}

typedef struct MQ135_data_t
{
	float rawADC;
	float estPPM;
	float estPPM_with_temp_hum;
	
}MQ135_Data;

void printMQ135(MQ135_Data data)
{
	printf("-- MQ135 %f %f %f\n", data.rawADC, data.estPPM, data.estPPM_with_temp_hum);
}

DHT_Data DHT_data;
MQ135_Data MQ135_data;


static bool readSensor(DHT_data_t* DHT_data, MQ135_data_t* MQ135_data)
{
		// float t, float h, float rawADC, float estPPM, float estPPM_with_temp_hum;

		// Get temperature 
		DHT_data->temperature = dht2.readTemperature();

		if (isnan(DHT_data->temperature))
		{
			DHT_data->temperature = -1.0f;
		}

		// Get humidity 
		DHT_data->humidity = dht2.readHumidity();
		if (isnan(DHT_data->humidity))
		{
			DHT_data->humidity = -1.0f;
		}

		MQ135_data->rawADC = gasSensorMQ135_0.getRawADC();
		MQ135_data->estPPM = gasSensorMQ135_0.getPPM();
		MQ135_data->estPPM_with_temp_hum = 0;

		if (DHT_data->temperature > 0.0f 
			&& DHT_data->temperature < 55.0f 
			&& DHT_data->humidity > 0.0f)
		{
			MQ135_data->estPPM_with_temp_hum = gasSensorMQ135_0.getCorrectedPPM(DHT_data->temperature, DHT_data->humidity);
			DHT_data->dewPoint  = dht2.computeDewPointInCelcius(DHT_data->temperature, DHT_data->humidity);
			DHT_data->heatIndex = dht2.computeHeatIndex(DHT_data->temperature,  DHT_data->humidity, false);
			DHT_data->heatIndexClassification = dht2.heatIndexClassification(DHT_data->heatIndex, false);
			return true;
		}
	return false;

		// printf("The adc1 value: %f %f %f %f %d \n", *t, *h, *rawADC, *estPPM, (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC));
}

void adc1task(void* arg)
{
	bool isReadSensorOk;
	isReadSensorOk = false;
    while (!isReadSensorOk)
    {
    	// DHT_data.temperature = -1;
    	// DHT_data.humidity = -1;
		readSensor(&DHT_data, &MQ135_data);

		printDHT(DHT_data);
		printMQ135(MQ135_data);
		// printf("The adc1 value: %f %f %f %f %f %d \n", t, h, rawADC, estPPM, estPPM_with_temp_hum, (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC));

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}








/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "api.thingspeak.com"
#define WEB_PORT "443"
#define WEB_URL "https://api.thingspeak.com/update?api_key=FG8XS7Q868L4MCYG"//GFW6TKN1F0G39LAO"//FG8XS7Q868L4MCYG

#define WEB_DELETE "https://api.thingspeak.com/channels/290009/feeds" //294713

static const char *TAG = "WIFI_HTTPS";

/* Root cert for howsmyssl.com, found in cert.c */
extern const char *server_root_cert;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
	    case SYSTEM_EVENT_STA_START:
	        esp_wifi_connect();
	        break;
	    case SYSTEM_EVENT_STA_GOT_IP:
	        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
	        break;
	    case SYSTEM_EVENT_STA_DISCONNECTED:
	        /* This is a workaround as ESP32 WiFi libs don't currently
	           auto-reassociate. */
	        esp_wifi_connect();
	        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
	        break;
	    default:
	        break;
    }
    return ESP_OK;
}



static void initialise_wifi(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    
    wifi_config_t wifi_config = 
    {
        .sta = {
        	// "Ngoc Duc",
        	// "08111989",
            "Hong_quan",
            "khongcocho",
			false
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    
    ESP_ERROR_CHECK( esp_wifi_start() );
    
}




static void https_get_task(void *pvParameters) {

    char buf[512];
    int ret, flags, len;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    ESP_LOGI(TAG, "Seeding the random number generator");

    mbedtls_ssl_config_init(&conf);

    mbedtls_entropy_init(&entropy);
    if((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        abort();
    }

    ESP_LOGI(TAG, "Loading the CA root certificate...");

    ret = mbedtls_x509_crt_parse(&cacert, (uint8_t*)server_root_cert, strlen(server_root_cert)+1);
    if(ret < 0)
    {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        abort();
    }

    ESP_LOGI(TAG, "Setting hostname for TLS session...");

     /* Hostname set here should match CN in server certificate */
    if((ret = mbedtls_ssl_set_hostname(&ssl, WEB_SERVER)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        abort();
    }

    ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");

    if((ret = mbedtls_ssl_config_defaults(&conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    /* MBEDTLS_SSL_VERIFY_OPTIONAL is bad for security, in this example it will print
       a warning if CA verification fails but it will continue to connect.
       You should consider using MBEDTLS_SSL_VERIFY_REQUIRED in your own code.
    */
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
// #ifdef MBEDTLS_DEBUG_C
//     mbedtls_debug_set_threshold(MBEDTLS_DEBUG_LEVEL);
//     mbedtls_ssl_conf_dbg(&conf, mbedtls_debug, NULL);
// #endif

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        goto exit;
    }

    while(1) 
    {
    	initialise_wifi();

        /* Wait for the callback to set the CONNECTED_BIT in the
           event group.
        */
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "Connected to AP");

        mbedtls_net_init(&server_fd);

        ESP_LOGI(TAG, "Connecting to %s:%s...", WEB_SERVER, WEB_PORT);

        if ((ret = mbedtls_net_connect(&server_fd, WEB_SERVER,
                                      WEB_PORT, MBEDTLS_NET_PROTO_TCP)) != 0)
        {
            ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
            goto exit;
        }

        ESP_LOGI(TAG, "Connected.");

        mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

        ESP_LOGI(TAG, "Performing the SSL/TLS handshake...");

        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
                goto exit;
            }
        }

        ESP_LOGI(TAG, "Verifying peer X.509 certificate...");

        if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0)
        {
            /* In real life, we probably want to close connection if ret != 0 */
            ESP_LOGW(TAG, "Failed to verify peer certificate!");
            bzero(buf, sizeof(buf));
            mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
            ESP_LOGW(TAG, "verification info: %s", buf);
        }
        else {
            ESP_LOGI(TAG, "Certificate verified.");
        }

        ESP_LOGI(TAG, "Writing HTTP request...");


		bool isReadSensorOk; isReadSensorOk = false;
		int count; count = 0;
		while (!isReadSensorOk && count < 100)
		{
			isReadSensorOk = readSensor(&DHT_data, &MQ135_data);
			vTaskDelay(100 / portTICK_RATE_MS);
			count++;
		}

		printDHT(DHT_data);
		printMQ135(MQ135_data);

    	// printf("The adc1 value: %f %f %f %f %f %d \n", t, h, rawADC, estPPM, estPPM_with_temp_hum, (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC));     
		

		if (isReadSensorOk)
		{	
			char reqbuf[512];

			sprintf(reqbuf,"GET %s&field1=%f&field2=%f&field3=%f&field4=%f&field5=%f&field6=%f&field7=%f \
							HTTP/1.1\n \
							Host: %s\n \
							User-Agent: esp-idf/1.0 esp32\n\n",
								WEB_URL, DHT_data.temperature, DHT_data.humidity, 
								MQ135_data.rawADC, MQ135_data.estPPM, MQ135_data.estPPM_with_temp_hum, 
								DHT_data.heatIndex, DHT_data.dewPoint, WEB_SERVER);

			ESP_LOGI(TAG, "req=[%s]",reqbuf);

			while((ret = mbedtls_ssl_write(&ssl, (const unsigned char *)reqbuf, strlen(reqbuf))) <= 0)
			{
				if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
				{
					ESP_LOGE(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
					goto exit;
				}
			}

			len = ret;
			ESP_LOGI(TAG, "%d bytes written", len);
			ESP_LOGI(TAG, "Reading HTTP response...");

			do
			{
				len = sizeof(buf) - 1;
				bzero(buf, sizeof(buf));
				ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf, len);

				if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
					continue;

				if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
					ret = 0;
					break;
				}

				if(ret < 0)
				{
					ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
					break;
				}

				if(ret == 0)
				{
					ESP_LOGI(TAG, "connection closed");
					break;
				}

				len = ret;
				ESP_LOGI(TAG, "%d bytes read", len);
				/* Print response directly to stdout as it is read */
				for(int i = 0; i < len; i++) {
					putchar(buf[i]);
				}
			} while(1);
        }
        mbedtls_ssl_close_notify(&ssl);

    exit:
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_free(&server_fd);

        if(ret != 0)
        {
            mbedtls_strerror(ret, buf, 100);
            ESP_LOGE(TAG, "Last error was: -0x%x - %s", -ret, buf);
        }


		ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_stop() );
        ESP_ERROR_CHECK( esp_wifi_deinit() );

        for(int countdown = 10; countdown >= 0; countdown--) {
        	
        	ESP_LOGI(TAG, "%d...", countdown);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        
        ESP_LOGI(TAG, "Starting again!");
    }
}



extern "C" void app_main()
{
	dht2.begin();
	// xTaskCreate(adc1task, "adc1task", 1024*3, NULL, 100, NULL);


	ESP_LOGI(TAG, "INITIALIZE WIFI\n")
	tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
   	
    xTaskCreate(&https_get_task, "https_get_task", 8192, NULL, 20, NULL);
}

