//----------------------------------------//
//  LoRaWanPacket_LoRa_join.ino
//
//  created 06 04 2021
//  by Luiz Henrique Cassettari
//----------------------------------------//

void setup()
{
  Serial.begin(115200);
  LoRa_setup();
  LoRa_sendMessage();
}

void loop() {
  LoRa_loop();
  if (runEvery(10000)) {
    if (!LoRa_busy())
    {
      Serial.println("Sleep");
      delay(10);
      LoRa_endSleep();
      LoRa_sendMessage();
      Serial.println("Send Message!");
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
