/*
 * HC-SR04 -> MIDI CC
 */

#include "HcSr04.h"
#include "ControlChangeSwitch.h"
#include "MidiInOut.h"

// Arduino Micro: echo must be on 0, 1, 2, 3 or 7 (pin 2 = switch)
const int trigPin = 9;
const int echoPin = 3;

const byte switchCount = 1;

ControlChangeSwitch switches[switchCount] = {
  { 2, { 1, 70 } },
};

HcSr04 sensor(trigPin, echoPin);

const float smoothing = 0.25f;
float smoothMidiValue = 0.0f;

byte currentMidiValue = 0;

const unsigned long switchPollIntervalMs = 20;
unsigned long lastSwitchPollMs = 0;

void pollSwitches() {
  const unsigned long nowMs = millis();
  if (nowMs - lastSwitchPollMs < switchPollIntervalMs) {
    return;
  }

  lastSwitchPollMs = nowMs;

  for (byte i = 0; i < switchCount; i++) {
    switches[i].handleStateChange(currentMidiValue);
  }
}

void setup() {

  for (byte i = 0; i < switchCount; i++) {
    switches[i].setup();
  }

  setupMIDI();
  sensor.begin();
}

void loop() {
  //midiLoop();
  pollSwitches();

  sensor.update();

  if (!sensor.hasNewReading()) {
    return;
  }

  const float distance = sensor.consumeDistanceCm();

  float distanceRatio = (distance - 2.0f) / 200.0f;
  distanceRatio = constrain(distanceRatio, 0.0f, 1.0f);

  float targetMidiValue = distanceRatio * 500.0f - 4.0f;
  targetMidiValue = constrain(targetMidiValue, 0.0f, 127.0f);

  smoothMidiValue += (targetMidiValue - smoothMidiValue) * smoothing;

  const byte midiValue = round(smoothMidiValue);

  if (midiValue == currentMidiValue) {
    return;
  }

  currentMidiValue = midiValue;
  ControlChangeSwitch::sendActiveValues(switches, switchCount, currentMidiValue);
}
