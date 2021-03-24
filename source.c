
#include "MKL25Z4.h"
#include <header.h>


#define RS 		(1<<11)
#define E 		(1<<10)
#define CS1 	(1<<8)
#define CS2 	(1<<9)
#define RST 	(1<<12) 		//PTA
#define CLK 	(1<<17)
#define DAT 	(1<<31)			//PTE
#define STR 	(1<<12)


#define SET_X 		0x40
#define SET_Y 		0xB8
#define START_LINE 	0xc0
#define DIS_ON 		0x3f



uint8_t ddram[64][128];

uint8_t curX, curY;


void ddram_init(){
	memset(ddram,0,sizeof(ddram));
}


void Ddram_Wr(uint8_t x, uint8_t y){
	ddram[x][y]=1;
}

/*uint8_t Read_Data(){

	uint8_t index=0, temp=0;
	uint8_t krok=curY*8;

	for(uint8_t j=krok; j < krok + 8; j++){

		if(ddram[j][curX]) temp |= (1<<index++);
	}
	return temp;
}*/

uint8_t Read_Data(){

	uint8_t temp=0;
	uint8_t krok=curY*8;

	for(uint8_t j=krok; j < krok + 8; j++){

		if(ddram[j][curX]) temp |= (1<< j % 8);
	}
	return temp;
}




void GLCD_Init(){

	GLCD_WrCmd(SET_X, 0);		// 0xC0 => Start Line
	GLCD_WrCmd(SET_Y, 0);				// 0x40 => Set Y address
	GLCD_WrCmd(START_LINE, 0);// 0xB8 => Set X address
	GLCD_WrCmd(DIS_ON, 0);				// 0x40 => Set Y address

	GLCD_WrCmd(SET_X, 1);		// 0xC0 => Start Line
	GLCD_WrCmd(SET_Y, 1);				// 0x40 => Set Y address
	GLCD_WrCmd(START_LINE, 1);    // 0xB8 => Set X address
	GLCD_WrCmd(DIS_ON, 1);

	ddram_init();

	curX=curY=0;
}

void GLCD_ClearScreen(){

	uint8_t i,j;

	for(i=0;i<64;i++){

		GLCD_GoTo(0, i/8);

		for(j=0;j<128;j++){

			GLCD_WrDat(0x00);
		}
	}
}


/*void GLCD_ClearScreen(){

	PTC->PSOR = CS2;
	PTC->PSOR = CS1;
	for(int i=0;i<8;i++){
		GLCD_Write_Cmd(0x40); //Y adress counter
		GLCD_Write_Cmd(0xb8 + i); //X page adress
		for(int j=0;j<0x40;j++)   //64
			GLCD_Write_Data(0x00);
	}
}*/

void GLCD_GoTo(uint8_t x, uint8_t y){

	curX=x;
	curY=y;
	//cyklus init....
	for(int i=0;i<2;i++){

		GLCD_WrCmd(SET_X | 0, i);
		GLCD_WrCmd(SET_Y | y, i);
		GLCD_WrCmd(START_LINE | 0, i);
	}

	GLCD_WrCmd(SET_X | (x % 64), (x / 64 ));
	GLCD_WrCmd(SET_Y | y, (x / 64 ));

}

//KRESLICI FUNKCE

void GLCD_SetPixel(uint8_t x, uint8_t y){

	uint8_t temp=0;
	Ddram_Wr(y,x);
	GLCD_GoTo(x, y/8);
	temp=Read_Data();
	//temp=(temp==1) ? 0 : temp;  //toto aby nebyly dvojte cary
	//temp |= (1 << y % 8);
	cekej(1);
	GLCD_WrDat(temp);

}

void H_line(uint8_t x1, uint8_t x2, uint8_t y1){

	for(uint8_t j=x1; j<=x2; j++){
		GLCD_SetPixel(j, y1);
		cekej(1);
	}
}

void V_line(uint8_t x1, uint8_t y1, uint8_t y2){

	for(uint8_t j=y1; j<=y2; j++){
		GLCD_SetPixel(x1, j);
		cekej(1);
	}
}

