#include "ControlChangeSwitch.h"
#include "MidiInOut.h"

ControlChangeSwitch::ControlChangeSwitch(int pin, CCMessageAddress valueMessage, bool inverted)
  : pin(pin), valueMessage(valueMessage), inverted(inverted) {
}

ControlChangeSwitch::ControlChangeSwitch(
  int pin,
  CCMessageAddress valueMessage,
  CCMessageAddress onOffMessage,
  bool inverted)
  : pin(pin),
    valueMessage(valueMessage),
    onOffMessage(onOffMessage),
    hasOnOffMessage(true),
    inverted(inverted) {
}

void ControlChangeSwitch::setup() {
  pinMode(pin, INPUT_PULLUP);
}

SwitchEvent ControlChangeSwitch::updateState() {
  const bool wasActive = active;
  active = digitalRead(pin) == LOW;

  if (active && !wasActive) {
    return SwitchEvent::Activated;
  }

  if (!active && wasActive) {
    return SwitchEvent::Deactivated;
  }

  return SwitchEvent::None;
}

bool ControlChangeSwitch::isActive() const {
  return active;
}

byte ControlChangeSwitch::mapValue(byte midiValue) const {
  return inverted ? midiValue : (127 - midiValue);
}

byte ControlChangeSwitch::getOffValue() const {
  return inverted ? 127 : 0;
}

void ControlChangeSwitch::sendValue(byte midiValue) const {
  sendMidiControlChange(valueMessage.channel, valueMessage.message, mapValue(midiValue));
}

void ControlChangeSwitch::sendRawValue(byte value) const {
  sendMidiControlChange(valueMessage.channel, valueMessage.message, value);
}

void ControlChangeSwitch::sendOnOffState(bool on) const {
  if (!hasOnOffMessage) {
    return;
  }

  sendMidiControlChange(onOffMessage.channel, onOffMessage.message, on ? 127 : 0);
}

void ControlChangeSwitch::handleStateChange(byte currentMidiValue) {
  switch (updateState()) {
    case SwitchEvent::Activated:
      sendValue(currentMidiValue);
      sendOnOffState(true);
      break;
    case SwitchEvent::Deactivated:
      sendRawValue(getOffValue());
      sendOnOffState(false);
      break;
    default:
      break;
  }
}

void ControlChangeSwitch::sendActiveValues(
  ControlChangeSwitch* switches,
  byte count,
  byte midiValue) {
  for (byte i = 0; i < count; i++) {
    if (switches[i].isActive()) {
      switches[i].sendValue(midiValue);
    }
  }
}
