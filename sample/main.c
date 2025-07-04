//	カメラOV7670(FIFOなし)動作テスト
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2025/05	@jsdiy

/*
【ポート対応】	ATmega328P	x:存在しない(PC6はRESET) -:空き
bit		7		6		5		4		3		2		1		0
portB	XTAL2	XTAL1	SCK		MISO	MOSI	CS#		-		XCLK	クロック入力,SPI,LCD,カメラ
portC	x		*		SCL		SDA		VSYNC	-		PCLK	DC		I2C,カメラ,LCD
portD	D7		D6		D5		D4		D3		D2		D1		D0		カメラ
*/
/*
【ATmega328P】
|543210---54321|
|CCCCCCGRVBBBBB|	R=Vref
>              |
|CDDDDDVGBBDDDB|
|601234--675670|

【ヒューズビット】	※工場出荷設定は Low:01100010(62), Hi:11-11001(D9)
Low: 10100110 (A6)
     ||||++++-- CKSEL[3:0] システムクロック選択	//外付けレゾネータ
     ||++-- SUT[1:0] 起動時間
     |+-- CKOUT (0:PB0にシステムクロックを出力)
     +-- CKDIV8 クロック分周初期値 (1:1/1, 0:1/8)	//分周なし
*/
/*
【ビルド環境】
Microchip Studio 7 (Version: 7.0.2594 - )
ATmega328P: 外付け16MHz
ヒューズLo=0xA6: クロック元＝セラミック振動子、PB0にクロック出力。
最適化オプション: -O3（積極的なインライン展開を伴う処理速度優先の最適化）

【更新履歴】
2025/03	開発開始
2025/06	シンプル版
	Program Memory Usage 	:	3930 bytes   12.0 % Full
	Data Memory Usage 		:	653 bytes   31.9 % Full
*/

#include <avr/io.h>
#include <util/delay.h>
#include "twilib.h"
#include "spilib.h"
#include "OV7670.h"
#include "ST77XXlib.h"

//ラインバッファ
//・「想定する解像度の幅 * 2byte」単位で確保する（RGB565は1画素2byte）。
//・バッファは1ライン分あればよい。
#define PIXELDATA_BUFSIZE_MAX	(320 * 2)	//QVGA 1ライン分を想定
static	uint8_t	pixelDataBuf[PIXELDATA_BUFSIZE_MAX];

//関数
static	void	App_CbDrawImage(int16_t lineIndex, uint8_t* dataBuffer, int16_t dataLength);

int	main(void)
{
	_delay_ms(200);

	//通信機能初期化
	TWI_MasterInit();
	SPI_MasterInit();

	//LCD初期化
	GLCD_Initialize();
	GLCD_RotateFlip(MC_Rot90 | MC_YFlip);
	TColor bgColor = GLCD_CreateColor(0x00, 0x00, 0xFF);	//青
	GLCD_ClearScreen(&bgColor);

	//カメラ初期化
	OV7670_Initialize();
	OV7670_SetCameraMode(CamRes_QQVGA);	//CamRes_QVGAも可

	while (1)
	{
		//カメラの画像を描画する
		OV7670_TakePicture(App_CbDrawImage, pixelDataBuf);
	}

	return 0;
}

//カメラの画像を描画する：コールバック関数
static	void	App_CbDrawImage(int16_t lineIndex, uint8_t* dataBuffer, int16_t dataLength)
{
	GLCD_DrawImage(0, lineIndex, OV7670_Width(), 1, dataBuffer, dataLength);
}
