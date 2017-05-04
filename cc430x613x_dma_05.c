
#include <msp430.h>
#include "deffs.h"

void dma_send(char * Buff, unsigned short len)
{
    static char TxBuff[TX_SEND_SIZE];
    memcpy(TxBuff, Buff, len);
    DMA0CTL &=~(DMAEN);
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) &TxBuff);
    DMA0SZ = len;
    DMA0CTL |= DMAEN;
}
char dma_rcv(char type)
{
    unsigned short val;
    char ret = 1;
   DMA1CTL &= ~(DMAEN); //170412
   val = DMA1SZ;

    if(val < RX_BUFF_SIZE)
    {
      rlen = (unsigned short)RX_BUFF_SIZE - val; //4 -for test
      ret = ParseBuffer(type);
      __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) &RcvBuff[0]);
      DMA1SZ = (unsigned short)RX_BUFF_SIZE;
     }
    DMA1CTL |= DMAEN;
    return ret;
}


void dma_init(void)
{
  PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs  
  P1MAP5 = PM_UCA0RXD;                      // Map UCA0RXD output to P2.6  need p1.5
  P1MAP6 = PM_UCA0TXD;                      // Map UCA0TXD output to P2.7  need p1.6
  PMAPPWD = 0;                              // Lock port mapping registers 
  
  P1DIR |= BIT6;                            // Set P1.7 as TX output
  P1SEL |= BIT5 + BIT6;                     // Select P2.6 & P2.7 to UART function p1.5 p1.6


    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
    UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

#ifdef SIML
   UCA0STAT |= UCLISTEN;  //temporary loop
#endif


  DMACTL0 = DMA1TSEL_16+DMA0TSEL_17;        // DMA0 - UCA0TXIFG
                                            // DMA1 - UCA0RXIFG
  // Setup DMA0
                                             // Source block address
  __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) &UCA0TXBUF);
                                            // Destination single address
  DMA0SZ = 1;                               // Block size
  DMA0CTL = DMASRCINCR_3+DMASBDB+DMALEVEL;  // src increment   ???

  // Setup DMA1
  __data16_write_addr((unsigned short) &DMA1SA,(unsigned long) &UCA0RXBUF);
                                            // Source block address
  __data16_write_addr((unsigned short) &DMA1DA,(unsigned long) &RcvBuff[0]);

                                            // Destination single address
  DMA1SZ = (unsigned short)RX_BUFF_SIZE;                               // Block size
  DMA1CTL = DMADSTINCR_3+DMASBDB+DMALEVEL+DMADT_0+DMAEN;  // dst increment  ???
}
#ifdef SIML
void sendtext(char * Data)
{
    unsigned short len;
    char Buff[TX_SEND_SIZE];
    len = strlen(Data);
    memcpy(Buff, Data, len);
    dma_send( Buff, len);
}
#endif



