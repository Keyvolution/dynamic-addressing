#include <Arduino.h>
#include <Wire.h>
#include "Bit.h"

const uint8_t DEFAULT_ADDRESS = 0x61;
const uint8_t ARP_GET_UDID    = 0x01;
const uint8_t ARP_ASSIGN_ADDR = 0x02;
const uint8_t BASE_NEW_ADDR   = 0x02;
const uint8_t MAX_RETRIES     = 0x03;

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

  for (uint8_t attempt = 0; attempt < MAX_RETRIES; attempt++)
  {
    // Send the ARP query command to the broadcast address
    Wire.beginTransmission(0);
    Wire.write(ARP_GET_UDID);
    byte error = Wire.endTransmission();

    if (error != 0) // If no response from any slave
    {
      delay(20); // Wait before retrying
      continue;
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
      break; // Device found, break retry loop
    }
    else
    {
      delay(20); // Retry on invalid response
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

    // Send the address assignment to the slave
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
    nextAddress++; // Increment address for next device
    return true;
  }

  return false; // No valid response means no device found
}

void loop() {}
