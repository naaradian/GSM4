
#include <msp430.h>
#include "deffs.h"

unsigned int t1;
unsigned int spitimer;
unsigned int sendtimer;
char bcl;
long dt;

void params_init()
{
    dt = 1492420083;
    OnlineReady = 0;
}

void int_init()
{
    WDTCTL = WDT_ADLY_16;                   // WDT 16ms, ACLK, interval timer
    SFRIE1 |= WDTIE;                          // Enable WDT interrupt
    t1 = 0;
    spitimer = 0;
    sendtimer = 0;
     __bis_SR_register(GIE);
}

void spi_task(void)
{
    static char is_spi_ok = 0;
    if(spitimer < SPI_TIME) return;
    spitimer = 0;
#ifdef SPISIML
    spi_transmit(); //t
#endif

#ifdef SPI_DMA
   spi_dma_rcv();
#endif

   if(SParseBuffer(0))
   {is_spi_ok = 0;}
   else{is_spi_ok++;}
   if(is_spi_ok > WD_SPI)
   {spi_init();  is_spi_ok = 0;  }
}

void delay(unsigned short del) // min 16 ms
{
    volatile unsigned int i = t1 + del;    //
    if(i < t1)
    {
        while(i < t1)
          {
            spi_task();
          }
    }
    else
    {
        while(i > t1)
           {
            spi_task();
           }
    }
}

void init(void)
{
    BInit();
    params_init();
    SBInit();
    dma_init();
    spi_init();
    int_init();
     modem_start(TIMES);
}

void polling(void)
{
    spi_task();
    SendData();
}

int main(void)
{
    init();
	while(1)
	{
	 polling();
	}
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void) //16 ms
{
   t1 += 16;
   spitimer  += 16;
   sendtimer += 16;
}
