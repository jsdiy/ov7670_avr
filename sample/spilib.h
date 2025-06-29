//	SPIライブラリ	for ATmegaシリーズ
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2020/03 - 2025/05	@jsdiy

#ifndef SPILIB_H_
#define SPILIB_H_


//ピンアサイン（ATmegaX8系は共通）
#define	SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define	SPI_SCK		(1 << PORTB5)
#define	SPI_MISO	(1 << PORTB4)
#define	SPI_MOSI	(1 << PORTB3)

//デバイス選択(SS#)
typedef	enum
{
	ESpi_Lcd	=	(1 << PORTB2),
	ESpi_Mem	=	(1 << PORTB1)

/*	【参考】バスに複数のSPIデバイスが接続されていて任意にセレクトしたい場合
	ESpi_Aaa	= 	(1 << PORTBa)
	ESpi_Bbb	=	(1 << PORTBb)
	ESpi_Nnn	=	(1 << PORTBn)
*/
}
ESpiSs;

#define	SPI_SS_GROUP	((uint8_t)ESpi_Lcd | (uint8_t)ESpi_Mem)

//関数
void	SPI_MasterInit(void);
void	SPI_SlaveSelect(ESpiSs ss);
void	SPI_SlaveDeselect(ESpiSs ss);
void	SPI_MasterTransmit(uint8_t data);
uint8_t	SPI_MasterReceive(void);

#endif	//SPILIB_H_
