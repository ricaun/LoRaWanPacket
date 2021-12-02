// ----------------------------------------------- //
// LoRaWanPacket.cpp
// ----------------------------------------------- //
//
// This library is not full LoRaWan
//
// ----------------------------------------------- //
// Data: 05/02/2018
// Author: Luiz H Cassettari
// ----------------------------------------------- //

#include <Arduino.h>
#include "crypto/LoRaUtilities.h"
#include "LoRaWanPacket.h"

LoRaWanPacketClass::LoRaWanPacketClass()
{
  setTimeout(0);
}

int LoRaWanPacketClass::begin()
{
  return 1;
}

void LoRaWanPacketClass::end()
{
}

size_t LoRaWanPacketClass::write(uint8_t c)
{
  payload_buf[payload_len++] = c;
  return 1;
};

size_t LoRaWanPacketClass::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;
  for (size_t i = 0; i < size; i++)
  {
    n += write(*(buffer + i));
  }
  return n;
}

int LoRaWanPacketClass::available()
{
  return payload_len - payload_position;
}

int LoRaWanPacketClass::read()
{
  return payload_position < payload_len ? payload_buf[payload_position++] : -1;
}

int LoRaWanPacketClass::peek()
{
  return payload_position < payload_len ? payload_buf[payload_position] : -1;
}

void LoRaWanPacketClass::flush()
{

}

unsigned int LoRaWanPacketClass::readInt()
{
  if (available() < 2) return 0;
  unsigned int i = 0;
  i += read() << 8;
  i += read();
  return i;
}

unsigned long LoRaWanPacketClass::readLong()
{
  if (available() < 4) return 0;
  unsigned long i = 0;
  i += read() << 24;
  i += read() << 16;
  i += read() << 8;
  i += read();
  return i;
}

uint8_t *LoRaWanPacketClass::buffer()
{
  return ((uint8_t *)&payload_buf) + payload_position;
}

int LoRaWanPacketClass::length()
{
  return available();
}


void LoRaWanPacketClass::clear()
{
  payload_len = 0;
  payload_position = 0;
}

// ----------------------------------------------- //
// ----------------------------------------------- //
// ----------------------------------------------- //

void LoRaWanPacketClass::join(const char *_aeui, const char *_akey)
{
  setAppKey(_akey);
  setAppEui(_aeui);
}

void LoRaWanPacketClass::join(const char *_deui, const char *_aeui, const char *_akey)
{
  setDevEui(_deui);
  setAppKey(_akey);
  setAppEui(_aeui);
}

// ---------------------------------------------------- //
// join keys
// ---------------------------------------------------- //

void LoRaWanPacketClass::setAppKey(uint8_t *_akey)
{
  memcpy(AppKey, _akey, 16);
}

void LoRaWanPacketClass::setAppKey(const char *_akey)
{
  LORA_HEX_TO_BYTE((char *) AppKey, (char *) _akey, 16);
}

void LoRaWanPacketClass::setAppEui(uint8_t *_aeui)
{
  memcpy(AppEui, _aeui, 8);
}

void LoRaWanPacketClass::setAppEui(const char *_aeui)
{
  LORA_HEX_TO_BYTE((char *) AppEui, (char *) _aeui, 8);
}

void LoRaWanPacketClass::setDevEui(uint8_t *_deui)
{
  memcpy(DevEui, _deui, 8);
}

void LoRaWanPacketClass::setDevEui(const char *_deui)
{
  LORA_HEX_TO_BYTE((char *) DevEui, (char *) _deui, 8);
}

void LoRaWanPacketClass::personalize(const char *_devAddr, const char *_nwkSKey, const char *_appSKey)
{
  uint8_t devAddr[4];
  uint8_t nwkSKey[16];
  uint8_t appSKey[16];
  LORA_HEX_TO_BYTE((char *)devAddr, (char *) _devAddr, 4);
  LORA_HEX_TO_BYTE((char *)nwkSKey, (char *) _nwkSKey, 16);
  LORA_HEX_TO_BYTE((char *)appSKey, (char *) _appSKey, 16);
  memcpy(DevAddr, devAddr, 4);
  memcpy(NwkSKey, nwkSKey, 16);
  memcpy(AppSKey, appSKey, 16);
}

void LoRaWanPacketClass::show()
{
  Serial.print("DevEui: ");
  _LORA_HEX_PRINTLN(Serial, DevEui, 8);
  Serial.print("AppEui: ");
  _LORA_HEX_PRINTLN(Serial, AppEui, 8);
  Serial.print("AppKey: ");
  _LORA_HEX_PRINTLN(Serial, AppKey, 16);

  Serial.print("DevAddr: ");
  _LORA_HEX_PRINTLN(Serial, DevAddr, 4);
  Serial.print("NwkSKey: ");
  _LORA_HEX_PRINTLN(Serial, NwkSKey, 16);
  Serial.print("AppSKey: ");
  _LORA_HEX_PRINTLN(Serial, AppSKey, 16);

  Serial.print("Packet: ");
  _LORA_HEX_PRINTLN(Serial, payload_buf, payload_len);
}

