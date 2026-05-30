#include <Arduino.h>

#include "my_relays.h"

#define RELAYS_COUNT 2


struct relay my_relays[RELAYS_COUNT];

static int change_relay(const struct relay *relay, bool new_state_open)
{
	Serial.println("change_relay()");

	if (new_state_open)
		digitalWrite(relay->gpio, HIGH);
	else
		digitalWrite(relay->gpio, LOW);
}

int my_relays_init(struct relays_service *service)
{
	service->relays = my_relays;
	service->relays_count = RELAYS_COUNT;

	relays_lib_init(service);

	service->relays[0].gpio = 13;
	service->relays[1].gpio = 12;

	pinMode(service->relays[0].gpio, OUTPUT);
	pinMode(service->relays[1].gpio, OUTPUT);

	service->relays[0].relay_change = change_relay;
	service->relays[1].relay_change = change_relay;

	service->relays[0].state_open = false;
	service->relays[1].state_open = false;

	service->relays[1].use_freq = true;
	service->relays[1].freq_open = 800;
	service->relays[1].freq_close = 800;



	Serial.println("Success setup relays");

	return 0;
}

int my_relays_refresh(struct relays_service *service)
{
	/*
	Serial.print("Relay 0 state: ");
	Serial.println(service->relays[0].state_open);
	Serial.print("Relay 1 state: ");
	Serial.println(service->relays[1].state_open);
	*/
	return relays_lib_refresh(service);
}
