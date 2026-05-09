#ifndef RELAYS_LIB_H
#define RELAYS_LIB_H

#include <stdint.h>


struct relay {
	bool open;			/* true - open, false - close */
	int16_t gpio;			/* aka pin; < 0 mean unset */
	uint32_t timer;			/* the millis() value when @state was changed */
	int16_t frequency;		/* freq < 0 -- do not use it and @freq_period */
	uint16_t freq_period;
};

#define RELAYS_DEFAULT_OPEN false	/* off */
#define RELAYS_DEFAULT_GPIO -1		/* unset */
#define RELAYS_DEFAULT_TIMER 0
#define RELAYS_DEFAULT_FREQ -1		/* off */
#define RELAYS_DEFAULT_FREQ_PERIOD 1000	/* 1s */


struct relays_service {
	struct relay *relays;

	uint16_t relays_count;
};


int relays_lib_init(struct relays_service *service);

#endif /* RELAYS_LIB_H */
