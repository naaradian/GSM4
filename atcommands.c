#include "deffs.h"

char OnlineData[ONLINE_DATA_SIZE];
char OnlineReady;

char at_command(char * Data,int tm , char type )
{
    BInit(); // prepare receive buffer
    dma_send(Data, strlen(Data));
#ifdef SIML
    delay(500);
    switch (type)
    {
    case 1 : sendtext("\n\rOK\n\r"); break;
    case 2 : sendtext("\n\rCONNECT OK\n\r"); break;
    case 3 : sendtext("\n\r>\n\r"); break;
    case 4 : sendtext("\n\rSEND OK\n\r"); break;
    case 6 : sendtext("\n\r 12,34,5678 OK\n\r"); break;

    }
#endif //SIML
    delay(tm);
    return dma_rcv(type);
}

char data_command(char * Data, int tm , char type, int len )
{
    BInit(); // prepare receive buffer
    dma_send(Data, len);
  #ifdef SIML
    delay(500);
    switch (type)
    {
    case 1 : sendtext("\n\rOK"); break;
    case 2 : sendtext("CONNECT OK"); break;
    case 3 : sendtext("\n\r>\n\r"); break;
    case 4 : sendtext("\n\rSEND OK\n\r"); break;
    }
#endif //SIML
    delay(tm);
    return dma_rcv(type);
}

char modem_send_data(char* Data, int len)  //big packet
{
      char SBUFF[] = {'A', 'T','+','C','I','P','S','E','N','D','=',' ',' ',' ','\r'}; //try send without ctrl-z

      sprintf(&SBUFF[11], "%d", len);
      if(len < 10) { SBUFF[12] = '\r';}
      else{
      if(len < 99) { SBUFF[13] = '\r';} else{ SBUFF[14] = '\r';}
      }

     // if(!at_command((char*)&SBUFF[0] ,5000, 3))
    if(!at_command((char*)&SBUFF[0] ,6000, 3))
    { printf("\n\r do not find >"); return 0;} //
   // printf("\n\r find >");
    data_command(Data,4000, 7, len); //3000!
    return 1;
}

char modem_start(char times)
{
    char auth[] = {9,'A','R','D','U','I','N','O','#','1',6,'M','S','P','4','3','0',2,'V','6', 0 , 26};  //addctrl-z && whu 2 ?
    while(times--)
    {
        at_command("ATE1\n\r" ,100, 1);
    }
        at_command("AT+IPR=9600\r " ,1000, 1); //set 9600
        at_command("AT+CMEE=1\r" ,100, 1); // return 2; //get code of error
        at_command("AT+CPIN?\r" ,100, 1); //return 5; //sim ready?
        at_command("AT+CREG?\r" ,500, 1);
        at_command("AT+CGATT?\r" ,500, 1); // return 8; //GPRS?
        at_command("AT+CIPSHUT\r" ,2000, 1); // return 9; //reconnect ????
        at_command("AT+CIPHEAD=1\r" ,100, 1); //return 11; //add header
        at_command("AT+CSTT=\"internet\",\"\",\"\"\r" ,5000, 1); // return 12; //run task ? name ok?
        at_command("AT+CIICR\r" ,5000, 1); // return 13; //connect gprs
        at_command("AT+CIFSR\r" ,5000, 1); //return 14; //get ip
        at_command("AT+CIPSTART=\"TCP\",\"regatav6.ru\",\"60009\"\r" ,5000, 2); // return 15; //get ip
        modem_send_data( auth, 21); //return 16;
        at_command("\n\rATE0\n\r" ,100, 1); //echo off



        return 0;
}

void SendData(void)
{
    if(sendtimer < SEND_TIME) return;
    sendtimer = 0;
    static char alarm_cnt = 0;
    char d;
    char TestData[] = {101, bcl};
 //   char TestData1[] = {1,'0','1','2','3','0','1','2','3','4','5','6','7','0','1','2','3','4','5','6','7', 26,0,0,0,0 };
    char TestData1[TX_SEND_SIZE]; //now 100

    int i;
    /*
    double lat = 59.934280;
    double lon = 30.335099;
    long dt = 1492420083;

    memcpy(&TestData1[1], &dt, sizeof(long));
    memcpy(&TestData1[1 + sizeof(long)], &lat, sizeof(double));
    memcpy(&TestData1[1 + sizeof(long) + sizeof(double)], &lon, sizeof(double));

    if(!modem_send_data( TestData1, 22) )
    {
        alarm_cnt++;
             if(alarm_cnt > ALARM_V)
             {
               modem_start(2);
               alarm_cnt = 0;
             }
    }
    else
    {
          alarm_cnt = 0;

    }
 */
 /*
     for(i = 0; i <22;i++)
     {
     printf("\n\r %d   %d   %d ", i, TestData1[i], OnlineData[i]);
     if(TestData1[i] != OnlineData[i]) printf("<---E ");
     }
 */

  if(OnlineReady)
    {
        memcpy(TestData1, OnlineData, SEND_DATA_SIZE);
        d = UseLifo(&TestData1[0],1);
   //     printf("\n\r");
   //     for(i = 0; i <30;i++)
   //       {
   //       printf(" %d", TestData1[i]);
    //      }

        if(!modem_send_data( TestData1, SEND_DATA_SIZE + (SEND_DATA_SIZE - 1) *d))
                                        {
                                   //      printf("\n\r Send GPS + Lifo Data To Modem Wrong \n\r");
                                            alarm_cnt++;
                                                 if(alarm_cnt > ALARM_V)
                                                 {
                                                   modem_start(TIMES);
                                                   alarm_cnt = 0;
                                                 }
                                        }
                                        else
                                        {
  //                                          printf("\n\r Send GPS + Lifo Data To Modem Ok \n\r");
                                              alarm_cnt = 0;
                                              OnlineReady = 0;
                                        }

    }  //onlineready
    else
    {
        if(Lifo_Used())
        {
            d = UseLifo(&TestData1[0],0);

       //     printf("\n\r");
       //     for(i = 0; i <30;i++)
        //      {
        //      printf(" %d", TestData1[i]);
         //     }

                      if(!modem_send_data( TestData1, 1 + (SEND_DATA_SIZE - 1) * d) )
                      {
                          alarm_cnt++;
                               if(alarm_cnt > ALARM_V)
                               {
                                 modem_start(TIMES);
                                 alarm_cnt = 0;
                               }
                      }
                      else
                      {
           //               printf("\n\r Send Lifo Data To Modem Ok \n\r");
                            alarm_cnt = 0;
                      }
        }//lifo used
          else
          {
              at_command("AT+CBC\r " ,1000, 6); //get battery charge
           //   if(!modem_send_data1( TestData, 2) )
              if(!modem_send_data( TestData, 2) )
              {
                  alarm_cnt++;
                  if(alarm_cnt > ALARM_V)
                  {
                      modem_start(TIMES);
                      alarm_cnt = 0;
                  }
              }
              else
              {
           //       printf("\n\r Send Battery Data To Modem Ok \n\r");
                  alarm_cnt = 0;
              }
         }
     }//olineready
}






