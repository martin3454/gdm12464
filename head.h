
#include "MKL25Z4.h"

void cekej (uint32_t delay_ms);
void Pin_Init();


//4094 FUNKCE SR
void SR_STR();
void SR_CLK();
void SR_L();
void SR_H();
void SR_Write_Data(uint8_t data);



void GLCD_Enable_H();
void GLCD_Enable_L();
void GLCD_RS_L();
void GLCD_RS_H();
void GLCD_EnableControler(uint8_t control);
void GLCD_DisableControler(uint8_t control);
void GLCD_WrCmd(uint8_t cmd, uint8_t control);
void GLCD_WrDat(uint8_t dat);
void GLCD_GoTo(uint8_t x, uint8_t y);
void GLCD_SetPixel(uint8_t x, uint8_t y);
void GLCD_ClearScreen();
void GLCD_Init();


void Draw_Line(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2);
void Draw_Obdelnik(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2);
void V_line(uint8_t x1, uint8_t y1, uint8_t y2);
void H_line(uint8_t x1, uint8_t x2, uint8_t y1);


//ddram
void ddram_init();
void Ddram_Clear();  //dodelat
uint8_t Read_Data();
void Ddram_Wr(uint8_t y, uint8_t x);
