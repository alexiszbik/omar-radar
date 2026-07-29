#include "HcSr04.h"

HcSr04* HcSr04::activeInstance = nullptr;

HcSr04::HcSr04(uint8_t trigPin, uint8_t echoPin)
  : trigPin(trigPin), echoPin(echoPin) {
}

void HcSr04::begin() {
  activeInstance = this;
  ready = false;

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(trigPin, LOW);

  const int echoInterrupt = digitalPinToInterrupt(echoPin);
  if (echoInterrupt == NOT_AN_INTERRUPT) {
    return;
  }

  attachInterrupt(echoInterrupt, echoIsr, CHANGE);
  ready = true;
}

bool HcSr04::isReady() const {
  return ready;
}

void HcSr04::echoIsr() {
  if (activeInstance != nullptr) {
    activeInstance->onEchoInterrupt();
  }
}

void HcSr04::onEchoInterrupt() {
  if (digitalRead(echoPin) == HIGH) {
    echoStartUs = micros();
  } else {
    echoDurationUs = micros() - echoStartUs;
    echoReady = true;
  }
}

void HcSr04::update() {
  const unsigned long nowUs = micros();
  const unsigned long nowMs = millis();

  switch (state) {
    case State::Idle:
      if (nowMs - lastMeasureMs >= MEASURE_INTERVAL_MS) {
        digitalWrite(trigPin, LOW);
        stateStartUs = nowUs;
        state = State::TriggerLow;
      }
      break;

    case State::TriggerLow:
      if (nowUs - stateStartUs >= 2) {
        digitalWrite(trigPin, HIGH);
        stateStartUs = nowUs;
        state = State::TriggerHigh;
      }
      break;

    case State::TriggerHigh:
      if (nowUs - stateStartUs >= 10) {
        digitalWrite(trigPin, LOW);
        echoReady = false;
        stateStartUs = nowUs;
        state = State::WaitEcho;
      }
      break;

    case State::WaitEcho:
      if (echoReady) {
        unsigned long durationUs;

        noInterrupts();
        durationUs = echoDurationUs;
        echoReady = false;
        interrupts();

        distanceCm = (durationUs * 0.0343f) / 2.0f;
        newReading = true;
        lastMeasureMs = nowMs;
        state = State::Idle;
      } else if (nowUs - stateStartUs >= ECHO_TIMEOUT_US) {
        lastMeasureMs = nowMs;
        state = State::Idle;
      }
      break;
  }
}

bool HcSr04::hasNewReading() const {
  return newReading;
}

float HcSr04::consumeDistanceCm() {
  newReading = false;
  return distanceCm;
}
