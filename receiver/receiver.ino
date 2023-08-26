#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#include <esp_wifi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define DATA_PIN    12
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

int maxBrightness = 64;
int transitionDuration = 1800;

struct LEDData {
  int effects;
};

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  LEDData receivedData;
  memcpy(&receivedData, data, sizeof(receivedData));

  if (receivedData.effects == 1) {
    // ripple turn blue to green
    ledTransitionEffect(transitionDuration);
  } else if (receivedData.effects == 2) {
    // turn red
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(255, 0, 0);
    }
    FastLED.show();
  } else if (receivedData.effects == 3) {
    // turn blue
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 255);
  }} else if (receivedData.effects == 0) {
    for (int i = 0; i < NUM_LEDS; i++) {
      // turn off leds
      leds[i] = CRGB(0, 0, 0);
    }
  } else if (receivedData.effects == 9) {
    runningWhiteLED(2000);
  }
  FastLED.show();
  }
  
  


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable detector
  Serial.begin(115200);
  

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESPNow Init Failed");
    return;
  }

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(64);

  esp_now_register_recv_cb(OnDataRecv);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  // Startup LED
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(9, 100 ,1);
  }

  FastLED.show();

  delay(2000);

  FastLED.clear();

  FastLED.show();

}

void loop() {
  // Receiver code doesn't need to do anything in the loop.
}

void ledTransitionEffect(int transitionDuration) {
  int startTime = millis();
  int currentTime;

  do {
    currentTime = millis() - startTime;

    if (currentTime > transitionDuration) {
      FastLED.setBrightness(maxBrightness);
      fill_solid(leds, NUM_LEDS, CRGB(9,100,1)); // Change to solid green color
    } else {
      float fraction = float(currentTime) / float(transitionDuration); // Fraction of transition completed
      float blinkFrequency = 0.5 + 5.0 * fraction; // Adjust the 0.5 and 5.0 for different blink speeds

      // Calculate the brightness using a sinusoidal function
      byte brightness = (maxBrightness/2.0) * (1 + sin(2 * PI * blinkFrequency * currentTime / 1000.0));
  
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 130, 100);
      }

      FastLED.setBrightness(brightness);
    }

    FastLED.show();
    delay(50); // Adjust as needed
  } while (currentTime <= transitionDuration);
}

void runningWhiteLED(int duration) {
    int position = 0; // Starting position
    unsigned long startTime = millis();  // Store the start time

    while (millis() - startTime < duration) {  // Run the effect for the specified duration
        for (int i = 0; i < NUM_LEDS; i++) {
            if (i == position) {
                leds[i] = CRGB::White;  // Set the current LED to white
            } else {
                leds[i] = CRGB::Black;  // Turn off all other LEDs
            }
        }

        FastLED.show();
        delay(100);  // Adjust this delay for the speed of the running LED

        position++;  // Move to the next LED
        if (position >= NUM_LEDS) {
            position = 0;  // Wrap back to the start if we've reached the end
        }
    }

    // Clear the LEDs after stopping the effect
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}
