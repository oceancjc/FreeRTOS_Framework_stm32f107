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
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
 
#include "sx1276-Hal.h"




void SX1276InitIo( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                            RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE );
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // Configure NSS as output
    GPIO_InitStructure.GPIO_Pin = NSS_PIN;
    GPIO_Init( NSS_IOPORT, &GPIO_InitStructure );
    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );  
    GPIO_InitStructure.GPIO_Pin = RESET_PIN;
    GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );	
    
    // Configure radio DIO0-5 as inputs
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin =  DIO0_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );   
    GPIO_InitStructure.GPIO_Pin =  DIO1_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure ); 
    GPIO_InitStructure.GPIO_Pin =  DIO2_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin =  DIO3_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure ); 
    GPIO_InitStructure.GPIO_Pin =  DIO4_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure ); 
    GPIO_InitStructure.GPIO_Pin =  DIO5_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );     
    GPIO_InitStructure.GPIO_Pin =  RXTX_PIN;
    GPIO_Init( RXTX_IOPORT, &GPIO_InitStructure );
		
	  NVIC_InitTypeDef NVIC_InitStructure;
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;	
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
  	NVIC_Init(&NVIC_InitStructure); 

		
    GPIO_EXTILineConfig(DIO0_EXTIRQPORT,DIO0_EXTIRQPIN);
    GPIO_EXTILineConfig(DIO1_EXTIRQPORT,DIO1_EXTIRQPIN);			
    GPIO_EXTILineConfig(DIO2_EXTIRQPORT,DIO2_EXTIRQPIN);		
    GPIO_EXTILineConfig(DIO3_EXTIRQPORT,DIO3_EXTIRQPIN);		
    GPIO_EXTILineConfig(DIO4_EXTIRQPORT,DIO4_EXTIRQPIN);		
    GPIO_EXTILineConfig(DIO5_EXTIRQPORT,DIO5_EXTIRQPIN);		

}

void SX1276SetReset( uint8_t state ){
    if(state == RADIO_RESET_ON){
       GPIO_ResetBits(RESET_IOPORT,RESET_PIN); 
    }
    else    GPIO_SetBits(RESET_IOPORT,RESET_PIN);
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i = 0;
    //NSS = 0;
    GPIO_ResetBits( NSS_IOPORT, NSS_PIN );
    SPI_RW_Byte(SPI_CHANNEL, addr | 0x80);
    for( i = 0; i < size; i++ )        SPI_RW_Byte( SPI_CHANNEL, buffer[i] );
    //NSS = 1;
    GPIO_SetBits( NSS_IOPORT, NSS_PIN );
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i = 0;
    //NSS = 0;
    GPIO_ResetBits( NSS_IOPORT, NSS_PIN );
    SPI_RW_Byte(SPI_CHANNEL, addr & 0x7F);
    for( i = 0; i < size; i++ )     buffer[i] = SPI_RW_Byte( SPI_CHANNEL, 0 );
    //NSS = 1;
    GPIO_SetBits( NSS_IOPORT, NSS_PIN );
    
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}
uint8_t SX1276ReadDio0( void )
{
    return GPIO_ReadInputDataBit( DIO0_IOPORT, DIO0_PIN );
}

inline uint8_t SX1276ReadDio1( void )
{
    return GPIO_ReadInputDataBit( DIO1_IOPORT, DIO1_PIN );
}

inline uint8_t SX1276ReadDio2( void )
{
    return GPIO_ReadInputDataBit( DIO2_IOPORT, DIO2_PIN );
}

inline uint8_t SX1276ReadDio3( void )
{
    return GPIO_ReadInputDataBit( DIO3_IOPORT, DIO3_PIN );
}

uint8_t SX1276ReadDio4( void )
{
    return GPIO_ReadInputDataBit( DIO4_IOPORT, DIO4_PIN );
}

//inline uint8_t SX1276ReadDio5( void )
//{
//    return IoePinGet( RF_DIO5_PIN );
//}

void SX1276WriteRxTx( uint8_t txEnable )
{
    if( txEnable != 0 )
    {
        //IoePinOn( FEM_CTX_PIN );
        //IoePinOff( FEM_CPS_PIN );
        GPIO_WriteBit( RXTX_IOPORT, RXTX_PIN, Bit_RESET );
    }
    else
    {
        //IoePinOff( FEM_CTX_PIN );
        //IoePinOn( FEM_CPS_PIN );
        GPIO_WriteBit( RXTX_IOPORT, RXTX_PIN, Bit_SET );
    }
}



void SX1276ExtiDIO0Enable( void ){
    EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_InitStructure.EXTI_Line=DIO0_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	 
}

void SX1276ExtiDIO1Enable( void ){
    EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_InitStructure.EXTI_Line=DIO1_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	  
}


void SX1276ExtiDIO2Enable( void ){
    EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_InitStructure.EXTI_Line=DIO2_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	 
}

void SX1276ExtiDIO3Enable( void ){
    EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_InitStructure.EXTI_Line=DIO3_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	  
}

void SX1276ExtiDIO4Enable( void ){
    EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_InitStructure.EXTI_Line=DIO4_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	 
}

void SX1276ExtiDIOAllDisable( void ){
    EXTI_InitTypeDef EXTI_InitStructure;
  	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  	EXTI_InitStructure.EXTI_Line=DIO0_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	 
  	EXTI_InitStructure.EXTI_Line=DIO1_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	
  	EXTI_InitStructure.EXTI_Line=DIO2_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	 
  	EXTI_InitStructure.EXTI_Line=DIO3_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);		
  	EXTI_InitStructure.EXTI_Line=DIO4_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	 
  	EXTI_InitStructure.EXTI_Line=DIO5_EXTLINE;	
  	EXTI_Init(&EXTI_InitStructure);	
}




