/*
 * deffs.h
 *
 *  Created on: 6 апр. 2017 г.
 *      Author: user
 */
#ifndef DEFFS_H_
#define DEFFS_H_

#include <string.h>
#include <stdlib.h>

#define DPRINT

#ifdef DPRINT
#include <stdio.h>
#endif

//#define SIML   //modem channel
//#define SPISIML //spi channel
//#define SPI_DMA

#ifdef SPISIML
#define MASTER
#endif

#define SEND_DATA_SIZE   (21)  //size for server now
//#define SEND_DATA_SIZE   (3)

#define TX_SEND_SIZE     (SEND_DATA_SIZE * 10)
#define ANS_BUFF_SIZE    (TX_SEND_SIZE)
#define RCV_BUFF_SIZE    (TX_SEND_SIZE)
#define RX_BUFF_SIZE     (TX_SEND_SIZE)
#define MAX_GET_LiFO     TX_SEND_SIZE / SEND_DATA_SIZE
#define SPI_RX_BUFF_SIZE (100)
#define SRCV_BUFF_SIZE   (100)
#define ONLINE_DATA_SIZE (27) //size of one message in future format with $ and *
#define MSG_LEN          (ONLINE_DATA_SIZE)
#define LIFO_SIZE        (60)
//#define LIFO_SIZE        (4)

#define WD_SPI  (5)

extern char OnlineReady;
extern char OnlineData[ONLINE_DATA_SIZE];
extern void BInit();
extern char ParseBuffer(char);
extern unsigned short rlen;
extern char RcvBuff[RCV_BUFF_SIZE];

extern void SBAdd(char);
extern void SBInit(void);

extern void dma_init(void);
extern void dma_send(char * , unsigned short );
extern char dma_rcv(char);
extern unsigned int t1;
extern unsigned int sendtimer;
extern void sendTextMessage(void);
extern char at_command(char * ,int , char);
extern char data_command(char *,int , char, int);
extern void setEchoMode(void);
extern void delay(unsigned short); // min 16 ms
extern void sendtext(char *);
extern void senddata(char);
extern void setTCPClientConnection(void);
void getID(void);
extern char modem_start(char);
extern char modem_init(void);
extern char modem_send_data(char*, int);
extern void SendData(void);
#define TIMES    (2)
#define ALARM_V  (2)
#define SEND_TIME (1000)
//#define SPI_TIME  (1000)
#define SPI_TIME  (500)

extern void spi_init(void);
extern void spi_receive(void);
extern void spi_transmit(void);
extern void spi_send(char* ,int);
extern void spi_dma_rcv(void);
extern char SParseBuffer(char);
extern char CheckCrc(char *, int);
extern unsigned int spitimer;
extern char bcl;
extern char fl_sl;
extern long dt;

extern void Lifo_Add(char *);
unsigned int Lifo_Used(void);
extern void Lifo_Get(char *);
char Lifo_Test(void);
char UseLifo(char *, char);

#endif /* DEFFS_H_ */
