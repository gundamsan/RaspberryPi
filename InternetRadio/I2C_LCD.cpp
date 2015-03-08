#include "I2C_LDC.h"

// WiringPi ���K�v�ł��B

/// <summary>
/// �R���X�g���N�^�B
/// </summary>
I2C_LCD::I2C_LCD()
{
	_fd = -1;
}

/// <summary>
/// �f�t�H���g�̃f�o�C�X�A�h���X�i0x3E�j��I2C LCD������������B
/// </summary>
/// <param name="fd">I2C Bus�̃t�@�C���f�B�X�N���v�^</param>
/// <remarks>
/// �f�t�H���g�̃f�o�C�X�A�h���X�i0x3E�j��I2C LCD������������B
/// </remarks>
void I2C_LCD::init(int fd)
{
	init(fd, LCD_ADDRESS);
}

/// <summary>
/// �A�h���X���w�肵�āAI2C LCD �̏���������B
/// </summary>
/// <param name="fd">I2C Bus�̃t�@�C���f�B�X�N���v�^</param>
/// <param name="lcd_address">I2C LCD �̃f�o�C�X�A�h���X</param>
/// <remarks>
/// lcd_address�Ŏw�肳�ꂽ�f�o�C�X�A�h���X�ł�I2C LCD������������B
/// </remarks>
void I2C_LCD::init(int fd, int lcd_address)
{
	if (fd > 0)	
		_fd = fd;
	else
	{
		printf("File Descriptor Error.");
		exit(-1);
	}

	if (ioctl(_fd, I2C_SLAVE, lcd_address) < 0)
	{
		printf("Unable to get bus access to tolk to slave\n");
		exit(-1);
	}

	// 40msec �x�~�B
	delay(40);
	// Function Set�B8bit bus mode, 2-line mode,normal font,normal instruction mode�B
	writeByte(LCD_RS_CMD, 0x38);
	// Function Set�Bextension instruction mode�ցB
	writeByte(LCD_RS_CMD, 0x39);
	// Internal OSC frequency(extension instruction mode)�ݒ�B
	writeByte(LCD_RS_CMD, 0x14);
	// Contrast set(extension instruction mode)�B�R���g���X�g�l����4bit�ݒ�B
	writeByte(LCD_RS_CMD, 0x70 | (LCD_CONTRAST & 0xF));
	// Power/ICON/Contrast set(extension instruction mode)�B
	// �A�C�R�� On,booster On,�R���g���X�g�l���2bit�ݒ�B
	writeByte(LCD_RS_CMD, 0x5c | ((LCD_CONTRAST >> 4) & 0x3));
	// Follower control�Binternal follower on, 
	writeByte(LCD_RS_CMD, 0x6c);
	// ���ԑ҂��B
	delay(300);

	// Function Set�Bnormal instruction mode�B
	writeByte(LCD_RS_CMD, 0x38);
	// Display On/Off�BDisplay On�ɐݒ�B
	writeByte(LCD_RS_CMD, 0x0c);
	// Clear Display�B
	writeByte(LCD_RS_CMD, 0x0c);
	// ���ԑ҂��B
	delay(2);
}

/// <summary>
/// I2C LCD �� ���[�h1�o�C�g + �f�[�^1�o�C�g�𑗐M����B
/// </summary>
/// <param name="rs">�R�}���h���[�h�iLCD_RS_CMD�j�܂���
/// �f�[�^���[�h(LCD_RS_DATA�j�̑I���B</param>
/// <param name="data">���M�f�[�^</param>
/// <remarks>
/// I2C LCD �� ���[�h�iLCD_RS_CMD / LCD_RS_DATA�j1�o�C�g��
/// �f�[�^��1�o�C�g���M����B
/// </remarks>
void I2C_LCD::writeByte(unsigned char rs, unsigned char data)
{
	unsigned char buf[2];

	if (rs == LCD_RS_CMD || rs == LCD_RS_DATA)
	{
		// LCD_RS_CMD �Ȃ�R�}���h���[�h�BLCD_RS_DATA �Ȃ�f�[�^���[�h�B

		buf[0] = rs;
		buf[1] = data;
		if (write(_fd, buf, 2) != 2)
		{
			printf("Error writeing to i2c slave1\n");
		}
	}
	else
	{
		// rs�̎w�肪LCD_RS_CMD,LCD_RS_DATA�ȊO�Ȃ�Ȃɂ����Ȃ��B
	}
}

