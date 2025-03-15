/*
   A dot animation that travels along rails
   (C) Voidstar Lab LLC 2021
*/

#ifndef RIPPLE_H_
#define RIPPLE_H_

// WARNING: These slow things down enough to affect performance. Don't turn on unless you need them!
// #define DEBUG_ADVANCEMENT  // Print debug messages about ripples' movement
// #define DEBUG_RENDERING  // Print debug messages about translating logical to actual position

#include "mapping.h"
#include <cstdint>

enum rippleState
{
    dead,
    withinNode, // Ripple isn't drawn as it passes through a node to keep the speed consistent
    travelingUpwards,
    travelingDownwards
};

enum rippleBehavior
{
    weaksauce = 0,
    feisty = 1,
    angry = 2,
    alwaysTurnsRight = 3,
    alwaysTurnsLeft = 4
};

float fmap(float x, float in_min, float in_max, float out_min, float out_max);

class Ripple
{
public:
    Ripple(int id);
    rippleState state;
    unsigned long color;

    /*
       If within a node: 0 is node, 1 is direction
       If traveling, 0 is segment, 1 is LED position from bottom
    */
    int position[2];

    // Place the Ripple in a node
    void start(uint8_t n, uint8_t d, unsigned long c, float s, unsigned long l, uint8_t b);

    void advance(uint8_t ledColors[40][14][3]);

private:
    void renderLed(uint8_t ledColors[40][14][3], unsigned long age);

    float speed;            // Each loop, ripples move this many LED's.
    unsigned long lifespan; // The ripple stops after this many milliseconds

    /*
        0: Always goes straight ahead if possible
        1: Can take 60-degree turns
        2: Can take 120-degree turns
    */
    uint8_t behavior;

    bool justStarted = false;

    float pressure;         // When Pressure reaches 1, ripple will move
    unsigned long birthday; // Used to track age of ripple

    static uint8_t rippleCount; // Used to give them unique ID's
    uint8_t rippleId;           // Used to identify this ripple in debug output
};

#endif