//	LCDドライバー	for ST7735/ST7789(SPI)	基本ライブラリ(簡易版)
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2020/05 - 2025/03	@jsdiy

#include <avr/io.h>
#include <util/delay.h>
#include "ST77XXlib.h"
#include "spilib.h"

#define	PIN_DC_COMMAND()	(GLCD_DC_PORT &= ~GLCD_DC)	//D/C:Lo →コマンド
#define	PIN_DC_DATA()		(GLCD_DC_PORT |= GLCD_DC)	//D/C:Hi →データ/コマンドのパラメータ

#define HiByte(a)	(((a) >> 8) & 0xFF)
#define LoByte(a)	((a) & 0xFF)

//ST77xxのコマンド
typedef	enum
{
	CmdSWRESET	= 0x01,	//Software reset
	CmdSLPOUT	= 0x11,	//Sleep out & booster on
	CmdINVON	= 0x21,	//Display Inversion On	（色反転。RGB_00:00:00が白, FF:FF:FFが黒）
	CmdDISPOFF	= 0x28,	//Display off
	CmdDISPON	= 0x29,	//Display on
	CmdCASET	= 0x2A,	//Column address set
	CmdRASET	= 0x2B,	//Row address set
	CmdRAMWR	= 0x2C,	//Memory write
	CmdMADCTL	= 0x36,	//Memory data access control
	CmdCOLMOD	= 0x3A	//Interface pixel format
}
ECommand;

//変数
static	int16_t	screenWidth, screenHeight;
static	int16_t	screenOffsetX, screenOffsetY;

//関数
static	void	TransmitByte(uint8_t data);
static	void	WriteCommand(ECommand cmd);
static	void	WriteCommandP1(ECommand cmd, uint8_t p1);
static	void	WriteCommandP4(ECommand cmd, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4);

//初期化
void	GLCD_Initialize(void)
{
	GLCD_DC_DDR		|= GLCD_DC;	//出力方向
	GLCD_DC_PORT	|= GLCD_DC;	//Hiを出力

	//画面サイズ設定
	screenWidth = GLCD_SCREEN_WIDTH;
	screenHeight = GLCD_SCREEN_HEIGHT;
	screenOffsetX = GLCD_SCREEN_OFFSET_X;
	screenOffsetY = GLCD_SCREEN_OFFSET_Y;

	SPI_SlaveSelect(ESpi_Lcd);

	//マイコンのリセット時を想定し、LCDをリセットする
	//・CmdSWRESETによりレジスタへデフォルト値をセットするのに5msかかり、その後スリープ・イン状態になる。
	//	その状態からCmdSLPOUTコマンドを送る前に120ms待つ必要がある。
	WriteCommand(CmdSWRESET);	_delay_ms(200);	//ソフトウェア・リセット

	//スリープ解除
	//・CmdSLPOUTコマンドを送った後、CmdSLPINコマンドを送る前には120ms待つ必要がある。
	WriteCommand(CmdSLPOUT);

	//Memory data access control
	WriteCommandP1(CmdMADCTL, 0x00);

	//色深度
#ifdef GLCD_ST7735
	/*
	0x03:	12-bit/pixel	RGB=4:4:4bit
	0x05:	16-bit/pixel	RGB=5:6:5bit
	0x06:	18-bit/pixel	RGB=6:6:6bit	<-default
	*/
	WriteCommandP1(CmdCOLMOD, 0x05);
#endif
#ifdef GLCD_ST7789
	/*	データシート	9.1.32 COLMOD (3Ah): Interface Pixel Format
	Bit		Description
	D7		Set to '0'
	D6:4	RGB interface color format
				'101' = 65K of RGB interface
				'110' = 262K of RGB interface
	D3		Set to '0'
	D2:0	Control interface color format
				'011' = 12bit/pixel
				'101' = 16bit/pixel
				'110' = 18bit/pixel
				'111' = 16M truncated
	*/
	WriteCommandP1(CmdCOLMOD, 0x55);
#endif

	//色反転（これが必要かはLCDモジュールによる）
	//・手持ちのST7735では不要、ST7789では必要だった。
#ifdef GLCD_ST7789
	WriteCommand(CmdINVON);
#endif

	//画面表示オン
	WriteCommand(CmdDISPON);

	SPI_SlaveDeselect(ESpi_Lcd);
}

