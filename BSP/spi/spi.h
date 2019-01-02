#ifndef __SPI_H
#define __SPI_H

#include <stdio.h>
#include <Net_Config.h>
#include <string.h>
#include <stdint.h>
//#include "stm32f10x_spi.h"
#include "stm32f10x.h"


/*******************************************************/
#define TX_ADDR_WIDTH 5	//发送地址宽度设置为5个字节
#define RX_ADDR_WIDTH 5	//接收地址宽度设置为5个字节
#define TX_DATA_WIDTH 4//发送数据宽度4个字节
#define RX_DATA_WIDTH 4//接收数据宽度4个字节
/*******************命令寄存器***************************/
#define  R_REGISTER      0x00//读取配置寄存器
#define  W_REGISTER      0x20//写配置寄存器
#define  R_RX_PAYLOAD 	 0x61//读取RX有效数据
#define  W_TX_PAYLOAD	 0xa0//写TX有效数据
#define  FLUSH_TX		 0xe1//清除TXFIFO寄存器
#define  FLUSH_RX		 0xe2//清除RXFIFO寄存器
#define  REUSE_TX_PL     0xe3//重新使用上一包有效数据
#define  NOP             0xff//空操作
/******************寄存器地址****************************/
#define  CONFIG          0x00//配置寄存器
#define  EN_AA			 0x01//使能自动应答
#define  EN_RXADDR       0x02//接收通道使能0-5个通道
#define  SETUP_AW        0x03//设置数据通道地址宽度3-5
#define  SETUP_RETR      0x04//建立自动重发
#define  RF_CH           0x05//射频通道设置
#define  RF_SETUP        0x06//射频寄存器
#define  STATUS          0x07//状态寄存器
#define  OBSERVE_TX      0x08//发送检测寄存器
#define  CD              0x09//载波
#define  RX_ADDR_P0      0x0a//数据通道0接收地址
#define  RX_ADDR_P1      0x0b//数据通道1接收地址
#define  RX_ADDR_P2      0x0c//数据通道2接收地址
#define  RX_ADDR_P3      0x0d//数据通道3接收地址
#define  RX_ADDR_P4      0x0e//数据通道4接收地址
#define  RX_ADDR_P5      0x0f//数据通道5接收地址
#define  TX_ADDR         0x10//发送地址
#define  RX_PW_P0        0x11//P0通道数据宽度设置
#define  RX_PW_P1        0x12//P1通道数据宽度设置
#define  RX_PW_P2        0x13//P2通道数据宽度设置
#define  RX_PW_P3        0x14//P3通道数据宽度设置
#define  RX_PW_P4        0x15//P4通道数据宽度设置
#define  RX_PW_P5        0x16//P5通道数据宽度设置
#define  FIFO_STATUS     0x17//FIFO状态寄存器


/*******************相关函数声明**************************/
extern void Initial_SPI(unsigned char channel,unsigned char mode);
extern void nRF24L01_Initial(void);
extern void TX_RX_Mode(U8 mode);
extern U8 SPI_RW_Byte(U8 channel,U8 data);
extern U8 NRF_W_Reg(U8 reg, U8 dat);
extern U8 NRF_R_Reg(U8 reg);
extern U8 NRF_WriteBuf(U8 reg, U8 *dat, U8 bytes);
extern U8 SPI_Read_Buf(U8 reg,U8 *dat,U8 bytes);
extern void nRF24L01_TxPacket(unsigned char *tx_buf);
extern void nRF24L01_RxPacket(unsigned char* rx_buf);
extern void nRF24L01_Config(void);
#endif