/// <summary>
/// I2C LCD ���N���A���āA�J�[�\�����z�[���ɖ߂��B
/// </summary>
void I2C_LCD::clear()
{
	writeByte(LCD_RS_CMD, LCD_CMD_CLEAR);
	delay(2);
	writeByte(LCD_RS_CMD, LCD_CMD_HOME);
	delay(2);
}

/// <summary>
/// I2C LCD �̃J�[�\���ʒu��ݒ肷��B
/// </summary>
/// <param name="col">�J�����i���j</param>
/// <param name="row">�s</param>
void I2C_LCD::setCursor(unsigned char col, unsigned char row)
{
	unsigned char offset[] = { 0x00, 0x40 };

	if (row > 1)	row = 1;
	if (col > 15)	col = 15;

	writeByte(LCD_RS_CMD, 0x80 | (col + offset[row]));
}

/// <summary>
/// �J�[�\���ʒu��1�����\������B
/// </summary>
/// <param name="c">�\�����镶��</param>
void I2C_LCD::put(unsigned char c)
{
	writeByte(LCD_RS_DATA, c);
}

/// <summary>
/// �J�[�\���ʒu���當�����\������B
/// </summary>
/// <param name="str">null(0x00)�I�[�̕����z��</param>
void I2C_LCD::puts(char *str)
{
	int i;
	for (i = 0; i < 16; i++)
	{
		if (str[i] == 0x00)
		{
			break;
		}
		else
		{
			put((unsigned char)str[i]);
		}
	}
}

/// <summary>
/// �A�h���X�ƃf�[�^���w�肵�āA�A�C�R���\���ݒ���s���B
/// </summary>
/// <param name="address_4bit">�A�h���X�i����4bit�j</param>
/// <param name="data_5bit">�f�[�^�i����5bit�j</param>
void I2C_LCD::setIcon(unsigned char address_4bit, unsigned char data_5bit)
{
	// Function Set. IS=1�ɃZ�b�g�Bextension instruction mode�ցB
	writeByte(LCD_RS_CMD, 0x39);
	// Icon Address�𑗐M�B
	writeByte(LCD_RS_CMD, 0x40 | (0x0f & address_4bit));
	// data���M�B
	writeByte(LCD_RS_DATA, 0x1f & data_5bit);
	// Function Set. IS=0�ɃZ�b�g�Bnormal instruction mode�ցB
	writeByte(LCD_RS_CMD, 0x38);
}

/// <summary>
/// ���O��`�����萔�ŃA�C�R����On/Off��ݒ肷��B
/// </summary>
/// <param name="icon_define">I2C_LCD.h����#define���Q��</param>
void I2C_LCD::setIcon(unsigned char icon_define)
{
	switch (icon_define)
	{
	case LCD_ICON_ANT_ON:
		setIcon(0x00, 0x10);
		break;
	case LCD_ICON_ANT_OFF:
		setIcon(0x00, 0x00);
		break;
	case LCD_ICON_WAVE_ON:
		setIcon(0x04, 0x10);
		break;
	case LCD_ICON_WAVE_OFF:
		setIcon(0x04, 0x00);
		break;
	case LCD_ICON_LOCK_ON:
		setIcon(0x09, 0x10);
		break;
	case LCD_ICON_LOCK_OFF:
		setIcon(0x09, 0x00);
		break;
	case LCD_ICON_BATT_FULL:
		setIcon(0x0d, 0x1e);
		break;
	case LCD_ICON_BATT_MID:
		setIcon(0x0d, 0x1a);
		break;
	case LCD_ICON_BATT_LESS:
		setIcon(0x0d, 0x12);
		break;
	case LCD_ICON_BATT_EMP:
		setIcon(0x0d, 0x02);
		break;
	case LCD_ICON_BATT_OFF:
		setIcon(0x0d, 0x00);
		break;
	default:
		break;
	}
}