// ----------------------------------------------------------------------------
// setPort
// ----------------------------------------------------------------------------
void LoRaWanPacketClass::setPort(uint8_t port)
{
  FPort = port;
}

// ----------------------------------------------------------------------------
// DevStatusReq = 0x06,       // u1:battery 0,1-254,255=?, u1:7-6:RFU,5-0:margin(-32..31)
// ----------------------------------------------------------------------------
bool LoRaWanPacketClass::IsDevStatusReq()
{
  bool isMac = lastMac == 0x06;
  if (isMac) lastMac = 0x00;
  return isMac;
}

// ----------------------------------------------------------------------------
// CHECKDEV
// Function to check the DEVICE
// Parameters:
//  - buf: LoRa buffer to check in bytes, 1...5 bytes contain the DEVICE
//  - len: Length of buffer in bytes
//
// ----------------------------------------------------------------------------
boolean LoRaWanPacketClass::checkDev(uint8_t *buf, uint8_t len)
{
  if (buf[1] == DevAddr[3])
    if (buf[2] == DevAddr[2])
      if (buf[3] == DevAddr[1])
        if (buf[4] == DevAddr[0])
        {
          return true;
        }
  return false;
}

// ----------------------------------------------------------------------------
// CHECKMIC
// Function to check the MIC computed for existing messages and for new messages
// Parameters:
//  - buf: LoRa buffer to check in bytes, last 4 bytes contain the MIC
//  - len: Length of buffer in bytes
//  - key: Key to use for MIC. Normally this is the NwkSKey
//
// ----------------------------------------------------------------------------
boolean LoRaWanPacketClass::checkMic(uint8_t *buf, uint8_t len, uint8_t *key)
{
  uint8_t cBuf[len + 1];

  for (uint8_t i = 0; i < len - 4; i++)
    cBuf[i] = buf[i];
  len -= 4;

  uint16_t count = (cBuf[7] * 256) + cBuf[6];
  uint8_t dir = 0;
  if (buf[0] == 0x60 || buf[0] == 0xA0)
  {
    dir = 1;
    if (buf[0] == 0xA0) FCtrl = FCT_ACK;
    else FCtrl = 0x00;
  }

  len += PayloadComputeMic(cBuf, len, key, count, dir);

  if (buf[len - 4] == cBuf[len - 4])
    if (buf[len - 3] == cBuf[len - 3])
      if (buf[len - 2] == cBuf[len - 2])
        if (buf[len - 1] == cBuf[len - 1])
        {
          return true;
        }
#ifdef LORAWAN_DEBUG
  if (debug)
    Serial.println("Check Mic Error");
#endif

  return false;
}

// ----------------------------------------------------------------------------
// decode buffer
// ----------------------------------------------------------------------------
int16_t LoRaWanPacketClass::decode()
{
  return decode(payload_buf, payload_len);
}

// ----------------------------------------------------------------------------
// decode
// ----------------------------------------------------------------------------
int16_t LoRaWanPacketClass::decode(uint8_t *buf, uint8_t len)
{

#ifdef LORAWAN_DEBUG
    if (debug)
    {
      Serial.print("Decode: ");
      _LORA_HEX_PRINTLN(Serial, payload_buf, payload_len);
    }
#endif

  // join decode
  if (buf[0] == 0x20)
    return decodeJoin(buf, len);
  // others decodes
  return decodePacket(buf, len);
}

