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
 * \file       sx1276-Hal.h
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#ifndef __SX1276_HAL_H__
#define __SX1276_HAL_H__

#include <stdbool.h>
//#include "stm32f10x.h"
#include "spi.h"

/*!
 * SX1276 RESET I/O definitions
 */
#define RESET_IOPORT                                GPIOE
#define RESET_PIN                                   GPIO_Pin_2

/*!
 * SX1276 SPI NSS I/O definitions
 */
#define SPI_CHANNEL                                 1
#define NSS_IOPORT                                  GPIOE
#define NSS_PIN                                     GPIO_Pin_14


/*!
 * SX1276 DIO pins  I/O definitions
 */
#define DIO0_IOPORT                                 GPIOA
#define DIO0_PIN                                    GPIO_Pin_3
#define DIO0_EXTIRQPORT                             GPIO_PortSourceGPIOA
#define DIO0_EXTIRQPIN                              GPIO_PinSource3
#define DIO0_EXTLINE                                EXTI_Line3

#define DIO1_IOPORT                                 GPIOC
#define DIO1_PIN                                    GPIO_Pin_0
#define DIO1_EXTIRQPORT                             GPIO_PortSourceGPIOC
#define DIO1_EXTIRQPIN                              GPIO_PinSource0
#define DIO1_EXTLINE                                EXTI_Line0

#define DIO2_IOPORT                                 GPIOC
#define DIO2_PIN                                    GPIO_Pin_4
#define DIO2_EXTIRQPORT                             GPIO_PortSourceGPIOC
#define DIO2_EXTIRQPIN                              GPIO_PinSource4
#define DIO2_EXTLINE                                EXTI_Line4

#define DIO3_IOPORT                                 GPIOC
#define DIO3_PIN                                    GPIO_Pin_1
#define DIO3_EXTIRQPORT                             GPIO_PortSourceGPIOC
#define DIO3_EXTIRQPIN                              GPIO_PinSource1
#define DIO3_EXTLINE                                EXTI_Line1

#define DIO4_IOPORT                                 GPIOC
#define DIO4_PIN                                    GPIO_Pin_2
#define DIO4_EXTIRQPORT                             GPIO_PortSourceGPIOC
#define DIO4_EXTIRQPIN                              GPIO_PinSource2
#define DIO4_EXTLINE                                EXTI_Line2

#define DIO5_IOPORT                                 GPIOC
#define DIO5_PIN                                    GPIO_Pin_6
#define DIO5_EXTIRQPORT                             GPIO_PortSourceGPIOC
#define DIO5_EXTIRQPIN                              GPIO_PinSource6
#define DIO5_EXTLINE                                EXTI_Line6
#define RXTX_IOPORT                                 GPIOC
#define RXTX_PIN                                    GPIO_Pin_7


/*!
 * DIO state read functions mapping
 */
#define DIO0                                        SX1276ReadDio0( )
#define DIO1                                        SX1276ReadDio1( )
#define DIO2                                        SX1276ReadDio2( )
#define DIO3                                        SX1276ReadDio3( )
#define DIO4                                        SX1276ReadDio4( )
#define DIO5                                        SX1276ReadDio5( )

// RXTX pin control see errata note
#define RXTX( txEnable )                            SX1276WriteRxTx( txEnable );

#define GET_TICK_COUNT( )                           ( TickCounter )
#define TICK_RATE_MS( ms )                          ( ms )

typedef enum
{
    RADIO_RESET_OFF,
    RADIO_RESET_ON,
}tRadioResetState;



/*!
 * \brief Initializes the radio interface I/Os
 */
void SX1276InitIo( void );

/*!
 * \brief Set the radio reset pin state
 * 
 * \param state New reset pin state
 */
void SX1276SetReset( uint8_t state );

/*!
 * \brief Writes the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \param [IN]: data New register value
 */
void SX1276Write( uint8_t addr, uint8_t data );

/*!
 * \brief Reads the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \param [OUT]: data Register value
 */
void SX1276Read( uint8_t addr, uint8_t *data );

/*!
 * \brief Writes multiple radio registers starting at address
 *
 * \param [IN] addr   First Radio register address
 * \param [IN] buffer Buffer containing the new register's values
 * \param [IN] size   Number of registers to be written
 */
void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads multiple radio registers starting at address
 *
 * \param [IN] addr First Radio register address
 * \param [OUT] buffer Buffer where to copy the registers data
 * \param [IN] size Number of registers to be read
 */
void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Writes the buffer contents to the radio FIFO
 *
 * \param [IN] buffer Buffer containing data to be put on the FIFO.
 * \param [IN] size Number of bytes to be written to the FIFO
 */
void SX1276WriteFifo( uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads the contents of the radio FIFO
 *
 * \param [OUT] buffer Buffer where to copy the FIFO read data.
 * \param [IN] size Number of bytes to be read from the FIFO
 */
void SX1276ReadFifo( uint8_t *buffer, uint8_t size );

/*!
 * \brief Gets the SX1276 DIO0 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
uint8_t SX1276ReadDio0( void );

/*!
 * \brief Gets the SX1276 DIO1 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1276ReadDio1( void );

/*!
 * \brief Gets the SX1276 DIO2 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1276ReadDio2( void );

/*!
 * \brief Gets the SX1276 DIO3 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1276ReadDio3( void );

/*!
 * \brief Gets the SX1276 DIO4 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
uint8_t SX1276ReadDio4( void );

/*!
 * \brief Gets the SX1276 DIO5 hardware pin status
 *
 * \retval status Current hardware pin status [1, 0]
 */
inline uint8_t SX1276ReadDio5( void );

/*!
 * \brief Writes the external RxTx pin value
 *
 * \remark see errata note
 *
 * \param [IN] txEnable [1: Tx, 0: Rx]
 */
void SX1276WriteRxTx( uint8_t txEnable );

void SX1276ExtiDIO0Enable( void );

void SX1276ExtiDIO1Enable( void );

void SX1276ExtiDIO2Enable( void );

void SX1276ExtiDIO3Enable( void );

void SX1276ExtiDIO4Enable( void );

void SX1276ExtiDIOAllDisable( void );

#endif //__SX1276_HAL_H__

