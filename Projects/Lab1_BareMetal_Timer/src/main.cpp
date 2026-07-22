#include <Arduino.h>
#include <PDM.h>

short sampleBuffer[256];
volatile int samplesRead = 0;

const int LED_PIN = LED_BUILTIN;
const int THRESHOLD = 800;   // Adjust this value

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(LED_PIN, OUTPUT);

  PDM.onReceive(onPDMdata);

  if (!PDM.begin(1, 16000)) {
    Serial.println("Microphone failed!");
    while (1);
  }

  Serial.println("Clap/Voice Detection Started");
}

void loop() {
  if (samplesRead > 0) {

    long sum = 0;

    for (int i = 0; i < samplesRead; i++) {
      sum += abs(sampleBuffer[i]);
    }

    int level = sum / samplesRead;

    Serial.println(level);

    if (level > THRESHOLD) {
      Serial.println("Sound Detected!");
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
    }

    samplesRead = 0;
  }
}