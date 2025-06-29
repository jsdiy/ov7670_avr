//	SPIライブラリ	for ATmegaシリーズ
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2020/03 - 2025/05	@jsdiy

#include <avr/io.h>
#include "spilib.h"

void	SPI_MasterInit(void)
{
	SPI_DDR |= (SPI_SCK | SPI_MOSI | SPI_SS_GROUP);	//出力方向
	SPI_DDR &= ~SPI_MISO;	//入力方向
	SPI_PORT &= ~(SPI_SCK | SPI_MOSI);	//Loを出力
	SPI_PORT |= SPI_SS_GROUP;	//デバイスを選択解除の状態とするためSSにHiを出力（SSは負論理）
	
	SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR)	//SPI有効化(1)、MSB先行(0)、マスターデバイス(1)
		| (1 << CPOL) | (1 << CPHA)		//SCKの極性・位相の設定
		| (0 << SPR1) | (0 << SPR0);	//クロック分周の設定
	SPSR = (1 << SPI2X);				//クロック分周の設定
	/*	SCKの極性・位相の設定<CPOL,CPHA>
	ST7735Sは <0,0> or <1,0> or <1,1>	SPIモード0,2,3
	ST7789Vは <1,0> or <1,1>				SPIモード2,3
	…で動作した。
	*/
	/*	クロック分周の設定(SPR1:SPR0:SPI2X)
	0:0:1 = f_osc/2（最速設定）
	0:0:0 = f_osc/4
	0:1:1 = f_osc/8
	…
	1:1:0 = f_osc/128（最も遅い設定）
	ST7735S/ST7789VはATmega328Pのシステムクロック(max:20MHz)によらず最速設定でok
	*/
}

//スレーブデバイスを選択する
void	SPI_SlaveSelect(ESpiSs ss)
{
	SPI_PORT &= ~ss;	//Loを出力（SSは負論理）
}

//スレーブデバイスを選択解除する
void	SPI_SlaveDeselect(ESpiSs ss)
{
	SPI_PORT |= ss;		//Hiを出力（SSは負論理）
}

//1byte送信（と同時に1byte受信）
void	SPI_MasterTransmit(uint8_t data)
{
	SPDR = data;	//転送開始（この瞬間にSCKが出力開始する）
	while (!(SPSR & (1<<SPIF)));	//転送完了待ち
	//転送完了（この時点でレジスタSPDRには受信データが格納されている。SCKは出力停止する）
}

//受信バッファからbyteデータを取り出す
uint8_t	SPI_MasterReceive(void)
{
	return SPDR;
}
