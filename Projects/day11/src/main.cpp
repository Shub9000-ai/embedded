#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

// BLE Service
BLEService imuService("19B10000-E8F2-537E-4F6C-D104768A1214");

// BLE Characteristic
BLEStringCharacteristic imuCharacteristic(
    "19B10001-E8F2-537E-4F6C-D104768A1214",
    BLERead | BLENotify,
    50
);

void setup()
{
    Serial.begin(9600);

    while (!Serial);

    if (!IMU.begin())
    {
        Serial.println("Failed to initialize IMU!");
        while (1);
    }

    if (!BLE.begin())
    {
        Serial.println("Failed to initialize BLE!");
        while (1);
    }

    BLE.setLocalName("Shubham");
    BLE.setDeviceName("Shubham");

    BLE.setAdvertisedService(imuService);

    imuService.addCharacteristic(imuCharacteristic);

    BLE.addService(imuService);

    imuCharacteristic.writeValue("Ready");

    BLE.advertise();

    Serial.println("------------------------");
    Serial.println("BLE Started");
    Serial.println("Device Name : Shubham");
    Serial.println("Waiting for Phone...");
    Serial.println("------------------------");
}

void loop()
{
    BLEDevice central = BLE.central();

    if (central)
    {
        Serial.print("Connected : ");
        Serial.println(central.address());

        while (central.connected())
        {
            float x, y, z;

            if (IMU.accelerationAvailable())
            {
                IMU.readAcceleration(x, y, z);

                String data =
                    "X:" + String(x, 2) +
                    " Y:" + String(y, 2) +
                    " Z:" + String(z, 2);

                imuCharacteristic.writeValue(data);

                Serial.println(data);

                delay(100);
            }
        }

        Serial.println("Disconnected");

        BLE.advertise();
    }
}