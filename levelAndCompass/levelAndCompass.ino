#include <Arduino.h>

//graphic library
#include <U8g2lib.h>

//digital compass sensor library
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

//IMU library
#include <CurieIMU.h>

//variable for LCD
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

//variable for digital compass sensor
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

boolean operationMode = true;

#define CIRCLE_ITER 176
byte circleX[CIRCLE_ITER];
byte circleY[CIRCLE_ITER];

void drawLevelPage(int8_t level) {
  u8g2.setFont(u8g2_font_crox3cb_mn);
  u8g2.drawVLine(0, 0, 63);
  u8g2.drawVLine(63, 0, 63);
  for (byte i = 0; i < 32; i++) {
    if (i % 2) {
      u8g2.drawHLine(1, i * 2, 7);
      u8g2.drawHLine(63 - 7, i * 2, 7);
    }
    else {
      u8g2.drawHLine(1, i * 2, 10);
      u8g2.drawHLine(63 - 10, i * 2, 10);
    }
  }
  u8g2.drawHLine(0, map(level, -90, 90, 62, 0), 62);
  byte xPosString = (u8g2.getDisplayWidth() * 3 / 4) - (u8g2.getStrWidth((String(level)).c_str()) / 2);
  u8g2.drawStr(xPosString, 40, String(level).c_str());

  xPosString = xPosString + u8g2.getStrWidth((String(level)).c_str());
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawUTF8(xPosString, 38, "°");
}

void drawCompassPage(int headingDegree) {
  u8g2.setFont(u8g2_font_courB08_tn);
  u8g2.drawHLine(0, 40, 127);
  u8g2.drawHLine(0, 63, 127);
  for (byte i = 0; i < 128; i++) {
    if (i % 2) {
      u8g2.drawVLine(i * 2, 41, 3);
      u8g2.drawVLine(i * 2, 63 - 3, 3);
    }
    else {
      u8g2.drawVLine(i * 2, 41, 5);
      u8g2.drawVLine(i * 2, 63 - 5, 5);
    }
  }
  u8g2.drawVLine(0, 36, 4);
  u8g2.drawVLine(63, 36, 4);
  u8g2.drawVLine(126, 36, 4);

  u8g2.setFont(u8g2_font_4x6_mf);
  u8g2.drawStr(0, 32, "0deg");
  byte xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("180deg") / 2);
  u8g2.drawStr(xPosString, 32, "180deg");
  xPosString = u8g2.getDisplayWidth() - (u8g2.getStrWidth("359deg"));
  u8g2.drawStr(xPosString, 32, "359deg");

  u8g2.drawVLine(map(headingDegree, 0, 359, 0, 128), 40, 27);
  u8g2.setFont(u8g2_font_logisoso16_tf);
  xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth((String(headingDegree) + "deg").c_str()) / 2);
  u8g2.drawStr(xPosString, 18, ((String(headingDegree) + "deg").c_str()));
}

void drawCompassPage2(int headingDegree) {
  u8g2.drawCircle(37, 31, 31);
  byte lineXY = map(90, 0, 359, 0, CIRCLE_ITER - 1);
  u8g2.drawHLine(circleX[lineXY] - 5, circleY[lineXY], 5);
  lineXY = map(270, 0, 359, 0, CIRCLE_ITER - 1);
  u8g2.drawHLine(circleX[lineXY], circleY[lineXY], 5);
  lineXY = map(0, 0, 359, 0, CIRCLE_ITER - 1);
  u8g2.drawVLine(circleX[lineXY], circleY[lineXY], 5);
  lineXY = map(180, 0, 359, 0, CIRCLE_ITER - 1);
  u8g2.drawVLine(circleX[lineXY], circleY[lineXY] - 5, 5);
  
  lineXY = map(headingDegree, 0, 359, 0, CIRCLE_ITER - 1);
  u8g2.drawLine(37, 31, circleX[lineXY], circleY[lineXY]);
  u8g2.setFont(u8g2_font_balthasar_regular_nbp_tf);
  byte xPosString = 37 - (u8g2.getStrWidth("U") / 2);
  u8g2.drawStr(xPosString, 17, String("U").c_str());
  
  u8g2.setFont(u8g2_font_crox3cb_tf);
  xPosString = (u8g2.getDisplayWidth() * 3 / 4) - (u8g2.getStrWidth((String(headingDegree)).c_str()) / 2);
  u8g2.drawStr(xPosString, 40, String(headingDegree).c_str());

  xPosString = xPosString + u8g2.getStrWidth((String(headingDegree)).c_str());
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.drawUTF8(xPosString, 38, "°");
}

void drawInitializationPage(int percentage) {
  u8g2.setFont(u8g2_font_balthasar_regular_nbp_tf);
  byte xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("Initializing...") / 2);
  u8g2.drawStr(xPosString, 30, "Initializing...");
  u8g2.drawFrame(u8g2.getDisplayWidth() / 8, 38, u8g2.getDisplayWidth() / 8 * 6, 5);
  u8g2.drawBox(u8g2.getDisplayWidth() / 8, 38, map(percentage, 0, 100, 0, u8g2.getDisplayWidth() / 8 * 6), 5);
}

void drawTiltErrorPage() {
  u8g2.setFont(u8g2_font_logisoso16_tf);
  byte xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("TILT ERROR") / 2);
  u8g2.drawStr(xPosString, 28, "TILT ERROR");
  u8g2.setFont(u8g2_font_crox1h_tf);
  u8g2.drawStr(13, 45, "Keep the compass");
  u8g2.drawStr(13, 59, "straight");
}

