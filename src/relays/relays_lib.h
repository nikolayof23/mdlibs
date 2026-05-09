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


struct relays_service {
	struct relay *relays;

	uint16_t relays_count;
};


#endif /* RELAYS_LIB_H */
