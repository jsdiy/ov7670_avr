//	OV7670(FIFOなし) ドライバー	for ATmega328P
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2025/03	@jsdiy

#ifndef OV7670_H_
#define OV7670_H_

#include "OV7670Register.h"

//ピンアサイン
#define	OV7670_CTRL_DDR		DDRC
#define	OV7670_CTRL_PORT	PORTC
#define	OV7670_CTRL_PIN		PINC
#define	OV7670_CTRL_SIOD	0	//TWIのSDAと接続
#define	OV7670_CTRL_SIOC	0	//TWIのSCLと接続
#define	OV7670_CTRL_VSYNC	(1<<PORTC3)
#define	OV7670_CTRL_HREF	(1<<PORTC2)
#define	OV7670_CTRL_PCLK	(1<<PORTC1)
#define	OV7670_CTRL_RESET	0	//VCCと接続
#define	OV7670_CTRL_PWDN	0	//GNDと接続
//
#define	OV7670_DATA_DDR		DDRD
#define	OV7670_DATA_PORT	PORTD
#define	OV7670_DATA_PIN		PIND	//D0..7
/*
XCLKについて：
ATmega328Pのヒューズビットの設定によりシステムクロックがPORTB0から出力される。これをXCLKとする。
OV7670データシートより、XCLKはこの範囲であること → 10MHz <= (Typ.24MHz) <= 48MHz
*/

//カメラ映像の解像度
typedef	enum
{
	CamRes_VGA,		//640x480	※ATmega328P@16MHz, XCLK:16MHz/64分周 でキャプチャが追いつかなかった
	CamRes_QVGA,	//320x240
	CamRes_QQVGA	//160x120
}
ECamResolution;

//カメラ映像のカラーモード
typedef	enum
{
	CamColor_RGB565,
	CamColor_YUYV	//グレースケールへの利用を想定（未実装）
}
ECamColorMode;

//関数
void	OV7670_Initialize(void);
void	OV7670_SetCameraMode(ECamResolution res);
int16_t	OV7670_Width(void);
int16_t	OV7670_Height(void);
int8_t	OV7670_BytePerPixel(void);
uint16_t	OV7670_CamComTest(void);
void	OV7670_ColorBar(uint8_t isOn);
void	OV7670_ColorBarTr(uint8_t isOn);	//半透明なカラーバー
//
void	OV7670_GetAwbGain(uint8_t* gainR, uint8_t* gainG, uint8_t* gainB);
void	OV7670_SetAwbGain(uint8_t gainR, uint8_t gainG, uint8_t gainB);
uint8_t	OV7670_GetBrightness(void);
void	OV7670_SetBrightness(uint8_t val);
uint8_t	OV7670_GetContrast(void);
void	OV7670_SetContrast(uint8_t val);

#endif	//OV7670_H_

//スレーブアドレスについて
/*
OV7670_DS(v1.4).pdf　の　P11
--
Register Set
Table 5 provides a list and description of the Device Control registers contained in the OV7670/OV7171. 
For all register Enable/Disable bits, ENABLE = 1 and DISABLE = 0. 
The device slave addresses are 42 for write and 43 for read.
--
レジスタ セット
表 5 は、OV7670/OV7171 に含まれるデバイス制御レジスタのリストと説明を示します。
すべてのレジスタの有効/無効ビットは、ENABLE = 1、DISABLE = 0 です。
デバイスのスレーブ アドレスは、書き込みの場合は 42、読み取りの場合は 43 です。
--
注意：
I2Cとして見たスレーブアドレスは0x21.　上記説明（42hと43h）は下記の意味からそのように書かれている。
マスターからスレーブへの、
	書き込み時は (0x21 << 1) | 0 → 0x42
	読み取り時は (0x21 << 1) | 1 → 0x43
*/
