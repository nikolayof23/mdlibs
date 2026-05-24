#ifndef RELAYS_LIB_H
#define RELAYS_LIB_H

#include "mdlibs.h"


/*
 * @state_open: true - relay open, false - relay close; def close
 * @gpio: pin, macro, what you need; < 0 means relay is not used; def -1
 * @timer: to find out how long a relay is in @state_open, run:
 *		millis() - @timer				def 0
 *
 * @use_freq: use or not frequency (@freq_open, @freq_close), ignore @state_open; def false
 * @freq_open: ms, how many times the relay will be OPEN; def 0
 * @freq_close: ms, how many times the relay will be CLOSED; def 0
 *
 * @relay_change: function to change relay state
 *		if relay_change(relay, true) -- open relay
 *		if relay_change(relay, false) -- close relay
 */
struct relay {
	bool state_open;	/* true - open, false - close */
	int16_t gpio;		/* aka pin; < 0 mean unset */
	uint32_t timer;		/* the millis() value when @state_open was changed */

	/* frequency */
	bool use_freq;		/* true - use @freq_open and @freq_close, ignore @state_open */
	uint32_t freq_open;	/* ms */
	uint32_t freq_close;	/* ms */

	/* functions */
	void (*relay_change)(const struct relay *relay, bool new_state_open);

	/* private data, do not use it */
	bool _real_state;
	uint32_t _change_timer;
};

struct relays_service {
	struct relay *relays;

	uint16_t relays_count;
};


int relays_lib_init(struct relays_service *service);
int relays_lib_refresh_relay(struct relay *relay);		/* one relay */
int relays_lib_refresh(struct relays_service *service);		/* all relays in service */

#endif /* RELAYS_LIB_H */
