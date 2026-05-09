#include "relays/relays_lib.h"


int relays_lib_init(struct relays_service *service)
{
	for (int i = 0; i < service->relays_count; i++) {
		service->relays[i].open = RELAYS_DEFAULT_OPEN;
		service->relays[i].gpio = RELAYS_DEFAULT_GPIO;
		service->relays[i].timer = RELAYS_DEFAULT_TIMER;
		service->relays[i].frequency = RELAYS_DEFAULT_FREQ;
		service->relays[i].freq_period = RELAYS_DEFAULT_FREQ_PERIOD;
	}

	return 0;
}
