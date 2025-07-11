//============================================================================
//       Модуль для работы с микросхемой флэш-памяти AT45DB081 (Atmel)
//============================================================================

#include "AT45DB081.h"

// установка и сбросы битов в переменных или регистрах
#define bit2mask(bit) (1<<bit)
// Установить бит
#define SetBit(Reg,Bit) Reg|=bit2mask(Bit)
// Сбросить бит
#define ClrBit(Reg,Bit) Reg&=~bit2mask(Bit)
// Прочитать бит
#define RdBit(Reg,Bit)  Reg&bit2mask(Bit)




//===========================================================================
//                   Вариант без вызова функций (быстрее)
//===========================================================================


void AT45Init(){
  
  P3SEL |= 0xF0;     // Pin P3.4,P3.5,P3.6,P3.7 is used by USART module
  P3DIR = 0xFB;  //бит 2 = 0 для SOMI
  P3OUT = 0x0F;  //младшая тетрада - для AT45DB081
  U0CTL = CHAR;
                               
  U0TCTL = 0|TXEPT|SSEL0;          // ACLK=1 TXEPT=1 флаг опустошения передатчика
  
  U0RCTL = 0;
  //U0BR0 = 52; //для 19200 и 1MHz
  //U0BR0 = 104; //для 9600 и 1MHz для 38400 и 4MHz
  //U0BR0 = 34; // для 115200 и 4MHz
  U0BR0 = 208; //для 4800 и 1MHz  для 19200 и 4MHz
  U0BR1 = 0x00;
  
  U0MCTL=0x00;   // no modulation
  ME1 = 0|UTXE0|URXE0;             // enable transmit/receive  UART0
  U0CTL &= ~SWRST;                 // Initialize USART state machine
  IE1 |= URXIE0;                   // Enable USART0 RX interrupt
  //IE1 |= URXIE0 + UTXIE0;          // Enable USART0 RX/TX interrupt
}

//чтение регистра статуса
//-----------------------
char AT45Status()
{
int i;
char inb, outb=0;

  //начало операции
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //задвигаем код команды
  inb = 0x57;
  //inb = 0xD7;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  //считываем байт статуса
  for(i=8;i>0;i--)
  {
    SetBit(AT45OUT, AT45_SCL); //SCL -> 1
    if(RdBit(AT45IN, AT45_SOMI)) outb |= 0x01;
    if(i>1) outb <<= 1; //следующий бит (последний не сдвигаем)
    ClrBit(AT45OUT, AT45_SCL); //SCL -> 0
  }
  //конец операции
  SetBit(AT45OUT, AT45_SCL); __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); __delay_cycles(4);

  return outb; //возвращаем байт статуса
}

//ожидание готовности (после записи или стирания)
//-----------------------------------------------
void AT45Wait()
{
char status = 0;
  while((status & 0x80) == 0)
    status = AT45Status();
}

//чтение массива байт из страницы памяти
//--------------------------------------
//baddr - адрес 1-го байта на странице
void AT45PageRead(unsigned paddr, char *buf, unsigned len, char baddr) 
{
int i;
char inb, outb;
unsigned val;
  //----- команда: чтение страницы - код 0x52 -----
  //начало операции
  ClrBit(AT45OUT, AT45_CS); 
  __delay_cycles(4); 
  ClrBit(AT45OUT, AT45_SCL);

  //задвигаем код команды
  inb = 0x52;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  
//задвигаем 3 произвольных бита, адрес страницы (12 бит) и старший бит baddr (0)
  val = paddr << 1;
  val &= 0xFFFE; //обнулим старший бит baddr (9-й)
  for(i=16;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //следующий бит
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //импульс
  }

  //задвигаем остальные 8 бит адреса начала на странице)
  inb = baddr;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  
  //32 произвольных бита (можно объединить с предыдущим задвигом)
  for(i=32;i>0;i--){
    SetBit(AT45OUT,AT45_SCL);
    ClrBit(AT45OUT,AT45_SCL);
  }
  
  
  //чтение массива байт
  for(int j=len;j>0;j--)
  {
    outb = 0;
    for(i=8;i>0;i--)
    {
      SetBit(AT45OUT, AT45_SCL); //SCL -> 1
      if(RdBit(AT45IN, AT45_SOMI)) outb |= 0x01;
      if(i>1) outb <<= 1; //следующий бит (последний не сдвигаем)
      ClrBit(AT45OUT, AT45_SCL); //SCL -> 0
    }
    *buf++ = outb;
  }
  
  //конец операции
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
}