// ----------------------------------------------------------------------------
// decodePacket
// ----------------------------------------------------------------------------
int16_t LoRaWanPacketClass::decodePacket(uint8_t *buf, uint8_t len)
{
  // check devid
  if (checkDev(buf, len) == false)
    return -1;

  // check mic
  if (checkMic(buf, len, NwkSKey))
  {
    uint16_t count = (buf[7] * 256) + buf[6];
    uint8_t dir = 0;

    // downlink code 0x60
    if (buf[0] == 0x60)
      dir = 1;

    // downlink confirm 0xA0
    if (buf[0] == 0xA0)
      dir = 1;

    if (dir == 1)
    {
      if (frameCountDown > count)
      {
        // frame down menor que count
        // downlink antigo
        // ignorar payload
        FCtrl = 0x00;
        return -2;
      }
      frameCountDown = count + 1;
    }

    // Add fctrl optional bytes / ignore
    uint8_t fctrl_opt = (buf[5] & FCT_OPTLEN);
    uint8_t fport = 0;//buf[8 + fctrl_opt];
    uint8_t mlength = 9 + fctrl_opt;

    payload_position = 0;
    payload_len = 0;

    len = len - 4; // remove MIC
    bool containPayload = (len > mlength);

    if (containPayload) // With Payload
    {
      payload_len = len - mlength;
      fport = buf[8 + fctrl_opt];
    }

    for (size_t i = 0; i < fctrl_opt; i++)
    {
      uint8_t mac = buf[(containPayload) ? 9 : 8 + i];
      Serial.print("mac: ");
      Serial.println(mac, HEX);
      if (mac == 0x06) lastMac = mac;
    }

#ifdef LORAWAN_DEBUG
    if (debug)
    {
      Serial.print("fctrl_opt: ");
      Serial.println(fctrl_opt);
      Serial.print("fport: ");
      Serial.println(fport);
      Serial.print("payload_len: ");
      Serial.println(payload_len);
      Serial.print("FCtrl: ");
      Serial.println(FCtrl, HEX);
      Serial.print("lastMac: ");
      Serial.println(lastMac, HEX);
    }
#endif

    PayloadEncode((uint8_t *)(buf + mlength), payload_len, AppSKey, DevAddr, count, dir);

    // modifica para payload
    memcpy(payload_buf, (uint8_t *)(buf + mlength), payload_len);

#ifdef LORAWAN_DEBUG
    if (debug)
    {
      Serial.print("Payload: ");
      _LORA_HEX_PRINTLN(Serial, payload_buf, payload_len);
    }
#endif

    return fport;
  }
  return 0;
}

// ----------------------------------------------------------------------------
// decodeJoin
// ----------------------------------------------------------------------------
int16_t LoRaWanPacketClass::decodeJoin(uint8_t *buf, uint8_t len)
{
  JoinDecrypt(buf + 1, len - 1, AppKey);

  if (JoinComputeMic(buf, len - 4, AppKey))
  {
    for (int i = 0; i < 4; i++)
      DevAddr[3 - i] = buf[7 + i];

    JoinComputeSKeys(AppKey, buf + 1, DevNonce, NwkSKey, AppSKey);

#ifdef LORAWAN_DEBUG
    if (debug)
    {
      Serial.print("DevAddr: ");
      _LORA_HEX_PRINTLN(Serial, DevAddr, 4);
      Serial.print("NwkSKey: ");
      _LORA_HEX_PRINTLN(Serial, NwkSKey, 16);
      Serial.print("AppSKey: ");
      _LORA_HEX_PRINTLN(Serial, AppSKey, 16);
    }
#endif

    clear();
    return PORT_OTAA_JOIN_ACCEPT;
  }
  return 0;
}

void LoRaWanPacketClass::randomJoin(){
  
  for(size_t i = 0; i < 4; i++)
  {
    DevAddr[i] = 0;
  }
  DevNonce = (uint16_t)random(255) << 8 | (uint16_t)random(255);
  frameCountDown = 0;
  frameCount = 0;
}

bool LoRaWanPacketClass::isJoin(){
  if (DevAddr[3] == 0 && DevAddr[2] == 0 && DevAddr[1] == 0 && DevAddr[0] == 0) 
    return true;
  return false;
}


int16_t LoRaWanPacketClass::JoinPacket()
{
  if (DevNonce == 0) 
  {
    DevNonce = (uint16_t)random(255) << 8 | (uint16_t)random(255);
  }

  payload_buf[0] = 0x00;

  DevNonce++;

  memset(DevAddr, 0, 4);
  memset(NwkSKey, 0, 16);
  memset(AppSKey, 0, 16);

  for (uint8_t i = 0; i < 8; i++)
    payload_buf[i + 1] = AppEui[7 - i];

  for (uint8_t i = 0; i < 8; i++)
    payload_buf[i + 1 + 8] = DevEui[7 - i];

  payload_buf[17] = (DevNonce & 0x00FF);
  payload_buf[18] = ((DevNonce >> 8) & 0x00FF);
  payload_len = 19;
  JoinComputeMic(payload_buf, payload_len, AppKey);
  payload_len += 4;

#ifdef LORAWAN_DEBUG
  if (debug)
    Serial.print("Join: ");
    _LORA_HEX_PRINTLN(Serial, payload_buf, payload_len);
#endif

  return 256;
}

