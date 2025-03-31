#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/core/log.h"
#include "FastLED.h"

// Avoid annoying compiler messages
#define FASTLED_INTERNAL
#define NUM_STRIPS 2
#define STRIP_1_LENGTH 238
#define STRIP_2_LENGTH 322
#define PIN_1 4
#define PIN_2 13
#define TARGET_FPS 5
#define MAX_BRIGHTNESS 50 // 0 to 255

//get colors from remote server instead of local effects
//#define REMOTE_JSON_MODE

//Power Limiting
#define PS_SAFE_LIMIT 8000 //Max safe power to draw from power supply in mA
	 
#define SIMULATED_BIOMETRICS_ENABLED false // Simulate heartbeat and EDA ripples

#define AUTO_PULSE_TIMEOUT 5000 // If no heartbeat is received in this many ms, begin firing random/simulated pulses
#define RANDOM_PULSE_TIME 2000  // Fire a random pulse every (this many) ms
#define AUTO_PULSE_CHANGE_TIME 30000


#define SIMULATED_HEARTBEAT_BASE_TIME 600 // Fire a simulated heartbeat pulse after at least this many ms
#define SIMULATED_HEARTBEAT_VARIANCE 200 // Add random jitter to simulated heartbeat
#define SIMULATED_EDA_BASE_TIME 1000	   // Same, but for inward EDA pulses
#define SIMULATED_EDA_VARIANCE 10000


namespace esphome {
namespace cube_light {

	static const char *const TAG = "CubeLight.light";

	enum class Features : int
	{
		PATTERN_RANDOM,
		PATTERN_CUBE,
		PATTERN_STARBURST
	};

	class CubeLight : public Component
	{
	public:
		CubeLight();
		void setup() override;
		void dump_config() override;
		void loop() override;
		// void receive_data(ArduinoJson::JsonObject data);
		
		void set_cube_enabled(bool en) { cubePulsesEnabled = en; }
		void set_starburst_enabled(bool en) { starburstPulsesEnabled = en; }
		void set_to_black();

		void set_feature_enabled(int feature, bool state){
			ESP_LOGD(TAG, "Setting Feature: %i to %i", feature, state);
			//ESP_LOGD(TAG, "enum %i", static_cast<int>(Features::PATTERN_RANDOM));
			switch(feature){
				case static_cast<int>(Features::PATTERN_RANDOM):
					randomPulsesEnabled = state;
					ESP_LOGD(TAG, "Random: %i", randomPulsesEnabled);
					break;
				case static_cast<int>(Features::PATTERN_CUBE):
					cubePulsesEnabled = state;
					break;
				case static_cast<int>(Features::PATTERN_STARBURST):
					starburstPulsesEnabled = state;
					break;
				default:
					ESP_LOGI(TAG, "Unknown feature: %s", feature);
					break;	
			}
		}

		CRGB *leds[NUM_STRIPS];
		bool state{false};
		byte ledColors[40][14][3]; // LED buffer - each ripple writes to this, then we write this to the strips
		float decay{0.97}; // Multiply all LED's by this amount each tick to create fancy fading tails

		bool randomPulsesEnabled = true;	// Fire random rainbow pulses from random nodes
		bool cubePulsesEnabled = true;		// Draw cubes at random nodes
		bool starburstPulsesEnabled = true; // Draw starbursts

		unsigned long lastRandomPulse;
		byte lastAutoPulseNode = 255;

		byte numberOfAutoPulseTypes = randomPulsesEnabled + cubePulsesEnabled + starburstPulsesEnabled;
		byte currentAutoPulseType = 255;
		unsigned long lastAutoPulseChange;
		unsigned long nextSimulatedHeartbeat;
		unsigned long nextSimulatedEda;
		unsigned long lastHeartbeat; // Track last heartbeat so we can detect noise/disconnections
		bool is_on = false;
	};

uint32_t hueToColor(unsigned int hue);

} // namespace cube_light
} //namespace esphome