//============================================================================
//       ������ ��� ������ � ����������� ����-������ AT45DB081 (Atmel)
//============================================================================

#include "AT45DB081.h"

// ��������� � ������ ����� � ���������� ��� ���������
#define bit2mask(bit) (1<<bit)
// ���������� ���
#define SetBit(Reg,Bit) Reg|=bit2mask(Bit)
// �������� ���
#define ClrBit(Reg,Bit) Reg&=~bit2mask(Bit)
// ��������� ���
#define RdBit(Reg,Bit)  Reg&bit2mask(Bit)




//===========================================================================
//                   ������� ��� ������ ������� (�������)
//===========================================================================


void AT45Init(){
  
  P3SEL |= 0xF0;     // Pin P3.4,P3.5,P3.6,P3.7 is used by USART module
  P3DIR = 0xFB;  //��� 2 = 0 ��� SOMI
  P3OUT = 0x0F;  //������� ������� - ��� AT45DB081
  U0CTL = CHAR;
                               
  U0TCTL = 0|TXEPT|SSEL0;          // ACLK=1 TXEPT=1 ���� ����������� �����������
  
  U0RCTL = 0;
  //U0BR0 = 52; //��� 19200 � 1MHz
  //U0BR0 = 104; //��� 9600 � 1MHz ��� 38400 � 4MHz
  //U0BR0 = 34; // ��� 115200 � 4MHz
  U0BR0 = 208; //��� 4800 � 1MHz  ��� 19200 � 4MHz
  U0BR1 = 0x00;
  
  U0MCTL=0x00;   // no modulation
  ME1 = 0|UTXE0|URXE0;             // enable transmit/receive  UART0
  U0CTL &= ~SWRST;                 // Initialize USART state machine
  IE1 |= URXIE0;                   // Enable USART0 RX interrupt
  //IE1 |= URXIE0 + UTXIE0;          // Enable USART0 RX/TX interrupt
}

//������ �������� �������
//-----------------------
char AT45Status()
{
int i;
char inb, outb=0;

  //������ ��������
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //��������� ��� �������
  inb = 0x57;
  //inb = 0xD7;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  //��������� ���� �������
  for(i=8;i>0;i--)
  {
    SetBit(AT45OUT, AT45_SCL); //SCL -> 1
    if(RdBit(AT45IN, AT45_SOMI)) outb |= 0x01;
    if(i>1) outb <<= 1; //��������� ��� (��������� �� ��������)
    ClrBit(AT45OUT, AT45_SCL); //SCL -> 0
  }
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); __delay_cycles(4);

  return outb; //���������� ���� �������
}

//�������� ���������� (����� ������ ��� ��������)
//-----------------------------------------------
void AT45Wait()
{
char status = 0;
  while((status & 0x80) == 0)
    status = AT45Status();
}

//������ ������� ���� �� �������� ������
//--------------------------------------
//baddr - ����� 1-�� ����� �� ��������
void AT45PageRead(unsigned paddr, char *buf, unsigned len, char baddr) 
{
int i;
char inb, outb;
unsigned val;
  //----- �������: ������ �������� - ��� 0x52 -----
  //������ ��������
  ClrBit(AT45OUT, AT45_CS); 
  __delay_cycles(4); 
  ClrBit(AT45OUT, AT45_SCL);

  //��������� ��� �������
  inb = 0x52;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  
//��������� 3 ������������ ����, ����� �������� (12 ���) � ������� ��� baddr (0)
  val = paddr << 1;
  val &= 0xFFFE; //������� ������� ��� baddr (9-�)
  for(i=16;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //��������� ���
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //�������
  }

  //��������� ��������� 8 ��� ������ ������ �� ��������)
  inb = baddr;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  
  //32 ������������ ���� (����� ���������� � ���������� ��������)
  for(i=32;i>0;i--){
    SetBit(AT45OUT,AT45_SCL);
    ClrBit(AT45OUT,AT45_SCL);
  }
  
  
  //������ ������� ����
  for(int j=len;j>0;j--)
  {
    outb = 0;
    for(i=8;i>0;i--)
    {
      SetBit(AT45OUT, AT45_SCL); //SCL -> 1
      if(RdBit(AT45IN, AT45_SOMI)) outb |= 0x01;
      if(i>1) outb <<= 1; //��������� ��� (��������� �� ��������)
      ClrBit(AT45OUT, AT45_SCL); //SCL -> 0
    }
    *buf++ = outb;
  }
  
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
}



