// LIBRARY IMPORTS
// ----------------------------------------------------------------------------
// Do not touch unless your custom sensor needs an additional library

#include <SPI.h>
#include <SD.h>

// ----------------------------------------------------------------------------

// PIN DEFINITIONS
// ----------------------------------------------------------------------------
// Note: the pins each thing is connected to may have changed. You will need to
// reference the schematic for the rocket board here:
// https://980.engr100.org/project/sensor-board.html

// put pin definitions here

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



// CSV FILE HEADER
// ----------------------------------------------------------------------------
// This is the string that goes at the top of your csv file. It is the column 
// headers for your spreadsheet. You can/should it to match your analog sensors

const int readingInterval = 10; //ms
const String header = "Time (ms),TMP36 (Raw),Voltage (Raw),Pressure (Raw), Accel X (Raw), Accel Y (Raw), Accel Z (Raw)";
const String defaultDataFileName = "data";
String dataPath = defaultDataFileName + ".csv";
bool firstBoot = true;

// ----------------------------------------------------------------------------

// HELPER FUNCTIONS
// ----------------------------------------------------------------------------
// Functions to call to make setup() and loop() simpler.

// --------------------------Tone function (built-in)--------------------------
// Play a 1000Hz tone on the buzzer for 2000ms:
// tone(buzzerPin, 1000, 2000);
// Play a 440Hz (A4) tone on the buzzer:
// tone(buzzerPin, 440);
// This plays until either a new tone is passed
// or until notone is called:
// noTone(buzzerPin);

// ----------------------------------------------------------------------------


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

    //Turn on LED to show setup is ok and play a tune
    digitalWrite(LED_1, HIGH);
    tone(BUZZER, 1000, 1000);
    delay(250);
    tone(BUZZER, 2000, 1000);
    delay(250);
    tone(BUZZER, 3000, 1000);
    delay(250);
    noTone(BUZZER);
}

void write(String data) {
    if((firstBoot == true)) { //Check if the data file already exists and create a new one if it does
        firstBoot = false;
        //Serial.println("Data file already exists, creating a new one");
        int i = 1;
        while (SD.exists(defaultDataFileName + String(i) + ".csv")) {
            i++;
        }
        dataPath = defaultDataFileName + String(i) + ".csv";
        File dataFile = SD.open(dataPath, FILE_WRITE);
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
            dataFile.println(header);
            dataFile.close();
        } //Write the first message to the file
    }
    File dataFile = SD.open(dataPath, FILE_WRITE);
    if (dataFile) {
        dataFile.println(data);
        dataFile.close();
    } else {
        Serial.println("Error opening file");
        while (1) { //Warning if the file can't be written.
          tone(BUZZER, 1000, 2000);
          digitalWrite(LED_2, HIGH);
          delay(2000);
          noTone(BUZZER);
          digitalWrite(LED_2, LOW);
          delay(2000);
        }
    }
}   

String takeReadings() { //Should be about 10ms to sample ADCs
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

void loop() {
    if (millis() % readingInterval == 0) { //Take readings every 10ms
        digitalWrite(LED_1, !digitalRead(LED_1)); //Heartbeat LED
        String data = takeReadings();
        write(data);
    }

  // flush sd card file outputs

  // do LED and buzzer things

  // For LED: you have 6 different LEDs at your disposal. You must at a minimum
  // 1. Communicate power status of board (ie on/off)
  // 2. Communicate when a countdown has been initiated and roughly how long
  //    until data recording begins
  // 3. Communicate when data recording is in progress

  // For Buzzer: 
  // - In general, mirroring LED requirements 2&3 may help you know
  //  the status of your rocket even if you can't see the LEDs
  // - You MUST have your buzzer making loud and frequent noises after landing
  //  following the launch to add in locating your rocket payload
  // - You will need to utilize some of your sensor data to know when the
  //  rocket has landed. What sensor(s) you use to determine this is 100% up to
  //  you. For example, if you had an altitude sensor you could check your
  //  altitude in every loop and see if it was similar to the altitude when
  //  launch occurred and turn on the buzzer then. You will need to test this
  //  thouroughly before launch day as it is how we will find hiding rockets.
  // - Points may be deducted if your rocket is especially annoying and buzzing
  //  frequently during the launch preperation and countdown period in front of
  //  the teaching staff
}