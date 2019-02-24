#include "i2c.h"

#define I2C_EE                      I2C1

void I2C_Configuration(void){
    I2C_InitTypeDef  I2C_InitStructure = { 0 };
    GPIO_InitTypeDef GPIO_InitStructure= { 0 };
    /* GPIOB Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB1Periph_I2C1, ENABLE); 
    /* eeprom wp Control Pin set */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);        // ??????,????

    /* PB6,7 SCL and SDA */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;  // ??????
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2C_EE);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;          
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;       //100K??
    I2C_Cmd(I2C_EE, ENABLE);  // ??I2C??
    I2C_Init(I2C_EE, &I2C_InitStructure); // ??I2C??
    I2C_AcknowledgeConfig(I2C_EE, ENABLE);

}






void I2C_RegisterByteWrite(uint8_t pBuffer, uint8_t devAddr, uint8_t regAddr){
	 I2C_GenerateSTART(I2C1, ENABLE);
	 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	 I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
	 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	 I2C_SendData(I2C1, regAddr);
	 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	 I2C_SendData(I2C1, pBuffer);
	 while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	 I2C_GenerateSTOP(I2C1, ENABLE);
}



int I2C_RegisterByteRead(uint8_t devAddr, uint8_t ReadAddr, uint8_t *pBuffer){
    if(pBuffer == NULL)    return -1;
	I2C_WaitResponse(devAddr);
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, ReadAddr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); 
	*pBuffer = I2C_ReceiveData(I2C1);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}



void I2C_WaitResponse(uint8_t devAddr){
    do{
        /* Send START condition */
        I2C_GenerateSTART(I2C1, ENABLE);
        /* Read I2C1 SR1 register */
        I2C_ReadRegister(I2C1, I2C_Register_SR1);
        /* Send EEPROM address for write */
        I2C_Send7bitAddress(I2C1, devAddr, I2C_Direction_Transmitter);
    }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
    /* Clear AF flag */
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);
}





