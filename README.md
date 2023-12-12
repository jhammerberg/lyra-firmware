# Lyra Firmware
Code for the Arduino Nano flight computer onboard team Lyra's Engineering 100 section 980 rocket.

## Features
- 5ms regular sensor polling (100ms writing interval)
- Dynamic file creation (no data-overwrites)
- Various function checks pre-flight
- Clear countdown timer
- Heartbeat LED & Buzzer tone
- Easy to change configuration

## Sensors
- MPX5100 (Pressure)
- ADXL335 (Accelerometer)
- TMP36 (Temperature)

## How to use
Flash the code using the Arduino IDE. The program will only start collecting data when it detects an SD card and can succesfully write data to it without any other problems. However, due to our sensors being analog, there is no check to make sure your sensor connections are working so make sure your modules are attached.