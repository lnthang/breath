#include "event_center.h"

EventGroupHandle_t EC_EventGroup;

void event_center_init(void)
{
	EC_EventGroup = xEventGroupCreate();
}