void drawLevelMenu() {
  u8g2.setFont(u8g2_font_crox5t_tf);
  byte xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("Inclination") / 2);
  u8g2.drawStr(xPosString, 30, "Inclination");
  xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("Level") / 2);
  u8g2.drawStr(xPosString, 56, "Level");
}

void drawCompassMenu() {
  u8g2.setFont(u8g2_font_crox5t_tf);
  byte xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("Digital") / 2);
  u8g2.drawStr(xPosString, 30, "Digital");
  xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("Compass") / 2);
  u8g2.drawStr(xPosString, 56, "Compass");
}

void drawWelcomeMenu() {
  u8g2.setFont(u8g2_font_crox5t_tf);
  byte xPosString = (u8g2.getDisplayWidth() / 2) - (u8g2.getStrWidth("WELCOME") / 2);
  u8g2.drawStr(xPosString, 43, "WELCOME");
}

int readCompass() {
  mag.begin();
  sensors_event_t event;
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  float declinationAngle = 0.22;
  heading += declinationAngle;
  if (heading < 0)
    heading += 2 * PI;
  if (heading > 2 * PI)
    heading -= 2 * PI;
  return heading * 180 / 3.1415;
}

static void eventCallback() {
  if (CurieIMU.getInterruptStatus(CURIE_IMU_MOTION)) {
    operationMode = !operationMode;
    u8g2.clearBuffer();
    if (operationMode)
      drawLevelMenu();
    else
      drawCompassMenu();
    u8g2.sendBuffer();
    delay(2500);
  }
}

void initCircle(int x0, int y0, int radius) {
  int x = radius;
  int y = 0;
  int err = 0;
  int iter = 0;
  while (x >= y) {
    u8g2.clearBuffer();
    circleX[(CIRCLE_ITER * 2 / 8) + iter] = x0 + x; circleY[(CIRCLE_ITER * 2 / 8) + iter] = y0 + y;
    circleX[(CIRCLE_ITER * 4 / 8 - 1) - iter] = x0 + y; circleY[(CIRCLE_ITER * 4 / 8 - 1) - iter] = y0 + x;
    circleX[(CIRCLE_ITER * 4 / 8) + iter] = x0 - y; circleY[(CIRCLE_ITER * 4 / 8) + iter] = y0 + x;
    circleX[(CIRCLE_ITER * 6 / 8 - 1) - iter] = x0 - x; circleY[(CIRCLE_ITER * 6 / 8 - 1) - iter] = y0 + y;
    circleX[(CIRCLE_ITER * 6 / 8) + iter] = x0 - x; circleY[(CIRCLE_ITER * 6 / 8) + iter] = y0 - y;
    circleX[(CIRCLE_ITER - 1) - iter] = x0 - y; circleY[(CIRCLE_ITER - 1) - iter] = y0 - x;
    circleX[0 + iter] = x0 + y; circleY[0 + iter] = y0 - x;
    circleX[(CIRCLE_ITER * 2 / 8 - 1) - iter] = x0 + x; circleY[(CIRCLE_ITER * 2 / 8 - 1) - iter] = y0 - y;

    if (err <= 0) {
      y += 1;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
    drawInitializationPage(map(iter + 1, 0, CIRCLE_ITER / 8, 0, 100));
    u8g2.sendBuffer();
    iter++;
  }
}

void setup(void) {
  u8g2.begin();
  mag.begin();

  CurieIMU.begin();
  CurieIMU.setAccelerometerRate(25);
  CurieIMU.setAccelerometerRange(2);
  CurieIMU.attachInterrupt(eventCallback);
  CurieIMU.setDetectionThreshold(CURIE_IMU_MOTION, 800); // 800mg
  CurieIMU.setDetectionDuration(CURIE_IMU_MOTION, 20);  // trigger times of consecutive slope data points
  CurieIMU.interrupts(CURIE_IMU_MOTION);

  Serial.begin(9600);
  u8g2.clearBuffer();
  drawWelcomeMenu();
  u8g2.sendBuffer();
  delay(2000);
  u8g2.clearBuffer();
  drawWelcomeMenu();
  u8g2.sendBuffer();
  delay(1000);
  initCircle(37, 31, 31);
  
  u8g2.clearBuffer();
  drawLevelMenu();
  u8g2.sendBuffer();
  delay(2500);
}

void loop(void) {
  int aix = CurieIMU.readAccelerometer(X_AXIS);
  int aiy = CurieIMU.readAccelerometer(Y_AXIS);
  int aiz = CurieIMU.readAccelerometer(Z_AXIS);
  u8g2.clearBuffer();
  if (operationMode) {
    drawLevelPage(map(aix, -16383, 16382, -90, 90));
  }
  else {
    if (aix < 5000 && aix > -5000 && aiy < 5000 && aiy > -5000) {
      drawCompassPage2(readCompass());
    }
    else {
      drawTiltErrorPage();
    }
  }
  u8g2.sendBuffer();
//  Serial.println(aix);
//  Serial.println(aiy);
//  Serial.println(aiz);
//  Serial.println();

  if (aiy < -13000 && aiy > -16000) {
    u8g2.setFlipMode(1);
  }
  else if (aiy > 13000 && aiy < 16000) {
    u8g2.setFlipMode(0);
  }
}
