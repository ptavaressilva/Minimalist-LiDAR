// Minimalist LiDAR
//  by Pedro Tavares Silva
//
// Tutorial at https://releasetheinnergeek.wordpress.com/
//
// Hardware: - Arduino Nano
//           - sg90 servo
//           - VL53L0X breakout of th GY-530 time of flight laser ranger
//           - SSD1306 0,96" I2C 128x64 OLED screen
//           - 6x M2-10 screws (nuts optional)
//           - 1x M3-8 screw + 2x M3 nuts
//
// GNU General Public License v2.0
//

byte measurement[128] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // array of sensor readings

const byte servoPosition[128] = {
  52,53,53,54,55,56,56,57,58,58,59,60,61,61,62,63,63,64,65,65,66,67,68,68,69,70,70,71,72,73,73,74,
  75,75,76,77,78,78,79,80,80,81,82,82,83,84,85,85,86,87,87,88,89,90,90,91,92,92,93,94,95,95,96,97,
  97,98,99,99,100,101,102,102,103,104,104,105,106,107,107,108,109,109,110,111,112,112,113,114,114,
  115,116,116,117,118,119,119,120,121,121,122,123,124,124,125,126,126,127,128,129,129,130,131,131,
  132,133,133,134,135,136,136,137,138,138,139,140,141,141,142}; // array of sensor positions

boolean servoDirection = 1; // 0 = left     1 = right

byte screenPosition = 0; // start on the left side of the screen

// ---------- SG90 --------------
#include <Servo.h>
#define PTS_ServoMin 52
#define PTS_ServoMax 142
#define PTS_ServoPin 5
Servo servo;

// ---------- OLED --------------
// For the appropriate u8g2 constructor for your OLED screen
// go to https://github.com/olikraus/u8g2/wiki/u8g2setupcpp

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE); // constructor

// math
#include <Math.h>

// ---------- VL53L0X -----------
#include "Adafruit_VL53L0X.h"
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t reading;

void setup() {

  // servo setup
  pinMode(PTS_ServoPin, OUTPUT);
  servo.attach(PTS_ServoPin);

  // OLED setup
  u8g2.begin();

  // VL53L0X setup
  lox.begin();
  lox.rangingTest(&reading, false);

}

void loop() {

  // move servo to next position ==============================================

  if ((screenPosition == 0) && (servoDirection == 0)) { // reached left side
    screenPosition = 1;
    servoDirection = 1;
  } else if ((screenPosition == 127) && (servoDirection == 1)) { // reached right side
    screenPosition = 126;
    servoDirection = 0;
  } else if (servoDirection == 0) { // going left
    screenPosition -= 1;
  } else { // going right
    screenPosition += 1;
  }

  servo.write(servoPosition[screenPosition]);

  // ...while servo moves

  // store previous reading ========================================================
  if (reading.RangeStatus != 4)
    measurement[screenPosition] = reading.RangeMilliMeter;
  else 
    measurement[screenPosition] = 255;

  // take next reading =============================================================
  lox.rangingTest(&reading, false);

  if ((screenPosition == 0) || (screenPosition == 127)) { // reached end of sweep, will display

      u8g2.firstPage();
      do {
        for (int i=0; i<128; i++) {
          u8g2.drawLine(
            63+int(63.0*sin(PI/180.0*((float)i-63.0)/64.0*45.0)*(float)measurement[i]/255.0),
            63-int(63.0*cos(PI/180.0*((float)i-63.0)/64.0*45.0)*(float)measurement[i]/255.0),
            i,
            0);
        }
      } while (u8g2.nextPage());

    } else delay(30); // give the servo time to move into position

}
