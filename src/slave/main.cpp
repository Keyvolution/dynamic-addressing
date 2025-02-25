#include <Arduino.h>
#include <Wire.h>
#include "TaskScheduler.h"

const uint8_t DEFAULT_ADDRESS = 0x61;
const uint8_t ARP_GET_UDID    = 0x01;
const uint8_t ARP_ASSIGN_ADDR = 0x02;

const uint8_t UDID[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

uint8_t ledVal = 0;
uint16_t twiInitJitter;
bool twiInitialized = false;
unsigned long previousTwiInitTime = 0;

uint8_t deviceAddress = DEFAULT_ADDRESS;
TaskScheduler scheduler;

void receiveEvent(int howMany);
void assignNewAddress();
void sendUDID();

void setup()
{
  Serial.begin(9600);

  randomSeed(analogRead(A1));
  twiInitJitter = random(100000);
  Wire.onReceive(receiveEvent);
}

void loop()
{
  // Initialize TWI after random delay
  if (micros() - previousTwiInitTime >= twiInitJitter && !twiInitialized)
  {
    Wire.begin(deviceAddress);
    TWAR = (deviceAddress << 1) | 1; // Enable broadcast reception
    twiInitialized = true;
    previousTwiInitTime = micros();
  }

  // Process the task queue
  scheduler.process();
}

void receiveEvent(int howMany)
{
  uint8_t cmd = Wire.read();
  if (cmd == ARP_GET_UDID)
  {
    randomSeed(analogRead(A1)); // Re-seed for randomness
    scheduler.executeDelayed(sendUDID, random(10, 100));
  }
  else if (cmd == ARP_ASSIGN_ADDR && howMany >= 2)
  {
    deviceAddress = Wire.read();
    scheduler.executeDelayed(assignNewAddress, 50);
  }

  // Flush any extra data
  while (Wire.available() > 0)
  {
    Wire.read();
  }
}

void sendUDID()
{
  Wire.write(UDID, sizeof(UDID));
}

void assignNewAddress()
{
  Wire.end();
  Wire.begin(deviceAddress);
  Serial.print("Assigned new address: ");
  Serial.println(deviceAddress, HEX);
}
