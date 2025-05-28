#include <Arduino.h>
#include <Wire.h>
#include <AX12A.h>
#include <XL320.h>


#define BaudRate1     115200ul
#define RX1PIN        8
#define TX1PIN        9
#define DirectionPin  10u
#define BaudRate2     57600ul
#define RX2PIN        4
#define TX2PIN        5

XL320 robot;

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

    Serial1.begin(BaudRate1, SERIAL_8N1, RX1PIN, TX1PIN);
    robot.begin(Serial1);
    Serial2.begin(BaudRate2, SERIAL_8N1, RX2PIN, TX2PIN);
    ax12a.begin(BaudRate2, DirectionPin, &Serial2);
}

void loop() {
}
