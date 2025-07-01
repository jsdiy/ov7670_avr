//	OV7670 Register
//	�w����t�]�x�H�쎺	https://github.com/jsdiy
//	2025/03	@jsdiy
//
//	linux�ł��x�[�X�ɁAQVGA,QQVGA,RGB565�ɑΉ��������W�X�^�ݒ�	��linux�ł�VGA,YUV�Œ�
//	https://github.com/torvalds/linux/blob/master/drivers/media/i2c/ov7670.c

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "OV7670Register.h"

//��{�ݒ�i�𑜓x/�J���[���[�h�ɂ��Ȃ����W�X�^�l�j
const	regval_list	OV7670_default_regs[]	PROGMEM =
{
	//	�f�t�H���g�l��0x0D=0b00001101.	Bit[2:1](0bxxxxx10x)��Reserved.
	//	Bit[3](UYVY or VYUY)��Bit[0](Auto output window)��0�Ƃ���̂ŁA0b00000100=0x04�ƂȂ�
	{ REG_TSLB, 0x04 },	// Line Buffer Test Option

	//linux�ł�VGA�Œ�Ȃ̂�0x00��ݒ肵�Ă��邪�A�𑜓x�ύX�ɑΉ�����ꍇ��DCWEN��ݒ肷��K�v����
	{ REG_COM3, COM3_DCWEN },

	{ REG_CLKRC, CLK_RSVD | 0x00 },	//0x00:�����Ȃ�	�����Ƃ���ύX����

	// Gamma curve values
	/*	linux�ł͉��L�ƂȂ��Ă��邪�A�F���̓f�B�X�v���C���̒�����l�̍D�݂ɂ��̂ŁA
		�����ł͐ݒ肵�Ȃ����ƂƂ���i�f�t�H���g�l�Ƃ���j
	{ REG_SLOP, 0x20 }, { REG_GAM1, 0x10 },
	{ REG_GAM2, 0x1e }, { REG_GAM3, 0x35 },
	{ REG_GAM4, 0x5a }, { REG_GAM5, 0x69 },
	{ REG_GAM6, 0x76 }, { REG_GAM7, 0x80 },
	{ REG_GAM8, 0x88 }, { REG_GAM9, 0x8f },
	{ REG_GAM10, 0x96 }, { REG_GAM11, 0xa3 },
	{ REG_GAM12, 0xaf }, { REG_GAM13, 0xc4 },
	{ REG_GAM14, 0xd7 }, { REG_GAM15, 0xe8 },
	*/

	// AGC and AEC parameters.  Note we start by disabling those features,
	//	then turn them only after tweaking the values.
	//	�� AGC(Auto Gain Celling), AEC(Auto Exposure(�I�o) Control)
	//{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT },	��REG_COM8��3��ݒ肵�Ă���̂ŁA�Ō�ȊO�͍폜����i�掿�ɉe���͂Ȃ������j
	{ REG_GAIN, 0x00 },
	{ REG_AECH, 0x00 },
	{ REG_COM4, 0x40 }, // magic reserved bit	��linux�łł�COM4/COM17�Ƃ�AEC_FULL�ƂȂ��Ă���
	{ REG_COM9, COM9_AGC_GAIN_4x | 0x08 }, // 4x gain | magic rsvd bit

	{ REG_BD50MAX, 0x05 },
	{ REG_BD60MAX, 0x07 },

	{ REG_AEW, 0x95 },	{ REG_AEB, 0x33 },
	{ REG_VPT, 0xe3 },	{ REG_HAECC1, 0x78 },
	{ REG_HAECC2, 0x68 },	{ 0xa1, 0x03 },	// magic
	{ REG_HAECC3, 0xd8 },	{ REG_HAECC4, 0xd8 },
	{ REG_HAECC5, 0xf0 },	{ REG_HAECC6, 0x90 },
	{ REG_HAECC7, 0x94 },
	//{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AEC },

	// Almost all of these are magic "reserved" values.
	//	��OV7670�ł́ureserved�v�͗\��ςƂ������u����J�v�Ƃ������Ӗ��Ŏg���Ă���
	{ REG_COM5, 0x61 },	// Reserved
	{ REG_COM6, 0x4b },	// Reset all timing when format changes
	/*	�f�t�H���g�l��XX�ƂȂ��Ă���B�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ 0x16, 0x02 },		// Reserved
	*/
	{ REG_MVFP, 0x07 },	// �����������]	��linux�łł͔��]�Ȃ�
	/*	�f�t�H���g�l��XX��������Abit[7:0]Reserved��������B�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ REG_ADCCTR1, 0x02 },// Reserved
	{ REG_ADCCTR2, 0x91 },// Reserved
	{ 0x29, 0x07 }, { 0x33, 0x0b }, { 0x35, 0x0b }, { 0x37, 0x1d }, { 0x38, 0x71 }, { 0x39, 0x2a },	// All Reserved
	*/
	{ REG_COM12, 0x78 },
	/*	�f�t�H���g�l��XX�ƂȂ��Ă���B�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ 0x4d, 0x40 }, { 0x4e, 0x20 },	// Reserved
	*/
	/*	REG_GFIX�̃f�t�H���g�l��0x00�ƂȂ��Ă���B����āA�ݒ肷��K�v���Ȃ��̂ō폜����B
	{ REG_GFIX, 0x00 },	// AWB Pre gain control
	*/
	{ REG_DBLV, DBLV_BYPASS },	// PLL control,Regulator control
	{ REG_REG74, 0x19 },	// Digital gain manual control
	/*	�f�t�H���g�l��XX�ƂȂ��Ă���B�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ 0x8d, 0x4f }, { 0x8e, 0x00 }, { 0x8f, 0x00 }, { 0x90, 0x00 }, { 0x91, 0x00 },	// Reserved
	*/
	/*	REG_DM_LNL(Dummy Row low 8bit)�̃f�t�H���g�l��0x00.	REG_DM_LNH(Dummy Row high 8bit)�͐ݒ肵�Ă��Ȃ��B
		����āAREG_DM_LNL��0x00�ɐݒ肷��K�v���Ȃ��̂ō폜����B
	{ REG_DM_LNL, 0x00 },
	*/
	/*	�f�t�H���g�l��XX�ƂȂ��Ă���B�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ 0x96, 0x00 }, { 0x9a, 0x80 },
	*/
	/*	0xb0,0xb2,0xb8�̓��W�X�^�����Ȃ�(RSVD)�A�f�t�H���g�l��XX�ƂȂ��Ă���B
		0xb0�́A���̐ݒ���폜����ƐF�������i�ԁE�������Ȃ����悤�ȐF�ɂȂ�j�̂ō폜���Ȃ��B
		0xb2,0xb8�́A�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ 0xb0, 0x84 },
	{ REG_ABLC1, 0x0c },
	{ 0xb2, 0x0e },
	{ REG_THL_ST, 0x82 },
	{ 0xb8, 0x0a },
	*/
	{ 0xb0, 0x84 },
	{ REG_ABLC1, 0x0c },	//REG_ABLC1=0xB1
	{ REG_THL_ST, 0x82 },	//REG_THL_ST=0xB3

	// More reserved magic, some of which tweaks white balance
	/*	linux�ł͉��L�ƂȂ��Ă��邪�A�F���̓f�B�X�v���C���̒�����l�̍D�݂ɂ��̂ŁA
		�����ł͐ݒ肵�Ȃ����ƂƂ���i�f�t�H���g�l�Ƃ���j
	{ REG_AWBC1, 0x0A },	//AWB Control 1	
	{ REG_AWBC2, 0xf0 },	//AWB Control 2
	{ REG_AWBC3, 0x34 },	//AWB Control 3
	{ REG_AWBC4, 0x58 },	//AWB Control 4
	{ REG_AWBC5, 0x28 },	//AWB Control 5
	{ REG_AWBC6, 0x3a },	//AWB Control 6
	{ REG_AWBC7, 0x88 },	//AWB Control 7
	{ REG_AWBC8, 0x88 },	//AWB Control 8
	{ REG_AWBC9, 0x44 },	//AWB Control 9
	{ REG_AWBC10, 0x67 },	//AWB Control 10
	{ REG_AWBC11, 0x49 },	//AWB Control 11
	{ REG_AWBC12, 0x0e },	//AWB Control 12
	*/
	{ REG_AWBCTR3, 0x0a },
	{ REG_AWBCTR2, 0x55 },
	{ REG_AWBCTR1, 0x11 },
	{ REG_AWBCTR0, 0x9f },	// "9e for advance AWB"

	// linux�łł͉��L	���f�t�H���g�l���Ɣ����΂ɋ���������F��������
	{ REG_GGAIN,0x40 },	// AWB Green gain		   default:00
	{ REG_BLUE, 0x40 },	// AWB Blue gain	(00-ff)default:80
	{ REG_RED,	0x60 },	// AWB Red gain		(00-ff)default:80

	{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT | COM8_AGC | COM8_AEC | COM8_AWB },

	//linux�łɂ͂Ȃ��ݒ肾���AESP32�����̐ݒ�ɍ��킹���i����VS_NEG��ESP32���L�j
	{ REG_COM10, COM10_PCLK_HB | COM10_VS_NEG },	// PCLK does not toggle during horizontal blank & VSYNC negative

	/*	�F�ϊ��}�g���N�X�̓J���[���[�h(RGB/YUV)�ɂ��قȂ�̂ŁA
		�J���[���[�h�ݒ莞�ɐF�ϊ��}�g���N�X��ݒ肷�邱�ƂƂ���i�����ł͏ȗ�����j
		�����L�Alinux�łł�YUV���̐F�ϊ��}�g���N�X���ݒ肳��Ă���
	// Matrix coefficients
	{ REG_MTX1, 0x80 },	// Matrix Coefficient 1
	{ REG_MTX2, 0x80 },	// Matrix Coefficient 2
	{ REG_MTX3, 0x00 },	// Matrix Coefficient 3
	{ REG_MTX4, 0x22 },	// Matrix Coefficient 4
	{ REG_MTX5, 0x5e },	// Matrix Coefficient 5
	{ REG_MTX6, 0x80 },	// Matrix Coefficient 6
	{ REG_MTXS, 0x9e },	// Matrix Coefficient Sign
	*/

	{ REG_COM16, COM16_AWBGAIN },	// AWB gain enable
	{ REG_EDGE,	0x00 },	// Edge enhancement factor
	{ REG_REG75, 0x05 },	// Edge enhanced lower limit
	{ REG_REG76, 0xe1 },	// Edge enhanced higher limit ,Black/white pixcel correction enable
	{ REG_DNSTH, 0x00 },	// De-noise Threshold
	{ REG_REG77, 0x01 },	// Offset, de-noise range control
	{ REG_COM13, 0xC3 },	// Gamma enable, UV saturation auto adjustment	��bit[0]�̓J���[���[�hYUV��UV-swap
	/*	REG_4B��[7:1]��Reserved�ŁA[0]�� UV average enable �Ƃ���Ă���B
		���W�X�^�̃f�t�H���g�l��0x00�����ALinux�ł�0x09��ݒ肵�Ă���A����ݒ肵�Ă���̂��͕s���B
		�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ REG_4B, 0x09 },
	*/
	{ REG_SATCTR, 0x60 },	// UV saturatin control min
	{ REG_COM16, COM16_YUV_ENHANC | COM16_DE_NOISE | COM16_AWBGAIN },
	{ REG_CONTRAS, 0x40 },	// Contrast Control

	/*	REG_ARBLM�͑S�r�b�g��Reserved�Ńf�t�H���g�l��0x11�ƂȂ��Ă���B
		����āA�ݒ肷��K�v���Ȃ��̂ō폜����B
	{ REG_ARBLM, 0x11 },
	*/
	{ REG_COM11, COM11_EXP | COM11_HZAUTO },	// Exposure timing can be less than limit of banding filter when light is too strong	��50/60Hz�������o
	{ REG_NT_CTRL, 0x80 | NT_CTRL_ROWPF | NT_CTRL_DMR_2x },	// Auto frame rate adjustment dummy row selection	��0x80��reserved���ꂽ�l
	/*	Reserved���W�X�^�Q�B
		�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ 0x96, 0x00 }, { 0x97, 0x30 },
	{ 0x98, 0x20 }, { 0x99, 0x30 },
	{ 0x9a, 0x84 }, { 0x9b, 0x29 },
	{ 0x9c, 0x03 },
	*/
	/*	REG_COM11��COM11_HZAUTO�r�b�g�𗧂ĂĂ���̂ŁA�����Ńf�t�H���g�l(�e0x99/0x7F)�ƈقȂ�l��ݒ肵�Ă��Ӗ��͂Ȃ��B
		�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ REG_BD50ST, 0x4c },
	{ REG_BD60ST, 0x3f },
	*/
	/*	0x78�̓��W�X�^�����Ȃ�(RSVD)�A�f�t�H���g�l��XX�ƂȂ��Ă���B�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	{ 0x78, 0x04 },
	*/

	/*	linux�ł́u����Ȃ��́v�Ƃ��ē��e�������炸���L��ݒ肵�Ă��邪�A
		�����Ă��掿�ɉe���͌����Ȃ������̂ō폜����B
	// Extra-weird stuff.  Some sort of multiplexor register
	{ 0x79, 0x01 }, { 0xc8, 0xf0 },
	{ 0x79, 0x0f }, { 0xc8, 0x00 },
	{ 0x79, 0x10 }, { 0xc8, 0x7e },
	{ 0x79, 0x0a }, { 0xc8, 0x80 },
	{ 0x79, 0x0b }, { 0xc8, 0x01 },
	{ 0x79, 0x0c }, { 0xc8, 0x0f },
	{ 0x79, 0x0d }, { 0xc8, 0x20 },
	{ 0x79, 0x09 }, { 0xc8, 0x80 },
	{ 0x79, 0x02 }, { 0xc8, 0xc0 },
	{ 0x79, 0x03 }, { 0xc8, 0x40 },
	{ 0x79, 0x05 }, { 0xc8, 0x30 },
	{ 0x79, 0x26 },
	*/
	
	{ 0xff, 0xff }	// END MARKER
};

