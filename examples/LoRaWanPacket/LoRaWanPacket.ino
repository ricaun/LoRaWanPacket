/*
  LoRaWanPacket
  This code encode the LoRaWan payload using the keys.
  created 16 03 2019
  by Luiz H. Cassettari
*/

#include <LoRaWanPacket.h>

const char *devAddr = "11111111";
const char *nwkSKey = "11111111111111111111111111111111";
const char *appSKey = "11111111111111111111111111111111";

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  LoRaWanPacket.personalize(devAddr, nwkSKey, appSKey);
}

void loop() {
  if (runEvery(5000))
  {
    LoRaWanPacket.clear();
    LoRaWanPacket.print("Hello World");
    if (LoRaWanPacket.encode()) 
    {
      LORA_HEX_PRINTLN(Serial, LoRaWanPacket.buffer(), LoRaWanPacket.length());
    }
  }
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

