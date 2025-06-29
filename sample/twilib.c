//	TWI(I2C)基本操作ライブラリ（マスター用）	for ATmegaシリーズ
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2013/09 - 2025/03	@jsdiy
/*
【更新履歴】
2013/09/27	v1.00	初版
2014/01/06	v1.10	TWI_BusSpeed(),TWI_BusSpeedReg()を追加
2016/10/10	v1.20	TWI_SlaveAddrW/R()の引数について、スレーブアドレスを7bitで与えることにした
2017/06	v2.00
	・usiTwiMasterに合わせてTWI_BusSpeed()を廃止、TWI_MasterInit()を導入。
	・TWI_ReadByte()の引数をretAckからisLastOneに変更。
2019/01	v2.01
	・F_CPU=16MHz用のレジスタ値を記述、動作確認完了。Rpu=10kΩ.
2025/03	v2.02
	・F_CPU=12MHz用のレジスタ値を記述、動作確認完了。Rpu=10kΩ.
	・TWI_Enable(),TWI_Disable()を追加。
*/

#include <avr/io.h>
#include "twilib.h"

//ステータスコード
enum E_TWI_StatusCode
{
	STM_START		= 0x08,	//A START condition has been transmitted
	STM_REPEAT		= 0x10,	//A repeated START condition has been transmitted
	
	//Master Transmitter Mode
	STMT_SLAW_ACK	= 0x18,	//SLA+W has been transmitted; ACK has been received
	STMT_SLAW_NACK	= 0x20,	//SLA+W has been transmitted; NOT ACK has been received
	STMT_DATA_ACK	= 0x28,	//Data byte has been transmitted; ACK has been received
	STMT_DATA_NACK	= 0x30,	//Data byte has been transmitted; NOT ACK has been received
	STMT_ARBI_LOST	= 0x38,	//Arbitration lost in SLA+W or data bytes
	
	//Master Receiver Mode
	STMR_ARBI_LOST	= 0x38,	//Arbitration lost in SLA+R or NOT ACK bit
	STMR_SLAR_ACK	= 0x40,	//SLA+R has been transmitted; ACK has been received
	STMR_SLAR_NACK	= 0x48,	//SLA+R has been transmitted; NOT ACK has been received
	STMR_DATA_ACK	= 0x50,	//Data byte has been received; ACK has been returned
	STMR_DATA_NACK	= 0x58,	//Data byte has been received; NOT ACK has been returned
	
	//Miscellaneous States
	STM_NOTHING		= 0xF8,	//No relevant state information available; TWINT = "0"
	STM_BUS_ERROR	= 0x00	//Bus error due to an illegal START or STOP condition
	
	/*
	//Slave Receiver Mode
	STSR_SLAW_ACK	= 0x60,	//Own SLA+W has been received; ACK has been returned
	STSR_ARBI_LOST_SLAW_ACK	= 0x68,	//Arbitration lost in SLA+R/W as Master; own SLA+W has been received; ACK has been returned
	STSR_GENERALCALL_ACK	= 0x70,	//General call address has been received; ACK has been returned
	STSR_ARBI_LOST_GENERALCALL_ACK	= 0x78,	//Arbitration lost in SLA+R/W as Master; General call address has been received; ACK has been returned
	STSR_PREV_ADDR_SLAW_ACK	= 0x80,	//Previously addressed with own SLA+W; data has been received; ACK has been returned
	STSR_PREV_ADDR_SLAW_NACK	= 0x88,	//Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
	STSR_PREV_ADDR_GENERALCALL_ACK	= 0x90,	//Previously addressed with general call; data has been received; ACK has been returned
	STSR_PREV_ADDR_GENERALCALL_NACK	= 0x98,	//Previously addressed with general call; data has been received; NOT ACK has been returned
	STSR_STOP	= 0xA0,	//A STOP condition or repeated START condition has been received while still addressed as Slave
	
	//Slave Transmitter Mode
	STST_SLAR	= 0xA8,	//Own SLA+R has been received;
	STST_ARBI_LOST_SLAR_ACK	= 0xB0,	// Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been returned
	STST_DATA_ACK	= 0xB8,	//Data byte in TWDR has been transmitted; ACK has been received
	STST_DATA_NACK	= 0xC0,	//Data byte in TWDR has been transmitted; NOT ACK has been received
	STST_LASTDATA_ACK	= 0xC8	//Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
	*/
};

//ステータス
//・E_TWI_StatusCodeの値を取る。
#define Status	(TWSR & 0xF8)

#define SLA_W_BIT	0
#define SLA_R_BIT	1

//I2Cのバススピード(SCL)をセットする
//・計算値をレジスタに代入する。
static	void	SetBusSpeed(uint8_t valueTWBR, uint8_t valueTWSR)
{
	TWBR = valueTWBR;
	TWSR = valueTWSR;
	
	/*
	計算式:
		TWBR = (F_CPU / scl - 16) / (2 * prescale)
		TWSR:TWPS1,0 = 00:1分周, 01:4分周, 10:16分周, 11:64分周
	例:
		TWBR = (8MHz / 400kHz - 16) / (2 * prescale) = 4 / (2 * 1) = 2, prescale=1
		TWSR = (0b00 << TWPS0)
	*/
}

