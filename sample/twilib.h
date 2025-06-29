//	TWI(I2C)基本操作ライブラリ（マスター用）	for ATmegaシリーズ
//	『昼夜逆転』工作室	https://github.com/jsdiy
//	2019/01 - 2025/03	@jsdiy

#ifndef TWILIB_H_
#define TWILIB_H_

// Defines controlling timing limits
//	TWI_STANDARD_MODE	: TWI STANDARD mode timing limits. SCL <= 100kHz
//	TWI_FAST_MODE		: TWI FAST mode timing limits.     SCL  = 100-400kHz
//	↓どちらか一方を有効にし、他方をコメントアウトする。
//
#define	TWI_STANDARD_MODE
//#define	TWI_FAST_MODE

void	TWI_MasterInit(void);
uint8_t	TWI_Start(void);
uint8_t	TWI_SlaveAddrW(uint8_t sla);
uint8_t	TWI_SlaveAddrR(uint8_t sla);
uint8_t	TWI_WriteByte(uint8_t data);
uint8_t	TWI_ReadByte(uint8_t isLastOne, uint8_t* data);
void	TWI_Stop(void);
void	TWI_Enable(void);
void	TWI_Disable(void);


#endif	//TWILIB_H_

/*	使用例

//データを書き込む
void	WriteToDevice(uint8_t data)
{
	TWI_Start();
	TWI_SlaveAddrW(SlaveAddress);
	TWI_WriteByte(data);
	//TWI_WriteByte(data2);
	//TWI_WriteByte(data3);
	//...
	TWI_Stop();
}

//指定したアドレスから値を読み出す
uint8_t	ReadFromDevice(RegAddr addr)
{
	TWI_Start();
	TWI_SlaveAddrW(SlaveAddress);
	TWI_WriteByte(addr);	//アドレスが16bitならこのように→ TWI_WriteByte(addrH); TWI_WriteByte(addrL);
	TWI_Stop();
	
	uint8_t data;
	TWI_Start();
	TWI_SlaveAddrR(SlaveAddress);
	TWI_ReadByte(1, &data);	//'1'はtrueの意味
	TWI_Stop();

	return data;
}

//指定したアドレスから連続してデータを読み込む
void	ReadFromDevice(uint8_t addr, uint8_t readLength, uint8_t* retBuf)
{
	TWI_Start();
	TWI_SlaveAddrW(SlaveAddress);
	TWI_WriteByte(addr);	//アドレスが16bitならこのように→ TWI_WriteByte(addrH); TWI_WriteByte(addrL);
	TWI_Stop();
	
	TWI_Start();
	TWI_SlaveAddrR(SlaveAddress);
	for (uint8_t i = 0; i < readLength; i++)
	{
		TWI_ReadByte((i == readLength - 1), retBuf + i);
	}
	TWI_Stop();
}

*/
