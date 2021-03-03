#include <Arduino.h>
#include "../lib/ota/ESP32_OTA.h"


void setup() {
    Serial.begin(115200);
    Serial.println("Booting");
    // change 'nameprefix' to unique name as you wish.
    setupWPS_OTA("LRH_Workshop");

    // your 'setup code' here.
}

void loop() {
    // your code here.
}
