 /*
  Arduino LSM6DS3 - Simple Accelerometer

  This example reads the acceleration values from the LSM6DS3
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Uno WiFi Rev 2 or Arduino Nano 33 IoT

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include <Arduino_LSM6DS3.h>
#define constG 9.8

void setup() {
  Serial.begin(2000000);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in m/s² | Gyroscope in degrees/second");
  Serial.println("AccX\tAccY\tAccZ | AngX\tAngY\tAngZ ");
}

void loop() {
  float Gx, Gy, Gz;
  float Accx, Accy, Accz;
  float Angx, Angy, Angz;

  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(Gx, Gy, Gz);
    IMU.readGyroscope(Angx, Angy, Angz);

    Accx = Gx * constG;
    Accy = Gy * constG;
    Accz = Gx * constG;

    Serial.print(Accx);
    Serial.print('\t');
    Serial.print(Accy);
    Serial.print('\t');
    Serial.println(Accz);
    Serial.print('\t');
    Serial.print(Angx);
    Serial.print('\t');
    Serial.print(Angy);
    Serial.print('\t');
    Serial.println(Angz);
  }
  

   delay(300);
}
