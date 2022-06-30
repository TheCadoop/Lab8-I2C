#include <Wire.h>
#include "oled-wing-adafruit.h"
#include <blynk.h>

SYSTEM_THREAD(ENABLED);

#define RED_PIN D6
#define YELLOW_PIN D7
#define GREEN_PIN D8

OledWingAdafruit display;
uint8_t displayInfo = 0;
uint8_t tempDisplayInfo = 0;
bool pressed = false;

const uint8_t ADDRESS = 0x60;
const uint8_t PROXIMITY_COMMAND = 0x08;
const uint8_t LIGHT_COMMAND = 0x09;
const uint8_t LIGHT_ENABLE = 0x00;
const uint8_t PROX_ENABLE = 0x03;

unsigned int read(uint8_t address, uint8_t command) {
  Wire.beginTransmission(address);
  Wire.write(command);
  Wire.endTransmission(false);

  Wire.requestFrom(address, 2);
  uint8_t lsb = Wire.read();
  uint8_t msb = Wire.read();

  return (unsigned int)((uint16_t)msb << 8 | lsb);
}

void setup() {
  Wire.begin();
  Serial.begin(9600);

  Wire.beginTransmission(ADDRESS);
  Wire.write(LIGHT_ENABLE);
  Wire.write(LIGHT_ENABLE);
  Wire.endTransmission(false);

  Wire.beginTransmission(ADDRESS);
  Wire.write(PROX_ENABLE);
  Wire.write(LIGHT_ENABLE);
  Wire.endTransmission(false);

  Blynk.begin("gev9UlWHupS8yCBJ2wpDXDNm6THMu0oS", IPAddress(167, 172, 234, 162), 8080);

  display.setup();

  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
}

void loop() {
  display.loop();
  
  unsigned int proxValue = read(ADDRESS, PROXIMITY_COMMAND);
  unsigned int lightValue = read(ADDRESS, LIGHT_COMMAND);

  if (proxValue < 65) {
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
    display.println(lightValue);
  } else if (displayInfo == 3) {
    Blynk.virtualWrite(V0, proxValue);
    Blynk.virtualWrite(V1, lightValue);
  }
  display.display();
}