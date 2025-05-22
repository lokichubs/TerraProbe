#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// RS485 Module Pins
#define RE 8
#define DE 7
#define RO 10
#define DI 11

// Soil Moisture Sensor Pins
#define sensorPower 6
#define sensorPin A0

// SD Card Module Pins
#define SD_CS 53  // Chip Select Pin for SD Card

// Button Pin
#define buttonPin 4
bool recording = false;  // Flag to control data recording

// Modbus RTU requests for reading NPK values
const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[]  = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[]  = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

// A variable to store NPK values
byte values[11];

// Setup SoftwareSerial for RS485 communication
SoftwareSerial mod(10, 11);

// File name for the TXT
String fileName = "";

void setup() {
  Serial.begin(9600);
  mod.begin(9600);

  // Set up RS485 control pins
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  // Set up Soil Moisture Sensor
  pinMode(sensorPower, OUTPUT);
  digitalWrite(sensorPower, LOW);  // Initially OFF

  // Set up Button
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card Initialization Failed!");
    return;
  }
  Serial.println("SD Card Initialized.");

  delay(500);
}

void loop() {
  // Check if the button is pressed
  if (digitalRead(buttonPin) == LOW) {
    recording = !recording; // Toggle recording state
    if (recording) {
      // Create a new file name with timestamp
      fileName = "soil_data_" + String(millis()) + ".txt"; // Unique file name
      Serial.print("Recording Started! Saving to: ");
      Serial.println(fileName);
      
      // Create a new TXT file and add a header
      File dataFile = SD.open(fileName, FILE_WRITE);
      if (dataFile) {
        dataFile.println("SoilMoisture Nitrogen Phosphorous Potassium");
        dataFile.close();
      } else {
        Serial.println("Error creating TXT file.");
      }
    } else {
      Serial.println("Recording Stopped!");
    }
    delay(1000); // Debounce delay
  }

  // If not recording, continue to read sensor values and display
  if (!recording) {
    // Read soil moisture value
    int soilMoisture = readSoilMoisture();

    // Read NPK values
    byte nitrogenValue = nitrogen();
    delay(250);
    byte phosphorousValue = phosphorous();
    delay(250);
    byte potassiumValue = potassium();
    delay(250);

    // Print sensor readings to Serial Monitor
    Serial.println("==== Sensor Readings ====");
    Serial.print("Soil Moisture: ");
    Serial.println(soilMoisture);
    Serial.print("Nitrogen: ");
    Serial.print(nitrogenValue);
    Serial.println(" mg/kg");
    Serial.print("Phosphorous: ");
    Serial.print(phosphorousValue);
    Serial.println(" mg/kg");
    Serial.print("Potassium: ");
    Serial.print(potassiumValue);
    Serial.println(" mg/kg");
    Serial.println("=========================");
  } else {
    // If recording, read soil moisture and NPK values
    int soilMoisture = readSoilMoisture();
    byte nitrogenValue = nitrogen();
    delay(250);
    byte phosphorousValue = phosphorous();
    delay(250);
    byte potassiumValue = potassium();
    delay(250);

    // Save data to SD Card
    saveToSD(soilMoisture, nitrogenValue, phosphorousValue, potassiumValue);

    // Print the data that was saved to the Serial Monitor
    Serial.print("Saved to SD Card: ");
    Serial.print(soilMoisture);
    Serial.print(", ");
    Serial.print(nitrogenValue);
    Serial.print(", ");
    Serial.print(phosphorousValue);
    Serial.print(", ");
    Serial.println(potassiumValue);
  }

  delay(2000);
}

// Function to read soil moisture sensor
int readSoilMoisture() {
  digitalWrite(sensorPower, HIGH);
  delay(10);
  int value = analogRead(sensorPin);
  digitalWrite(sensorPower, LOW);
  return value;
}

// Function to read Nitrogen from NPK sensor
byte nitrogen() {
  return readNPK(nitro);
}

// Function to read Phosphorous from NPK sensor
byte phosphorous() {
  return readNPK(phos);
}

// Function to read Potassium from NPK sensor
byte potassium() {
  return readNPK(pota);
}

// Function to communicate with RS485 and retrieve NPK values
byte readNPK(const byte *command) {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);
  
  if (mod.write(command, 8) == 8) {
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    for (byte i = 0; i < 7; i++) {
      values[i] = mod.read();
    }
  }
  return values[4];
}

// Function to save data to SD Card
void saveToSD(int soilMoisture, byte nitrogen, byte phosphorous, byte potassium) {
  File dataFile = SD.open(fileName, FILE_WRITE);
  
  if (dataFile) {
    dataFile.print(soilMoisture);
    dataFile.print(" ");
    dataFile.print(nitrogen);
    dataFile.print(" ");
    dataFile.print(phosphorous);
    dataFile.print(" ");
    dataFile.println(potassium);
    dataFile.close();
    Serial.println("Data saved to SD card.");
  } else {
    Serial.println("Error opening SD file.");
  }
}