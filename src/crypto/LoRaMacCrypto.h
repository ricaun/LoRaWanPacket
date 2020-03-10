/*!
 * \file      LoRaMacCrypto.h
 *
 * \brief     LoRa MAC layer cryptography implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \defgroup    LORAMAC_CRYPTO  LoRa MAC layer cryptography implementation
 *              This module covers the implementation of cryptographic functions
 *              of the LoRaMAC layer.
 * 
 * 
 */


#ifndef __LORAMAC_CRYPTO_H__
#define __LORAMAC_CRYPTO_H__

void LoRaMacJoinComputeMic( uint8_t *data, uint8_t len, uint8_t *key, uint32_t *mic );
void LoRaMacJoinDecrypt( uint8_t *data, uint8_t len, uint8_t *key);
void LoRaMacJoinDecrypt( const uint8_t *data, uint8_t len, const uint8_t *key, uint8_t *decBuffer );
void LoRaMacJoinComputeSKeys( uint8_t *key, uint8_t *appNonce, uint16_t devNonce, uint8_t *nwkSKey, uint8_t *appSKey);

void LoRaMacComputeMic( uint8_t *data, uint8_t len, uint8_t *key, uint32_t address, uint8_t dir, uint32_t count, uint32_t *mic );
void LoRaMacPayloadEncrypt( uint8_t *data, uint8_t len, uint8_t *key, uint32_t address, uint8_t dir, uint32_t count );
void LoRaMacPayloadEncrypt( uint8_t *data, uint8_t len, uint8_t *key, uint32_t address, uint8_t dir, uint32_t count, uint8_t *decBuffer);
void LoRaMacPayloadDecrypt( uint8_t *data, uint8_t len, uint8_t *key, uint32_t address, uint8_t dir, uint32_t count );
void LoRaMacPayloadDecrypt( uint8_t *data, uint8_t len, uint8_t *key, uint32_t address, uint8_t dir, uint32_t count, uint8_t *decBuffer );

uint8_t JoinComputeMic(uint8_t *data, uint8_t len, uint8_t *key);
void JoinDecrypt(uint8_t *data, uint8_t len, uint8_t *key);
void JoinComputeSKeys(uint8_t *key, uint8_t *appNonce, uint16_t devNonce, uint8_t *nwkSKey, uint8_t *appSKey);

uint8_t PayloadEncode(uint8_t *buf, uint8_t len, uint8_t *key, uint8_t *dev, uint32_t count, uint8_t dir);
uint8_t PayloadComputeMic(uint8_t *data, uint8_t len, uint8_t *key, uint32_t count, uint8_t dir);

void mXor(uint8_t *buf, uint8_t *key);
void shift_left(uint8_t *buf, uint8_t len);
void generate_subkey(uint8_t *key, uint8_t *k1, uint8_t *k2);

#endif // __LORAMAC_CRYPTO_H__
