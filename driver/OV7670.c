//	OV7670(FIFOなし) ドライバー	for ATmega328P
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2025/03	@jsdiy

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "OV7670.h"
#include "twilib.h"

#define	OV7670_SlaveAddress	0x21	//(0x42 >> 1)
#define	OV7670_ProductID	0x76	//OV7670の場合

//変数
static	int16_t	width, height;
static	int8_t bytePerPixel;

//関数
int16_t	OV7670_Width(void) { return width; }
int16_t	OV7670_Height(void) { return height; }
int8_t	OV7670_BytePerPixel(void) { return bytePerPixel; }
static	void	WriteRegister(uint8_t addr, uint8_t data);
static	uint8_t	ReadRegister(uint8_t addr);
static	void	RefreshCLKRC(void);
static	void	SoftwareReset(void);
static	void	SetRegParams(const regval_list* list);
static	void	SetResolution(ECamResolution camRes);
static	void	SetWindow(uint16_t hStart, uint16_t vStart);
static	void	SetColorMode(ECamColorMode colMode);
static	void	SetClockDiv(uint8_t xclkPreScale);

//初期化
void	OV7670_Initialize(void)
{
	//ポート設定
	OV7670_CTRL_DDR		&=	~(OV7670_CTRL_VSYNC | OV7670_CTRL_HREF | OV7670_CTRL_PCLK);	//入力方向
	OV7670_CTRL_PORT	|=	 (OV7670_CTRL_VSYNC | OV7670_CTRL_HREF | OV7670_CTRL_PCLK);	//プルアップ
	OV7670_DATA_DDR		= 0x00;	//入力方向(8bit)
	OV7670_DATA_PORT	= 0xFF;	//プルアップ(8bit)
	//値を読むときは、	uint8_t data = OV7670_DATA_PIN;

	width = height = 0;
	bytePerPixel = 2;	//RGB565,YUYV以外には対応しない前提
}

//カメラの動作モードを設定する
void	OV7670_SetCameraMode(ECamResolution res)
{
	//レジスタをリセットする
	SoftwareReset();

	//レジスタ初期値をセットする
	SetRegParams(OV7670_default_regs);

	//内部クロックをセットする	→解像度による
	//SetClockDiv(xx - 1);

	//解像度をセットする
	SetResolution(res);

	//カラーモードをセットする
	SetColorMode(CamColor_RGB565);
}

//クロックを供給する
void	OV7670_SupplyXclk(void)
{
	//AVRではPWMでクロック生成して出力するか、ヒューズ設定でCLKOピンからシステムクロックを出力する
	
	/*	XCLKの分周と逓倍について
	・OV7670の仕様は InputClock(XCLK): 10MHz <= (Typ.24MHz) <= 48MHz
		MCUはこの範囲でXCLKを生成・出力すること。
	・OV7670内ではXCLKを1～64分周まで可。フレームレート(fps)を落とせる。
		- 分周はCLKRCレジスタの設定で「入力XCLK / (1～64)」。分母は1刻み。デフォルトは「入力XCLK / 1」。
			F_internalClock = F_inputClock / (bit[5:0] + 1)
	・PCLKはデフォルトではXCLKと同値（逓倍処理をバイパス）。
		- フレームレートを落としたい実行環境ではPCLKを逓倍する必要はない。
	*/
}

//レジスタに値を書き込む
static	void	WriteRegister(uint8_t addr, uint8_t data)
{
	TWI_Enable();

	_delay_us(5);	//【SCCB通信の注意点】参照
	TWI_Start();
	TWI_SlaveAddrW(OV7670_SlaveAddress);
	TWI_WriteByte(addr);
	TWI_WriteByte(data);
	TWI_Stop();

	TWI_Disable();
}

