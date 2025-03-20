/*
   A dot animation that travels along rails
   (C) Voidstar Lab LLC 2021
*/

// WARNING: These slow things down enough to affect performance. Don't turn on unless you need them!
// #define DEBUG_ADVANCEMENT  // Print debug messages about ripples' movement
// #define DEBUG_RENDERING  // Print debug messages about translating logical to actual position

#include "ripple.h"
#include "mapping.h"
#include <cstdint>
#include "esphome/core/log.h"
#include <Arduino.h>

// #define DEBUG_ADVANCEMENT
// #define DEBUG_RENDERING

using namespace std;
static const char *const TAG = "CubeLightOutput.light";

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

    Ripple::Ripple(int id) : rippleId(id)
    {
        state = dead;
        // Serial.print("Instanced ripple #");
        // Serial.println(rippleId);
    }

   


    // Place the Ripple in a node
    void Ripple::start(uint8_t n, uint8_t d, unsigned long c, float s, unsigned long l, uint8_t b)
    {
#ifdef DEBUG_ADVANCEMENT
        ESP_LOGD(TAG, "Ripple start node/seg: %i dir/pos: %i color: %X", n, d, c);
#endif
        color = c;
        speed = s;
        lifespan = l;
        behavior = b;

        birthday = millis();
        pressure = 0;
        state = withinNode;

        position[0] = n;
        position[1] = d;

        justStarted = true;

        // Serial.print("Ripple ");
        // Serial.print(rippleId);
        // Serial.print(" starting at node ");
        // Serial.print(position[0]);
        // Serial.print(" direction ");
        // Serial.println(position[1]);
    }

    void Ripple::advance(uint8_t ledColors[40][14][3])
    {
        // ESP_LOGD(TAG, "Advance");
        unsigned long age = millis() - birthday;

        if (state == dead)
            return;

        pressure += fmap(float(age), 0.0, float(lifespan), speed, 0.0); // Ripple slows down as it ages
        // TODO: Motion of ripple is severely affected by loop speed. Make it time invariant

        if (pressure < 1 && (state == travelingUpwards || state == travelingDownwards))
        {
            // Ripple is visible but hasn't moved - render it to avoid flickering
            renderLed(ledColors, age);
        }

        while (pressure >= 1)
        {
#ifdef DEBUG_ADVANCEMENT
             ESP_LOGD(TAG, "Ripple advancing: %i", rippleId);
#endif

            switch (state)
            {
            case withinNode:
            {
                if (justStarted)
                {
                    justStarted = false;
                }
                else
                {
#ifdef DEBUG_ADVANCEMENT
                    // Serial.print("  Picking direction out of node ");
                    // Serial.print(position[0]);
                    // Serial.print(" with agr. ");
                    // Serial.println(behavior);
                     ESP_LOGD(TAG,"Picking direction out of node %i with agr. %i", position[0],behavior);
#endif

                    int newDirection = -1;

                    int sharpLeft = (position[1] + 1) % 6;
                    int wideLeft = (position[1] + 2) % 6;
                    int forward = (position[1] + 3) % 6;
                    int wideRight = (position[1] + 4) % 6;
                    int sharpRight = (position[1] + 5) % 6;

                    if (behavior <= 2)
                    { // Semi-random aggressive turn mode
                        // The more aggressive a ripple, the tighter turns it wants to make.
                        // If there aren't any segments it can turn to, we need to adjust its behavior.
                        uint8_t anger = behavior;

                        while (newDirection < 0)
                        {
                            if (anger == 0)
                            {
                                int forwardConnection = nodeConnections[position[0]][forward];

                                if (forwardConnection < 0)
                                {
                                    // We can't go straight ahead - we need to take a more aggressive angle
#ifdef DEBUG_ADVANCEMENT
                                    // Serial.println("  Can't go straight - picking more agr. path");
                                    ESP_LOGD(TAG,"Can't go straight - picking more agr. path");
#endif
                                    anger++;
                                }
                                else
                                {
#ifdef DEBUG_ADVANCEMENT
                                    // Serial.println("  Going forward");
                                     ESP_LOGD(TAG, "Going forward");
#endif
                                    newDirection = forward;
                                }
                            }

                            if (anger == 1)
                            {
                                int leftConnection = nodeConnections[position[0]][wideLeft];
                                int rightConnection = nodeConnections[position[0]][wideRight];

                                if (leftConnection >= 0 && rightConnection >= 0)
                                {
#ifdef DEBUG_ADVANCEMENT
                                ESP_LOGD(TAG,"  Turning left or right at random");
#endif
                                    newDirection = random(2) ? wideLeft : wideRight;
                                }
                                else if (leftConnection >= 0)
                                {
#ifdef DEBUG_ADVANCEMENT
                                     ESP_LOGD(TAG,"  Can only turn left");
#endif
                                    newDirection = wideLeft;
                                }
                                else if (rightConnection >= 0)
                                {
#ifdef DEBUG_ADVANCEMENT
                                     ESP_LOGD(TAG," Can only turn right");
#endif
                                    newDirection = wideRight;
                                }
                                else
                                {
#ifdef DEBUG_ADVANCEMENT
                                    ESP_LOGD(TAG, "  Can't make wide turn - picking more agr. path");
#endif
                                    anger++; // Can't take shallow turn - must become more aggressive
                                }
                            }

                            if (anger == 2)
                            {
                                int leftConnection = nodeConnections[position[0]][sharpLeft];
                                int rightConnection = nodeConnections[position[0]][sharpRight];

                                if (leftConnection >= 0 && rightConnection >= 0)
                                {
#ifdef DEBUG_ADVANCEMENT
                                    ESP_LOGD(TAG, "  Turning left or right at random");
#endif
                                    newDirection = random(2) ? sharpLeft : sharpRight;
                                }
                                else if (leftConnection >= 0)
                                {
#ifdef DEBUG_ADVANCEMENT
                                    ESP_LOGD(TAG, "  Can only turn left");
#endif
                                    newDirection = sharpLeft;
                                }
                                else if (rightConnection >= 0)
                                {
#ifdef DEBUG_ADVANCEMENT
                                    ESP_LOGD(TAG, "  Can only turn right");
#endif
                                    newDirection = sharpRight;
                                }
                                else
                                {
#ifdef DEBUG_ADVANCEMENT
                                    ESP_LOGD(TAG, "  Can't make tight turn - picking less agr. path");
#endif
                                    anger--; // Can't take tight turn - must become less aggressive
                                }
                            }

                            // Note that this can't handle some circumstances,
                            // like a node with segments in nothing but the 0 and 3 positions.
                            // Good thing we don't have any of those!
                        }
                    }
                    else if (behavior == alwaysTurnsRight)
                    {
                        for (int i = 1; i < 6; i++)
                        {
                            int possibleDirection = (position[1] + i) % 6;

                            if (nodeConnections[position[0]][possibleDirection] >= 0)
                            {
                                newDirection = possibleDirection;
                                break;
                            }
                        }

#ifdef DEBUG_ADVANCEMENT
                        ESP_LOGD(TAG, "  Turning as rightward as possible");
#endif
                    }
                    else if (behavior == alwaysTurnsLeft)
                    {
                        for (int i = 5; i >= 1; i--)
                        {
                            int possibleDirection = (position[1] + i) % 6;

                            if (nodeConnections[position[0]][possibleDirection] >= 0)
                            {
                                newDirection = possibleDirection;
                                break;
                            }
                        }

#ifdef DEBUG_ADVANCEMENT
                        ESP_LOGD(TAG, "  Turning as leftward as possible");
#endif
                    }

#ifdef DEBUG_ADVANCEMENT
                    // Serial.print("  Leaving node ");
                    // Serial.print(position[0]);
                    // Serial.print(" in direction ");
                    // Serial.println(newDirection);
                     ESP_LOGD(TAG,"  Leaving node %i in direction %i", position[0], newDirection);
#endif

                    position[1] = newDirection;
                }

                position[0] = nodeConnections[position[0]][position[1]]; // Look up which segment we're on

#ifdef DEBUG_ADVANCEMENT
                // Serial.print("  and entering segment ");
                // Serial.println(position[0]);
                ESP_LOGD(TAG,"and entering segment %i", position[0]);
#endif

                if (position[1] == 5 || position[1] == 0 || position[1] == 1)
                { // Top half of the node
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG,"  (starting at bottom)");
#endif
                    state = travelingUpwards;
                    position[1] = 0; // Starting at bottom of segment
                }
                else
                {
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG, "  (starting at top)");
#endif
                    state = travelingDownwards;
                    position[1] = 13; // Starting at top of 14-LED-long strip
                }
                break;
            }

            case travelingUpwards:
            {
                position[1]++;

                if (position[1] >= 14)
                {
                    // We've reached the top!
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG, "  Reached top of seg. %i", position[0]);
                    // Serial.println(position[0]);
#endif
                    // Enter the new node.
                    int segment = position[0];
                    position[0] = segmentConnections[position[0]][0];
                    for (int i = 0; i < 6; i++)
                    {
                        // Figure out from which direction the ripple is entering the node.
                        // Allows us to exit in an appropriately aggressive direction.
                        int incomingConnection = nodeConnections[position[0]][i];
                        if (incomingConnection == segment)
                            position[1] = i;
                    }
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG, "  Entering node %i from direction $i", position[0], position[1]);
                    // Serial.print(position[0]);
                    // Serial.print(" from direction ");
                    // Serial.println(position[1]);
#endif
                    state = withinNode;
                }
                else
                {
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG, "  Moved up to seg. %i LED %i", position[0], position[1]);
                    // Serial.print(position[0]);
                    // Serial.print(" LED ");
                    // Serial.println(position[1]);
