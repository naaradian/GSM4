#include "deffs.h"

const char OK_STR[] = {'O','K', 0};

char RcvBuff[RCV_BUFF_SIZE];
unsigned short rlen;

void BInit()
{
   int i;
   for(i = 0; i < (RCV_BUFF_SIZE); i ++)
   {
     RcvBuff[i] = 0;
   }
   rlen = 0;
}

void parse_bcl(char* Buff)
 {
    Buff = strchr(Buff, ',');
    Buff++;
    bcl = (char)strtoul(Buff, NULL, 10);
//    printf("\n\r BCL = %d \n\r", bcl);
}


 char findchar(char s, char * Buff,int len)
 {
   //  printf("\n\r findchar len : %d" , len);
     int i;
     for(i = 0; i < len; i++)
     {
  //       printf("  ?%c(%d)",  Buff[i] ,  Buff[i]);
         if(s == Buff[i]) return 1;
     }
     return 0;
 }

 char ParseBuffer(char type)
 {
   char ret = 0;
   int i;
   if(rlen)
   {
    switch (type)
    {
    case 3 :// printf("\n\parse rrcv buf:");
            // for(i = 0; i < 40; i++ )
            // {printf(" %d:%c(%d) ",i, RcvBuff[i],RcvBuff[i]);}

           //  printf("\n\rrcv buf:%s",RcvBuff );
             if(findchar('>', RcvBuff,RCV_BUFF_SIZE)) ret = 1;//
             break;
    case 6 : //hier need parse battery
             parse_bcl(RcvBuff);
             if(strstr(RcvBuff,  OK_STR))
             ret = 1;
             break;
    default : ret = 1;
    } //switch

#ifdef DPRINT
   printf("\n\r ");
//____________________________________________for view  in variables
       for(i = 0; i < rlen; i ++)
        {
         if(type != 7)
         {
         printf("%c ", RcvBuff[i] );
         }
         else
         {
      //   printf(" %d:%c(%d) ", i,  Ans[i] ,  Ans[i] );
   //      printf(" %c  (%d) ", Ans[i] ,  Ans[i] );
         }
        } //for
#endif
 }//if
#ifdef DPRINT
//   printf("\n\rret = %d ", ret);
#endif
return ret;
}


