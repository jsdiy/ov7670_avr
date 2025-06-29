//	LCD�h���C�o�[	for ST7735/ST7789(SPI)	��{���C�u����(�ȈՔ�)
//	�w����t�]�x�H�쎺	https://github.com/jsdiy
//	2020/05 - 2025/03	@jsdiy

#ifndef ST77XXLIB_H_
#define ST77XXLIB_H_

//�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|
//	���[�U�[�ɂ��ݒ�
//�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|

//�s���A�T�C��
#define	GLCD_DC_PORT	PORTC
#define	GLCD_DC_DDR		DDRC
#define	GLCD_DC			(1 << PORTC0)	//Lo:Command, Hi:Data/Param

/*	ATmegaX8�nSPI��LCD�Ƃ̃s���A�T�C��
	[SPI]			[LCD]
	PB5:SCK		--	SCL
	PB4:MISO	--	�s�v
	PB3:MOSI	--	SDA
	PB2:SS#		--	CS#
	�C�ӂ̃s��	--	DC
	�C�ӂ̃s��or�s�v	--	RESET#
*/

//LCD�R���g���[���[�i�Е���L���ɂ��A�����̓R�����g�A�E�g����j
//#define	GLCD_ST7735
#define	GLCD_ST7789

//�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|�|

//��ʃT�C�Y�A�I�t�Z�b�g
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

//�F�\����
typedef	struct
{
	uint8_t	hiByte, loByte;	//RGB565�̏�ʃo�C�g�A���ʃo�C�g
}
TColor;

//Memory data access control
typedef	enum
{
	MC_YFlip	= (1 << 7),	//bit7(MY)	MCU���烁�����ւ̏������ݕ����c	0:������, 1:�t�����i�������]�j
	MC_XFlip	= (1 << 6),	//bit6(MX)	MCU���烁�����ւ̏������ݕ����c	0:������, 1:�t�����i�������]�j
	MC_Rot90	= (1 << 5),	//bit5(MV)	MCU���烁�����ւ̏������ݕ����c	0:���������D��, 1:���������D��i90�x��]�j
	MC_RefleshBtoT	= (1 << 4),	//bit4(ML)	LCD�p�l���̃��t���b�V�������c	0:Top�s��Bottom�s����, 1:Bottom�s��Top�s����
	MC_PixOrderBGR	= (1 << 3),	//bit3(RGB)	���������RGB�f�[�^��LCD�p�l����RGB��f�̕��я��̑Ή��c	0:RGB, 1:BGR
	MC_RefleshRtoL	= (1 << 2),	//bit2(MH)	LCD�p�l���̃��t���b�V�������c	0;Left��Right�����, 1:Right��Left�����
	/*
	�E�f�t�H���g��0x00
	�ELCD�̐ݒu�����ɍ��킹�Đ������]�^�������]�^90�x��]������ɂ́AMY/MX/MV�r�b�g���w�肷��΂悢�B
	�E�K���ȐF��\�����Ă݂āARGB���������΂ɉ��߂���Ă�����ARGB�r�b�g���w�肷��B
		���΂ɉ��߂����̂̓h���C�oIC��LCD�p�l���̌����̖��B�v���O�����̖��ł͂Ȃ��B
	�ELCD�p�l���̃��t���b�V��������ύX����K�v������Ƃ���΁ALCD�p�l�����ړ����Ă��ă`�������C�ɂȂ邩�A
		�������ւ̏������ݕ����Ƃ̊֌W�ɂ�����邩�A���炢�Ȃ̂Œʏ��ML/MH�r�b�g�͈ӎ����Ȃ��Ă悢�Ǝv���B
	*/
}
EMadCtl;

//�֐�
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
