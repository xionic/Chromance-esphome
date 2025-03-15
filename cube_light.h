#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"

#include "FastLED.h"

// Avoid annoying compiler messages
#define FASTLED_INTERNAL
#define NUM_STRIPS 2
#define STRIP_1_LENGTH 238
#define STRIP_2_LENGTH 322
#define PIN_1 4
#define PIN_2 13

//Power Limiting
//#define POWER_PER_LED_COLOR 20 //60mA for each ws2812B, /3 for each of the colors on a die
#define PS_SAFE_LIMIT 8000 //Max safe power to draw from power supply in mA

// If you don't have an EmotiBit or don't feel like wearing it, that's OK
// We'll fire automatic pulses
#define RANDOM_PULSES_ENABLED true		 // Fire random rainbow pulses from random nodes
#define CUBE_PULSES_ENABLED true		 // Draw cubes at random nodes
#define STARBURST_PULSES_ENABLED true		 // Draw starbursts
#define SIMULATED_BIOMETRICS_ENABLED false // Simulate heartbeat and EDA ripples

#define AUTO_PULSE_TIMEOUT 5000 // If no heartbeat is received in this many ms, begin firing random/simulated pulses
#define RANDOM_PULSE_TIME 2000  // Fire a random pulse every (this many) ms
#define AUTO_PULSE_CHANGE_TIME 30000


#define SIMULATED_HEARTBEAT_BASE_TIME 600 // Fire a simulated heartbeat pulse after at least this many ms
#define SIMULATED_HEARTBEAT_VARIANCE 200 // Add random jitter to simulated heartbeat
#define SIMULATED_EDA_BASE_TIME 1000	   // Same, but for inward EDA pulses
#define SIMULATED_EDA_VARIANCE 10000

#ifdef CUBE_LOGGING
#define CUBE_LOG(msg) ESP_LOGD(msg)
#else 
#define CUBE_LOG(msg)
#endif

namespace esphome {
namespace cube_light {

class CubeLightOutput : public light::LightOutput, public Component {
	public:
		CubeLightOutput();
		void setup() override;
		light::LightTraits get_traits() override;
		void write_state(light::LightState *state) override;
		void dump_config() override;
		void loop() override;
		CRGB* leds[NUM_STRIPS];
		bool state{false};
		byte ledColors[40][14][3]; // LED buffer - each ripple writes to this, then we write this to the strips
		float decay{0.97}; // Multiply all LED's by this amount each tick to create fancy fading tails


		unsigned long lastRandomPulse;
		byte lastAutoPulseNode = 255;

		byte numberOfAutoPulseTypes = RANDOM_PULSES_ENABLED + CUBE_PULSES_ENABLED + STARBURST_PULSES_ENABLED;
		byte currentAutoPulseType = 255;
		unsigned long lastAutoPulseChange;
		unsigned long nextSimulatedHeartbeat;
		unsigned long nextSimulatedEda;
		unsigned long lastHeartbeat; // Track last heartbeat so we can detect noise/disconnections
};

} // namespace cube_light
} //namespace esphome