#include <Arduino.h>
#include <Wire.h>
#include <AX12A.h>
#include <XL320.h>


#define I2C_ADDRESS          0x20
#define BAUD_RATE1           115200ul
#define RX1_PIN              9
#define TX1_PIN              8
#define XL320_BANNER_ID      4
#define XL320_BANNER_OFFSET  0
#define XL320_MAGNET_ID      6
#define XL320_MAGNET_OFFSET  25
#define XL320_RIGHT_ID       7
#define XL320_RIGHT_OFFSET   180
#define XL320_LEFT_ID        8
#define XL320_LEFT_OFFSET    200
#define XL320_PUMP_ID        9
#define XL320_PUMP_OFFSET    230
#define DIRECTION_PIN        10u
#define BAUD_RATE2           57600ul
#define RX2_PIN              4
#define TX2_PIN              5
#define AX12A_ID             1
#define AX12A_OFFSET         0
#define PUMP_PIN             A6
#define VALVE_PIN            A7

XL320 xl320;

// constexpr short SWITCH_PINS[2] = {2, 3};

void setMagnetExtOn() {
    xl320.moveJoint(XL320_RIGHT_ID, XL320_RIGHT_OFFSET);
    delay(10);
    xl320.moveJoint(XL320_LEFT_ID, XL320_LEFT_OFFSET);
    delay(10);
}

void setMagnetExtOff() {
    xl320.moveJoint(XL320_RIGHT_ID, XL320_RIGHT_OFFSET + 614);
    delay(10);
    xl320.moveJoint(XL320_LEFT_ID, XL320_LEFT_OFFSET + 614);
    delay(10);
}

void setBannerOpen() {
    xl320.moveJoint(XL320_BANNER_ID, XL320_BANNER_OFFSET);
    delay(10);
}

void setBannerClose() {
    xl320.moveJoint(XL320_BANNER_ID, XL320_BANNER_OFFSET + 307);
    delay(10);
}

void setMagnetIntOn() {
    xl320.moveJoint(XL320_MAGNET_ID, XL320_MAGNET_OFFSET);
    delay(10);
}

void setMagnetIntOff() {
    xl320.moveJoint(XL320_MAGNET_ID, XL320_MAGNET_OFFSET + 200);
    delay(10);
}

void setPumpUp() {
    xl320.moveJoint(XL320_PUMP_ID, XL320_PUMP_OFFSET);
    delay(10);
}

void setPumpDown() {
    xl320.moveJoint(XL320_PUMP_ID, XL320_PUMP_OFFSET + 307);
    delay(10);
}

void setPliersUp() {
    ax12a.move(AX12A_ID, AX12A_OFFSET);
    delay(10);
}

void setPliersDown() {
    ax12a.move(AX12A_ID, AX12A_OFFSET + 400);
    delay(10);
}

void setPumpOn() {
    analogWrite(PUMP_PIN, 255);
}

void setPumpOff() {
    analogWrite(PUMP_PIN, 0);
}

void setValveOpen() {
    analogWrite(VALVE_PIN, 255);
}

void setValveClose() {
    analogWrite(VALVE_PIN, 0);
}

void takePlank() {
    // Baisser servo + fermer valve + allumer pompe
    setPumpDown();
    delay(100);
    setValveClose();
    setPumpOn();
}

void putDownPlank() {
    // Ouvrir valve + arrêter pompe + relever servo
    setValveOpen();
    setPumpOff();
    delay(10);
    setPumpUp();
}

void takeCans() {
    setMagnetIntOn();
    setMagnetExtOn();
}

void putDownCans() {
    setMagnetIntOff();
    setMagnetExtOff();
}

void takeEverything() {
    // setPliersDown();
    takeCans();
    delay(200);
    takePlank();
}

void putDownEverything() {
    // setPliersDown();
    putDownCans();
    putDownPlank();
}

void performAction(const int state, const int id) {
    switch (id) {
    case 0:
        if (state == 0) {
            putDownEverything();
        }
        if (state == 1) {
            takeEverything();
        }
    case 1:
        if (state == 0) {
            setBannerClose();
        }
        if (state == 1) {
            setBannerOpen();
        }
    default: break;
    }
}

void receiveEvent(const int size) {
    Wire.read(); // le premier est toujours n'importe quoi

    char message[100];
    int state; // 0 = désactivé, 1 = activé
    int id; // 0 = pince, 1 = bannière

    Wire.readBytes(message, size);
    message[size] = '\0';

    if (sscanf(message, "%d %d", &state, &id) == 2) {
        if (0 <= state < 2 && 0 <= id < 2) {
            performAction(state, id);
        }
    }
}

void setup() {
    // Init Serial USB
    Serial.begin(BAUD_RATE1);

    // Init Wire I2C
    /*
    Wire.begin(I2C_ADDRESS);
    Serial.println("I2C Init");
    Wire.onReceive(receiveEvent);

    // Init PINs
    for (const unsigned char i : SWITCH_PINS) {
        pinMode(i, INPUT);
    }
    */

    // Init Servo XL320 et AX-12A
    Serial1.begin(BAUD_RATE1, SERIAL_8N1, RX1_PIN, TX1_PIN);
    xl320.begin(Serial1);
    for (const int id : {
             XL320_BANNER_ID, XL320_MAGNET_ID, XL320_RIGHT_ID, XL320_LEFT_ID, XL320_PUMP_ID
         }) {
        xl320.setJointSpeed(id, 800);
        delay(10);
    }
    Serial2.begin(BAUD_RATE2, SERIAL_8N1, RX2_PIN, TX2_PIN);
    ax12a.begin(BAUD_RATE2, DIRECTION_PIN, &Serial2);
    ax12a.setEndless(AX12A_ID, OFF);
    setPumpUp();
}

void loop() {
}
