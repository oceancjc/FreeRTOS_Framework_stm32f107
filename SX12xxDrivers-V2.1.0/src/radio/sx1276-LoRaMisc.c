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
 * \file       sx1276-LoRaMisc.c
 * \brief      SX1276 RF chip high level functions driver
 *
 * \remark     Optional support functions.
 *             These functions are defined only to easy the change of the
 *             parameters.
 *             For a final firmware the radio parameters will be known so
 *             there is no need to support all possible parameters.
 *             Removing these functions will greatly reduce the final firmware
 *             size.
 *
 * \version    2.0.0 
 * \date       May 6 2013
 * \author     Gregory Cristian
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */


#include "sx1276-LoRaMisc.h"

/*!
 * SX1276 definitions
 */
#define XTAL_FREQ                                   32000000
#define FREQ_STEP                                   61.03515625



void SX1276LoRaSetRFFrequency( SX1276LRDevice_t *device, uint32_t freq )
{
    device->LoRaSettings.RFFrequency = freq;

    freq = ( uint32_t )( ( double )freq / ( double )FREQ_STEP );
    device->SX1276LR->RegFrfMsb = ( uint8_t )( ( freq >> 16 ) & 0xFF );
    device->SX1276LR->RegFrfMid = ( uint8_t )( ( freq >> 8 ) & 0xFF );
    device->SX1276LR->RegFrfLsb = ( uint8_t )( freq & 0xFF );
    SX1276WriteBuffer( REG_LR_FRFMSB, &device->SX1276LR->RegFrfMsb, 3 );
}

uint32_t SX1276LoRaGetRFFrequency( SX1276LRDevice_t *device )
{
	  uint32_t rfFreq = 0;
    SX1276ReadBuffer( REG_LR_FRFMSB, &device->SX1276LR->RegFrfMsb, 3 );
    rfFreq = ( ( uint32_t )device->SX1276LR->RegFrfMsb << 16 ) | ( ( uint32_t )device->SX1276LR->RegFrfMid << 8 ) | ( ( uint32_t )device->SX1276LR->RegFrfLsb );
    rfFreq = ( uint32_t )( ( double )device->LoRaSettings.RFFrequency * ( double )FREQ_STEP );
	  device->LoRaSettings.RFFrequency = rfFreq;
    return device->LoRaSettings.RFFrequency;
}



