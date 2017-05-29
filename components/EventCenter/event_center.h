#ifndef __EVENT_CENTER_H__
#define __EVENT_CENTER_H__

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define EC_EVENT_GOT_IP_BIT		(1UL << 0UL)

extern EventGroupHandle_t EC_EventGroup;

void event_center_init(void);

#endif	/* __EVENT_CENTER_H__ */
