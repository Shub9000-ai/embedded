/*
  Arduino Nano 33 BLE Sense
  APDS9960 Color Sensor + Grove LCD RGB Backlight

  Detects:
  RED
  GREEN
  BLUE
  YELLOW
  CYAN
  MAGENTA
  WHITE
  BLACK

  Initial LCD:
  No Color
  Detected
*/

#include <Arduino_APDS9960.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

String lastColor = "";

void showColor(String name, int R, int G, int B)
{
  if (lastColor != name)
  {
    lcd.clear();
    lcd.setRGB(R, G, B);

    lcd.setCursor(0, 0);
    lcd.print("Detected:");

    lcd.setCursor(0, 1);
    lcd.print(name);

    lastColor = name;
  }
}

void showNoColor()
{
  if (lastColor != "NONE")
  {
    lcd.clear();
    lcd.setRGB(255,255,255);

    lcd.setCursor(0,0);
    lcd.print("No Color");

    lcd.setCursor(0,1);
    lcd.print("Detected");

    lastColor = "NONE";
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  lcd.begin(16,2);

  lcd.setRGB(255,255,255);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Initializing");

  lcd.setCursor(0,1);
  lcd.print("Sensor...");

  if (!APDS.begin())
  {
    lcd.clear();
    lcd.print("Sensor Error");

    Serial.println("APDS9960 not found.");

    while(1);
  }

  delay(1500);

  showNoColor();

  Serial.println("--------------------------------");
  Serial.println("Color Sensor Ready");
  Serial.println("--------------------------------");
}

void loop()
{
  int r, g, b;

  if (APDS.colorAvailable())
  {
    APDS.readColor(r, g, b);

    Serial.print("R=");
    Serial.print(r);

    Serial.print(" G=");
    Serial.print(g);

    Serial.print(" B=");
    Serial.println(b);

    int brightness = r + g + b;

    // Ignore dark objects
    if (brightness < 120)
    {
      showNoColor();
      delay(200);
      return;
    }

    // WHITE
    if(r>180 && g>180 && b>180)
    {
      showColor("WHITE",255,255,255);
    }

    // BLACK
    else if(brightness<180)
    {
      showColor("BLACK",0,0,0);
    }

    // RED
    else if(r>g+60 && r>b+60)
    {
      showColor("RED",255,0,0);
    }

    // GREEN
    else if(g>r+60 && g>b+60)
    {
      showColor("GREEN",0,255,0);
    }

    // BLUE
    else if(b>r+60 && b>g+60)
    {
      showColor("BLUE",0,0,255);
    }

    // YELLOW
    else if(r>160 && g>160 && b<100)
    {
      showColor("YELLOW",255,255,0);
    }

    // CYAN
    else if(g>150 && b>150 && r<100)
    {
      showColor("CYAN",0,255,255);
    }

    // MAGENTA
    else if(r>150 && b>150 && g<100)
    {
      showColor("MAGENTA",255,0,255);
    }

    else
    {
      lcd.clear();

      lcd.setRGB(255,255,255);

      lcd.setCursor(0,0);
      lcd.print("Unknown");

      lcd.setCursor(0,1);
      lcd.print("Color");

      lastColor = "UNKNOWN";
    }

    delay(300);
  }
}