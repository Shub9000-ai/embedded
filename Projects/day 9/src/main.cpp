#include <Arduino_APDS9960.h>

int count = 0;
bool objectDetected = false;
int color = 0;   // 0 = Red, 1 = Green, 2 = Blue

void setColor(int c) {
  // Turn all LEDs OFF (RGB LED is active LOW)
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  switch (c) {
    case 0: digitalWrite(LEDR, LOW); break;  // Red
    case 1: digitalWrite(LEDG, LOW); break;  // Green
    case 2: digitalWrite(LEDB, LOW); break;  // Blue
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);   // plain onboard LED (pin 13)
  digitalWrite(LED_BUILTIN, LOW);

  setColor(color);

  if (!APDS.begin()) {
    Serial.println("APDS9960 not found!");
    while (1);
  }

  Serial.println("Ready...");
}

void loop() {
  if (APDS.proximityAvailable()) {
    int proximity = APDS.readProximity();

    if (proximity > 200 && !objectDetected) {
      objectDetected = true;

      count++;

      Serial.print("Count = ");
      Serial.println(count);

      // Blink the normal onboard LED once per count
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);

      // When count reaches 10
      if (count >= 10) {
        count = 0;                 // Reset counter
        color = (color + 1) % 3;   // Next color
        setColor(color);

        Serial.println(">>> Color Changed <<<");
      }
    }

    // Reset detection when object moves away
    if (proximity < 100) {
      objectDetected = false;
    }
  }
}