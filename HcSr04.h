#pragma once

#include <Arduino.h>

class HcSr04 {
public:
  HcSr04(uint8_t trigPin, uint8_t echoPin);

  void begin();
  bool isReady() const;
  void update();

  bool hasNewReading() const;
  float consumeDistanceCm();

private:
  enum class State : uint8_t { Idle, TriggerLow, TriggerHigh, WaitEcho };

  void onEchoInterrupt();
  static void echoIsr();

  uint8_t trigPin;
  uint8_t echoPin;

  State state = State::Idle;
  unsigned long stateStartUs = 0;
  unsigned long lastMeasureMs = 0;

  volatile unsigned long echoStartUs = 0;
  volatile unsigned long echoDurationUs = 0;
  volatile bool echoReady = false;

  float distanceCm = 0.0f;
  bool newReading = false;
  bool ready = false;

  static const unsigned long MEASURE_INTERVAL_MS = 20;
  static const unsigned long ECHO_TIMEOUT_US = 30000;

  static HcSr04* activeInstance;
};
