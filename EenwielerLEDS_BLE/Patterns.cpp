// 
// 
// 

#include "Patterns.h"

#define MAX_DELAY 1500.0f
#define MIN_DELAY 20.0f

namespace patterns {
    inline void Pride::calc() {
        static uint16_t sPseudotime = 0;
        static uint16_t sLastMillis = 0;
        static uint16_t sHue16 = 0;

        uint8_t sat8 = beatsin88(87, 220, 250);
        uint8_t brightdepth = beatsin88(341, 96, 224);
        uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
        uint8_t msmultiplier = beatsin88(147, 23, 60);

        uint16_t hue16 = sHue16;//gHue * 256;
        uint16_t hueinc16 = beatsin88(113, 1, 3000);

        uint16_t ms = millis();
        uint16_t deltams = ms - sLastMillis;
        sLastMillis = ms;
        sPseudotime += deltams * msmultiplier;
        sHue16 += deltams * beatsin88(400, 5, 9);
        uint16_t brightnesstheta16 = sPseudotime;

        for (uint16_t i = 0; i < num_leds; i++) {
            hue16 += hueinc16;
            uint8_t hue8 = hue16 / 256;

            brightnesstheta16 += brightnessthetainc16;
            uint16_t b16 = sin16(brightnesstheta16) + 32768;

            uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
            uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
            bri8 += (255 - brightdepth);

            CRGB newcolor = CHSV(hue8, sat8, bri8);

            uint16_t pixelnumber = i;
            pixelnumber = (num_leds - 1) - pixelnumber;

            nblend(leds[pixelnumber], newcolor, 64);
        }
    }

    inline void Parts::calc() {
        if (millis() - lastT >= *rotDelay && *rotDelay >= 0) {
            if (*direction) {
                rot++;
            }
            else {
                rot--;
                if (rot < 0) {
                    rot = num_leds-1;
                }
            }
            
            rot = rot % num_leds;

            lastT = millis();
        }

        for (int i = 0; i < num_leds; i++)
        {
            uint8_t part = (i * num_parts) / num_leds;
            leds[(i + rot) % num_leds] = colors[part % num_colors];
        }
    }

    void Rainbow::calc()
    {   
        if (millis() - lastT >= (*hueDelay)/5 && *hueDelay >= 0) {
            if (*direction) {
                hue++;
            }
            else {
                hue--;
            }

            lastT = millis();
        }

        fill_rainbow(leds, num_leds, hue, deltahue);
    }

    void Solid::calc()
    {
        fill_solid(leds, num_leds, colors[0]);
    }

    int delayFromSpeed(uint8_t speed) {
        if (speed != 0) {
            return (1.0f / (float)speed - 1.0f / 255.0f) * (MAX_DELAY - MIN_DELAY) / (1.0f - 1.0f / 255.0f) + MIN_DELAY;
        }
        else {
            return -1;
        }
    }
}