void SX1276LoRaSetRFPower( SX1276LRDevice_t *device, int8_t power )
{
    SX1276Read( REG_LR_PACONFIG, &device->SX1276LR->RegPaConfig );
    SX1276Read( REG_LR_PADAC, &device->SX1276LR->RegPaDac );
    
    if( ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {
        if( ( device->SX1276LR->RegPaDac & 0x87 ) == 0x87 )
        {
            if( power < 5 )                power = 5;
            else if( power > 20 )          power = 20;
            device->SX1276LR->RegPaConfig = ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
            device->SX1276LR->RegPaConfig = ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if( power < 2 )                power = 2;
            else if( power > 17 )          power = 17;
            device->SX1276LR->RegPaConfig = ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
            device->SX1276LR->RegPaConfig = ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if( power < -1 )                 power = -1;
        else if( power > 14 )            power = 14;

        device->SX1276LR->RegPaConfig = ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_MAX_POWER_MASK ) | 0x70;
        device->SX1276LR->RegPaConfig = ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1276Write( REG_LR_PACONFIG, device->SX1276LR->RegPaConfig );
    device->LoRaSettings.Power = power;
}

int8_t SX1276LoRaGetRFPower( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_PACONFIG, &device->SX1276LR->RegPaConfig );
    SX1276Read( REG_LR_PADAC, &device->SX1276LR->RegPaDac );

    if( ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {
        if( ( device->SX1276LR->RegPaDac & 0x07 ) == 0x07 )
        {
            device->LoRaSettings.Power = 5 + ( device->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
        }
        else
        {
            device->LoRaSettings.Power = 2 + ( device->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
        }
    }
    else
    {
        device->LoRaSettings.Power = -1 + ( device->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_OUTPUTPOWER_MASK );
    }
    return device->LoRaSettings.Power;
}

void SX1276LoRaSetSignalBandwidth( SX1276LRDevice_t *device, uint8_t bw )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &device->SX1276LR->RegModemConfig1 );
    device->SX1276LR->RegModemConfig1 = ( device->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_BW_MASK ) | ( bw << 4 );
    SX1276Write( REG_LR_MODEMCONFIG1, device->SX1276LR->RegModemConfig1 );
    device->LoRaSettings.SignalBw = bw;
}

uint8_t SX1276LoRaGetSignalBandwidth( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &device->SX1276LR->RegModemConfig1 );
    device->LoRaSettings.SignalBw = ( device->SX1276LR->RegModemConfig1 & ~RFLR_MODEMCONFIG1_BW_MASK ) >> 4;
    return device->LoRaSettings.SignalBw;
}

void SX1276LoRaSetSpreadingFactor( SX1276LRDevice_t *device, uint8_t factor )
{

    if( factor > 12 )        factor = 12;
    else if( factor < 6 )    factor = 6;
    if( factor == 6 )        SX1276LoRaSetNbTrigPeaks( device, 5 );
    else                     SX1276LoRaSetNbTrigPeaks( device, 3 );

    SX1276Read( REG_LR_MODEMCONFIG2, &device->SX1276LR->RegModemConfig2 );    
    device->SX1276LR->RegModemConfig2 = ( device->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SF_MASK ) | ( factor << 4 );
    SX1276Write( REG_LR_MODEMCONFIG2, device->SX1276LR->RegModemConfig2 );    
    device->LoRaSettings.SpreadingFactor = factor;
}

uint8_t SX1276LoRaGetSpreadingFactor( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_MODEMCONFIG2, &device->SX1276LR->RegModemConfig2 );   
    device->LoRaSettings.SpreadingFactor = ( device->SX1276LR->RegModemConfig2 & ~RFLR_MODEMCONFIG2_SF_MASK ) >> 4;
    return device->LoRaSettings.SpreadingFactor;
}

void SX1276LoRaSetErrorCoding( SX1276LRDevice_t *device, uint8_t value )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &device->SX1276LR->RegModemConfig1 );
    device->SX1276LR->RegModemConfig1 = ( device->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_CODINGRATE_MASK ) | ( value << 1 );
    SX1276Write( REG_LR_MODEMCONFIG1, device->SX1276LR->RegModemConfig1 );
    device->LoRaSettings.ErrorCoding = value;
}

uint8_t SX1276LoRaGetErrorCoding( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &device->SX1276LR->RegModemConfig1 );
    device->LoRaSettings.ErrorCoding = ( device->SX1276LR->RegModemConfig1 & ~RFLR_MODEMCONFIG1_CODINGRATE_MASK ) >> 1;
    return device->LoRaSettings.ErrorCoding;
}

void SX1276LoRaSetPacketCrcOn( SX1276LRDevice_t *device, bool enable )
{
    SX1276Read( REG_LR_MODEMCONFIG2, &device->SX1276LR->RegModemConfig2 );
    device->SX1276LR->RegModemConfig2 = ( device->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK ) | ( enable << 2 );
    SX1276Write( REG_LR_MODEMCONFIG2, device->SX1276LR->RegModemConfig2 );
    device->LoRaSettings.CrcOn = enable;
}

void SX1276LoRaSetPreambleLength( SX1276LRDevice_t *device, uint16_t value )
{
    SX1276ReadBuffer( REG_LR_PREAMBLEMSB, &device->SX1276LR->RegPreambleMsb, 2 );

    device->SX1276LR->RegPreambleMsb = ( value >> 8 ) & 0x00FF;
    device->SX1276LR->RegPreambleLsb = value & 0xFF;
    SX1276WriteBuffer( REG_LR_PREAMBLEMSB, &device->SX1276LR->RegPreambleMsb, 2 );
}

uint16_t SX1276LoRaGetPreambleLength( SX1276LRDevice_t *device )
{
    SX1276ReadBuffer( REG_LR_PREAMBLEMSB, &device->SX1276LR->RegPreambleMsb, 2 );
    return ( ( device->SX1276LR->RegPreambleMsb & 0x00FF ) << 8 ) | device->SX1276LR->RegPreambleLsb;
}

