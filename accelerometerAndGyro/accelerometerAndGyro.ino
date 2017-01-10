#include <CurieIMU.h>

int xAxisRawAcc, yAxisRawAcc, zAxisRawAcc;
int xAxisRawGy, yAxisRawGy, zAxisRawGy;

void setup() {
  CurieIMU.begin();
  CurieIMU.setAccelerometerRange(2);
  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
  CurieIMU.autoCalibrateGyroOffset();
  Serial.begin(9600);
}

void loop() {
  CurieIMU.readAccelerometer(xAxisRawAcc, yAxisRawAcc, zAxisRawAcc);
  CurieIMU.readGyro(xAxisRawGy, yAxisRawGy, zAxisRawGy);
  float xAxisAcc = xAxisRawAcc / 32768.0 * CurieIMU.getAccelerometerRange();
  float yAxisAcc = yAxisRawAcc / 32768.0 * CurieIMU.getAccelerometerRange();
  float zAxisAcc = zAxisRawAcc / 32768.0 * CurieIMU.getAccelerometerRange();
  float xAxisGy = (xAxisRawGy / 32768.9) * CurieIMU.getGyroRange();
  float yAxisGy = (yAxisRawGy / 32768.9) * CurieIMU.getGyroRange();
  float zAxisGy = (zAxisRawGy / 32768.9) * CurieIMU.getGyroRange();

  Serial.print("Accelerometer (x) : " + String(xAxisAcc) + "g\r\n");
  Serial.print("Accelerometer (y) : " + String(yAxisAcc) + "g\r\n");
  Serial.print("Accelerometer (z) : " + String(zAxisAcc) + "g\r\n");
  Serial.print("Gyroscope (x)     : " + String(xAxisGy) + char(176) + "/s\r\n");
  Serial.print("Gyroscope (y)     : " + String(yAxisGy) + char(176) + "/s\r\n");
  Serial.print("Gyroscope (z)     : " + String(zAxisGy) + char(176) + "/s\r\n\r\n");
  delay(1000);
}
