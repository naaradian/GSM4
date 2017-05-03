#include "deffs.h"

struct rec
{
    char body[MSG_LEN - 1]; //do not use first symbol
};

struct GSM_LIFO
{
   struct rec Buff[LIFO_SIZE];
};

struct GSM_LIFO lifo;
unsigned int  LHead;
unsigned int LLastHead;

void LBInit()
{
   LHead = 0;
   LLastHead = 0;
}

void Lifo_Add(char * d)
{
    int i;
     for(i = 0; i < MSG_LEN - 1; i++)
    {
        lifo.Buff[LHead].body[i] = d[i + 1];  //first symbol is ot need
    }
    LHead++;
    if(LHead >= LIFO_SIZE) LHead = 0;
    if(LLastHead < LIFO_SIZE) LLastHead++;
}

unsigned int Lifo_Used(void)
{
  return LLastHead;
}

void Lifo_Get(char * d)
{
    int i;
    if(LHead == 0) LHead = LIFO_SIZE - 1;
    else LHead--;
    if(LLastHead > 0) LLastHead--;
    for(i = 0; i < SEND_DATA_SIZE - 1; i++) //first symbol do not used
    {
       d[i] = lifo.Buff[LHead].body[i];
    }
 }

char UseLifo(char *Target, char flag)
{
    char ret = 0;
    char vol = (char)MAX_GET_LiFO - flag;
    if(!flag)  //sended lifo data only
    {
    while(vol)
    {
        if(Lifo_Used())
        {
            Lifo_Get(&Target[1 + (SEND_DATA_SIZE - 1) * ret]);  //do not fill first symbols
            ret++; vol--;
        }
        else break;
    }
    Target[0] = ret;
    }
    else  //flag == 1 send gps + lifo
    {
        while(vol)
          {
              if(Lifo_Used())
              {
                  Lifo_Get(&Target[SEND_DATA_SIZE + (SEND_DATA_SIZE - 1) * ret]);  //do not fill first symbols
                  ret++; vol--;
              }
              else break;
          }
    Target[0] = ret + 1;
    }
//  printf("\n\r Lifo ret = %d ", ret);
 return ret;
}

/*
char Lifo_Test(void)
{
    char TBuff[MSG_LEN];
    char ret = 1;
    int i,j;

    for(j = 0; j < LIFO_SIZE; j ++)
    {
    for(i = 0; i < MSG_LEN; i ++)
    {
        TBuff[i] = i + j;
    }
    Lifo_Add(TBuff);
    }

    for(j = 0; i < LIFO_SIZE; i ++)
     {
        Lifo_Get(TBuff);
        printf("\n\r");
     for(i = 0; i < MSG_LEN; i ++)
     { printf(" %d ", TBuff[i]);

         if(TBuff[i] != (i + LIFO_SIZE - 1 - j))
         {
      //       printf("\n\r %d != %d", TBuff[i], i+j );
             ret = 0;
         }
     }
    }
  if(ret)
  {
      printf("\n\r Lifo Test Ok");
  };
  return ret;
}

*/











