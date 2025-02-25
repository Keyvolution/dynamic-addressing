#include <Arduino.h>
#include <Wire.h>
#include "Bit.h"

void setup()
{
  Serial.begin(9600);
  Wire.begin();
}

void loop()
{
  uint16_t value = 1234;

  Serial.print("Sending value: ");
  Serial.println(value);

  // Broadcast message to all devices
  Wire.beginTransmission(0);
  sendBytes(value);
  uint8_t err = Wire.endTransmission();

  if (err != 0)
  {
    Serial.print("Error: ");
    Serial.println(err);
  }

  delay(100);
}
