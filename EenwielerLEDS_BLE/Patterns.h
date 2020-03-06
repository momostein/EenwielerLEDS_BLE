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

    class Parts : public Pattern 
    {    
    public:

        Parts(CRGB* leds, int num_leds, CRGB* colors, uint8_t num_colors, uint8_t num_parts, int* rotDelay, bool* direction) :
            Pattern(leds, num_leds), colors(colors), num_colors(num_colors), num_parts(num_parts), rotDelay(rotDelay), direction(direction) {}

        void calc();

    private:
        uint8_t num_parts;
        uint8_t num_colors;
        CRGB* colors;       

        int* rotDelay;
        bool* direction;

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
        Rainbow(CRGB* leds, unsigned int num_leds, int* hueDelay, bool* direction, uint8_t deltahue = 7) :
            Pattern(leds, num_leds), hueDelay(hueDelay), direction(direction), deltahue(deltahue) {}

        void calc();

    private:
        uint8_t deltahue;
        
        int* hueDelay;
        bool* direction;

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

    class Marquee : public Pattern
    {
    public:
        Marquee(CRGB* leds, unsigned int num_leds, CRGB* colors, uint8_t num_colors, int* rotDelay, bool*direction):
            Pattern(leds, num_leds), colors(colors), num_colors(num_colors), rotDelay(rotDelay), direction(direction) {}

        void calc();

    private:
        uint8_t num_parts;
        uint8_t num_colors;
        CRGB* colors;

        int* rotDelay;
        bool* direction;

        int rot = 0;
        int color = 0;

        int lastT = 0;
    };

    int delayFromSpeed(uint8_t speed);
}
#endif

