// Patterns.h

#ifndef _PATTERNS_h
#define _PATTERNS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <arduino.h>
#else
	#include <WProgram.h>
#endif

#include <FastLED.h>
namespace patterns {
    class Pattern
    {
    public:
        Pattern(CRGB* leds, unsigned int num_leds) : 
            leds(leds), num_leds(num_leds) {}

        virtual void calc() {}

    protected:
        const int num_leds;
        CRGB* leds;
        

    };

    class Parts : Pattern 
    {    
    public:

        Parts(CRGB* leds, int num_leds, CRGB* colors, uint8_t num_colors, uint8_t num_parts, int rotDelay, int8_t rotAmount = 1) :
            Pattern(leds, num_leds), colors(colors), num_colors(num_colors), num_parts(num_parts), rotDelay(rotDelay), rotAmount(rotAmount) {}

        void calc();

        int8_t rotAmount;
        uint8_t num_parts;
        unsigned int rotDelay;

    private:
        // Requires 2 colors
        uint8_t num_colors;
        CRGB* colors;       

        int rot;
        int lastT;
    };

    class Pride : public Pattern
    {
    public:
        Pride(CRGB* leds, unsigned int num_leds) : Pattern(leds, num_leds) {}
        void calc();
    };

    class Rainbow : public Pattern
    {
    public:
        Rainbow(CRGB* leds, unsigned int num_leds, int hueDelay, uint8_t deltahue = 7) : 
            Pattern(leds, num_leds), hueDelay(hueDelay), deltahue(deltahue) {}

        void calc();

        uint8_t deltahue;
        int hueDelay;

    private:
        uint8_t hue;
        int lastT;
    };

    class Solid : public Pattern {
    public:
        Solid(CRGB* leds, unsigned int num_leds, CRGB* colors) : 
            Pattern(leds, num_leds), colors(colors) {}

        void calc();

    private:
        CRGB* colors;
    };

}



#endif

