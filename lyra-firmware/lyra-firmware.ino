/*
 * lyra-firmware.ino
 *
 *  Created on: Nov 2, 2023
 *      Author: jackmh
 */
// ----------------------------------------------------------------------------
// LIBRARY IMPORTS
// ----------------------------------------------------------------------------

#include <SPI.h>
#include <SD.h>

// ----------------------------------------------------------------------------
// PIN DEFINITIONS
// ----------------------------------------------------------------------------

#define PRES_PIN A0
#define IMU_X A1
#define IMU_Y A2
#define IMU_Z A3
#define BAT_PIN A6
#define TMP_PIN A7
#define SD_CS 10
#define LED_1 4
#define LED_2 5
#define LED_3 6
#define LED_4 7
#define LED_5 8
#define LED_6 9
#define BUZZER 3

// ----------------------------------------------------------------------------
// CONSTANTS
// ----------------------------------------------------------------------------

const bool activeTone = true; //If a tone will play while recording data
int startupTime = 30; //seconds
const int readingInterval = 5; //ms (cannot be lower than 1ms, lower values have more error)
const String header = "Time (ms),TMP36 (Raw),Voltage (Raw),Pressure (Raw), Accel X (Raw), Accel Y (Raw), Accel Z (Raw)";
const String defaultDataFileName = "data";
String dataPath = defaultDataFileName + ".csv"; //ok not a constant cause this needs to be changed

String takeReadings() {
  int tmp_raw = analogRead(TMP_PIN);
  int pres_raw = analogRead(PRES_PIN);
  int bat_raw = analogRead(BAT_PIN);
  int x_raw = analogRead(IMU_X);
  int y_raw = analogRead(IMU_Y);
  int z_raw = analogRead(IMU_Z);

  String dataString = 
    String(millis()) + "," + 
    String(tmp_raw) + "," + 
    String(bat_raw) + "," + 
    String(pres_raw) + "," + 
    String(x_raw) + "," + 
    String(y_raw) + "," + 
    String(z_raw);
  
  return dataString;
}

void setup() {
  //Digital outputs
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_5, OUTPUT);
  pinMode(LED_6, OUTPUT);
  Serial.begin(9600);
  delay(100); // Delay for serial monitor

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
      Serial.println("Card failed, or not present");
      while (1) { //Warning if the SD Card isn't found
          tone(BUZZER, 1000, 2000);
          digitalWrite(LED_2, HIGH);
          delay(2000);
          noTone(BUZZER);
          digitalWrite(LED_2, LOW);
          delay(2000);
      }
  }
  Serial.println("Card initialized.");

  //Creates a new data file if there is already one that exists with the same name
  int i = 1;
  while (SD.exists(defaultDataFileName + String(i) + ".csv")) {
      i++;
  }
  dataPath = defaultDataFileName + String(i) + ".csv";
  File dataFile = SD.open(dataPath, FILE_WRITE);

  //Write the header & check if the file is open-able
  if (!dataFile) { 
      Serial.println("Error opening file");
      while (1) { //Warning if the SD Card isn't found
          tone(BUZZER, 1000, 2000);
          digitalWrite(LED_2, HIGH);
          delay(2000);
          noTone(BUZZER);
          digitalWrite(LED_2, LOW);
          delay(2000);
      }
  } else {
  }

  //Turn on LED to show setup is ok and play a tune
  digitalWrite(LED_1, HIGH);
  tone(BUZZER, 1000);
  delay(250);
  tone(BUZZER, 2000);
  delay(250);
  tone(BUZZER, 3000);
  delay(250);
  noTone(BUZZER);

  //Countdown timer until sensors start recording
  tone(BUZZER, 500, 100);
  delay(2000);
  long actualTime = ((startupTime*1000)+(millis()));
  Serial.println(actualTime);
  i = 15;
  while (millis() < actualTime) { //Will run the loop until the startup time has elapsed
    tone(BUZZER, 500, 50); //Beep
    digitalWrite(LED_1, !digitalRead(LED_1));
    delay((startupTime*1000)/i); //Delay by a fraction of the startup time
    if (((startupTime*1000)/(i*2)) > 50) { //Time between beeps gets smaller and smaller until it is below 50ms 
      i = (i*1.1);
    }
  }
  if (activeTone) {
    tone(BUZZER, 500); //Tone to show that it's recording
  } 
}

void loop() {
  File dataFile = SD.open(dataPath, FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error opening file");
    while (1) { //Warning if the file can't be written.
      tone(BUZZER, 1000, 2000);
      digitalWrite(LED_2, HIGH);
      delay(2000);
      noTone(BUZZER);
      digitalWrite(LED_2, LOW);
      delay(2000);
    }
  } else {
    dataFile.println(header);
    while (1) { //Yes this is a loop inside of a loop but I had to do it so the SD card can be closed.
      if (millis() % readingInterval == 1) { //Take readings every reading interval + or - 1ms
        String data = takeReadings();
        dataFile.println(data);
      }
      if (millis() % 100 == 1) { //Every 100ms write to the SD card
        dataFile.flush(); //Writes data from buffer to the SD card without having to close and reopen it
      }
    }
    dataFile.close();
  }
}