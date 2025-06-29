//	LCD�h���C�o�[	for ST7735/ST7789(SPI)	��{���C�u����(�ȈՔ�)
//	�w����t�]�x�H�쎺	https://github.com/jsdiy
//	2020/05 - 2025/03	@jsdiy

#include <avr/io.h>
#include <util/delay.h>
#include "ST77XXlib.h"
#include "spilib.h"

#define	PIN_DC_COMMAND()	(GLCD_DC_PORT &= ~GLCD_DC)	//D/C:Lo ���R�}���h
#define	PIN_DC_DATA()		(GLCD_DC_PORT |= GLCD_DC)	//D/C:Hi ���f�[�^/�R�}���h�̃p�����[�^

#define HiByte(a)	(((a) >> 8) & 0xFF)
#define LoByte(a)	((a) & 0xFF)

//ST77xx�̃R�}���h
typedef	enum
{
	CmdSWRESET	= 0x01,	//Software reset
	CmdSLPOUT	= 0x11,	//Sleep out & booster on
	CmdINVON	= 0x21,	//Display Inversion On	�i�F���]�BRGB_00:00:00����, FF:FF:FF�����j
	CmdDISPOFF	= 0x28,	//Display off
	CmdDISPON	= 0x29,	//Display on
	CmdCASET	= 0x2A,	//Column address set
	CmdRASET	= 0x2B,	//Row address set
	CmdRAMWR	= 0x2C,	//Memory write
	CmdMADCTL	= 0x36,	//Memory data access control
	CmdCOLMOD	= 0x3A	//Interface pixel format
}
ECommand;

//�ϐ�
static	int16_t	screenWidth, screenHeight;
static	int16_t	screenOffsetX, screenOffsetY;

//�֐�
static	void	TransmitByte(uint8_t data);
static	void	WriteCommand(ECommand cmd);
static	void	WriteCommandP1(ECommand cmd, uint8_t p1);
static	void	WriteCommandP4(ECommand cmd, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4);

