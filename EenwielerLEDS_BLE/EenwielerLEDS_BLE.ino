/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <FastLED.h>

#include "Patterns.h"

// How many leds in your strip?
#define NUM_LEDS 23

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 12

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define COL_PRIM_UUID       "71c19398-3f77-46db-ac13-6b652978e868"


// Define the array of leds
CRGB leds[NUM_LEDS];

// Color palette
CRGB colors[2] = {
    CRGB(0,     0,      255),
    CRGB(0,     0,    0   )
};

int counter = 0;

patterns::Parts parts(leds, NUM_LEDS, colors, 2, 4, 50, -1);
patterns::Pride pride(leds, NUM_LEDS);
patterns::Rainbow rainbow(leds, NUM_LEDS, 10, 255/NUM_LEDS);
patterns::Solid solid(leds, NUM_LEDS, colors);

class MyCallbackHandler : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.print("CONNETED BIACH\t");
        Serial.println(counter++);
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.print("DISCONNECTED BIACH\t");
        Serial.println(counter++);
    }
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) {

        if (pCharacteristic->getUUID().toString() == COL_PRIM_UUID) {
            Serial.println("*********");
            Serial.print("New color: ");
            for (int i = 0; i < 3; i++) {
                Serial.print(pCharacteristic->getData()[i]);


                Serial.print('\t');
            }
            Serial.println();
            Serial.println("*********");

            colors[0] = CRGB(pCharacteristic->getData()[0], pCharacteristic->getData()[1], pCharacteristic->getData()[2]);

        }
        else {

            std::string value = pCharacteristic->getValue();

            if (value.length() > 0) {
                Serial.println("*********");
                Serial.print("New value: ");
                for (int i = 0; i < value.length(); i++)
                    Serial.print(value[i]);

                Serial.println();
                Serial.println("*********");
            }
        }
    }
};

void setup() {
    Serial.begin(115200);

    Serial.println("Starting LED work!");

    LEDS.addLeds<WS2813, DATA_PIN, GRB>(leds, NUM_LEDS);
    LEDS.setBrightness(84);

    Serial.println("Black");
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);

    }
    FastLED.show();

    Serial.println("Starting BLE work!");

    BLEDevice::init("Eenwieler LED");
    BLEServer* pServer = BLEDevice::createServer();

    pServer->setCallbacks(new MyCallbackHandler());

    BLEService* pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic* pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    BLECharacteristic* pColPrim = pService->createCharacteristic(
        COL_PRIM_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    uint8_t white[3] = { 255, 255, 255 };
    pColPrim->setValue(white, 3);
    pColPrim->setCallbacks(new MyCallbacks());

    pCharacteristic->setValue("Hello World says Neil");
    pCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {

    parts.calc();

    FastLED.show();
}