void GLCD_Obdelnik(uint8_t x, uint8_t y, uint8_t a, uint8_t b){

	uint8_t j;
	for (j = 0; j < b; j++) {
		GLCD_SetPixel(x, y + j);
		GLCD_SetPixel(x + a - 1, y + j);
	}

	for (j = 0; j < a; j++)	{
		GLCD_SetPixel(x + j, y);
		GLCD_SetPixel(x + j, y + b - 1);
	}

}

void GLCD_WrCmd(uint8_t cmd, uint8_t control){

	cekej(1);
	SR_Write_Data(cmd);
	GLCD_RS_L();
	GLCD_EnableControler(control);
	GLCD_Enable_H();
	cekej(1);
	GLCD_Enable_L();
	GLCD_DisableControler(control);

}

void GLCD_WrDat(uint8_t dat){

	cekej(1);
	SR_Write_Data(dat);
	GLCD_RS_H();
	GLCD_EnableControler(curX / 64);
	GLCD_Enable_H();
	cekej(1);
	GLCD_Enable_L();
	GLCD_DisableControler(curX / 64);
	curX++;
}



void GLCD_EnableControler(uint8_t control){
	switch(control){
	case 0: PTC->PSOR = CS1; break;
	case 1: PTC->PSOR = CS2; break;
	}
}
void GLCD_DisableControler(uint8_t control){
	switch(control){
	case 0: PTC->PCOR = CS1; break;
	case 1: PTC->PCOR = CS2; break;
	}
}


// DATA SIGNAL
void GLCD_RS_H(){
	PTC->PSOR = RS;
}

//INS SIGNAL
void GLCD_RS_L(){
	PTC->PCOR = RS;
}

//ENABLE SIGNAL H
void GLCD_Enable_H(){
	PTC->PSOR = E;
}

//ENABLE SIGNAL L
void GLCD_Enable_L(){
	PTC->PCOR = E;
}

//INICIALIZACE PINU
void Pin_Init(){

	SIM->SCGC5 |= 1<<11;
	SIM->SCGC5 |= 1<<9;
	SIM->SCGC5 |= 1<<13;

	PORTC->PCR[8]= 0x100;
	PORTC->PCR[9]= 0x100;
	PORTC->PCR[10]= 0x100;
	PORTC->PCR[11]= 0x100;
	PORTC->PCR[12]= 0x100;
	PORTC->PCR[17]= 0x100;

	PORTA->PCR[12]= 0x100;
	PORTE->PCR[31]= 0x100;

	PTC->PDDR |= RS;
	PTC->PCOR = RS;

	PTC->PDDR |= E;
	PTC->PCOR = E;			//mozna SOR nwm

	PTC->PDDR |= CS1;
	PTC->PCOR = CS1;

	PTC->PDDR |= CS2;
	PTC->PCOR = CS2;

	PTC->PDDR |= STR;
	PTC->PCOR = STR;

	PTC->PDDR |= CLK;
	PTC->PCOR = CLK;

	PTA->PDDR |= RST;
	PTA->PSOR = RST;    //vypnuti resetu

	PTE->PDDR |= DAT;
	PTE->PCOR = DAT;

	cekej(100);

}

//CEKEJ
void cekej(uint32_t delay_ms){
static volatile uint32_t i;
	for(i = 0; i < 10*delay_ms; i++);
}


/*
 * 4094 FUNKCE
 * POSLI DATA
 * HODINY
 * STROBO
 * ZAPIS H
 * ZAPIS L
 */
void SR_Write_Data(uint8_t data){

	for(int i=0;i<8;i++){

		if((data << i) & 0b10000000){

			SR_H();
		}
		else{

			SR_L();
		}

		SR_CLK();
	}
	SR_STR();
}

void SR_CLK(){

	PTC->PCOR =CLK;
	PTC->PSOR =CLK;
	cekej(1);
	PTC->PCOR =CLK;
	cekej(1);
}

void SR_STR(){

	PTC->PCOR =STR;
	PTC->PSOR =STR;
	cekej(1);
	PTC->PCOR =STR;
}

void SR_L(){
	PTE->PCOR = DAT;
}

void SR_H(){
	PTE->PSOR = DAT;
}