//レジスタから値を読み出す
/*	【SCCB通信の注意点】
・OV7670_DS(v1.4) - P6 - Table 4. Functional and AC Characteristics
	SCCB Timing - BUF Bus free time before new START - [Min] 1.3 μs
	→新しいスタート前のバスの空き時間：最小1.3us

・SCCBSpec_AN(v2.2) - 3.1.1 Start of Data Transmission によると、
	Two timing parameters are defined for the start of transmission, tPRC and tPRA.（略）
	The tPRA is defined as the pre-active time of SCCB_E.（略）
	The minimum value of tPRA is 1.25 μs.（略）
	→TWI_START開始時、前回のTWI_STOPから1.25us以上経過している必要がある。
	　全てのTWI_START前に時間待ちすれば確実。5usで安定動作した。2usでは通信に失敗した（AVR内蔵8MHzの場合）。
*/
static	uint8_t	ReadRegister(uint8_t addr)
{
	TWI_Enable();

	_delay_us(5);
	TWI_Start();
	TWI_SlaveAddrW(OV7670_SlaveAddress);
	TWI_WriteByte(addr);
	TWI_Stop();

	uint8_t data;
	_delay_us(5);
	TWI_Start();
	TWI_SlaveAddrR(OV7670_SlaveAddress);
	TWI_ReadByte(1, &data);	//'1'はtrueの意味
	TWI_Stop();

	TWI_Disable();

	return data;
}

//SCCB通信テスト
uint16_t	OV7670_CamComTest(void)
{
	uint16_t pid = ReadRegister(REG_PID);
	uint16_t ver = ReadRegister(REG_VER);
	//uint8_t midh = ReadRegister(REG_MIDH);
	//uint8_t midl = ReadRegister(REG_MIDL);

	return (pid == OV7670_ProductID) ? ((pid << 8) | ver) : 0x0000;
}

//レジスタをリセットする
static	void	SoftwareReset(void)
{
	//レジスタのリセット完了まで最大1msかかる（データシートより）
	WriteRegister(REG_COM7, COM7_RESET);
	_delay_ms(5);
}

//レジスタ値を書き込む（レジスタ＋値のリスト）
static	void	SetRegParams(const regval_list* list)
{
	while (1)
	{
		uint8_t addr = pgm_read_byte(&list->reg_num);
		uint8_t data = pgm_read_byte(&list->value);
		if ((addr == 0xFF) && (data == 0xFF)) { break; }
		WriteRegister(addr, data);
		list++;
	}
}

//解像度に応じたwindowサイズと内部クロックを設定する
static	void	SetResolution(ECamResolution camRes)
{
	uint8_t val = ReadRegister(REG_COM7);
	val &= ~COM7_FMT_MASK;	//bit[5:3]

	switch (camRes)
	{
#if (0)	//ATmega328P@16MHz, XCLK:16MHz/64分周 でキャプチャが追いつかなかった
	case	CamRes_VGA:
		val |= COM7_FMT_VGA;
		WriteRegister(REG_COM7, val);
		SetRegParams(OV7670_vga);
		SetWindow(158, 10);
		width = 640;	height = 480;
		SetClockDiv(64 - 1);
		break;
#endif

	case	CamRes_QVGA:
		val |= COM7_FMT_QVGA;
		WriteRegister(REG_COM7, val);
		SetRegParams(OV7670_qvga);
		SetWindow(/*180*/176, 12);	//カラーバーずれ軽減
		width = 320;	height = 240;

		//内部クロックをセットする:QVGA	
		//ポーリング方式で1行ごとに、
		//・描画のみ			(19～64)-1
		//・データ保存のみ		(23～64)-1
		//・描画＋データ保存	(41～64)-1
		SetClockDiv(23 - 1);
		break;

	default:
	case	CamRes_QQVGA:
		val |= COM7_FMT_QVGA;
		WriteRegister(REG_COM7, val);
		SetRegParams(OV7670_qqvga);
		SetWindow(/*190*/184, 10);	//カラーバーずれ軽減
		width = 160;	height = 120;

		//内部クロックをセットする:QQVGA
		//ポーリング方式で1行ごとに、
		//・描画のみ			(4～64)-1
		//・データ保存のみ		(5～64)-1
		//・描画＋データ保存	(8～64)-1
		SetClockDiv(5 - 1);
		break;
	}
}

//CLKRCレジスタをリフレッシュする
static	void	RefreshCLKRC(void)
{
	uint8_t val = ReadRegister(REG_CLKRC);
	WriteRegister(REG_CLKRC, val);
}

