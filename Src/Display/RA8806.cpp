//RA8806.cpp

//WG160160E5 display using the RA8806 controller 

#include "RA8806.h"



display_error_code RA8806::printnStr(uint8_t row, 
                                     uint8_t col, 
                                     const char *s, 
                                     uint8_t len)
{
  if((row > DISP_Height/FONTSIZE_Vn - 1)||
     (col + len > DISP_Width/FONTSIZE_Hn)){
       printnStr(row, 0, "  OUT EXCEEDS SIZE", 18);
       return EXCEEDS_SIZE; 
  }
  if((row == ACC_SIGN_PLACE_ROW)&&
     (col + len > ACC_SIGN_PLACE_COLUMN)){
       printnStr(row, 0, "GOES TO ACC SIGN", 16);
       return EXCEEDS_SIZE; 
  }
     
  uint16_t pos = (DISP_Hn*FONTSIZE_Vn)*row + col;

  write_reg(0x12,0x91);

  for (int cntr = 0;cntr < len;cntr++) 
  {
    char *fontbuf;
    char c = s[cntr];
    //convert windows-1251 into IBM866
    if (c >= 0xF0) c -= 0x10;
    else if (c >= 0xC0) c -= 0x40;
    fontbuf = (char *)font[c];
    setcursor(pos);    
    for (int i=0; i<FONTSIZE_Vn; i++)put_data(*fontbuf++);
    pos++;
  }
  return DISPLAY_OUTPUT_OK;
}

void RA8806::printAcc(uint8_t level){
  if(!(level < ACC_MAX_LEVEL))level = ACC_MAX_LEVEL;
  uint16_t pos = (DISP_Hn*FONTSIZE_Vn)*ACC_SIGN_PLACE_ROW \
                 + ACC_SIGN_PLACE_COLUMN;
  write_reg(0x12,0x91);
  char *accbuf = (char *)acc[level];
  setcursor(pos); 
  for (int i = 0; i < FONTSIZE_Vn; i++)put_data(*accbuf++);  
}

display_error_code RA8806::printStr(uint8_t row,uint8_t col,const char *s){ 
  
  uint8_t max_len = DISP_Width / FONTSIZE_Hn;
  uint8_t len = 0;
  const char *p = s; // Temporary pointer to traverse the string
  
  // Count characters until null terminator or max_len
  while (*p != '\0' && len < max_len) {
      len++;
      p++;
  }
  // Pass the ORIGINAL string (s) and valid length (len)
  return printnStr(row, col, s, len);
}



void RA8806::init()            //      E5
{
  DisplayGPIO::instance().init(); 
  
  write_reg(0x00,0x00);       //REGISTER 00H     
  write_reg(0x01,0x04); //work in active window,12MHz clock  
  write_reg(0x03,0x08);    //      inverse bit order
  //TEXT FONT BOLD,DATA TO DDRAM,ALIGN OFF
  //CURSOR AUTO INCREASE AS WRITE,//CURSOR FLASH ON
  write_reg(0x10,0x00);
  write_reg(0x11,0x00); //ra8822  CURSOR HEIGHT(4-7) 2 PIXEL  
  //ONLY PAGE1 
  write_reg(0x12,0x91);   //ra8822 CURSOR HEIGHT(4-7) 2 PIXEL   ;MAMR
  write_reg(0x20,0x13);    //30(*8) SEGMENT(0~29)
  write_reg(0x30,0x9f); //64 COMMON
  write_reg(0x40,0x00); //DISPLAY LEFT SET;
  write_reg(0x50,0x00); //DISPLAY TOP SET
  write_reg(0x21,0x13); //ACTIVE RIGHT DISPLAY SET
  write_reg(0x31,0x9f);  //ACTIVE BOTTOM DISPLAY SET
  write_reg(0x41,0x00);  //ACTIVE LEFT SIDE
  write_reg(0x51,0x00);  //ACTIVE TOP SIDE
  write_reg(0x60,0x00);  //SET Cursor SEGMENT ADDRESS
  write_reg(0x70,0x00);  //SET Cursor COMMON ADDRESS
  write_reg(0x80,0x33);  //SET Blink Time
  write_reg(0x90,0x04);   //FRAME RATE SET(reg:90h)//70Hz 12MHZ 160*160
  write_reg(0xf0,0x00);
  write_reg(0x60,0x00); //SET Cursor SEGMENT ADDRESS
  write_reg(0x70,0x00); //SET Cursor COMMON ADDRESS 
  
  put_command(0xB0);
  for(int i=0; i<20*160; i++){
      put_data(0);
     __delay_cycles(10);
  }          
  write_reg(0x00,0x04);  //   turn ON 
  write_reg(0x60,0x00); //SET Cursor SEGMENT ADDRESS
  write_reg(0x70,0x00); //SET Cursor COMMON ADDRESS  
}

void RA8806::write_reg(uint8_t reg, uint8_t data){
  put_command(reg);
  put_data(data);
}

void RA8806::put_command(uint8_t cmd)         //      6800    E5
{
  DisplayGPIO& dispGPIO = DisplayGPIO::instance();
  __no_operation();
  dispGPIO.setE(false); //LCD_E_0; 
  dispGPIO.setRS(false); //LCD_RS_0;
  __no_operation(); //setRS(false); //LCD_RS_0; 
  dispGPIO.setRS(true); //LCD_RS_1;
  dispGPIO.setRW(false); //LCD_RW_0;
  __delay_cycles(10);
  dispGPIO.setData(cmd); // LCD_PORT_DATA = cmd;
  __delay_cycles(10);
  dispGPIO.setE(true); // LCD_E_1;
  __no_operation();
  __no_operation();
  dispGPIO.setE(false); //LCD_E_0;
  dispGPIO.setRS(false); //LCD_RS_0;
  __no_operation();
} 

void RA8806::put_data(uint8_t data)        //      6800    E5
{
  DisplayGPIO& dispGPIO = DisplayGPIO::instance();
  __no_operation();
  dispGPIO.setE(false); //LCD_E_0;
  dispGPIO.setRS(true); //LCD_RS_1; 
  __no_operation(); //LCD_RS_1; 
  dispGPIO.setRS(false); //LCD_RS_0;
  dispGPIO.setRW(false); //LCD_RW_0;   
  __delay_cycles(10);
  dispGPIO.setData(data); //LCD_PORT_DATA = data;
  __delay_cycles(10);
  dispGPIO.setE(true); //LCD_E_1;
  __no_operation();
  __no_operation(); 
  dispGPIO.setE(false); //LCD_E_0;
  dispGPIO.setRS(true); //LCD_RS_1;
  __no_operation();  
}

void RA8806::setcursor(uint16_t position)
{
   uint16_t LCD_curs_x = position % DISP_Hn;
   uint16_t LCD_curs_y = (position - LCD_curs_x)/DISP_Hn; 
  
   write_reg(0x60,LCD_curs_x); //SET Cursor SEGMENT ADDRESS
   write_reg(0x70,LCD_curs_y); //SET Cursor COMMON ADDRESS
   
   put_command(0xB0);
}

void RA8806::clrscr(void)
{
  setcursor(0);
  __delay_cycles(10);
      
  for(int i=0; i<20*160; i++){
      put_data(0);
      __delay_cycles(10);
  }
}