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
//�������� ���������� (����� ������)
void AT45Wait();
//������ ������� ���� �� �������� ������
void AT45PageRead(unsigned paddr, char *buf, unsigned len, char baddr = 0);
//������ ������� ���� � �������� ������ ��� ��������
//void AT45PageWrite(unsigned paddr, char *buf, unsigned len);
void AT45PageWrite(unsigned paddr, char *buf, unsigned len, char baddr = 0);
//������ ������� ���� � �������� ������ �� ���������
void AT45PageErWrite(unsigned paddr, char *buf, unsigned len);
//������� ����
void AT45BlockErase(unsigned blockaddr);
//������� ��������
void AT45PageErase(unsigned paddr);
//������� ��� ������
void AT45ChipErase();

