#include "application.h"

SYSTEM_MODE(AUTOMATIC);

int LED = D0;                         // LED is connected to D1

void setup() {
    pinMode(LED, OUTPUT);               // sets pin as output
}

void loop() {
  if (Particle.connected() == false) {
    Particle.connect();
  }
  digitalWrite(LED, HIGH);          // sets the LED on
  delay(1000);                       // waits for 200mS
  digitalWrite(LED, LOW);           // sets the LED off
  delay(1000);                       // waits for 200mS
}
