#include "SparkFun_VCNL4040_Arduino_Library.h"
#include <Wire.h>
#include "oled-wing-adafruit.h"
#include <blynk.h>

SYSTEM_THREAD(ENABLED);

#define RED_PIN D6
#define YELLOW_PIN D7
#define GREEN_PIN D8

VCNL4040 proximitySensor;
OledWingAdafruit display;
uint8_t displayInfo = 0;
uint8_t tempDisplayInfo = 0;
bool pressed = false;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  proximitySensor.begin();
  proximitySensor.powerOnAmbient();

  Blynk.begin("gev9UlWHupS8yCBJ2wpDXDNm6THMu0oS", IPAddress(167, 172, 234, 162), 8080);

  display.setup();

  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
}

void loop() {
  display.loop();
  
  unsigned int proxValue = proximitySensor.getProximity();
  unsigned int proxLight = proximitySensor.getAmbient();
  Serial.println(proxValue);

  if (proxValue < 35) {
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(RED_PIN, LOW);
  } else if (proxValue < 130) {
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
  } else {
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(YELLOW_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
  }

  if (display.pressedA()) {
    displayInfo = 1;
  }

  if (display.pressedB()) {
    displayInfo = 2;
  }

  if (display.pressedC()) {
    if (!pressed) {
      tempDisplayInfo = displayInfo;
      displayInfo = 3;
    } else {
      displayInfo = tempDisplayInfo;
    }
    pressed = !pressed;
  }

  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(5, 8);

  if (displayInfo == 1) {
    display.println(proxValue);
  } else if (displayInfo == 2) {
    display.println(proxLight);
  } else if (displayInfo == 3) {
    Blynk.virtualWrite(V0, proxValue);
    Blynk.virtualWrite(V1, proxLight);
  }
  display.display();
}