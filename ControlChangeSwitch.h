#pragma once

#include <Arduino.h>

enum class SwitchEvent : uint8_t { None, Activated, Deactivated };

struct CCMessageAddress {
    int channel;
    int message;
};

class ControlChangeSwitch {
public:
  ControlChangeSwitch(int pin, CCMessageAddress valueMessage, bool inverted = false);
  ControlChangeSwitch(int pin, CCMessageAddress valueMessage, CCMessageAddress onOffMessage, bool inverted = false);

  void setup();
  SwitchEvent updateState();
  bool isActive() const;
  byte mapValue(byte midiValue) const;
  byte getOffValue() const;

  void sendValue(byte midiValue) const;
  void sendRawValue(byte value) const;
  void handleStateChange(byte currentMidiValue);

  static void sendActiveValues(ControlChangeSwitch* switches, byte count, byte midiValue);

private:
  void sendOnOffState(bool on) const;

  int pin;
  CCMessageAddress valueMessage;
  CCMessageAddress onOffMessage;
  bool hasOnOffMessage = false;
  bool inverted;
  bool active = false;
};
