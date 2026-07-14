/*
  Arduino Nano 33 BLE
  Accelerometer with RGB LED

  Left Tilt (X < -0.5)      -> RED LED
  Right Tilt (X > 0.5)      -> BLUE LED
  Forward Tilt (Y > 0.5)    -> GREEN LED
  Backward Tilt (Y < -0.5)  -> RED + BLUE (Purple)

  Prints X, Y and Z values on Serial Monitor.
*/

#include <Arduino_LSM9DS1.h>

void setup() {

  Serial.begin(9600);

  while (!Serial);

  // RGB LED Pins
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // Turn OFF all LEDs
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  // Initialize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.println("Accelerometer Started");
  Serial.println("---------------------");
  Serial.println("X\tY\tZ");
}

void loop() {

  float x, y, z;

  if (IMU.accelerationAvailable()) {

    IMU.readAcceleration(x, y, z);

    // Print X, Y, Z values
    Serial.print(x);
    Serial.print("\t");
    Serial.print(y);
    Serial.print("\t");
    Serial.println(z);

    // Turn OFF all LEDs
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);

    // LEFT
    if (x < -0.5) {
      digitalWrite(LEDR, LOW);
    }

    // RIGHT
    else if (x > 0.5) {
      digitalWrite(LEDB, LOW);
    }

    // FORWARD
    else if (y > 0.5) {
      digitalWrite(LEDG, LOW);
    }

    // BACKWARD
    else if (y < -0.5) {
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDB, LOW);
    }
  }

  delay(100);
}