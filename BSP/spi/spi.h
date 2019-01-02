#ifndef __SPI_H
#define __SPI_H

#include <stdio.h>
#include <Net_Config.h>
#include <string.h>
#include <stdint.h>
//#include "stm32f10x_spi.h"
#include "stm32f10x.h"


/*******************************************************/
#define TX_ADDR_WIDTH 5	//���͵�ַ�������Ϊ5���ֽ�
#define RX_ADDR_WIDTH 5	//���յ�ַ�������Ϊ5���ֽ�
#define TX_DATA_WIDTH 4//�������ݿ��4���ֽ�
#define RX_DATA_WIDTH 4//�������ݿ��4���ֽ�
/*******************����Ĵ���***************************/
#define  R_REGISTER      0x00//��ȡ���üĴ���
#define  W_REGISTER      0x20//д���üĴ���
#define  R_RX_PAYLOAD 	 0x61//��ȡRX��Ч����
#define  W_TX_PAYLOAD	 0xa0//дTX��Ч����
#define  FLUSH_TX		 0xe1//���TXFIFO�Ĵ���
#define  FLUSH_RX		 0xe2//���RXFIFO�Ĵ���
#define  REUSE_TX_PL     0xe3//����ʹ����һ����Ч����
#define  NOP             0xff//�ղ���
/******************�Ĵ�����ַ****************************/
#define  CONFIG          0x00//���üĴ���
#define  EN_AA			 0x01//ʹ���Զ�Ӧ��
#define  EN_RXADDR       0x02//����ͨ��ʹ��0-5��ͨ��
#define  SETUP_AW        0x03//��������ͨ����ַ���3-5
#define  SETUP_RETR      0x04//�����Զ��ط�
#define  RF_CH           0x05//��Ƶͨ������
#define  RF_SETUP        0x06//��Ƶ�Ĵ���
#define  STATUS          0x07//״̬�Ĵ���
#define  OBSERVE_TX      0x08//���ͼ��Ĵ���
#define  CD              0x09//�ز�
#define  RX_ADDR_P0      0x0a//����ͨ��0���յ�ַ
#define  RX_ADDR_P1      0x0b//����ͨ��1���յ�ַ
#define  RX_ADDR_P2      0x0c//����ͨ��2���յ�ַ
#define  RX_ADDR_P3      0x0d//����ͨ��3���յ�ַ
#define  RX_ADDR_P4      0x0e//����ͨ��4���յ�ַ
#define  RX_ADDR_P5      0x0f//����ͨ��5���յ�ַ
#define  TX_ADDR         0x10//���͵�ַ
#define  RX_PW_P0        0x11//P0ͨ�����ݿ������
#define  RX_PW_P1        0x12//P1ͨ�����ݿ������
#define  RX_PW_P2        0x13//P2ͨ�����ݿ������
#define  RX_PW_P3        0x14//P3ͨ�����ݿ������
#define  RX_PW_P4        0x15//P4ͨ�����ݿ������
#define  RX_PW_P5        0x16//P5ͨ�����ݿ������
#define  FIFO_STATUS     0x17//FIFO״̬�Ĵ���


/*******************��غ�������**************************/
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
