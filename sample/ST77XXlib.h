//	LCDドライバー	for ST7735/ST7789(SPI)	基本ライブラリ(簡易版)
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2020/05 - 2025/03	@jsdiy

#ifndef ST77XXLIB_H_
#define ST77XXLIB_H_

//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
//	ユーザーによる設定
//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－

//ピンアサイン
#define	GLCD_DC_PORT	PORTC
#define	GLCD_DC_DDR		DDRC
#define	GLCD_DC			(1 << PORTC0)	//Lo:Command, Hi:Data/Param

/*	ATmegaX8系SPIとLCDとのピンアサイン
	[SPI]			[LCD]
	PB5:SCK		--	SCL
	PB4:MISO	--	不要
	PB3:MOSI	--	SDA
	PB2:SS#		--	CS#
	任意のピン	--	DC
	任意のピンor不要	--	RESET#
*/

//LCDコントローラー（片方を有効にし、他方はコメントアウトする）
//#define	GLCD_ST7735
#define	GLCD_ST7789

//－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－

//画面サイズ、オフセット
#ifdef	GLCD_ST7735
#define	GLCD_SCREEN_WIDTH	128
#define	GLCD_SCREEN_HEIGHT	160
#define	GLCD_SCREEN_OFFSET_X	0
#define	GLCD_SCREEN_OFFSET_Y	0
#endif
#ifdef	GLCD_ST7789
#define	GLCD_SCREEN_WIDTH	240
#define	GLCD_SCREEN_HEIGHT	320
#define	GLCD_SCREEN_OFFSET_X	0
#define	GLCD_SCREEN_OFFSET_Y	0
#endif

//色構造体
typedef	struct
{
	uint8_t	hiByte, loByte;	//RGB565の上位バイト、下位バイト
}
TColor;

//Memory data access control
typedef	enum
{
	MC_YFlip	= (1 << 7),	//bit7(MY)	MCUからメモリへの書き込み方向…	0:順方向, 1:逆方向（垂直反転）
	MC_XFlip	= (1 << 6),	//bit6(MX)	MCUからメモリへの書き込み方向…	0:順方向, 1:逆方向（水平反転）
	MC_Rot90	= (1 << 5),	//bit5(MV)	MCUからメモリへの書き込み方向…	0:水平方向優先, 1:垂直方向優先（90度回転）
	MC_RefleshBtoT	= (1 << 4),	//bit4(ML)	LCDパネルのリフレッシュ方向…	0:Top行→Bottom行方向, 1:Bottom行→Top行方向
	MC_PixOrderBGR	= (1 << 3),	//bit3(RGB)	メモリ上のRGBデータとLCDパネルのRGB画素の並び順の対応…	0:RGB, 1:BGR
	MC_RefleshRtoL	= (1 << 2),	//bit2(MH)	LCDパネルのリフレッシュ方向…	0;Left列→Right列方向, 1:Right列→Left列方向
	/*
	・デフォルトは0x00
	・LCDの設置方向に合わせて垂直反転／水平反転／90度回転させるには、MY/MX/MVビットを指定すればよい。
	・適当な色を表示してみて、RGB成分が反対に解釈されていたら、RGBビットを指定する。
		反対に解釈されるのはドライバICとLCDパネルの結線の問題。プログラムの問題ではない。
	・LCDパネルのリフレッシュ方向を変更する必要があるとすれば、LCDパネルが移動していてチラつきが気になるか、
		メモリへの書き込み方向との関係にこだわるか、くらいなので通常はML/MHビットは意識しなくてよいと思う。
	*/
}
EMadCtl;

//関数
void	GLCD_Initialize(void);
TColor	GLCD_CreateColor(uint8_t red8bit, uint8_t green8bit, uint8_t blue8bit);
void	GLCD_SetDrawArea(int16_t x, int16_t y, int16_t w, int16_t h);
void	GLCD_WriteColor(const TColor* color, int32_t repeatCount);
void	GLCD_ClearScreen(const TColor* color);
void	GLCD_RotateFlip(EMadCtl param);
void	GLCD_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, const TColor* color);
void	GLCD_DrawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t* buf, int32_t bufLength);
//
void	GLCD_GetScreenSize(int16_t* width, int16_t* height);

#endif	//ST77XXLIB_H_