bool SX1276LoRaGetPacketCrcOn( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_MODEMCONFIG2, &device->SX1276LR->RegModemConfig2 );
    device->LoRaSettings.CrcOn = ( device->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON ) >> 1;
    return device->LoRaSettings.CrcOn;
}

void SX1276LoRaSetImplicitHeaderOn( SX1276LRDevice_t *device, bool enable )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &device->SX1276LR->RegModemConfig1 );
    device->SX1276LR->RegModemConfig1 = ( device->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK ) | ( enable );
    SX1276Write( REG_LR_MODEMCONFIG1, device->SX1276LR->RegModemConfig1 );
    device->LoRaSettings.ImplicitHeaderOn = enable;
}

bool SX1276LoRaGetImplicitHeaderOn( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_MODEMCONFIG1, &device->SX1276LR->RegModemConfig1 );
    device->LoRaSettings.ImplicitHeaderOn = ( device->SX1276LR->RegModemConfig1 & RFLR_MODEMCONFIG1_IMPLICITHEADER_ON );
    return device->LoRaSettings.ImplicitHeaderOn;
}

void SX1276LoRaSetRxSingleOn( SX1276LRDevice_t *device, bool enable )
{
    device->LoRaSettings.RxSingleOn = enable;
}

bool SX1276LoRaGetRxSingleOn( SX1276LRDevice_t *device )
{
    return device->LoRaSettings.RxSingleOn;
}

void SX1276LoRaSetFreqHopOn( SX1276LRDevice_t *device, bool enable )
{
    device->LoRaSettings.FreqHopOn = enable;
}

bool SX1276LoRaGetFreqHopOn( SX1276LRDevice_t *device )
{
    return device->LoRaSettings.FreqHopOn;
}

void SX1276LoRaSetHopPeriod( SX1276LRDevice_t *device, uint8_t value )
{
    device->SX1276LR->RegHopPeriod = value;
    SX1276Write( REG_LR_HOPPERIOD, device->SX1276LR->RegHopPeriod );
    device->LoRaSettings.HopPeriod = value;
}

uint8_t SX1276LoRaGetHopPeriod( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_HOPPERIOD, &device->SX1276LR->RegHopPeriod );
    device->LoRaSettings.HopPeriod = device->SX1276LR->RegHopPeriod;
    return device->LoRaSettings.HopPeriod;
}

void SX1276LoRaSetTxPacketTimeout( SX1276LRDevice_t *device, uint32_t value )
{
    device->LoRaSettings.TxPacketTimeout = value;
}

uint32_t SX1276LoRaGetTxPacketTimeout( SX1276LRDevice_t *device )
{
    return device->LoRaSettings.TxPacketTimeout;
}

void SX1276LoRaSetRxPacketTimeout( SX1276LRDevice_t *device, uint32_t value )
{
    device->LoRaSettings.RxPacketTimeout = value;
}

uint32_t SX1276LoRaGetRxPacketTimeout( SX1276LRDevice_t *device )
{
    return device->LoRaSettings.RxPacketTimeout;
}

void SX1276LoRaSetPayloadLength( SX1276LRDevice_t *device, uint8_t value )
{
    device->SX1276LR->RegPayloadLength = value;
    SX1276Write( REG_LR_PAYLOADLENGTH, device->SX1276LR->RegPayloadLength );
    device->LoRaSettings.PayloadLength = value;
}

uint8_t SX1276LoRaGetPayloadLength( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_PAYLOADLENGTH, &device->SX1276LR->RegPayloadLength );
    device->LoRaSettings.PayloadLength = device->SX1276LR->RegPayloadLength;
    return device->LoRaSettings.PayloadLength;
}

void SX1276LoRaSetPa20dBm( SX1276LRDevice_t *device, bool enale )
{
    SX1276Read( REG_LR_PADAC, &device->SX1276LR->RegPaDac );
    SX1276Read( REG_LR_PACONFIG, &device->SX1276LR->RegPaConfig );

    if( ( device->SX1276LR->RegPaConfig & RFLR_PACONFIG_PASELECT_PABOOST ) == RFLR_PACONFIG_PASELECT_PABOOST )
    {    
        if( enale == true )            device->SX1276LR->RegPaDac = 0x87;
    }
    else                               device->SX1276LR->RegPaDac = 0x84;

    SX1276Write( REG_LR_PADAC, device->SX1276LR->RegPaDac );
}

