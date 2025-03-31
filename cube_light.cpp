
#include "cube_light.h"
#include "esphome/core/log.h"
#include "mapping.h"
#include "ripple.h"


namespace esphome {
namespace cube_light {

// These ripples are endlessly reused so we don't need to do any memory management
#define numberOfRipples 30
Ripple ripples[numberOfRipples] = {
    Ripple(0),
    Ripple(1),
    Ripple(2),
    Ripple(3),
    Ripple(4),
    Ripple(5),
    Ripple(6),
    Ripple(7),
    Ripple(8),
    Ripple(9),
    Ripple(10),
    Ripple(11),
    Ripple(12),
    Ripple(13),
    Ripple(14),
    Ripple(15),
    Ripple(16),
    Ripple(17),
    Ripple(18),
    Ripple(19),
    Ripple(20),
    Ripple(21),
    Ripple(22),
    Ripple(23),
    Ripple(24),
    Ripple(25),
    Ripple(26),
    Ripple(27),
    Ripple(28),
    Ripple(29),
};


CubeLight::CubeLight(){}
/*
light::LightTraits CubeLight::get_traits()
{
    auto traits = light::LightTraits();
    traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
    return traits;
}

void CubeLight::write_state(light::LightState *state) {}
*/
void CubeLight::dump_config()
{

    ESP_LOGCONFIG(TAG, "CubeLight config");
}

void CubeLight::setup()
{
    leds[0] = new CRGB[STRIP_1_LENGTH];
    leds[1] = new CRGB[STRIP_2_LENGTH];

    FastLED.setMaxPowerInVoltsAndMilliamps(5, PS_SAFE_LIMIT); // Set Current Limit
    ESP_LOGI(TAG, "Limited power to %i mA", PS_SAFE_LIMIT);
    FastLED.addLeds<NEOPIXEL, PIN_1>(leds[0], STRIP_1_LENGTH);
    FastLED.addLeds<NEOPIXEL, PIN_2>(leds[1], STRIP_2_LENGTH);
    //    strips[i].setBrightness(125);  // If your PSU sucks, use this to limit the current        
   
    FastLED.clear();
    lastHeartbeat = millis();
    
     //init color arrays
    set_to_black();
    for (int segment = 0; segment < 40; segment++)
    {       
        for (int fromBottom = 0; fromBottom < 14; fromBottom++)
        {
            for(int i = 0; i < 3; i++)
            {
                ledColors[segment][fromBottom][i] = 0x00;
            }
        }
    }
    FastLED.show();
}

unsigned long lastLoopTime = 0;

void CubeLight::loop()
{
    // limit loop run frequency to FPS to allow stable effects and let ESPHome do it's thing
    if (millis() - lastLoopTime < 1000 / TARGET_FPS)
    {
        return;
    }
    unsigned long lastLoopTime = millis();

#ifdef REMOTE_JSON_MODE
    FastLED.show();
#else
    if (!is_on || (!randomPulsesEnabled && !cubePulsesEnabled && !starburstPulsesEnabled))
    {
        set_to_black();
        FastLED.show();
        return;
    }

        unsigned long benchmark = millis();

    // Fade all dots to create trails
    for (int strip = 0; strip < 40; strip++)
    {
        for (int led = 0; led < 14; led++)
        {
            for (int i = 0; i < 3; i++)
            {
                ledColors[strip][led][i] *= decay;
            }
        }
    }

    for (int i = 0; i < numberOfRipples; i++)
    {
        ripples[i].advance(ledColors);
    }

    for (int segment = 0; segment < 40; segment++)
    {
        for (int fromBottom = 0; fromBottom < 14; fromBottom++)
        {
            int strip = ledAssignments[segment][0];
            int led = round(fmap(
                fromBottom,
                0, 13,
                ledAssignments[segment][2], ledAssignments[segment][1])
            );            

             leds[strip][led]
                .setRGB(
                    ledColors[segment][fromBottom][0],
                    ledColors[segment][fromBottom][1],
                    ledColors[segment][fromBottom][2]);
                }
    }

    FastLED.show();

    if (millis() - lastHeartbeat >= AUTO_PULSE_TIMEOUT)    {
        
        // When biometric data is unavailable, visualize at random
        if (numberOfAutoPulseTypes && millis() - lastRandomPulse >= RANDOM_PULSE_TIME)
        {
            unsigned int baseColor = random(0xFFFF);

           
            ESP_LOGD(TAG, "loop1");
            if (currentAutoPulseType == 255 || (numberOfAutoPulseTypes > 1 && millis() - lastAutoPulseChange >= AUTO_PULSE_CHANGE_TIME))
            {
                byte possiblePulse = 255;
                while (true)
                {
                    possiblePulse = random(3);
                    //ESP_LOGD(TAG, "%i", possiblePulse);
                    /*if (possiblePulse == currentAutoPulseType)
                        continue;*/

                    switch (possiblePulse)
                    {
                    case 0:
                        if (!randomPulsesEnabled)
                            continue;
                        break;

                    case 1:
                        if (!cubePulsesEnabled)
                            continue;
                        break;

                    case 2:
                        if (!starburstPulsesEnabled)
                            continue;
                        break;

                    default:
                        continue;
                    }

                    currentAutoPulseType = possiblePulse;
                    lastAutoPulseChange = millis();
                    break;
                }
                ESP_LOGD(TAG, "picked pulse type %i", currentAutoPulseType);
            }

            // testing - force type
            // currentAutoPulseType = 0;

             switch (currentAutoPulseType)
            {
            case 0:
            {
                // ESP_LOGD(TAG, "Case 0");
                int node = 0;
                bool foundStartingNode = false;
                while (!foundStartingNode)
                {
                    node = random(25);
                    foundStartingNode = true;
                    for (int i = 0; i < numberOfBorderNodes; i++)
                    {
                        // Don't fire a pulse on one of the outer nodes - it looks boring
                        if (node == borderNodes[i])
                            foundStartingNode = false;
                    }

                    if (node == lastAutoPulseNode)
                        foundStartingNode = false;
                }
                 ESP_LOGD(TAG, "Selected Node %i", node);
                lastAutoPulseNode = node;

                for (int i = 0; i < 6; i++)
                {

                    if (nodeConnections[node][i] >= 0)
                    {
                        for (int j = 0; j < numberOfRipples; j++)
                        {
                            if (ripples[j].state == dead)
                            {
                                ripples[j]
                                    .start(
                                        node,
                                        i,
                                        hueToColor(baseColor),
                                        // (float(random(100)) / 100.0) * .1 + .5, //speed
                                        float(random(100)) / 100.0 * .2 + .5,
                                        3000, // lifespan
                                        1);

                                break;
                            }
                        }
                    }
                }                
                break;
            }

            case 1:
            {
                // ESP_LOGD(TAG, "Case 1");
                int node = cubeNodes[random(numberOfCubeNodes)];

                while (node == lastAutoPulseNode)
                    node = cubeNodes[random(numberOfCubeNodes)];

                lastAutoPulseNode = node;

                byte behavior = random(2) ? alwaysTurnsLeft : alwaysTurnsRight;

                for (int i = 0; i < 6; i++)
                {
                    if (nodeConnections[node][i] >= 0)
                    {
                        for (int j = 0; j < numberOfRipples; j++)
                        {
                            if (ripples[j].state == dead)
                            {
                                ripples[j].start(
                                    node,
                                    i,
                                    hueToColor(baseColor),
                                    .5,
                                    2000,
                                    behavior);

                                break;
                            }
                        }
                    }
                }
                break;
            }

            case 2:
            {
                // ESP_LOGD(TAG, "Case 2");
                byte behavior = random(2) ? alwaysTurnsLeft : alwaysTurnsRight;

                lastAutoPulseNode = starburstNode;

                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < numberOfRipples; j++)
                    {
                        if (ripples[j].state == dead)
                        {
                            ripples[j].start(
                                starburstNode,
                                i,
                                hueToColor(baseColor),
                                .65,
                                1500,
                                behavior);

                            break;
                        }
                    }
                }
                break;
            }

            default:
                break;
            }
            lastRandomPulse = millis();
        }

