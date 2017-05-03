#include "deffs.h"
#include <msp430.h>

char SRcvBuff[SRCV_BUFF_SIZE];
signed int SBuffSize;
signed int  SHead;
signed int STail;
signed int LastHead;
char fl_sl;

void SBInit()
{
    int i;
   SBuffSize = SRCV_BUFF_SIZE;  //have variable  sizes of buffer!!!!!!!
   SHead = 0l;
   STail = 0l;
   for(i = 0; i < (SRCV_BUFF_SIZE); i ++)
   {
     SRcvBuff[i] = 0;
   }
   LastHead = 0;
   fl_sl = 0;
}

void SBAdd(char elem)
{
  if(LastHead < SBuffSize)  LastHead ++;
   SRcvBuff[SHead] = elem;
  SHead++;
  if(SHead == SBuffSize) SHead = 0;
}

 char SBGet()
{
   if(LastHead)  LastHead --;
   STail++;    //t
    if(STail == SBuffSize)
   {
     STail = 0l;
     return SRcvBuff[SBuffSize-1];
   }
  else
  {
   return SRcvBuff[STail-1];   //t
  }
}

 char SBGetLast()
 {
    if(LastHead)  LastHead --;
    SHead--;    //t
     if(SHead  < 0)
    {
      SHead = SBuffSize - 1;
      return SRcvBuff[SBuffSize-1];
    }

   else
   {
    return SRcvBuff[SHead];   //t
   }
 }



 unsigned SBUsed()
{
  int n = SHead - STail;
  if( n >= 0 ) return (unsigned)n;
  else return (unsigned)(n+SBuffSize);
}

 unsigned SBUsedLast()
 {
  return (unsigned)(LastHead);
 }

 unsigned char Modify(unsigned char byte)
 {
     unsigned char ret;
     switch(byte)
     {
         case '0': ret = '\\' ; break;
         case '1': ret = '$'; break;
     }
  //   printf("\n\r Modify ret = %c ", ret);
     return ret;
 }



 char SParseBuffer(char type)
 {
   char ret = 0;
   static int cnt;
   char b;
   static char Ans[ONLINE_DATA_SIZE];
   while( SBUsed())
        {
          b = SBGet();
         switch(b)
         {
         case '\\' : fl_sl = 1; break;
         case '$':  cnt = 0;  Ans[cnt++] = 1; break; //set first element to 1
         case '*': // printf("\n\r cnt* = %d ",cnt );
             if (cnt == ONLINE_DATA_SIZE-1)
             {
               if(CheckCrc(Ans, ONLINE_DATA_SIZE))
               {
                 ret =1;
                 if (!OnlineReady)
                 {
                     memcpy(OnlineData,Ans, ONLINE_DATA_SIZE);
                     OnlineReady = 1;
                 }
                 else
                 {/* send data to lifo  lifoready = 1 */
                    Lifo_Add(Ans);
                 }
               } //crc
               else
               {
                  printf("\n\r crc wrong");
               } //crc
               cnt = 0;
             } //cnt =...
             else
             {
                 Ans[cnt++] = b;
             }
             break;
         default : if(cnt < ONLINE_DATA_SIZE)
         {
           if(!fl_sl) { Ans[cnt++] = b;    /*  printf(" Ans[%d] = %c%d ", cnt, b, b); */}
           else { Ans[cnt++] = Modify(b); fl_sl = 0;}
         }
         } //switch
//#ifdef DPRINT
     //    if(b == '$')
     //    {
         //    printf("\n\rsb>");
      //   }
     //    printf(" %d:%c(%d) ",cnt, Ans[cnt] , Ans[cnt] );
//#endif
       //  cnt++;
     } //while
  return ret;
}


