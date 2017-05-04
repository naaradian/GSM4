//   initialization waits for DCO to stabilize against ACLK.
//   ACLK = ~32.768kHz, MCLK = SMCLK = DCO ~ 1048kHz.  BRCLK = SMCLK/2

#include <msp430.h>
#include "deffs.h"

#ifdef SPI_DMA
unsigned char MST_Data,SLV_Data;

char SPI_RX_Buff0[SPI_RX_BUFF_SIZE];
char SPI_RX_Buff1[SPI_RX_BUFF_SIZE];
unsigned short spi_rcv_cnt;
char spi_dma_rxbuf_flag;

void spi_clear_buff(char * Buff, unsigned short len)
{
 //   printf("\n\r spi  len: %d ", len);
    while(len--)
  {
       SBAdd(*Buff);
   //t    printf(" %d ", *Buff);
       Buff++;

  }
}

void spi_dma_rcv(void)
{
    unsigned short len;
    unsigned short val;
   DMA2CTL &= ~(DMAEN); //170412
    val = DMA2SZ;
    DMA2CTL |= DMAEN;

    if(val < SPI_RX_BUFF_SIZE)
    {
    //set to enover buffer and use data
        DMA2CTL &= ~(DMAEN);

        if(!spi_dma_rxbuf_flag)
        {
            __data16_write_addr((unsigned short) &DMA2DA,(unsigned long) &SPI_RX_Buff1[0]);
            spi_dma_rxbuf_flag = 1;
        }
        else
        {
            __data16_write_addr((unsigned short) &DMA2DA,(unsigned long) &SPI_RX_Buff0[0]);
             spi_dma_rxbuf_flag = 0;
        }
        DMA2SZ = (unsigned short)SPI_RX_BUFF_SIZE;
        DMA2CTL |= DMAEN;
        len = (unsigned short)SPI_RX_BUFF_SIZE - val; //4 -for test

        spi_dma_rxbuf_flag ? spi_clear_buff(&SPI_RX_Buff0[0], len) : spi_clear_buff(&SPI_RX_Buff1[0], len);
       //   printf("\n\rdma: %d ", len);

     }
}
#endif

void spi_init(void)
{
printf("\n\r spi init");

  PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs  
  P1MAP3 = PM_UCB0SIMO;                     // Map UCB0SIMO  to P1.3
  P1MAP2 = PM_UCB0SOMI;                     // Map UCB0SOMI to P1.2
  P1MAP4 = PM_UCB0CLK;                      // Map UCB0CLK output to P1.4
  PMAPPWD = 0;                              // Lock port mapping registers  
   
  P1DIR |= BIT2; //BIT2;            // ACLK, MCLK, SMCLK set out to pins
  P1SEL |= BIT3 + BIT2 + BIT4;   // debugging purposes.

  UCB0CTL1 |= UCSWRST;  // **Put state machine in reset**

#ifdef MASTER
  UCB0CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    // 3-pin, 8-bit SPI master
#else
  UCB0CTL0 = UCSYNC+UCCKPL+UCMSB;               //slave
#endif
                                           // Clock polarity high, MSB
  UCB0CTL1 |= UCSSEL_2;                     // SMCLK
 //t UCB0BR0 = 0x02;                           // /2
  UCB0BR0 = 0;
  UCB0BR1 = 0;
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

#ifndef SPI_DMA
  UCB0IE |= UCRXIE;   //without this do not receive?
#endif

#ifdef SPISIML
   UCB0STAT |= UCLISTEN;  //temporary loop
#endif

#ifdef SPI_DMA
   DMACTL1 |= DMA2TSEL_18;                       //UCB0 receive ?
 //  DMACTL0 = DMA2TSEL_18;                       //UCB0 receive ?
   DMACTL4 |= ROUNDROBIN;

  __data16_write_addr((unsigned short) &DMA2SA,(unsigned long) &UCB0RXBUF);
                                             // Source block address
  __data16_write_addr((unsigned short) &DMA2DA,(unsigned long) &SPI_RX_Buff0[0]);

                                             // Destination single address
   DMA2SZ = (unsigned short)SPI_RX_BUFF_SIZE;                               // Block size
   spi_dma_rxbuf_flag = 0;
   spi_rcv_cnt = 0;
   DMA2CTL = DMADSTINCR_3+DMASBDB+DMALEVEL+DMADT_0+DMAEN;  // dst increment  ???
 //  DMA2CTL = DMADSTINCR_3+DMASBDB+DMALEVEL+DMADT_5+DMAEN;  // dst increment  ???

  MST_Data = 0x00;                          // Initialize data values
  SLV_Data = 0x00;                          //
#endif
}

char CheckCrc(char * Buff, int len)
{
 char ret = 0;
 int i;
 unsigned char crc= 0;
//printf("\n\r");
 for(i = 1; i < len - 2; i++)
 {
     crc+=Buff[i];
 //    printf(" %d", Buff[i]) ;
 }
 //printf(" %d", Buff[i]) ;
 crc -=1;
 if(Buff[len-2] == crc) {ret = 1;}
// else {
  //   printf( "\n\r Buff = %d  crc = %d \n\r" , Buff[len-2] , crc);
  // for(i = 1; i < len - 2; i++)
   //  {
    //      printf(" %d", Buff[i]);
    // }
    // printf(" %d", Buff[i]) ;
 // }

 return ret;
}

#ifdef SPISIML
void AddCrc(char * Buff, int len)
{
 int i;
 unsigned char crc= 0;
 for(i = 1; i < len - 2; i++)
 {
     crc+=Buff[i];
 }
 crc -=1;
 Buff[len-2] = crc;
}

void spi_transmit(void)
{
//   struct Controller_ControlFrame {
//       long dt, lat, lon, alt, course, speed; // 6*4 bytes
//   };
  char TestData1[] ={'$','0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','l','m','n','o','p','q','r','*'};
//  long dt = 1492420083;
  double lat = 59.934280;
  double lon = 30.335099;
//  long course = 12345678;
//  long speed = 87654321;
  dt++;
   memcpy(&TestData1[1], &dt, sizeof(long));
   memcpy(&TestData1[1 + sizeof(long)], &lat, sizeof(double));
   memcpy(&TestData1[1 + sizeof(long) + sizeof(double)], &lon, sizeof(double));
   spi_send(TestData1, ONLINE_DATA_SIZE);
}

void spi_send(char* TestData1, int size)
{
   int i;
   AddCrc( TestData1, size);

   UCB0TXBUF = TestData1[0];
   while (!(UCB0IFG&UCTXIFG));
   for(i = 1; i < size; i++) //t
   {
     while (!(UCB0IFG&UCTXIFG));
     switch(TestData1[i])
     {
     case '\\':  UCB0TXBUF = TestData1[i];
                  while (!(UCB0IFG&UCTXIFG));
                  __delay_cycles(40);
                  UCB0TXBUF = '0';  break;
     case '$' :   UCB0TXBUF = '\\';
                     while (!(UCB0IFG&UCTXIFG));
                   __delay_cycles(40);
                  UCB0TXBUF = '1';  break;
     default :
                 UCB0TXBUF = TestData1[i];                 // Send next value
     }
      __delay_cycles(40);
    }
}
#endif //SIML

#pragma vector=USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
//  static int cnt;
//  char tmp = UCB0RXBUF;
//  SBAdd(tmp);
    SBAdd(UCB0RXBUF);
//  if(tmp =='$' )   { printf("\n\r cnt =  %d >", cnt);   cnt = 0;   }
 //  printf (" %d ", tmp);
 //  cnt++;
}
