#include <Arduino.h>
#include "ESP32_OTA.h"

void read() {

}

void execute() {

}


void write() {

}


void setup() {
    Serial.begin(115200);
    Serial.println("Booting");
    // low-rise house workshop (낮은집 작업실)
    setupWPS_OTA("LRH_Workshop");

}

void loop() {
    read();
    delay(1);
    execute();
    delay(1);
    write();
    delay(1);
}
