#include "MidiInOut.h"
#include <MIDIUSB.h>

void setupMIDI() {
  Serial.begin(115200);
}
/*
void midiLoop() {
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
      MidiUSB.sendMIDI(rx);
      MidiUSB.flush();
    }
  } while (rx.header != 0);
}
*/
void sendMidiControlChange(int channel, int control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}
