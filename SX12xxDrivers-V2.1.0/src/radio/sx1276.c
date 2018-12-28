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
 * \file       sx1276.c
 * \brief      SX1276 RF chip driver
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */

#include "sx1276.h"


/*!
 * SX1276 registers variable
 */

void SX1276Init( SX1276LRDevice_t *device )
{
    SX1276DeviceInit(device);
    SX1276InitIo( );
    SX1276Reset( device );
    SX1276Read(REG_LR_VERSION, &device->SX1276LR->RegVersion);
	  if (device->SX1276LR->RegVersion != 0x12)    return;    //SPI verifiy
    SX1276SetLoRaOn(device );
    // Initialize LoRa modem
    SX1276LoRaInit( device );
    
//#endif

}

void SX1276Reset( SX1276LRDevice_t* device )
{
    SX1276LoRaReset(device);
}


void SX1276SetLoRaOn( SX1276LRDevice_t *device )
{
    SX1276LoRaSetOpMode( device, RFLR_OPMODE_SLEEP );
    device->SX1276LR->RegOpMode |= RFLR_OPMODE_LONGRANGEMODE_ON;
	  if( device->LoRaSettings.RFFrequency < 525000000 )    device->SX1276LR->RegOpMode |= RFLR_OPMODE_FREQMODE_ACCESS_LF;
	  else                                                  device->SX1276LR->RegOpMode &= ~RFLR_OPMODE_FREQMODE_ACCESS_LF;
    SX1276Write( REG_LR_OPMODE, device->SX1276LR->RegOpMode );
    SX1276LoRaSetOpMode( device, RFLR_OPMODE_STANDBY );
                                        // RxDone               RxTimeout                   FhssChangeChannel           CadDone
    device->SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                        // CadDetected          ModeReady
    device->SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
    SX1276WriteBuffer( REG_LR_DIOMAPPING1, &device->SX1276LR->RegDioMapping1, 2 );
    SX1276ReadBuffer( REG_LR_OPMODE, (uint8_t*)device->SX1276LR + 1, 0x70-1 );  //Readback all init reg values
}




//double SX1276ReadRssi( void )
//{
//    if( LoRaOn == false )
//    {
//        return SX1276FskReadRssi( );
//    }
//    else
//    {
//        return SX1276LoRaReadRssi( );
//    }
//}

//uint8_t SX1276ReadRxGain( void )
//{
//    if( LoRaOn == false )
//    {
//        return SX1276FskReadRxGain( );
//    }
//    else
//    {
//        return SX1276LoRaReadRxGain( );
//    }
//}

//uint8_t SX1276GetPacketRxGain( void )
//{
//    if( LoRaOn == false )
//    {
//        return SX1276FskGetPacketRxGain(  );
//    }
//    else
//    {
//        return SX1276LoRaGetPacketRxGain(  );
//    }
//}

//int8_t SX1276GetPacketSnr( void )
//{
//    if( LoRaOn == false )
//    {
//         while( 1 )
//         {
//             // Useless in FSK mode
//             // Block program here
//         }
//    }
//    else
//    {
//        return SX1276LoRaGetPacketSnr(  );
//    }
//}

//double SX1276GetPacketRssi( void )
//{
//    if( LoRaOn == false )
//    {
//        return SX1276FskGetPacketRssi(  );
//    }
//    else
//    {
//        return SX1276LoRaGetPacketRssi( );
//    }
//}

//uint32_t SX1276GetPacketAfc( void )
//{
//    if( LoRaOn == false )
//    {
//        return SX1276FskGetPacketAfc(  );
//    }
//    else
//    {
//         while( 1 )
//         {
//             // Useless in LoRa mode
//             // Block program here
//         }
//    }
//}

//void SX1276StartRx( void )
//{
//    if( LoRaOn == false )
//    {
//        SX1276FskSetRFState( RF_STATE_RX_INIT );
//    }
//    else
//    {
//        SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
//    }
//}

//void SX1276GetRxPacket( void *buffer, uint16_t *size )
//{
//    if( LoRaOn == false )
//    {
//        SX1276FskGetRxPacket( buffer, size );
//    }
//    else
//    {
//        SX1276LoRaGetRxPacket( buffer, size );
//    }
//}

//void SX1276SetTxPacket( const void *buffer, uint16_t size )
//{
//    if( LoRaOn == false )
//    {
//        SX1276FskSetTxPacket( buffer, size );
//    }
//    else
//    {
//        SX1276LoRaSetTxPacket( buffer, size );
//    }
//}

//uint8_t SX1276GetRFState( void )
//{
//    if( LoRaOn == false )
//    {
//        return SX1276FskGetRFState( );
//    }
//    else
//    {
//        return SX1276LoRaGetRFState( );
//    }
//}

//void SX1276SetRFState( uint8_t state )
//{
//    if( LoRaOn == false )
//    {
//        SX1276FskSetRFState( state );
//    }
//    else
//    {
//        SX1276LoRaSetRFState( state );
//    }
//}

//uint32_t SX1276Process( void )
//{
//    if( LoRaOn == false )
//    {
//        return SX1276FskProcess( );
//    }
//    else
//    {
//        return SX1276LoRaProcess( );
//    }
//}


