
  Arduino Nano 33 BLE Sense
  APDS9960 Proximity Sensor
  Grove LCD RGB Backlight

  Displays proximity on LCD
  Sends proximity to PyCharm


#include <Arduino_APDS9960.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

void setup() {

  Serial.begin(9600);
  while (!Serial);

  // Initialize LCD
  lcd.begin(16, 2);

  lcd.setRGB(255, 255, 255);

  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait");

  // Initialize APDS9960
  if (!APDS.begin()) {
    lcd.clear();
    lcd.print("Sensor Error");
    while (1);
  }

  delay(2000);

  lcd.clear();
}

void loop() {

  if (APDS.proximityAvailable()) {

    int proximity = APDS.readProximity();

    // Send to PyCharm
    Serial.println(proximity);

    // Calculate brightness
    int brightness;

    if (proximity <= 20) {
      brightness = 0;
    }
    else {
      brightness = map(proximity, 21, 255, 20, 255);
      brightness = constrain(brightness, 20, 255);
    }

    // Set LCD backlight
    lcd.setRGB(brightness, brightness, brightness);

    // Update LCD
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Proximity:");

    lcd.setCursor(0, 1);
    lcd.print(proximity);
    lcd.print("    ");

    delay(200);
  }
}