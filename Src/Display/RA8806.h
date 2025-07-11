//RA8806.h

//WG160160E5 display using the RA8806 controller 

#pragma once

#include "DisplayGPIO.h"
#include "IDisplay.h"
#include "Singleton.h"

// Display font size in bytes per character (vertical size of the font)
enum {
    FONTSIZE_Hn     = 8,
    FONTSIZE_Vn     = 16,

    DISP_Width      = 160,  //  количество точек по горизонтали
    DISP_Height     = 160, //  количество точек по вертикали

    DISP_Hn = DISP_Width / FONTSIZE_Hn,  //  полусимволов в строке
    ACC_MAX_LEVEL = 7,
    ACC_SIGN_PLACE_ROW = 0,
    ACC_SIGN_PLACE_COLUMN = 18
};

const char font[256][FONTSIZE_Vn]=     
// 4kb - 256 символов, каждый представлен матрицей
// из 16 байтов (16х8=128 бит)
{                        
            #include "8x16_3_my.h"
};
const char acc[ACC_MAX_LEVEL][FONTSIZE_Vn]=  
// 7 символов для аккумулятора, каждый представлен
// матрицей из 16 байтов (16х8=128 бит)
{                        
            #include "8x16_3_acc.h"
};


class RA8806 : public IDisplay, public Singleton<RA8806> {
  friend class Singleton<RA8806>;
  
public:
  
  
  virtual void init();           //      E5
  virtual void printAcc(uint8_t level);
  virtual display_error_code printnStr(uint8_t row,
                              uint8_t col, 
                              const char *s, 
                              uint8_t len = 1);
  
  virtual display_error_code printStr(uint8_t row,uint8_t col,const char *s);
  virtual void clrscr(void);
  
private: 
  RA8806(){}                   // private constructor
  
  void write_reg(uint8_t reg, uint8_t data);
  void put_command(uint8_t cmd);         //      6800    E5
  void put_data(uint8_t data);      //      6800    E
  void setcursor(uint16_t position);
  
};