//RGB565
const	regval_list	OV7670_rgb565[]	PROGMEM =
{
	//{ REG_RGB444, 0x00 },	// No RGB444 please	���킴�킴�����K�v�Ȃ�
	{ REG_COM1, 0x00 },
	{ REG_COM15, COM15_R00FF | COM15_RGB565 },	// RGB565
	//{ REG_TSLB, 0x04 },	��default_regs[]�Őݒ肵�Ă���̂ł����ł͕s�v
	//{ REG_COM9, 0x08 },	// 0x08 is reserved bit	��default_regs[]�Őݒ肵�Ă���̂ł����ł͕s�v
	{ REG_MTX1, 0xb3 },	// matrix coefficient 1
	{ REG_MTX2, 0xb3 },	// matrix coefficient 2
	{ REG_MTX3, 0x00 },	// vb
	{ REG_MTX4, 0x3d },	// matrix coefficient 4
	{ REG_MTX5, 0xa7 },	// matrix coefficient 5
	{ REG_MTX6, 0xe4 },	// matrix coefficient 6
	{ REG_MTXS, 0x9e },	// Matrix Coefficient Sign
	{ REG_COM13, COM13_GAMMA | COM13_UVSAT },
	{ 0xff, 0xff }	// END MARKER
};

//YUV442
const	regval_list	OV7670_yuv422[]	PROGMEM =
{
	//{ REG_RGB444, 0x00 },	// No RGB444 please	���킴�킴�����K�v�Ȃ�
	{ REG_COM1, 0x00 },
	{ REG_COM15, COM15_R00FF },
	//{ REG_COM9, 0x08 },	// 0x08 is reserved bit	��default_regs[]�Őݒ肵�Ă���̂ł����ł͕s�v
	{ REG_MTX1, 0x80 },	// matrix coefficient 1
	{ REG_MTX2, 0x80 },	// matrix coefficient 2
	{ REG_MTX3, 0x00 },	// vb
	{ REG_MTX4, 0x22 },	// matrix coefficient 4
	{ REG_MTX5, 0x5e },	// matrix coefficient 5
	{ REG_MTX6, 0x80 },	// matrix coefficient 6
	{ REG_MTXS, 0x9e },	// Matrix Coefficient Sign
	{ REG_COM13, COM13_GAMMA | COM13_UVSAT | COM13_UVSWAP },
	{ 0xff, 0xff }		// END MARKER
};

