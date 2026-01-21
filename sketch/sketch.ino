// pandamusrex-midi-to-cv
// D2: MIDI serial in (from optocoupler)
// D3: CV clock out

#include <SoftwareSerial.h>

#define MIDI_BAUD_RATE 31250

#define MIDI_RX_PIN 2
#define MIDI_TX_PIN 3 // Not used at this moment
#define MODULAR_CLK_OUT_PIN 4
#define MODULAR_RST_OUT_PIN 5


SoftwareSerial midiSerial = SoftwareSerial(MIDI_RX_PIN, MIDI_TX_PIN);

void setup() {
  Serial.begin(115200); // serial monitor, not MIDI
  Serial.print("Ready\r\n");

  pinMode(MIDI_RX_PIN, INPUT);
  pinMode(MIDI_TX_PIN, OUTPUT);
  pinMode(MODULAR_CLK_OUT_PIN, OUTPUT);
  pinMode(MODULAR_RST_OUT_PIN, OUTPUT);

  digitalWrite(MODULAR_CLK_OUT_PIN, LOW);
  digitalWrite(MODULAR_RST_OUT_PIN, LOW);

  midiSerial.begin(MIDI_BAUD_RATE);
}

int charAvailable = 0;
int charRead = 0;
int charCount = 0;

int measure = 1;
int beat = 1;
int sixteenth = 1; // aka subbeat kinda
int pulse = 0;



void loop() {
  charAvailable = midiSerial.available();
  if (charAvailable > 0) {
    charRead = midiSerial.read();
    switch (charRead) {
      case 0xFA:
        charCount = 0;
        Serial.print("\r\n\r\nSTART ");
        break;
      case 0xFC:
        Serial.print("STOP ");
        break;
      case 0xF8:
        // Serial.print("CLK ");

        if (sixteenth == 1) {
          if (beat == 1) { // Send RST on first beat of each measure
            if (pulse == 0) {
              digitalWrite(MODULAR_RST_OUT_PIN, HIGH);
            }
            if (pulse == 1) {
              digitalWrite(MODULAR_RST_OUT_PIN, LOW);
            }
          }

          // Send CLK on each beat
          if (pulse == 0) {
            digitalWrite(MODULAR_CLK_OUT_PIN, HIGH);
          }
          if (pulse == 1) {
            digitalWrite(MODULAR_CLK_OUT_PIN, LOW);
          }

        }

        pulse += 1;
        if (pulse >= 6) { // 6 pulses per sixteenth (i.e. 24 per quarter)
          pulse = 0;
          sixteenth += 1;
          if (sixteenth > 4) { // 4 sixteenths per beat (i.e. per quarter)
            sixteenth = 1;
            beat = beat + 1;
            if (beat > 4) { // we are assuming 4 beats per measure here https://ericjknapp.com/2019/09/26/midi-measures/
              beat = 1;
              measure += 1;
            }
          }

          //Serial.print(measure, DEC);
          //Serial.print(".");
          //Serial.print(beat, DEC);
          //Serial.print(".");
          //Serial.print(sixteenth, DEC);
          //Serial.print("");
        }
        break;
      case 0xF2:
        charCount = 0;
        Serial.print("\r\n\r\nSPP ");
        // TODO - decode SPP and update measure, beat and sixteenth
        break;
      default:
        Serial.print(charRead, HEX); // to serial monitor
        Serial.print(" ");
    }

    //charCount += 1;
    //if (charCount >= 8) {
      //charCount = 0;
      //Serial.print("\r\n");
    //}
  }
}