//レジスタ設定：ウインドウ
/*
・VGA,QVGA,QQVGAを想定。
・QVGA,QQVGAであってもVGAの縦横サイズを設定する（ダウンサンプリング元の解像度を設定する）
・VGA系では次式の関係がある（CIF系はCIFの縦横サイズで計算）
	hStop = hStart + 640 - 784;
	vStop = vStart + 480;

「784」の出どころは、
・データシート(v1.4)
	Figure 6. VGA Frame Timing
		HREF: t_Line = 784t_P
・OV7670 Implementation Guide (V1.0) - P14
	Table 3-4. Dummy Pixel and Row
		Dummy Pixel： 1 digital count is equal to 1/784 row period
・OV7670 Implementation Guide (V1.0) - P15
	The OV7670/OV7171 array always outputs VGA resolution so the row interval is:
		tROW INTERVAL = 2 x (784 + Dummy Pixels) x tINT CLK
*/
static	void	SetWindow(uint16_t hStart, uint16_t vStart)
{
	uint8_t val;

	//HSTART,HSTOPにそれぞれ上位8ビットを設定する。HREFにそれぞれの下位3ビットを設定する。
	uint16_t hStop = (hStart + 640) % 784;
	uint8_t valHStart = (hStart >> 3) & 0xFF;
	uint8_t valHStop = (hStop >> 3) & 0xFF;
	uint8_t valHref = ((hStop & 0x07) << 3) | ((hStart & 0x07) << 0);	//注意：stopが上位ビット、startが下位ビット
	WriteRegister(REG_HSTART, valHStart);
	WriteRegister(REG_HSTOP, valHStop);
	val = ReadRegister(REG_HREF);
	val &= ~(HREF_HSP_MASK | HREF_HST_MASK);	//HSTOP,HSTARTのマスク
	val |= valHref;
	WriteRegister(REG_HREF, val);

	//VSTART,VSTOPにそれぞれ上位8ビットを設定する。VREFにそれぞれの下位2ビットを設定する。
	uint16_t vStop = vStart + 480;
	uint8_t valVStart = (vStart >> 2) & 0xFF;
	uint8_t valVStop = (vStop >> 2) & 0xFF;
	uint8_t valVref = ((vStop & 0x03) << 2) | ((vStart & 0x03) << 0);	//注意：stopが上位ビット、startが下位ビット
	WriteRegister(REG_VSTART, valVStart);
	WriteRegister(REG_VSTOP, valVStop);
	val = ReadRegister(REG_VREF);
	val &= ~(VREF_VSP_MASK | VREF_VST_MASK);	//VSTOP,VSTARTのマスク
	val |= valVref;
	WriteRegister(REG_VREF, val);
}

//カラーモードを設定する
static	void	SetColorMode(ECamColorMode colMode)
{
	uint8_t val = ReadRegister(REG_COM7);
	val &= ~COM7_COLOR_MASK;	//bit[2,0]

	switch (colMode)
	{
	default:
	case	CamColor_RGB565:
		val |= COM7_RGB;
		WriteRegister(REG_COM7, val);
		SetRegParams(OV7670_rgb565);		
		break;

	case	CamColor_YUYV:
		val |= COM7_YUV;
		WriteRegister(REG_COM7, val);
		SetRegParams(OV7670_yuv422);
		break;
	}
}

//内部クロックを設定する
//引数	CLKRCレジスタにセットする分周値（0:分周なし, 1:2分周, 2, 3, …, 62, 63:64分周）
static	void	SetClockDiv(uint8_t xclkPreScale)
{
	//分周値	F(internal clock) = F(input clock) / (Bit[5:0] + 1)
	uint8_t val = ReadRegister(REG_CLKRC);
	WriteRegister(REG_CLKRC, (val & ~CLKRC_PRESCALE_MASK) | xclkPreScale);

	//逓倍値	DBLV_BYPASS, DBLV_CLK_x4, _x6, _x8
	val = ReadRegister(REG_DBLV);
	WriteRegister(REG_DBLV, (val & ~DBLV_CLK_MASK) | DBLV_BYPASS);
	
	RefreshCLKRC();
}

