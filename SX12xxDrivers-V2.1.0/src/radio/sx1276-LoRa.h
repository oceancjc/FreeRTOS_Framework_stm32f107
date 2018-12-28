/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1276-LoRa.h
 * \brief      SX1276 RF chip driver mode LoRa
 *
 * \version    2.0.B2 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __SX1276_LORA_H__
#define __SX1276_LORA_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "sx1276-Hal.h"
#include "sx1276-LoRaMisc.h"

#ifdef FreeRTOS
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "timers.h"
    #include "semphr.h"
#endif	

/*!
 * SX1276 LoRa General parameters definition
 */



/*!
 * RF packet definition
 */
#define RF_BUFFER_SIZE_MAX                          256
#define RF_BUFFER_SIZE                              256

/*!
 * RF state machine
 */
//LoRa
void SX1276DeviceInit( SX1276LRDevice_t* sx1278device_p );

void SX1276LoRaReset( SX1276LRDevice_t* sx1278device_p );

/*!
 * \brief Initializes the SX1276
 */
void SX1276LoRaInit( SX1276LRDevice_t* sx1278device_p );

/*!
 * \brief Sets the SX1276 to datasheet default values
 */
void SX1276LoRaSetDefaults( SX1276LRDevice_t* sx1278device_p );

/*!
 * \brief Enables/Disables the LoRa modem
 *
 * \param [IN]: enable [true, false]
 */
void SX1276LoRaSetLoRaOn( SX1276LRDevice_t* sx1278device_p, bool enable );

/*!
 * \brief Sets the SX1276 operating mode
 *
 * \param [IN] opMode New operating mode
 */
void SX1276LoRaSetOpMode( SX1276LRDevice_t* sx1278device_p, uint8_t opMode );

/*!
 * \brief Gets the SX1276 operating mode
 *
 * \retval opMode Current operating mode
 */
uint8_t SX1276LoRaGetOpMode( SX1276LRDevice_t* device );

/*!
 * \brief Reads the current Rx gain setting
 *
 * \retval rxGain Current gain setting
 */
uint8_t SX1276LoRaReadRxGain( SX1276LRDevice_t* device );

/*!
 * \brief Trigs and reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
double SX1276LoRaReadRssi( SX1276LRDevice_t* device );

/*!
 * \brief Gets the Rx gain value measured while receiving the packet
 *
 * \retval rxGainValue Current Rx gain value
 */
uint8_t SX1276LoRaGetPacketRxGain( SX1276LRDevice_t* device );

/*!
 * \brief Gets the SNR value measured while receiving the packet
 *
 * \retval snrValue Current SNR value in [dB]
 */
int8_t SX1276LoRaGetPacketSnr( SX1276LRDevice_t* device );

/*!
 * \brief Gets the RSSI value measured while receiving the packet
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
double SX1276LoRaGetPacketRssi( SX1276LRDevice_t* device );

/*!
 * \brief Sets the radio in Rx mode. Waiting for a packet
 */
void SX1276LoRaStartRx( SX1276LRDevice_t* device );

/*!
 * \brief Gets a copy of the current received buffer
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
void SX1276LoRaGetRxPacket( SX1276LRDevice_t* device, void *buffer, uint16_t *size );

/*!
 * \brief Sets a copy of the buffer to be transmitted
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
void SX1276LoRaSetTxPacket( SX1276LRDevice_t* device, const void *buffer, uint16_t size );

/*!
 * \brief Gets the current RFState
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
uint8_t SX1276LoRaGetRFState( SX1276LRDevice_t* device );

/*!
 * \brief Sets the new state of the RF state machine
 *
 * \param [IN]: state New RF state machine state
 */
//void SX1276LoRaSetRFState( SX1276LRDevice_t* device, uint8_t state );

/*!
 * \brief Process the LoRa modem Rx and Tx state machines depending on the
 *        SX1276 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
uint32_t SX1276LoRaProcess( SX1276LRDevice_t* device );
void SX1276RxStart( SX1276LRDevice_t* device );
void SX1276CadStart( SX1276LRDevice_t* device );
void SX276TxStart( SX1276LRDevice_t* device );
void SX1276SetSleep( SX1276LRDevice_t* device );
void SX1276Send( SX1276LRDevice_t* device, uint8_t* data, uint8_t TxPacketSize );

void SX1276IrqDIO0( SX1276LRDevice_t* device );
uint8_t SX276IrqCad( SX1276LRDevice_t* device );
#endif //__SX1276_LORA_H__

