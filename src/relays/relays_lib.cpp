#include "relays/relays_lib.h"

#define RELAY_CHANGED(rel) ((rel)->state_open != (rel)->_real_state)

int relays_lib_init(struct relays_service *service)
{
	for (int i = 0; i < service->relays_count; i++) {
		struct relay *relay = &service->relays[i];

		relay->state_open = false;
		relay->_real_state = false;
		relay->gpio = -1;
		relay->timer = 0;
		relay->use_freq = false;
		relay->freq_open = 0;
		relay->freq_close = 0;
	}

	return 0;
}

static int set_relay(struct relay *relay, bool state)
{
	relay->relay_change(relay, state);
	relay->timer = millis();
	relay->state_open = state;
	relay->_real_state = state;

	return 0;
}

int relays_lib_refresh_relay(struct relay *relay)
{
	if (!relay->use_freq && RELAY_CHANGED(relay)) {
		return set_relay(relay, relay->state_open);
	} else if (relay->use_freq) {
		uint32_t diff;

		diff = millis() - relay->timer;
		if (relay->state_open && (diff >= relay->freq_open))
			return set_relay(relay, false);
		else if (!relay->state_open && (diff >= relay->freq_close))
			return set_relay(relay, true);
	}

	return 0;
}

int relays_lib_refresh(struct relays_service *service)
{
	int ret = 0;

	for (int i = 0; i < service->relays_count; i++) {
		ret = relays_lib_refresh_relay(&service->relays[i]);
		if (ret)
			return ret;
	}

	return ret;
}
