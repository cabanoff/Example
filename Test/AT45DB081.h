//AT45DB081.h

#pragma once
#include "main.h"

/***/
#define AT45IN  P3IN
#define AT45OUT P3OUT
#define AT45_CS   0
#define AT45_SIMO 1
#define AT45_SOMI 2
#define AT45_SCL  3
/***/


void AT45Init();
//ожидание готовности (после записи)
void AT45Wait();
//чтение массива байт из страницы памяти
void AT45PageRead(unsigned paddr, char *buf, unsigned len, char baddr = 0);
//запись массива байт в страницу памяти без стирания
//void AT45PageWrite(unsigned paddr, char *buf, unsigned len);
void AT45PageWrite(unsigned paddr, char *buf, unsigned len, char baddr = 0);
//запись массива байт в страницу памяти со стиранием
void AT45PageErWrite(unsigned paddr, char *buf, unsigned len);
//стереть блок
void AT45BlockErase(unsigned blockaddr);
//стереть страницу
void AT45PageErase(unsigned paddr);
//стереть всю память
void AT45ChipErase();

