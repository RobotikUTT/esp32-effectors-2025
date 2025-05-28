#include <Arduino.h>
#include <Wire.h>
#include <AX12A.h>


constexpr uint8_t EFF_PIN[] = {1, 2, 3, 4};

void receiveEvent(const int size) {
    Wire.read(); // le premier est toujours n'importe quoi

    char message[100];
    int state;
    int id;

    Wire.readBytes(message, size);
    message[size] = '\0';

    if (sscanf(message, "%d %d", &state, &id) == 2) {
        if (id >= 0 && id < 4) { digitalWrite(EFF_PIN[id], state ? HIGH : LOW); }
    }
}

void setup() {
    Wire.begin(0x20);
    Wire.onReceive(receiveEvent);

    for (const unsigned char i : EFF_PIN) {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
}

void loop() {
}
