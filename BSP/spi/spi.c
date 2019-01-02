#include "spi.h"

#define CE(x)        x ? GPIO_SetBits(GPIOC,GPIO_Pin_2) : GPIO_ResetBits(GPIOB,GPIO_Pin_2)       //For spi 1  
#define CSN(x)       x ? GPIO_SetBits(GPIOC,GPIO_Pin_3) : GPIO_ResetBits(GPIOB,GPIO_Pin_3)            //For spi 1
#define IRQ GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)                                                                                //For spi 1         

uint8_t TX_ADDRESS[TX_ADDR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 
uint8_t RX_ADDRESS[RX_ADDR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; 

void Initial_SPI(unsigned char channel, unsigned char mode){ //SPI1可用，在以太网情况下SPI2无法使用，应选择GPIO口模拟
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;
    if(channel==1){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if(channel==2){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        
        GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = mode & 0x02;    //SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = mode & 0x01;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    if(channel==1){
        SPI_Init(SPI1, &SPI_InitStruct);
        SPI_Cmd(SPI1, ENABLE);
    }
    else if(channel == 2){
        SPI_Init(SPI2, &SPI_InitStruct);
        SPI_Cmd(SPI2, ENABLE);    
    }
    else;
}


void nRF24L01_Initial(void){
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);
    /*CE CSN Initial*/
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    /*IRQ Initial*/
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    CE(0);
    CSN(1);
    
    Initial_SPI(1,0);
//    nRF24L01_Config();
}
void nRF24L01_Config(void){
    CE(0);    CSN(1);        
    NRF_W_Reg(W_REGISTER+EN_AA,0x01);       // 使能接收通道0自动应答
    NRF_W_Reg(W_REGISTER+EN_RXADDR,0x01);   // 使能接收通道0
    NRF_W_Reg(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
    NRF_W_Reg(W_REGISTER+RF_CH,0x40);       // 选择射频通道0x40
    NRF_W_Reg(W_REGISTER+RF_SETUP,0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益    
    NRF_W_Reg(W_REGISTER+ CONFIG, 0x0e);  
    CE(1); 
}    

uint8_t SPI_RW_Byte(uint8_t channel,uint8_t data){
    if(channel == 1){
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
        SPI_I2S_SendData(SPI1,data);
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);
        return SPI_I2S_ReceiveData(SPI1);    
    }
    else if(channel == 2){
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)==RESET);
        SPI_I2S_SendData(SPI2,data);
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE)==RESET);
        return SPI_I2S_ReceiveData(SPI2);        
    }
    else    return 0x00;
}
/*********************  寄存器读写  ********************************/
uint8_t NRF_W_Reg(uint8_t reg, uint8_t dat){
        uint8_t status;
        CSN(0);
        status = SPI_RW_Byte(1,reg);
        SPI_RW_Byte(1,dat);
        CSN(1);
        return status;
}
uint8_t NRF_R_Reg(uint8_t reg){ //dat = 0x00 means read reg
        uint8_t status;
        CSN(0);
        SPI_RW_Byte(1,reg);
        status = SPI_RW_Byte(1,0x00);
        CSN(1);
        return status;
}

/*******************  数据发送&接收  *********************************/
uint8_t NRF_WriteBuf(uint8_t reg, uint8_t *dat, uint8_t bytes){
        uint8_t status, cnt;
        CE(0);    CSN(0);
        status = SPI_RW_Byte(1,reg);
        for(cnt = 0;cnt<bytes;cnt++)    SPI_RW_Byte(1,*dat++);
        CSN(1);
        return status;
}

uint8_t SPI_Read_Buf(uint8_t reg,uint8_t *dat,uint8_t bytes){
        uint8_t status,byte_ctr;
        CSN(0);
        status=SPI_RW_Byte(1,reg);
        for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)        dat[byte_ctr]=SPI_RW_Byte(1,0);
        CSN(1);
        return(status);
}

void nRF24L01_TxPacket(unsigned char *tx_buf){
        CE(0);            
        NRF_WriteBuf(W_REGISTER + TX_ADDR, TX_ADDRESS, TX_ADDR_WIDTH); //写寄存器指令+接收地址使能指令+接收地址+地址宽度
        NRF_WriteBuf(W_REGISTER + RX_ADDR_P0, TX_ADDRESS, TX_ADDR_WIDTH);//为了应答接收设备，接收通道0地址和发送地址相同
        NRF_WriteBuf(W_TX_PAYLOAD, tx_buf, TX_DATA_WIDTH);         //写入数据 
        CE(1);        
}

void nRF24L01_RxPacket(unsigned char* rx_buf){ //应该用中断来接收
        uint8_t status;
        TX_RX_Mode(0);
        CE(0);
        status=NRF_R_Reg(STATUS); 
        if(status & 0x40)        SPI_Read_Buf(R_RX_PAYLOAD,rx_buf,RX_DATA_WIDTH);// read receive payload from RX_FIFO buffer
        NRF_W_Reg(W_REGISTER + STATUS,status);
        SPI_RW_Byte(1,FLUSH_RX); 
        CE(1);
}

void TX_RX_Mode(uint8_t mode){ // 0-----R         1-------T
        CE(0);
        mode ? NRF_W_Reg(W_REGISTER + CONFIG, 0x0e):NRF_W_Reg(W_REGISTER + CONFIG, 0x0f); // 
        CE(1);

}
