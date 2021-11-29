// ----------------------------------------------- //
// LoRaWanPacket.h
// ----------------------------------------------- //
//
// This library is not full LoRaWan
//
// ----------------------------------------------- //
// Data: 05/02/2018
// Author: Luiz H Cassettari
// ----------------------------------------------- //


#ifndef LORAMAC_H
#define LORAMAC_H

#include <Arduino.h>
#include "crypto/LoRaUtilities.h"
#include "crypto/LoRaMacCrypto.h"

#define LORAWAN_BUF_SIZE 128

#define PORT_OTAA_JOIN_ACCEPT 500

//#define LORAWAN_DEBUG true;

class LoRaWanPacketClass : public Stream{
public:

	uint8_t debug = 1;

	uint32_t frameCount = 0;
	uint32_t frameCountDown = 0;
	uint8_t FPort = 0x01;
	uint8_t FCtrl = 0x00;
	uint8_t lastMac = 0x00;

	// ----------------------------------------------- //
	uint8_t DevEui[8];
	uint8_t AppEui[8];
	uint8_t AppKey[16];
	uint16_t DevNonce = 0x0000;
	
	// ----------------------------------------------- //
	uint8_t DevAddr[4];
	uint8_t NwkSKey[16];
	uint8_t AppSKey[16];

	uint8_t payload_buf[LORAWAN_BUF_SIZE];
	uint8_t payload_len = 0;
	uint8_t payload_position = 0;

	LoRaWanPacketClass();

	int begin();
	void end();

	// from Print
	virtual size_t write(uint8_t byte);
	virtual size_t write(const uint8_t *buffer, size_t size);
	// from Stream
	virtual int available();
	virtual int read();
	virtual int peek();
	virtual void flush();

	unsigned int readInt();
	unsigned long readLong();

	void clear();
	uint8_t *buffer();
	int length();

	void join(const char *_aeui, const char *_akey);
	void join(const char *_deui, const char *_aeui, const char *_akey);
	void personalize(const char *_devAddr, const char *_nwkSKey, const char *_appSKey);

	void setAppKey(uint8_t *_akey);
	void setAppKey(const char *_akey);
	void setAppEui(uint8_t *_aeui);
	void setAppEui(const char *_aeui);
	void setDevEui(uint8_t *_deui);
	void setDevEui(const char *_deui);

	void show();

	// configs set port send
	void setPort(uint8_t port);

	// decode/encode functions
	int16_t decode();
	int16_t encode();
	
	void randomJoin();

	bool isJoin();
	int16_t JoinPacket();

private:

	// decode/encode functions
	int16_t decode(uint8_t *buf, uint8_t len);
	int16_t decodePacket(uint8_t *buf, uint8_t len);
	int16_t decodeJoin(uint8_t *buf, uint8_t len);
	int16_t encoder(byte fport = 0);

	// check functions
	boolean checkDev(uint8_t *buf, uint8_t len);
	boolean checkMic(uint8_t *buf, uint8_t len, uint8_t *key);

	bool IsDevStatusReq();
};

extern LoRaWanPacketClass LoRaWanPacket;

#endif
