#include <SPI.h>
#include <LoRa.h>

#include <LoRaWanPacket.h>

const int csPin = 10;
const int resetPin = 9;
const int irqPin = 2;

const char *devAddr = "11111111";
const char *nwkSKey = "11111111111111111111111111111111";
const char *appSKey = "11111111111111111111111111111111";

struct LoRa_config
{
  long Frequency;
  int SpreadingFactor;
  long SignalBandwidth;
  int CodingRate4;
  bool enableCrc;
  bool invertIQ;
  int SyncWord;
  int PreambleLength;
};

static LoRa_config txLoRa = {903900000, 10, 125000, 5, true, false, 0x34, 8};

void LoRa_setConfig(struct LoRa_config config)
{
  LoRa.setFrequency(config.Frequency);
  LoRa.setSpreadingFactor(config.SpreadingFactor);
  LoRa.setSignalBandwidth(config.SignalBandwidth);
  LoRa.setCodingRate4(config.CodingRate4);
  if (config.enableCrc)
    LoRa.enableCrc();
  else
    LoRa.disableCrc();
  if (config.invertIQ)
    LoRa.enableInvertIQ();
  else
    LoRa.disableInvertIQ();
  LoRa.setSyncWord(config.SyncWord);
  LoRa.setPreambleLength(config.PreambleLength);
}

void LoRa_TxMode()
{
  LoRa_setConfig(txLoRa);
  LoRa.idle();
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  LoRaWanPacket.personalize(devAddr, nwkSKey, appSKey);

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(txLoRa.Frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();

  LoRa_sendMessage();
}

void loop() {
  if (runEvery(10000)) {

    LoRa_sendMessage();

    Serial.println("Send Message!");
  }
}

void LoRa_sendMessage()
{
  LoRa_TxMode();
  LoRaWanPacket.clear();
  LoRaWanPacket.print("Hello World");
  if (LoRaWanPacket.encode()) 
  {
    LoRa.beginPacket();
    LoRa.write(LoRaWanPacket.buffer(), LoRaWanPacket.length());
    LoRa.endPacket();
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

