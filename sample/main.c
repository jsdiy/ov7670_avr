//	�J����OV7670(FIFO�Ȃ�)����e�X�g
//	�w����t�]�x�H�쎺	https://github.com/jsdiy
//	2025/05	@jsdiy

/*
�y�|�[�g�Ή��z	ATmega328P	x:���݂��Ȃ�(PC6��RESET) -:��
bit		7		6		5		4		3		2		1		0
portB	XTAL2	XTAL1	SCK		MISO	MOSI	CS#		-		XCLK	�N���b�N����,SPI,LCD,�J����
portC	x		*		SCL		SDA		VSYNC	-		PCLK	DC		I2C,�J����,LCD
portD	D7		D6		D5		D4		D3		D2		D1		D0		�J����
*/
/*
�yATmega328P�z
|543210---54321|
|CCCCCCGRVBBBBB|	R=Vref
>              |
|CDDDDDVGBBDDDB|
|601234--675670|

�y�q���[�Y�r�b�g�z	���H��o�אݒ�� Low:01100010(62), Hi:11-11001(D9)
Low: 10100110 (A6)
     ||||++++-- CKSEL[3:0] �V�X�e���N���b�N�I��	//�O�t�����]�l�[�^
     ||++-- SUT[1:0] �N������
     |+-- CKOUT (0:PB0�ɃV�X�e���N���b�N���o��)
     +-- CKDIV8 �N���b�N���������l (1:1/1, 0:1/8)	//�����Ȃ�
*/
/*
�y�r���h���z
Microchip Studio 7 (Version: 7.0.2594 - )
ATmega328P: �O�t��16MHz
�q���[�YLo=0xA6: �N���b�N�����Z���~�b�N�U���q�APB0�ɃN���b�N�o�́B
�œK���I�v�V����: -O3�i�ϋɓI�ȃC�����C���W�J�𔺂��������x�D��̍œK���j

�y�X�V�����z
2025/03	�J���J�n
2025/06	�V���v����
	Program Memory Usage 	:	3930 bytes   12.0 % Full
	Data Memory Usage 		:	653 bytes   31.9 % Full
*/

#include <avr/io.h>
#include <util/delay.h>
#include "twilib.h"
#include "spilib.h"
#include "OV7670.h"
#include "ST77XXlib.h"

//���C���o�b�t�@
//�E�u�z�肷��𑜓x�̕� * 2byte�v�P�ʂŊm�ۂ���iRGB565��1��f2byte�j�B
//�E�o�b�t�@��1���C��������΂悢�B
#define PIXELDATA_BUFSIZE_MAX	(320 * 2)	//QVGA 1���C������z��
static	uint8_t	pixelDataBuf[PIXELDATA_BUFSIZE_MAX];

//�֐�
static	void	App_CbDrawImage(int16_t lineIndex, uint8_t* dataBuffer, int16_t dataLength);

int	main(void)
{
	_delay_ms(200);

	//�ʐM�@�\������
	TWI_MasterInit();
	SPI_MasterInit();

	//LCD������
	GLCD_Initialize();
	GLCD_RotateFlip(MC_Rot90 | MC_YFlip);
	TColor bgColor = GLCD_CreateColor(0x00, 0x00, 0xFF);	//��
	GLCD_ClearScreen(&bgColor);

	//�J����������
	OV7670_Initialize();
	OV7670_SetCameraMode(CamRes_QQVGA);	//CamRes_QVGA����

	while (1)
	{
		//�J�����̉摜��`�悷��
		OV7670_TakePicture(App_CbDrawImage, pixelDataBuf);
	}

	return 0;
}

//�J�����̉摜��`�悷��F�R�[���o�b�N�֐�
static	void	App_CbDrawImage(int16_t lineIndex, uint8_t* dataBuffer, int16_t dataLength)
{
	GLCD_DrawImage(0, lineIndex, OV7670_Width(), 1, dataBuffer, dataLength);
}