//запись массива байт в страницу памяти без стирания
//---------------------------------------------------
//baddr - адрес 1-го байта на странице
void AT45PageWrite(unsigned paddr, char *buf, unsigned len, char baddr) 
{
int i;
char inb;
unsigned val;
char pbuf[256];

  
  //NEW!!!!!
  //=====================   
  for(int i=0; i<256; i++)  //подготавливаем буфер
    pbuf[i] = 255;        
  
  for(int i=0; i<len; i++)  //вставляем buf в pbuf, начиная с baddr
    pbuf[baddr+i] = buf[i];
  //=====================

  
  //----- команда: запись в буфер - код 0x84 -----
  //начало операции
  ClrBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
  ClrBit(AT45OUT, AT45_SCL);
  //задвигаем код команды
  inb = 0x84;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }

  //установка бита на SIMO в 0
  ClrBit(AT45OUT, AT45_SIMO);
  //задвигаем 16 разряда : все нулевые
  for(i=16;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //адрес на странице (начало записи)
  inb = 0; //DEBUG
  for(i=8;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  
  //передача массива байт
  //for(int j=len;j>0;j--)
  for(int j=0;j<256;j++)   //NEW!!!!! 
  {
    //inb = *buf++;
    inb = pbuf[j];  //NEW!!!!!
    for(i=8;i>0;i--)
    {
      if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
      else ClrBit(AT45OUT, AT45_SIMO);
      SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
      inb <<= 1; //следующий бит
    }
  }
  //конец операции
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);
  __delay_cycles(4);
  //
  //------------------------------------------------------
  //----- команда: запись буфера в память - код 0x88 ----- (без стирания!!!)
  //------------------------------------------------------
  //начало операции
  ClrBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
  ClrBit(AT45OUT, AT45_SCL);
  //задвигаем код команды
  inb = 0x88;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
//задвигаем 3 произвольных бита, адрес страницы (12 бит) и старший бит baddr (0)
  val = paddr << 1;
  val &= 0xFFFE; //обнулим старший бит baddr (9-й)
  for(i=16;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //следующий бит
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
  }
  //задвигаем нулевой baddr
  ClrBit(AT45OUT, AT45_SIMO);
  for(i=8;i>0;i--){SetBit(AT45OUT, AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //конец операции
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
}


//запись массива байт в страницу памяти со стиранием
//---------------------------------------------------
void AT45PageErWrite(unsigned paddr, char *buf, unsigned len)
{
int i;
char inb;
unsigned val;
  //----- команда: запись страницы - код 0x82 -----
  //начало операции
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //задвигаем код команды
  inb = 0x82;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  //задвигаем 3 произвольных бита
  for(i=3;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //задвигаем адрес страницы (12 бит)
  val = paddr << 4;
  for(i=12;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //следующий бит
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
  }
  //задвигаем 9 нулевых бит (начало страницы)
  ClrBit(AT45OUT, AT45_SIMO);
  for(i=9;i>0;i--){SetBit(AT45OUT, AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //передача массива байт
  for(int j=len;j>0;j--)
  {
    inb = *buf++;
    for(i=8;i>0;i--)
    {
      if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
      else ClrBit(AT45OUT, AT45_SIMO);
      SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
      inb <<= 1; //следующий бит
    }
  }
  //конец операции
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
}


//стереть страницу
//------------------------
void AT45PageErase(unsigned paddr)
{
int i;
char inb;
unsigned val;
  //начало операции
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //задвигаем код команды
  inb = 0x81;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  //задвигаем 3 произвольных бита
  for(i=3;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //задвигаем адрес страницы (12 бит)
  val = paddr << 4;
  for(i=12;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //следующий бит
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
  }
  //задвигаем 9 произвольных бит
  for(i=9;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //конец операции
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);  
  __delay_cycles(4);
}


//стереть блок (8 страниц)
//------------------------
void AT45BlockErase(unsigned blockaddr)
{
int i;
char inb;
unsigned val;
  //----- команда: стереть блок - код 0x50 -----
  //начало операции
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //задвигаем код команды
  inb = 0x50;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  //задвигаем 3 произвольных бита
  for(i=3;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //задвигаем адрес блока (9 бит)
  val = blockaddr << 7;
  for(i=9;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //следующий бит
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
  }
  //задвигаем 12 произвольных бит
  for(i=12;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //конец операции
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);  
  __delay_cycles(4);
}


//стереть всю память
//------------------
void AT45ChipErase()
{
int i;
char inb;

  //----- команда: стереть блок - код 0x50 -----
  //начало операции
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //задвигаем код команды
  inb = 0xC7;  //C7H, 94H, 80H and 9AH
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  inb = 0x94;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  inb = 0x80;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  inb = 0x9A;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //импульс
    inb <<= 1; //следующий бит
  }
  //конец операции
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);  
  __delay_cycles(4);
}


