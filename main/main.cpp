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

MQ135 gasSensorMQ135_0 = MQ135(ADC1_CHANNEL_0);

#define DHTPIN2            GPIO_NUM_17         // Pin which is connected to the DHT sensor.
#define DHTTYPE            DHT22     // DHT 22 (AM2302)

#ifndef HIGH
    #define HIGH 1
#endif
#ifndef LOW
    #define LOW 0
#endif

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview
DHT_Unified dht2(DHTPIN2, DHTTYPE);

const char *SENSORTAG = "SENSORS";

static float t,h,rawADC,estPPM;
static void readSensor(float *t, float *h, float *rawADC, float *estPPM)
{
	// Get temperature event and print its value.
		sensors_event_t event;  
		dht2.temperature().getEvent(&event);

		if (!isnan(event.temperature)) 
		{
			*t = event.temperature;
		}

		// Get humidity event and print its value.
		dht2.humidity().getEvent(&event);
		if (!isnan(event.relative_humidity)) 
		{
			*h = event.relative_humidity;
		}

		*rawADC = gasSensorMQ135_0.getRawADC();
		*estPPM = gasSensorMQ135_0.getPPM();

		printf("The adc1 value: %f %f %f %f %d \n", *t, *h, *rawADC, *estPPM, (uint32_t) (clock() * 1000 / CLOCKS_PER_SEC));
}

void adc1task(void* arg)
{
    while(1)
    {
		readSensor(&t, &h, &rawADC, &estPPM);

        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

extern "C" void app_main()
{
	dht2.begin();
	xTaskCreate(adc1task, "adc1task", 1024*3, NULL, 10, NULL);
}