#if (0)	// ATmega328P��16MHz/64�����ł̓L���v�`�����ǂ����Ȃ�����
//VGA	(640x480)
const	regval_list	OV7670_vga[]	PROGMEM	=
{
	{ REG_COM3,	0x00 },	// No scaling
	{ REG_COM14, 0x00 },
	{ REG_SCALING_XSC, 0x3a },	// Horizontal scale factor	//Mystery scaling numbers
	{ REG_SCALING_YSC, 0x35 },	// Vertical scale factor	//Mystery scaling numbers
	{ REG_SCALING_DCWCTR, 0x00 },	// Down sampling nothing
	{ REG_SCALING_PCLK_DIV, SCALING_PCLK_DIV_RSVD | SCALING_PCLK_DIV_1 },	// DSP scale control Clock divide by 1
	{ REG_SCALING_PCLK_DELAY, 0x02 },
	{ 0xff, 0xff }	// END MARKER
};
#endif

//QVGA	(320x240)
const	regval_list	OV7670_qvga[]	PROGMEM =
{
	{ REG_COM14, COM14_DCWEN | COM14_PCLKDIV_2 },	// divide by 2	��VGA��1/2�k��
	{ REG_SCALING_XSC, 0x3a },	// Horizontal scale factor	//Mystery scaling numbers
	{ REG_SCALING_YSC, 0x35 },	// Vertical scale factor	//Mystery scaling numbers
	{ REG_SCALING_DCWCTR, SCALING_DCWCTR_VDS_by_2 | SCALING_DCWCTR_HDS_by_2 },	// down sampling by 2
	{ REG_SCALING_PCLK_DIV, SCALING_PCLK_DIV_RSVD | SCALING_PCLK_DIV_2 },	// DSP scale control Clock divide by 2
	{ REG_SCALING_PCLK_DELAY, 0x02 },
	{ 0xff, 0xff }	// END MARKER
};

//QQVGA	(160x120)
const	regval_list OV7670_qqvga[]	PROGMEM =
{
	{ REG_COM14, COM14_DCWEN | COM14_MANUAL | COM14_PCLKDIV_4 },	// divide by 4	��VGA��1/4�k��
	{ REG_SCALING_XSC, 0x3a },	// Horizontal scale factor	//Mystery scaling numbers
	{ REG_SCALING_YSC, 0x35 },	// Vertical scale factor	//Mystery scaling numbers
	{ REG_SCALING_DCWCTR, SCALING_DCWCTR_VDS_by_4 | SCALING_DCWCTR_HDS_by_4 },	// down sampling by 4
	{ REG_SCALING_PCLK_DIV, SCALING_PCLK_DIV_RSVD | SCALING_PCLK_DIV_4 },	// DSP scale control Clock divide by 4
	{ REG_SCALING_PCLK_DELAY, 0x02 },
	{ 0xff, 0xff }	// END MARKER

};
