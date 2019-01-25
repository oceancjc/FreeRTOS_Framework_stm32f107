/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START ((uint16_t)0x0000)
#define REG_INPUT_NREGS 25
#define REG_HOLD_START  ((uint16_t)0x0000)
#define REG_HOLD_NREGS  30

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS] = {
    0x0000,0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,0x8888,0x9999,
    0xAAAA,0xBBBB,0xCCCC,0xDDDD,0xEEEE,0xFFFF,0xEEEE,0xDDDD,0xCCCC,0xBBBB,
    0xAAAA,0x9999,0x8888,0x7777,0x6666
};
static USHORT   usRegHoldStart  = REG_HOLD_START;
static USHORT   usRegHoldBuf[REG_HOLD_NREGS] = { //Notice: 0 is for other useage
    0x147b,0x3f8e,0x147b,0x400e,0x1eb8,0x4055,0x147b,0x408e,0x0123,0x1234,
    0x2345,0x3456,0x4567,0x5678,0x6789,0x789A,0x89AB,0x9ABC,0xABCD,0xBCDE,
    0xDCBA,0xCBA9,0xBA98,0xA987,0x9876,0x8765,0x7654,0x6543,0x5432,0x4321
};
extern uint8_t  g_Meter_Data[];
extern uint8_t  g_Check_Data[];


/* ----------------------- Start implementation -----------------------------*/
void modbus_Tsk(void *pvParameters){
    /*TODO: Address is 0x0A, Baud Rate = 9600, needs change*/
    eMBErrorCode eStatus = eMBInit( MB_RTU, 0x0A, 0, 9600, MB_PAR_NONE ); 
    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );
    while(1){
        ( void )eMBPoll(  );
        /* Here we simply count the number of poll cycles. */
        usRegInputBuf[0]++;
    }
}



/**
* @brief Read input registers only
* @param pucRegBuffer    read: data returned
*        usAddress       register start address
*        usNRegs         number of registers
* @retval eStatus 
*/
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex = 0;

    if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else        eStatus = MB_ENOREG;
    
    return eStatus;
}



/**
* @brief Processing Holding registers, write/read register
* @param pucRegBuffer write: input data pointer    read: data returned
*        usAddress    register start address
*        usNRegs      number of registers
*        eMode        operation mode, write or read
* @retval eStatus 
*/
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    if( ( usAddress >= REG_HOLD_START ) && ( usAddress + usNRegs <= REG_HOLD_START + REG_HOLD_NREGS ) ){
        iRegIndex = ( int )( usAddress - usRegHoldStart );
        switch(eMode){
            case MB_REG_WRITE:
                while( usNRegs > 0 ){
                    usRegHoldBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    usRegHoldBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
                break;
            case MB_REG_READ:
                while( usNRegs > 0 ){
                    *pucRegBuffer++ = ( uint8_t )( usRegHoldBuf[iRegIndex] >> 8 );
                    *pucRegBuffer++ = ( uint8_t )( usRegHoldBuf[iRegIndex] & 0xFF );
                    iRegIndex++;
                    usNRegs--;
                }
                break;
            default:
                return MB_EINVAL;     
        }
    }
    else        eStatus = MB_ENOREG;
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
