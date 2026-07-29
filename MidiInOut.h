#pragma once

#include <Arduino.h>

void setupMIDI();
void midiLoop();
void sendMidiControlChange(int channel, int control, byte value);