        if (SIMULATED_BIOMETRICS_ENABLED)
        {
            // Simulated heartbeat
            if (millis() >= nextSimulatedHeartbeat)
            {
                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < numberOfRipples; j++)
                    {
                        if (ripples[j].state == dead)
                        {
                            ripples[j].start(
                                15,
                                i,
                                0xEE1111,
                                float(random(100)) / 100.0 * .1 + .4,
                                1000,
                                0);

                            break;
                        }
                    }
                }

                nextSimulatedHeartbeat = millis() + SIMULATED_HEARTBEAT_BASE_TIME + random(SIMULATED_HEARTBEAT_VARIANCE);
            }

            // Simulated EDA ripples
            if (millis() >= nextSimulatedEda)
            {
                for (int i = 0; i < 10; i++)
                {
                    for (int j = 0; j < numberOfRipples; j++)
                    {
                        if (ripples[j].state == dead)
                        {
                            byte targetNode = borderNodes[random(numberOfBorderNodes)];
                            byte direction = 255;

                            while (direction == 255)
                            {
                                direction = random(6);
                                if (nodeConnections[targetNode][direction] < 0)
                                    direction = 255;
                            }

                            ripples[j].start(
                                targetNode,
                                direction,
                                0x1111EE,
                                float(random(100)) / 100.0 * .5 + 2,
                                300,
                                2);

                            break;
                        }
                    }
                }

                nextSimulatedEda = millis() + SIMULATED_EDA_BASE_TIME + random(SIMULATED_EDA_VARIANCE);
            }
        }

    }
    //ESP_LOGD(TAG, "Benchmark: %i", millis() - benchmark);
    //  Serial.print("Benchmark: ");
    //  Serial.println(millis() - benchmark);
#endif
}

void CubeLight::set_to_black()
{
    for (int i = 0; i < STRIP_1_LENGTH; i++)
    {
        leds[0][i] = CRGB::Black;
    }
    for (int i = 0; i < STRIP_2_LENGTH; i++)
    {
        leds[1][i] = CRGB::Black;
    }
}

// void CubeLightOutput::receive_data(ArduinoJson::JsonObject data)
// {
//     ESP_LOGD(TAG, "%i", data["str1"][0]);
//     /*
//         for (int i = 0; i < STRIP_1_LENGTH; i++)
//         {
//             leds[0][i].r = data["str1"][i][0];
//             leds[0][i].g = data["str1"][i][1];
//             leds[0][i].b = data["str1"][i][2];
//         }
//         for (int i = 0; i < STRIP_2_LENGTH; i++)
//         {
//             leds[0][i].r = data["str2"][i][0];
//             leds[0][i].g = data["str2"][i][1];
//             leds[0][i].b = data["str2"][i][2];
//         }*/
// }

    uint32_t hueToColor(unsigned int hue)
{
    CHSV hsv = CHSV(hue, 255, MAX_BRIGHTNESS);
    CRGB rgb;
    hsv2rgb_spectrum(hsv, rgb);
    return (rgb.r << 16) | (rgb.g << 8) | rgb.b;
}

} // namespace cube_light
} // namespace esphome
