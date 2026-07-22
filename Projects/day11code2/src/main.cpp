#include <Arduino.h>
#include <ArduinoBLE.h>

const int ledPin = LED_BUILTIN;

// BLE Service
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");

// BLE Characteristic (Read + Write)
BLEStringCharacteristic ledCharacteristic(
    "19B10001-E8F2-537E-4F6C-D104768A1214",
    BLERead | BLEWrite,
    20
);

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  if (!BLE.begin()) {
    Serial.println("Failed to start BLE!");
    while (1);
  }

  BLE.setLocalName("Shubham");
  BLE.setDeviceName("Shubham");

  BLE.setAdvertisedService(ledService);

  ledService.addCharacteristic(ledCharacteristic);
  BLE.addService(ledService);

  ledCharacteristic.writeValue("OFF");

  BLE.advertise();

  Serial.println("BLE LED Control Started");
  Serial.println("Device Name: Shubham");
  Serial.println("Waiting for connection...");
}

void loop() {

  BLEDevice central = BLE.central();

  if (central) {

    Serial.print("Connected: ");
    Serial.println(central.address());

    while (central.connected()) {

      if (ledCharacteristic.written()) {

        String command = ledCharacteristic.value();

        command.trim();
        command.toUpperCase();

        Serial.print("Received: ");
        Serial.println(command);

        if (command == "ON") {
          digitalWrite(ledPin, HIGH);
          Serial.println("LED ON");
        }
        else if (command == "OFF") {
          digitalWrite(ledPin, LOW);
          Serial.println("LED OFF");
        }
      }
    }

    Serial.println("Disconnected");

    BLE.advertise();
  }
}