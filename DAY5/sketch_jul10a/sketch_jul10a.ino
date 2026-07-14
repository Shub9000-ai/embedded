#include <SPI.h>

void setup() {
  // Start Serial Monitor
  Serial.begin(9600);
  while (!Serial);

  // Initialize SPI bus
  SPI.begin();

  // Configure Chip Select (CS) pin
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); // Deselect slave

  Serial.println("SPI Master Demo Started");
}

void loop() {
  // Select slave (though none is connected)
  digitalWrite(10, LOW);

  // Send a byte (0x42 = 'B') and read response
  byte response = SPI.transfer(0x42);

  // Deselect slave
  digitalWrite(10, HIGH);

  // Print response to Serial Monitor
  Serial.print("Received: ");
  Serial.println(response, HEX);

  delay(1000); // Wait 1 second
}