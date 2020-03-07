/*
    Control a WS2813 ledstrip from bluetooth
    By Brecht and Wannes
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <FastLED.h>

#include "Patterns.h"

/*  ----------------------
        BLE UUIDS
    ----------------------  */

#define DEVICE_NAME "Eenwieler van Brecht"

    // See the following for generating UUIDs :
    // https://www.uuidgenerator.net/

#define SERVICE_UUID        "78f04453-8357-41f5-981e-6eec701224ea"

#define PATTERN_UUID        "6781b711-4a50-4e20-8a39-d8b7bc3d14a8"
#define BRIGHTNESS_UUID     "6cd7aa8c-c080-4195-ad5d-49babaffa170"

#define COL_PRIM_UUID       "71c19398-3f77-46db-ac13-6b652978e868"
#define COL_SEC_UUID        "adec8b74-7e24-4d05-83e7-e7a3d4e592d3"

#define SPEED_UUID          "1cc0ef4e-7e5b-47a5-bb6e-80e3f7438e91"

/*  ----------------------
        LED OPTIONS
    ----------------------  */

    // How many leds in your strip?
#define NUM_LEDS 23
#define DATA_PIN 12

#define NUM_PATTERNS 7
#define NUM_COLORS   3

// Define the array of leds
CRGB leds[NUM_LEDS];

// Color palette
CRGB colors[NUM_COLORS] = {
    CRGB(255,   0,      0),
    CRGB(0,     255,    0),
    CRGB(0,     0,      255)
};

const String color_names[NUM_COLORS] = {
    "primary",
    "secondary",
    "tertiary"
};

const char* color_uuids[NUM_COLORS] = {
    "71c19398-3f77-46db-ac13-6b652978e868",
    "adec8b74-7e24-4d05-83e7-e7a3d4e592d3",
    "39012b50-6a5e-49ae-8ce7-68feb6fc917a"
};

// Speed and direction
uint8_t speed = 150;
int rotDelay = patterns::delayFromSpeed(speed);

bool direction = false;

// Current pattern
uint8_t pattern = 5;

// Patterns
patterns::Pattern* pattern_array[NUM_PATTERNS] = {
    new patterns::Solid(leds, NUM_LEDS, colors),
    new patterns::Parts(leds, NUM_LEDS, colors, 2, 2, &rotDelay, &direction),
    new patterns::Parts(leds, NUM_LEDS, colors, 2, 4, &rotDelay, &direction),
    new patterns::Parts(leds, NUM_LEDS, colors, 3, 3, &rotDelay, &direction),
    new patterns::Marquee(leds, NUM_LEDS, colors, 3, &rotDelay, &direction),
    new patterns::Rainbow(leds, NUM_LEDS, &rotDelay, &direction, 255 / NUM_LEDS),
    new patterns::Pride(leds, NUM_LEDS)
};

// Pattern names
const String pattern_names[NUM_PATTERNS]{
    "Solid",
    "Halves",
    "Quarters",
    "Triples",
    "Marquee",
    "Rainbow",
    "Pride"
};

int counter = 0;

/*  ----------------------
        Callbacks
    ----------------------  */

class ServerCallback : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.print("A device connected, counter:\t");
        Serial.println(counter++);
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.print("A device disconnected, counter:\t");
        Serial.println(counter++);
    }
};

class PatternCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pChar) {
        Serial.println("*********");
        Serial.print("Pattern changed: ");

        pattern = pChar->getData()[0] % NUM_PATTERNS;
        Serial.println(pattern_names[pattern]);

        Serial.println();
        Serial.println("*********");
    }

    void onRead(BLECharacteristic* pChar) {
        uint8_t data[] = { pattern };
        pChar->setValue(data, 1);
    }
};

class ColorCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pChar) {
        Serial.println("*********");
        Serial.print(color_names[color]);
        Serial.print(" color changed:");

        colors[color] = CRGB(pChar->getData()[0], pChar->getData()[1], pChar->getData()[2]);

        Serial.print("\tR"); Serial.print(pChar->getData()[0]);
        Serial.print("\tG"); Serial.print(pChar->getData()[1]);
        Serial.println("\tB"); Serial.print(pChar->getData()[2]);

        Serial.println();
        Serial.println("*********");
    }

    void onRead(BLECharacteristic* pChar) {
        uint8_t color_data[] = { colors[color].r, colors[color].g, colors[color].b };
        pChar->setValue(color_data, 3);
    }

public:
    ColorCallback(CRGB* colors, int color) : colors(colors), color(color) {}

private:
    CRGB* colors;
    int color;
};

class BrightnessCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pChar) {
        Serial.println("*********");
        Serial.print("Brightness changed: ");

        uint8_t brightness = pChar->getData()[0];
        FastLED.setBrightness(brightness);

        Serial.println(brightness);

        Serial.println();
        Serial.println("*********");
    }

    void onRead(BLECharacteristic* pChar) {
        uint8_t data[] = { FastLED.getBrightness() };
        pChar->setValue(data, 1);
    }
};

class SpeedCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pChar) {
        Serial.println("*********");
        Serial.print("Speed changed: ");

        speed = pChar->getData()[0];
        rotDelay = patterns::delayFromSpeed(speed);
        direction = pChar->getData()[1];

        Serial.print(speed);
        Serial.print('\t');
        Serial.println(direction);

        Serial.println();
        Serial.println("*********");
    }

    void onRead(BLECharacteristic* pChar) {
        uint8_t data[] = { speed, direction };
        pChar->setValue(data, 2);
    }
};


/*  ----------------------
        Setup
    ----------------------  */

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

    BLEDevice::init(DEVICE_NAME);
    BLEServer* pServer = BLEDevice::createServer();

    pServer->setCallbacks(new ServerCallback());

    BLEService* pService = pServer->createService(SERVICE_UUID);

    /*  ----------------------
            Characteristics
        ----------------------  */

        // Pattern Characteristic
    BLECharacteristic* pPattern = pService->createCharacteristic(
        PATTERN_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    pPattern->setCallbacks(new PatternCallback);

    // Brightness Characteristic
    BLECharacteristic* pBrightness = pService->createCharacteristic(
        BRIGHTNESS_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    pBrightness->setCallbacks(new BrightnessCallback);

    // Color Characteristics
    for (int i = 0; i < NUM_COLORS; i++)
    {
        BLECharacteristic* pCol = pService->createCharacteristic(
            color_uuids[i],
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
        );

        pCol->setCallbacks(new ColorCallback(colors, i));
    }

    BLECharacteristic* pSpeed = pService->createCharacteristic(
        SPEED_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    pSpeed->setCallbacks(new SpeedCallback);

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

/*  ----------------------
        Loop
    ----------------------  */

void loop() {
    pattern_array[pattern]->calc();

    FastLED.show();
}

void eventEffect(CRGB color) {

}