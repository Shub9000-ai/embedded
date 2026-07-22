#include <Arduino.h>
#include <PDM.h>
#include <arduinoFFT.h>

#define SAMPLES 512
#define SAMPLING_FREQUENCY 16000

double vReal[SAMPLES];
double vImag[SAMPLES];

short sampleBuffer[256];
volatile int samplesRead = 0;

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

void onPDMdata() {
    int bytesAvailable = PDM.available();
    PDM.read(sampleBuffer, bytesAvailable);
    samplesRead = bytesAvailable / 2;
}

void setup() {
    Serial.begin(115200);
    while (!Serial);

    PDM.onReceive(onPDMdata);

    if (!PDM.begin(1, SAMPLING_FREQUENCY)) {
        Serial.println("Failed to start microphone!");
        while (1);
    }

    Serial.println("Frequency Analyzer Started");
}

void loop() {

    static int index = 0;

    if (samplesRead > 0) {

        for (int i = 0; i < samplesRead && index < SAMPLES; i++) {
            vReal[index] = sampleBuffer[i];
            vImag[index] = 0;
            index++;
        }

        samplesRead = 0;
    }

    if (index >= SAMPLES) {

        FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
        FFT.compute(FFTDirection::Forward);
        FFT.complexToMagnitude();

        double peak = FFT.majorPeak();

        Serial.print("Dominant Frequency: ");
        Serial.print(peak);
        Serial.println(" Hz");

        index = 0;
    }
}