bool SX1276LoRaGetPa20dBm( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_PADAC, &device->SX1276LR->RegPaDac );
    return ( ( device->SX1276LR->RegPaDac & 0x07 ) == 0x07 ) ? true : false;
}

void SX1276LoRaSetPAOutput( SX1276LRDevice_t *device, uint8_t outputPin )
{
    uint8_t regVal = 0;
	  SX1276Read( REG_LR_PACONFIG, &regVal );
    regVal = (regVal & RFLR_PACONFIG_PASELECT_MASK ) | outputPin;
	  device->SX1276LR->RegPaConfig = regVal;
    SX1276Write( REG_LR_PACONFIG, regVal );
}

uint8_t SX1276LoRaGetPAOutput( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_PACONFIG, &device->SX1276LR->RegPaConfig );
    return device->SX1276LR->RegPaConfig & ~RFLR_PACONFIG_PASELECT_MASK;
}

void SX1276LoRaSetPaRamp( SX1276LRDevice_t *device, uint8_t value )
{
    uint8_t regVal = 0;
	  SX1276Read( REG_LR_PARAMP, &regVal );
    regVal = ( regVal & RFLR_PARAMP_MASK ) | ( value & ~RFLR_PARAMP_MASK );
	  device->SX1276LR->RegPaRamp = regVal;
    SX1276Write( REG_LR_PARAMP, regVal );
}

uint8_t SX1276LoRaGetPaRamp( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_PARAMP, &device->SX1276LR->RegPaRamp );
    return device->SX1276LR->RegPaRamp & ~RFLR_PARAMP_MASK;
}

void SX1276LoRaSetSymbTimeout( SX1276LRDevice_t *device, uint16_t value )
{
    SX1276ReadBuffer( REG_LR_MODEMCONFIG2, &device->SX1276LR->RegModemConfig2, 2 );

    device->SX1276LR->RegModemConfig2 = ( device->SX1276LR->RegModemConfig2 & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) | ( ( value >> 8 ) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK );
    device->SX1276LR->RegSymbTimeoutLsb = value & 0xFF;
    SX1276WriteBuffer( REG_LR_MODEMCONFIG2, &device->SX1276LR->RegModemConfig2, 2 );
}

uint16_t SX1276LoRaGetSymbTimeout( SX1276LRDevice_t *device )
{
    SX1276ReadBuffer( REG_LR_MODEMCONFIG2, &device->SX1276LR->RegModemConfig2, 2 );
    return ( ( device->SX1276LR->RegModemConfig2 & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK ) << 8 ) | device->SX1276LR->RegSymbTimeoutLsb;
}

void SX1276LoRaSetLowDatarateOptimize( SX1276LRDevice_t *device, bool enable )
{
    SX1276Read( REG_LR_MODEMCONFIG3, &device->SX1276LR->RegModemConfig3 );
    device->SX1276LR->RegModemConfig3 = ( device->SX1276LR->RegModemConfig3 & RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK ) | ( enable << 3 );
    SX1276Write( REG_LR_MODEMCONFIG3, device->SX1276LR->RegModemConfig3 );
}

bool SX1276LoRaGetLowDatarateOptimize( SX1276LRDevice_t *device )
{
    SX1276Read( REG_LR_MODEMCONFIG3, &device->SX1276LR->RegModemConfig3 );
    return ( ( device->SX1276LR->RegModemConfig3 & RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON ) >> 3 );
}

void SX1276LoRaSetNbTrigPeaks( SX1276LRDevice_t *device, uint8_t value )
{
    SX1276Read( 0x31, &device->SX1276LR->RegDetectOptimize );
    device->SX1276LR->RegDetectOptimize = ( device->SX1276LR->RegDetectOptimize & 0xF8 ) | value;
    SX1276Write( 0x31, device->SX1276LR->RegDetectOptimize );
}

uint8_t SX1276LoRaGetNbTrigPeaks( SX1276LRDevice_t *device )
{
    SX1276Read( 0x31, &device->SX1276LR->RegDetectOptimize );
    return ( device->SX1276LR->RegDetectOptimize & 0x07 );
}

