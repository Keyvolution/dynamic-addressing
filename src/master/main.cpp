#include <Arduino.h>
#include <Wire.h>
#include "Bit.h"

const uint8_t DEFAULT_ADDRESS = 0x61;
const uint8_t ARP_GET_UDID    = 0x01;
const uint8_t ARP_ASSIGN_ADDR = 0x02;
const uint8_t BASE_NEW_ADDR   = 0x02;
const uint8_t MAX_RETRIES     = 0x02;

uint8_t nextAddress = BASE_NEW_ADDR;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  while (discoverAndAssignOneDevice())
  {
    delay(50); // Short delay between assignments
  }

  Serial.println("ARP complete: all devices assigned.");
}

bool discoverAndAssignOneDevice()
{
  uint8_t receivedUDID[16];
  bool validResponse = false;

  // Send the ARP query command to the broadcast address
  Wire.beginTransmission(0);
  Wire.write(ARP_GET_UDID);
  byte error = Wire.endTransmission();

  if (error != 0)
  {
    return false; // No device responded
  }

  // Request 16 bytes of UDID data
  int bytesReceived = Wire.requestFrom(DEFAULT_ADDRESS, 16);
  if (bytesReceived == 16)
  {
    // Read UDID into receivedUDID array
    for (int i = 0; i < 16; i++)
    {
      if (Wire.available())
      {
        receivedUDID[i] = Wire.read();
      }
    }
    validResponse = true;
  }

  if (!validResponse)
  {
    // Retry mechanism in case of collision or invalid response
    for (int i = 0; i < MAX_RETRIES; i++)
    {
      delay(20); // Wait before retrying
      Wire.beginTransmission(DEFAULT_ADDRESS);
      Wire.write(ARP_GET_UDID);
      if (Wire.endTransmission() != 0)
        return false; // Retry if no response

      if (Wire.requestFrom(DEFAULT_ADDRESS, 16) == 16)
      {
        for (int j = 0; j < 16; j++)
        {
          if (Wire.available())
          {
            receivedUDID[j] = Wire.read();
          }
        }
        validResponse = true;
        break;
      }
    }
  }

  if (validResponse)
  {
    Serial.print("Found device UDID: ");
    for (int i = 0; i < 16; i++)
    {
      Serial.print(receivedUDID[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Send the assign address command with the new address
    Wire.beginTransmission(DEFAULT_ADDRESS);
    Wire.write(ARP_ASSIGN_ADDR);
    Wire.write(nextAddress);
    if (Wire.endTransmission() != 0)
    {
      Serial.println("Error assigning address.");
      return false; // Retry on failure
    }
    Serial.print("Assigned address: ");
    Serial.println(nextAddress, HEX);
    nextAddress++;
    return true;
  }

  return false; // No valid response means no device to assign
}

void loop()
{
}