//初期化
void	TWI_MasterInit(void)
{
	//バススピードを設定する
	//・とりあえずF_CPU=1MHz,8MHzに対応した値のみ記述してある。
	//・その他のF_CPU値の場合は適宜計算し、TWBR,TWSRを設定する。
	
#ifdef TWI_STANDARD_MODE
	/*	SCL=100kHz
	F_CPU(MHz):	1.6	1.8	4	8	12	16	20
	TWBR:		0	1	12	32	52	72	92
	TWSR:		0	0	0	0	0	0	0
	*/
	if		(F_CPU <=  1600000UL) { SetBusSpeed(0, 0); }
	else if	(F_CPU ==  8000000UL) { SetBusSpeed(32, 0); }
	else if	(F_CPU == 12000000UL) { SetBusSpeed(52, 0); }
	else if	(F_CPU == 16000000UL) { SetBusSpeed(72, 0); }	//(18, 1)でもok
	//else if ...
	else						  { SetBusSpeed(92, 0); }
#else	//TWI_FAST_MODE
	/*	SCL=400kHz
	F_CPU(MHz):	6.4	8	12	16	20
	TWBR:		0	2	7	12	17
	TWSR:		0	0	0	0	0
	*/
	if		(F_CPU <=  6400000UL) { SetBusSpeed(0, 0); }
	else if	(F_CPU ==  8000000UL) { SetBusSpeed(2, 0); }
	else if	(F_CPU == 12000000UL) { SetBusSpeed(7, 0); }
	else if	(F_CPU == 16000000UL) { SetBusSpeed(12, 0); }	//(3, 1)でもok
	//else if ...
	else						  { SetBusSpeed(17, 0); }
#endif
}

//スタートを送信する
//戻り値	非0:成功, 0:失敗
uint8_t	TWI_Start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	return (Status == STM_START || Status == STM_REPEAT);
}

//バスエラー発生確認と対応処理
static	void	BusErrorCheck(void)
{
	//バスエラー発生時は TWCR:TWINT=1, TWCR:TWSTO=1 とする（ストップを送信すればよい）
	if (Status == STM_BUS_ERROR) { TWI_Stop(); }
}

//スレーブデバイスのアドレス／任意のデータを送信する
//戻り値:	ステータス
static	uint8_t	MasterTransmit(uint8_t byteValue)
{
	TWDR = byteValue;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	BusErrorCheck();
	return Status;
}

//スレーブデバイスのアドレスを指定する：書き込み方向
//引数:	スレーブアドレス(7bit)
//戻り値	非0:成功, 0:失敗
uint8_t	TWI_SlaveAddrW(uint8_t sla)
{
	return (MasterTransmit((sla << 1) | SLA_W_BIT) == STMT_SLAW_ACK);
}

//スレーブデバイスのアドレスを指定する：読み込み方向
//引数:	スレーブアドレス(7bit)
//戻り値	非0:成功, 0:失敗
uint8_t	TWI_SlaveAddrR(uint8_t sla)
{
	return (MasterTransmit((sla << 1) | SLA_R_BIT) == STMR_SLAR_ACK);
}

//データを送信する
//戻り値	非0:成功, 0:失敗
uint8_t	TWI_WriteByte(uint8_t data)
{
	return (MasterTransmit(data) == STMT_DATA_ACK);
}

//データを受信する
//引数	isLastOne:	1:この読み込みが最後(NACKを返す), 0:この後にまだ読み込む(ACKを返す)
//		data:	読み込んだ値が格納される
//戻り値	非0:成功, 0:失敗
//
//【例：I2C-EEPROMのシーケンシャル読み込み】
//・ ACKを返す… データを1個受信し、続けて次のデータを要求する場合。
//・NACKを返す… データを1個受信して完了する場合（次にストップを送信する）。
uint8_t	TWI_ReadByte(uint8_t isLastOne, uint8_t* data)
{
	uint8_t retAck = isLastOne ? 0 : 1;
	
	//スレーブの動作を決める
	//・今回のデータ受信のあと次もデータを要求するか（ACKを返す）、
	//	次は終了を通知するか（NACKを返す）、を指定する。
	TWCR = (1 << TWINT) | (retAck << TWEA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	BusErrorCheck();
	
	//受信した値を取得する
	//・受信動作の成否によらずレジスタの値を取得する。
	*data = TWDR;
	
	return (Status == (retAck ? STMR_DATA_ACK : STMR_DATA_NACK));	//受信動作の成否はここで判定される
}

//ストップを送信する
void	TWI_Stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

//TWI機能を有効化する（TWIバスを待機状態にする）
//・SCL,SDAピンをI/Oピンとして利用している状態から、TWI機能としての利用に切り替える際に呼び出す。
void	TWI_Enable(void)
{
	//TWIバスの待機状態はSCL,SDA共にHi
	DDRC	|= (1 << PORTC5) | (1 << PORTC4);	//SCL,SDAを出力方向に設定
	PORTC	|= (1 << PORTC5) | (1 << PORTC4);	//SCL,SDAにHiを出力
}

//TWI機能を無効化する
//・SCL,SDAピンを解放し、I/Oピンとして利用できるようにする。
//・外部プルアップ抵抗を接続していると思うので、そのことを考慮して利用すること。
void	TWI_Disable(void)
{
	//TWCR &= ~(1 << TWEN);
	TWCR = 0x00;	//単純にこれでよい
}
