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
	/**
	 * If the user changes the 'state_open' field, we disable the frequency
	 * so that the next time this function is entered, it does not turn on
	 * the relay, since 'state_open == _real_state'
	 */
	if (RELAY_CHANGED(relay)) {
		relay->use_freq = false;
		return set_relay(relay, relay->state_open);
	}

	if (relay->use_freq) {
		uint32_t diff;

		if ((relay->state_open && (relay->freq_close == 0)) ||
		   (!relay->state_open && (relay->freq_open == 0)))
			return;

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