//������ ������� ���� � �������� ������ ��� ��������
//---------------------------------------------------
//baddr - ����� 1-�� ����� �� ��������
void AT45PageWrite(unsigned paddr, char *buf, unsigned len, char baddr) 
{
int i;
char inb;
unsigned val;
char pbuf[256];

  
  //NEW!!!!!
  //=====================   
  for(int i=0; i<256; i++)  //�������������� �����
    pbuf[i] = 255;        
  
  for(int i=0; i<len; i++)  //��������� buf � pbuf, ������� � baddr
    pbuf[baddr+i] = buf[i];
  //=====================

  
  //----- �������: ������ � ����� - ��� 0x84 -----
  //������ ��������
  ClrBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
  ClrBit(AT45OUT, AT45_SCL);
  //��������� ��� �������
  inb = 0x84;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); 
    ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }

  //��������� ���� �� SIMO � 0
  ClrBit(AT45OUT, AT45_SIMO);
  //��������� 16 ������� : ��� �������
  for(i=16;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //����� �� �������� (������ ������)
  inb = 0; //DEBUG
  for(i=8;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  
  //�������� ������� ����
  //for(int j=len;j>0;j--)
  for(int j=0;j<256;j++)   //NEW!!!!! 
  {
    //inb = *buf++;
    inb = pbuf[j];  //NEW!!!!!
    for(i=8;i>0;i--)
    {
      if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
      else ClrBit(AT45OUT, AT45_SIMO);
      SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
      inb <<= 1; //��������� ���
    }
  }
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);
  __delay_cycles(4);
  //
  //------------------------------------------------------
  //----- �������: ������ ������ � ������ - ��� 0x88 ----- (��� ��������!!!)
  //------------------------------------------------------
  //������ ��������
  ClrBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
  ClrBit(AT45OUT, AT45_SCL);
  //��������� ��� �������
  inb = 0x88;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
//��������� 3 ������������ ����, ����� �������� (12 ���) � ������� ��� baddr (0)
  val = paddr << 1;
  val &= 0xFFFE; //������� ������� ��� baddr (9-�)
  for(i=16;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //��������� ���
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
  }
  //��������� ������� baddr
  ClrBit(AT45OUT, AT45_SIMO);
  for(i=8;i>0;i--){SetBit(AT45OUT, AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
}


//������ ������� ���� � �������� ������ �� ���������
//---------------------------------------------------
void AT45PageErWrite(unsigned paddr, char *buf, unsigned len)
{
int i;
char inb;
unsigned val;
  //----- �������: ������ �������� - ��� 0x82 -----
  //������ ��������
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //��������� ��� �������
  inb = 0x82;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  //��������� 3 ������������ ����
  for(i=3;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //��������� ����� �������� (12 ���)
  val = paddr << 4;
  for(i=12;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //��������� ���
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
  }
  //��������� 9 ������� ��� (������ ��������)
  ClrBit(AT45OUT, AT45_SIMO);
  for(i=9;i>0;i--){SetBit(AT45OUT, AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //�������� ������� ����
  for(int j=len;j>0;j--)
  {
    inb = *buf++;
    for(i=8;i>0;i--)
    {
      if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
      else ClrBit(AT45OUT, AT45_SIMO);
      SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
      inb <<= 1; //��������� ���
    }
  }
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS); 
  __delay_cycles(4);
}


//������� ��������
//------------------------
void AT45PageErase(unsigned paddr)
{
int i;
char inb;
unsigned val;
  //������ ��������
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //��������� ��� �������
  inb = 0x81;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  //��������� 3 ������������ ����
  for(i=3;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //��������� ����� �������� (12 ���)
  val = paddr << 4;
  for(i=12;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //��������� ���
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
  }
  //��������� 9 ������������ ���
  for(i=9;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);  
  __delay_cycles(4);
}


//������� ���� (8 �������)
//------------------------
void AT45BlockErase(unsigned blockaddr)
{
int i;
char inb;
unsigned val;
  //----- �������: ������� ���� - ��� 0x50 -----
  //������ ��������
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //��������� ��� �������
  inb = 0x50;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  //��������� 3 ������������ ����
  for(i=3;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //��������� ����� ����� (9 ���)
  val = blockaddr << 7;
  for(i=9;i>0;i--)
  {
    if(val & 0x8000) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    val <<= 1; //��������� ���
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
  }
  //��������� 12 ������������ ���
  for(i=12;i>0;i--){SetBit(AT45OUT,AT45_SCL);ClrBit(AT45OUT,AT45_SCL);}
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);  
  __delay_cycles(4);
}


//������� ��� ������
//------------------
void AT45ChipErase()
{
int i;
char inb;

  //----- �������: ������� ���� - ��� 0x50 -----
  //������ ��������
  ClrBit(AT45OUT, AT45_CS); __delay_cycles(4); ClrBit(AT45OUT, AT45_SCL);
  //��������� ��� �������
  inb = 0xC7;  //C7H, 94H, 80H and 9AH
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  inb = 0x94;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  inb = 0x80;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  inb = 0x9A;
  for(i=8;i>0;i--)
  {
    if(inb & 0x80) SetBit(AT45OUT, AT45_SIMO);
    else ClrBit(AT45OUT, AT45_SIMO);
    SetBit(AT45OUT, AT45_SCL); ClrBit(AT45OUT, AT45_SCL); //�������
    inb <<= 1; //��������� ���
  }
  //����� ��������
  SetBit(AT45OUT, AT45_SCL); 
  __delay_cycles(4);
  SetBit(AT45OUT, AT45_CS);  
  __delay_cycles(4);
}