#endif
                }
                break;
            }

            case travelingDownwards:
            {
                position[1]--;
                if (position[1] < 0)
                {
                    // We've reached the bottom!
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG, "  Reached bottom of seg. %i", position[0]);
                    // Serial.println(position[0]);
#endif
                    // Enter the new node.
                    int segment = position[0];
                    position[0] = segmentConnections[position[0]][1];
                    for (int i = 0; i < 6; i++)
                    {
                        // Figure out from which direction the ripple is entering the node.
                        // Allows us to exit in an appropriately aggressive direction.
                        int incomingConnection = nodeConnections[position[0]][i];
                        if (incomingConnection == segment)
                            position[1] = i;
                    }
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG, "  Entering node %i from direction %i", position[0], position[1]);
#endif
                    state = withinNode;
                }
                else
                {
#ifdef DEBUG_ADVANCEMENT
                    ESP_LOGD(TAG, "  Moved down to seg. %i LED %i", position[0], position[1]);
#endif
                }
                break;
            }

            default:
                break;
            }

            pressure -= 1;

            if (state == travelingUpwards || state == travelingDownwards)
            {
                // Ripple is visible - render it
                renderLed(ledColors, age);
            }
        }

#ifdef DEBUG_ADVANCEMENT
        ESP_LOGD(TAG, "  Age is now %i/%i", age, lifespan);
