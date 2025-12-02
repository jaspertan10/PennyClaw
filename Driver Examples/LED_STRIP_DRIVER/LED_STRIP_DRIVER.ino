#include <Adafruit_NeoPixel.h>
#include <math.h>

#define LED_PIN       6
#define LED_COUNT     5          // however many indicator LEDs you want
#define MAX_BRIGHT    150        // gentle brightness peak (0–255)
#define MIN_BRIGHT    8          // don’t go fully dark
#define BREATH_TIME   3000       // 3 seconds per full breath cycle (slow)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show();
}

void loop() {
  updatePendingBreath();
}

// --- Soft Yellow Breathing Effect ---
void updatePendingBreath() {
  static uint32_t start = millis();
  uint32_t now = millis();

  // How far we are into the breath cycle (0 to 1)
  float phase = float((now - start) % BREATH_TIME) / BREATH_TIME;

  // Smooth breathing curve using a cosine wave:
  // 0 → 1 → 0
  float breath = (1.0f - cosf(phase * 2.0f * PI)) * 0.5f;

  // Map breathing curve to brightness
  uint8_t brightness = (uint8_t)(MIN_BRIGHT + breath * (MAX_BRIGHT - MIN_BRIGHT));

  // Yellow = Red + Green
  uint32_t color = strip.Color(brightness, brightness, 0);

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, color);
  }

  strip.show();
}
