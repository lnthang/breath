#include <stdio.h>
#include "freertos/FreeRTOS.h"

#include "mqttclient_mq135.h"
#include "event_center.h"

void mqttclient_mq135_task(void *vparams)
{
	xEventGroupWaitBits(EC_EventGroup, EC_EVENT_GOT_IP_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

	printf("Device is connected\n");
	while(1)
	{
		vTaskDelay(2000/portTICK_PERIOD_MS);
		printf("In mqttclient_mq135_task\n");
	}
}