#endif

        if (lifespan && age >= lifespan)
        {
            // We dead
#ifdef DEBUG_ADVANCEMENT
            ESP_LOGD(TAG, "  Lifespan is up! Ripple is dead.");
#endif
            state = dead;
            position[0] = position[1] = pressure = age = 0;
        }
    }

    void Ripple::renderLed(uint8_t ledColors[40][14][3], unsigned long age)
    {
        int strip = ledAssignments[position[0]][0];
        int led = ledAssignments[position[0]][2] + position[1];

        int red = ledColors[position[0]][position[1]][0];
        int green = ledColors[position[0]][position[1]][1];
        int blue = ledColors[position[0]][position[1]][2];

        ledColors[position[0]][position[1]][0] = uint8_t(min(255, max(0, int(fmap(float(age), 0.0, float(lifespan), (color >> 8) & 0xFF, 0.0)) + red)));
        ledColors[position[0]][position[1]][1] = uint8_t(min(255, max(0, int(fmap(float(age), 0.0, float(lifespan), (color >> 16) & 0xFF, 0.0)) + green)));
        ledColors[position[0]][position[1]][2] = uint8_t(min(255, max(0, int(fmap(float(age), 0.0, float(lifespan), color & 0xFF, 0.0)) + blue)));

#ifdef DEBUG_RENDERING
        ESP_LOGD(TAG, "Rendering ripple position %i at strip %i LED %i color r%u g%u b%u", position[1], strip, led), ledColors[position[0]][position[1]][0], ledColors[position[0]][position[1]][1], ledColors[position[0]][position[1]][2];
        ESP_LOGD(TAG, "Color: %X %X %X %X", color, ledColors[position[0]][position[1]][0], ledColors[position[0]][position[1]][1], ledColors[position[0]][position[1]][2]);
        // Serial.print("Rendering ripple position (");
        // Serial.print(Rendering ripple position);
        // Serial.print(',');
        // Serial.print(position[1]);
        // Serial.print(") at Strip ");
        // Serial.print(strip);
        // Serial.print(", LED ");
        // Serial.print(led);
        // Serial.print(", color 0x");
        // for (int i = 0; i < 3; i++)
        // {
        //     if (ledColors[position[0]][position[1]][i] <= 0x0F)
        //         Serial.print('0');
        //     Serial.print(ledColors[position[0]][position[1]][i], HEX);
        // }
        // Serial.println();
#endif

};