//������
void	GLCD_Initialize(void)
{
	GLCD_DC_DDR		|= GLCD_DC;	//�o�͕���
	GLCD_DC_PORT	|= GLCD_DC;	//Hi���o��

	//��ʃT�C�Y�ݒ�
	screenWidth = GLCD_SCREEN_WIDTH;
	screenHeight = GLCD_SCREEN_HEIGHT;
	screenOffsetX = GLCD_SCREEN_OFFSET_X;
	screenOffsetY = GLCD_SCREEN_OFFSET_Y;

	SPI_SlaveSelect(ESpi_Lcd);

	//�}�C�R���̃��Z�b�g����z�肵�ALCD�����Z�b�g����
	//�ECmdSWRESET�ɂ�背�W�X�^�փf�t�H���g�l���Z�b�g����̂�5ms������A���̌�X���[�v�E�C����ԂɂȂ�B
	//	���̏�Ԃ���CmdSLPOUT�R�}���h�𑗂�O��120ms�҂K�v������B
	WriteCommand(CmdSWRESET);	_delay_ms(200);	//�\�t�g�E�F�A�E���Z�b�g

	//�X���[�v����
	//�ECmdSLPOUT�R�}���h�𑗂�����ACmdSLPIN�R�}���h�𑗂�O�ɂ�120ms�҂K�v������B
	WriteCommand(CmdSLPOUT);

	//Memory data access control
	WriteCommandP1(CmdMADCTL, 0x00);

	//�F�[�x
#ifdef GLCD_ST7735
	/*
	0x03:	12-bit/pixel	RGB=4:4:4bit
	0x05:	16-bit/pixel	RGB=5:6:5bit
	0x06:	18-bit/pixel	RGB=6:6:6bit	<-default
	*/
	WriteCommandP1(CmdCOLMOD, 0x05);
#endif
#ifdef GLCD_ST7789
	/*	�f�[�^�V�[�g	9.1.32 COLMOD (3Ah): Interface Pixel Format
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

	//�F���]�i���ꂪ�K�v����LCD���W���[���ɂ��j
	//�E�莝����ST7735�ł͕s�v�AST7789�ł͕K�v�������B
#ifdef GLCD_ST7789
	WriteCommand(CmdINVON);
#endif

	//��ʕ\���I��
	WriteCommand(CmdDISPON);

	SPI_SlaveDeselect(ESpi_Lcd);
}

//��ʂ���]�^���]������
void	GLCD_RotateFlip(EMadCtl param)
{
	SPI_SlaveSelect(ESpi_Lcd);
	WriteCommandP1(CmdMADCTL, param);
	SPI_SlaveDeselect(ESpi_Lcd);

	//90�x��]�̏ꍇ�A�c������������
	if (param & MC_Rot90)
	{
		screenWidth = GLCD_SCREEN_HEIGHT;
		screenHeight = GLCD_SCREEN_WIDTH;
		screenOffsetX = GLCD_SCREEN_OFFSET_Y;
		screenOffsetY = GLCD_SCREEN_OFFSET_X;
	}
}

//LCD��byte�l�𑗐M����
static	void	TransmitByte(uint8_t data)
{
	SPI_MasterTransmit(data);
}

//LCD�ɃR�}���h���o�͂���
static	void	WriteCommand(ECommand cmd)
{
	PIN_DC_COMMAND();
	TransmitByte(cmd);
}

//LCD�ɃR�}���h�ƃp�����[�^���o�͂���
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

//LCD�ɐF�f�[�^���o�͂���
void	GLCD_WriteColor(const TColor* color, int32_t repeatCount)
{
	while (repeatCount--)
	{
		TransmitByte(color->hiByte);
		TransmitByte(color->loByte);
	}
}

//�F���쐬����
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

//LCD�ɕ`��̈���w�肷��i�N���b�s���O�͍l������Ȃ��j
//����	x,y:	��ʏ�̕`��J�n���W
//		w,h:	�c���T�C�Y
void	GLCD_SetDrawArea(int16_t x, int16_t y, int16_t w, int16_t h)
{
	//�`��RAM��̎n�_�E�I�_���w�肷��
	//�E�����̈Ӗ��F(cmd, 16bit�l�̏��8bit, 16bit�l�̉���8bit, 16bit�l�̏��8bit, 16bit�l�̉���8bit)
	int16_t startX = x + screenOffsetX;
	int16_t startY = y + screenOffsetY;
	int16_t endX = startX + w - 1;
	int16_t endY = startY + h - 1;
	WriteCommandP4(CmdCASET, HiByte(startX), LoByte(startX), HiByte(endX), LoByte(endX));
	WriteCommandP4(CmdRASET, HiByte(startY), LoByte(startY), HiByte(endY), LoByte(endY));

	WriteCommand(CmdRAMWR);
	PIN_DC_DATA();
}

//��ʑS�̂�h��Ԃ�
void	GLCD_ClearScreen(const TColor* color)
{
	GLCD_FillRect(0, 0, screenWidth, screenHeight, color);
}

//��`��h��Ԃ�
void	GLCD_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, const TColor* color)
{
	SPI_SlaveSelect(ESpi_Lcd);
	GLCD_SetDrawArea(x, y, w, h);
	int32_t pixelCount = (int32_t)w * (int32_t)h;
	GLCD_WriteColor(color, pixelCount);
	SPI_SlaveDeselect(ESpi_Lcd);
}

//�摜��`��
//����:	x,y:	�`���̍��W
//		w,h:	�摜�̕��E����
//		buf:	RGB565����ʃo�C�g�E���ʃo�C�g�̏��Ɋi�[���ꂽ�z��
//		bufLength:	buf�z��̃T�C�Y�iRGB565��1��f2byte�Ȃ̂ŕK�v�ȃf�[�^�̒�����w*h*2byte�j
//���ӁF	�摜�͉�ʓ��Ɏ��܂��Ă��邱�Ɓi�N���b�s���O���l�����Ȃ��̂Łj
void	GLCD_DrawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t* buf, int32_t bufLength)
{
	SPI_SlaveSelect(ESpi_Lcd);
	GLCD_SetDrawArea(x, y, w, h);
	for (int32_t i = 0; i < bufLength; i++) { TransmitByte(buf[i]); }
	SPI_SlaveDeselect(ESpi_Lcd);
}

//��ʃT�C�Y���擾����(glcdtextlib����Ă΂��)
void	GLCD_GetScreenSize(int16_t* width, int16_t* height)
{
	*width = screenWidth;
	*height = screenHeight;
}
