#include <Arduino.h>
#include <Wire.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin();
}

void loop()
{
  unsigned int value = 1234; // e.g., 0x04 0xD2

  // Broadcast message to all devices
  Wire.beginTransmission(0);
  Wire.write(highByte(value));
  Wire.write(lowByte(value));

  Serial.print("Sending value: ");
  Serial.println(value);
  uint8_t err = Wire.endTransmission();

  if (err != 0)
  {
    Serial.print("Error: ");
    Serial.println(err);
  }

  delay(100);
}