//画面を回転／反転させる
void	GLCD_RotateFlip(EMadCtl param)
{
	SPI_SlaveSelect(ESpi_Lcd);
	WriteCommandP1(CmdMADCTL, param);
	SPI_SlaveDeselect(ESpi_Lcd);

	//90度回転の場合、縦横を交換する
	if (param & MC_Rot90)
	{
		screenWidth = GLCD_SCREEN_HEIGHT;
		screenHeight = GLCD_SCREEN_WIDTH;
		screenOffsetX = GLCD_SCREEN_OFFSET_Y;
		screenOffsetY = GLCD_SCREEN_OFFSET_X;
	}
}

//LCDにbyte値を送信する
static	void	TransmitByte(uint8_t data)
{
	SPI_MasterTransmit(data);
}

//LCDにコマンドを出力する
static	void	WriteCommand(ECommand cmd)
{
	PIN_DC_COMMAND();
	TransmitByte(cmd);
}

//LCDにコマンドとパラメータを出力する
static	void	WriteCommandP1(ECommand cmd, uint8_t p1)
{
	WriteCommand(cmd);
	PIN_DC_DATA();
	TransmitByte(p1);
}
//
static	void	WriteCommandP4(ECommand cmd, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4)
{
	WriteCommand(cmd);
	PIN_DC_DATA();
	TransmitByte(p1);
	TransmitByte(p2);
	TransmitByte(p3);
	TransmitByte(p4);
}

//LCDに色データを出力する
void	GLCD_WriteColor(const TColor* color, int32_t repeatCount)
{
	while (repeatCount--)
	{
		TransmitByte(color->hiByte);
		TransmitByte(color->loByte);
	}
}

//色を作成する
TColor	GLCD_CreateColor(uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t	rr	= red	* 0x1F / 0xFF;
	uint8_t	gg	= green	* 0x3F / 0xFF;
	uint8_t	bb	= blue	* 0x1F / 0xFF;
	uint16_t	rgb565 = (rr << (5 + 6)) | (gg << 5) | bb;

	TColor	color;
	color.hiByte = HiByte(rgb565);
	color.loByte = LoByte(rgb565);
	return color;
}

//LCDに描画領域を指定する（クリッピングは考慮されない）
//引数	x,y:	画面上の描画開始座標
//		w,h:	縦横サイズ
void	GLCD_SetDrawArea(int16_t x, int16_t y, int16_t w, int16_t h)
{
	//描画RAM上の始点・終点を指定する
	//・引数の意味：(cmd, 16bit値の上位8bit, 16bit値の下位8bit, 16bit値の上位8bit, 16bit値の下位8bit)
	int16_t startX = x + screenOffsetX;
	int16_t startY = y + screenOffsetY;
	int16_t endX = startX + w - 1;
	int16_t endY = startY + h - 1;
	WriteCommandP4(CmdCASET, HiByte(startX), LoByte(startX), HiByte(endX), LoByte(endX));
	WriteCommandP4(CmdRASET, HiByte(startY), LoByte(startY), HiByte(endY), LoByte(endY));

	WriteCommand(CmdRAMWR);
	PIN_DC_DATA();
}

//画面全体を塗りつぶす
void	GLCD_ClearScreen(const TColor* color)
{
	GLCD_FillRect(0, 0, screenWidth, screenHeight, color);
}

//矩形を塗りつぶす
void	GLCD_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, const TColor* color)
{
	SPI_SlaveSelect(ESpi_Lcd);
	GLCD_SetDrawArea(x, y, w, h);
	int32_t pixelCount = (int32_t)w * (int32_t)h;
	GLCD_WriteColor(color, pixelCount);
	SPI_SlaveDeselect(ESpi_Lcd);
}

//画像を描く
//引数:	x,y:	描画先の座標
//		w,h:	画像の幅・高さ
//		buf:	RGB565が上位バイト・下位バイトの順に格納された配列
//		bufLength:	buf配列のサイズ（RGB565は1画素2byteなので必要なデータの長さはw*h*2byte）
//注意：	画像は画面内に収まっていること（クリッピングを考慮しないので）
void	GLCD_DrawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t* buf, int32_t bufLength)
{
	SPI_SlaveSelect(ESpi_Lcd);
	GLCD_SetDrawArea(x, y, w, h);
	for (int32_t i = 0; i < bufLength; i++) { TransmitByte(buf[i]); }
	SPI_SlaveDeselect(ESpi_Lcd);
}

//画面サイズを取得する(glcdtextlibから呼ばれる)
void	GLCD_GetScreenSize(int16_t* width, int16_t* height)
{
	*width = screenWidth;
	*height = screenHeight;
}
