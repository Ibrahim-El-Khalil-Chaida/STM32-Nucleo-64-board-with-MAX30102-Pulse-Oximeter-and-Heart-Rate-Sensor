#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <LiquidCrystal_I2C.h>

// ===== Constants & Pin Definitions =====
static const uint8_t PULSE_LED_PIN    = 11;   // PWM pin for measurement indicator
static const uint8_t READ_LED_PIN     = 13;   // Blinks on each sample read
static const uint8_t LCD_I2C_ADDR     = 0x27; // I²C address of the LCD
static const uint16_t BUFFER_SIZE     = 100;  // Number of samples (4 s at 25 SPS)

static const byte  LED_BRIGHTNESS     = 60;   // 0–255 (0 = off, 255 = 50 mA)
static const byte  SAMPLE_AVERAGE     = 4;    // Averaging factor: 1,2,4,8,16,32
static const byte  LED_MODE           = 2;    // 1=Red only, 2=Red+IR, 3=all three
static const byte  SAMPLE_RATE        = 100;  // Samples per second
static const int   PULSE_WIDTH        = 411;  // LED pulse width (69,118,215,411)
static const int   ADC_RANGE          = 4096; // ADC range (2048,4096,8192,16384)

MAX30105       sensor;
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, 2, 1, 0, 4, 5, 6, 7);

// Data buffers and measurement variables
uint16_t redBuffer[BUFFER_SIZE];
uint16_t irBuffer[BUFFER_SIZE];
int32_t  bufferLength = BUFFER_SIZE;
int32_t  heartRate, spo2;
int8_t   validHeartRate, validSpO2;

void setup() {
  // Initialize serial for debugging/logging
  Serial.begin(115200);

  // Setup LED indicators
  pinMode(PULSE_LED_PIN, OUTPUT);
  pinMode(READ_LED_PIN, OUTPUT);

  // Initialize I²C LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();

  // Initialize MAX30105 sensor at Fast I²C (400 kHz)
  if (!sensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("ERROR: MAX30105 not found!"));
    while (1) delay(1000); // Halt
  }
  // Configure sensor settings
  sensor.setup(LED_BRIGHTNESS, SAMPLE_AVERAGE, LED_MODE,
               SAMPLE_RATE, PULSE_WIDTH, ADC_RANGE);

  // Prompt user to place finger
  lcd.setCursor(0, 0);
  lcd.print("Reposez Un  Doigt");
  // Wait until IR reading exceeds threshold (~presence of finger)
  while (sensor.getIR() < 30000) {
    delay(50);
  }
  lcd.clear();
  lcd.print("Reading...");
  readInitialBuffer(); // Fill initial sample buffer
  lcd.clear();
}

void loop() {
  // Perform rolling-sample acquisition and display updates forever
  updateRollingSamples();
  // Calculate HR & SpO2
  maxim_heart_rate_and_oxygen_saturation(
    irBuffer, bufferLength, redBuffer,
    &spo2, &validSpO2, &heartRate, &validHeartRate
  );
  // Show results on LCD
  displayResults();
}

// Read the first 100 samples into buffers (initial calibration)
void readInitialBuffer() {
  for (uint16_t i = 0; i < BUFFER_SIZE; ++i) {
    // Wait for data ready
    while (!sensor.available()) sensor.check();
    redBuffer[i] = sensor.getRed();
    irBuffer[i]  = sensor.getIR();
    sensor.nextSample();
  }
}

// Shift out the oldest 25 samples, read 25 new ones
void updateRollingSamples() {
  // Shift 75 samples up
  for (uint16_t i = 25; i < BUFFER_SIZE; ++i) {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25]  = irBuffer[i];
  }
  // Read 25 new samples
  for (uint16_t i = 75; i < BUFFER_SIZE; ++i) {
    while (!sensor.available()) sensor.check();
    digitalWrite(READ_LED_PIN, !digitalRead(READ_LED_PIN));
    redBuffer[i] = sensor.getRed();
    irBuffer[i]  = sensor.getIR();
    sensor.nextSample();
  }
}

// Print heart rate & SpO2 to LCD
void displayResults() {
  lcd.setCursor(0, 0);
  lcd.print("HR: ");
  validHeartRate ? lcd.print(heartRate) : lcd.print("--");
  lcd.print(" bpm");

  lcd.setCursor(0, 1);
  lcd.print("SpO2: ");
  validSpO2 ? lcd.print(spo2) : lcd.print("--");
  lcd.print(" %");
}
