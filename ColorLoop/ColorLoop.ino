#pragma once

#include <MeOrion.h>

MeRGBLed led(PORT_6, SLOT_1, 15);

void setup() {}

void loop() {
    static float r = 0.0f, g = 0.0f, b = 0.0f;

    for (uint8_t i = 1; i < 15; i++)
        led.setColorAt(i, 64 * (1 + sin(i / 2.0 + r / 4.0)),
                       64 * (1 + sin(i / 1.0 + g / 9.0 + 2.1)),
                       64 * (1 + sin(i / 3.0 + b / 14.0 + 4.2)));

    led.show();
    r += random(1, 6) / 6.0;
    g += random(1, 6) / 6.0;
    b += random(1, 6) / 6.0;
}
