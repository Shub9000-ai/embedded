void setup() {
  pinMode(LEDB, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDR, OUTPUT);
}

void loop() {
  digitalWrite(LEDB, LOW);    // Red ON
  digitalWrite(LEDG, LOW);   // Green OFF
  digitalWrite(LEDR, LOW);   // Blue OFF
}