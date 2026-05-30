#include "my_relays.h"
#include "relays/relays_lib.h"


struct system_service {
  struct relays_service relays_priv;
} my_system;


void setup() {
  int ret;

  Serial.begin(9600);

  ret = my_relays_init(&my_system.relays_priv);
  if (ret)
    goto err;


  return;

err:
  for (;;)
    ;
}

void loop() {
  my_relays_refresh(&my_system.relays_priv);
}