// ----------------------------------------------------------------------------
// SENSORPACKET
// The gateway may also have local sensors that need reporting.
// We will generate a message in gateway-UDP format for upStream messaging
// so that for the backend server it seems like a LoRa node has reported a
// sensor value.
//
// NOTE: We do not need ANY LoRa functions here since we are on the gateway.
// We only need to send a gateway message upstream that looks like a node message.
//
// NOTE:: This function does encrypt the sensorpayload, and the backend
//    picks it up fine as decoder thinks it is a MAC message.
//
// Par 4.0 LoraWan spec:
//  PHYPayload =  ( MHDR | MACPAYLOAD | MIC )
// which is equal to
//          ( MHDR | ( FHDR | FPORT | FRMPAYLOAD ) | MIC )
//
//  This function makes the totalpackage and calculates MIC
// Te maximum size of the message is: 12 + ( 9 + 2 + 64 ) + 4
// So message size should be lass than 128 bytes if Payload is limited to 64 bytes.
// ----------------------------------------------------------------------------

int16_t LoRaWanPacketClass::encode()
{
  return encoder(FPort);
}

int16_t LoRaWanPacketClass::encoder(byte fport)
{
  if (DevAddr[3] == 0 && DevAddr[2] == 0 && DevAddr[1] == 0 && DevAddr[0] == 0)
    return JoinPacket();

  if (fport > 0)
    FPort = fport;

  uint8_t mac = 0; // Force DevStatusReq Mac
  if (IsDevStatusReq()) mac = 3;
  
  uint8_t mlength = 9 + mac;

  uint8_t buf[payload_len + 1 + mac];
  // fix payload > 9
  memcpy((uint8_t *)(buf), payload_buf, payload_len);
  memcpy((uint8_t *)(payload_buf + mlength), buf, payload_len);

  //memcpy((uint8_t *)(payload_buf + 9), payload_buf, payload_len);

  // In the next few bytes the fake LoRa message must be put
  // PHYPayload = MHDR | MACPAYLOAD | MIC
  // MHDR, 1 byte
  // MIC, 4 bytes

  // ------------------------------
  // MHDR (Para 4.2), bit 5-7 MType, bit 2-4 RFU, bit 0-1 Major
  payload_buf[0] = 0x40; // MHDR 0x40 == unconfirmed up message,
                         // FRU and major are 0

  // -------------------------------
  // FHDR consists of 4 bytes addr, 1 byte Fctrl, 2 byte FCnt, 0-15 byte FOpts
  // We support ABP addresses only for Gateways
  payload_buf[1] = DevAddr[3]; // Last byte[3] of address
  payload_buf[2] = DevAddr[2];
  payload_buf[3] = DevAddr[1];
  payload_buf[4] = DevAddr[0]; // First byte[0] of Dev_Addr

  payload_buf[5] = FCtrl + mac;               // FCtrl is normally 0
  payload_buf[6] = frameCount % 0x100; // LSB
  payload_buf[7] = frameCount / 0x100; // MSB

  FCtrl = 0x00; // clear FCtrl

  // -------------------------------
  // FPort, either 0 or 1 bytes. Must be != 0 for non MAC messages such as user payload
  //
  payload_buf[8+mac] = FPort; // FPort must not be 0

  // DevStatusReq = 0x06,       // u1:battery 0,1-254,255=?, u1:7-6:RFU,5-0:margin(-32..31)
  if (IsDevStatusReq())
  {
    payload_buf[8] = 0x06;
    payload_buf[9] = 0xFF;
    uint8_t snr = 0b00100000;
    payload_buf[10] = (uint8_t)(0b00111111 & snr);
    mac = 0;
  }

  // FRMPayload; Payload will be AES128 encoded using AppSKey
  // See LoRa spec para 4.3.2
  // You can add any byte string below based on you personal choice of sensors etc.
  //
  // Payload bytes in this example are encoded in the LoRaCode(c) format

  // we have to include the AES functions at this stage in order to generate LoRa Payload.
  if (payload_len > 0) {
    uint8_t CodeLength = PayloadEncode((uint8_t *)(payload_buf + mlength), payload_len, AppSKey, DevAddr, frameCount, 0);
    mlength += CodeLength; // length inclusive sensor data
  }
  // MIC, Message Integrity Code
  // As MIC is used by TTN (and others) we have to make sure that
  // framecount is valid and the message is correctly encrypted.
  // Note: Until MIC is done correctly, TTN does not receive these messages
  //     The last 4 bytes are MIC bytes.
  //

  mlength += PayloadComputeMic((uint8_t *)(payload_buf), mlength, NwkSKey, frameCount, 0);

  frameCount++;
  payload_len = mlength;

#ifdef LORAWAN_DEBUG
  if (debug)
    Serial.print("Encode: ");
    _LORA_HEX_PRINTLN(Serial, payload_buf, payload_len);
#endif
  return 1;
}

LoRaWanPacketClass LoRaWanPacket;