//カメラの画像を取得する
void	OV7670_TakePicture(TFpCallback func, uint8_t* pixelDataBuf)
{
	int16_t	lineLoopCount = OV7670_Height();
	int16_t lineIdx = 0;
	int16_t dataLength = OV7670_Width() * OV7670_BytePerPixel();

	//フレームの開始を待つ
	while (OV7670_CTRL_PIN & OV7670_CTRL_VSYNC);	//Hiの間（現在のフレーム継続中）
	//while (!(OV7670_CTRL_PIN & PCINT_VSYNC));		//Loの間（現在のフレームが終了）	※これを検出する必要はない
	//ここからフレーム開始

	//行ごとの処理
	while (lineLoopCount--)
	{
		//1行分の画素データを取得する
		int16_t bufIdx = 0;
		int16_t pixelDataLoopCount = dataLength;
		while (pixelDataLoopCount--)
		{
			//画素データの切り替わり（立ち下がり）を待ち、画素データを取得し、
			//次の画素データのタイミング（立ち上がり）までやり過ごす
			while (OV7670_CTRL_PIN & OV7670_CTRL_PCLK);	//Hiの間
			pixelDataBuf[bufIdx++] = OV7670_DATA_PIN;
			while (!(OV7670_CTRL_PIN & OV7670_CTRL_PCLK));	//Loの間
		}

		//1ライン分の画像データ処理
		(*func)(lineIdx, pixelDataBuf, dataLength);
		lineIdx++;
	}
}

//カラーバーを表示する／しない
void	OV7670_ColorBar(uint8_t isOn)
{
	uint8_t val = ReadRegister(REG_COM17);
	val = isOn ? (val | COM17_CBAR) : (val & ~COM17_CBAR);
	WriteRegister(REG_COM17, val);
}
//
void	OV7670_ColorBarTr(uint8_t isOn)
{
	uint8_t val = ReadRegister(REG_COM7);
	val = isOn ? (val | COM7_CBAR) : (val & ~COM7_CBAR);
	WriteRegister(REG_COM7, val);
}

//水平・垂直反転
//引数	MVFP_MIRROR:	Mirror image
//		MVFP_FLIP:		Vertical flip
//		MVFP_MIRROR|MVFP_FLIP:	rotate 180-degree
//		0:	normal image
void	OV7670_Flip(uint8_t mvfp)
{
	uint8_t val = ReadRegister(REG_MVFP);
	val &= ~MVFP_FLIP_MASK;
	val |= (mvfp & MVFP_FLIP_MASK);
	WriteRegister(REG_MVFP, val);
}

//AWB(Automatic White Balance)：現在の設定値を取得する	
void	OV7670_GetAwbGain(uint8_t* gainR, uint8_t* gainG, uint8_t* gainB)
{
	*gainG = ReadRegister(REG_GGAIN);	// AWB Green gain	※default:00
	*gainB = ReadRegister(REG_BLUE);	// AWB Blue gain (00-ff)default:80
	*gainR = ReadRegister(REG_RED);		// AWB Red gain (00-ff)default:80
}

//AWB(Automatic White Balance)：ゲインを設定する
void	OV7670_SetAwbGain(uint8_t gainR, uint8_t gainG, uint8_t gainB)
{
	WriteRegister(REG_GGAIN, gainG);	// AWB Green gain	※default:00
	WriteRegister(REG_BLUE, gainB);		// AWB Blue gain (00-ff)default:80
	WriteRegister(REG_RED, gainR);		// AWB Red gain (00-ff)default:80
}

//明るさ	デフォルト値は0x00
uint8_t	OV7670_GetBrightness(void) { return ReadRegister(REG_BRIGHT); }
void	OV7670_SetBrightness(uint8_t val) { WriteRegister(REG_BRIGHT, val); }

//濃さ	デフォルト値は0x40
uint8_t	OV7670_GetContrast(void) { return ReadRegister(REG_CONTRAS); }
void	OV7670_SetContrast(uint8_t val) { WriteRegister(REG_CONTRAS, val); }
