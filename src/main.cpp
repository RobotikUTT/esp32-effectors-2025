#include <Arduino.h>
#include <Wire.h>

const uint8_t EFF_PIN[] = {1, 2, 3, 4};

void receiveEvent(int size) {
  Wire.read(); // le premier est toujours n'importe quoi

  char message[100];
  int state;
  int id;

  Wire.readBytes(message, size);
  message[size] = '\0';

  if (sscanf(message, "%d %d", &state, &id) == 2) {
    if (id >= 0 && id < 4) {
      digitalWrite(EFF_PIN[id], state ? HIGH : LOW);
    }
  }
}

void setup() {
  Wire.begin(0x20);
  Wire.onReceive(receiveEvent);

  for (int i = 0; i < 4; i++) {
    pinMode(EFF_PIN[i], OUTPUT);
    digitalWrite(EFF_PIN[i], LOW);
  }
}

void loop() {
}