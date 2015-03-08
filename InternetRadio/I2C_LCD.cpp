#include "I2C_LDC.h"

// WiringPi が必要です。

/// <summary>
/// コンストラクタ。
/// </summary>
I2C_LCD::I2C_LCD()
{
	_fd = -1;
}

/// <summary>
/// デフォルトのデバイスアドレス（0x3E）でI2C LCDを初期化する。
/// </summary>
/// <param name="fd">I2C Busのファイルディスクリプタ</param>
/// <remarks>
/// デフォルトのデバイスアドレス（0x3E）でI2C LCDを初期化する。
/// </remarks>
void I2C_LCD::init(int fd)
{
	init(fd, LCD_ADDRESS);
}

/// <summary>
/// アドレスを指定して、I2C LCD の初期化する。
/// </summary>
/// <param name="fd">I2C Busのファイルディスクリプタ</param>
/// <param name="lcd_address">I2C LCD のデバイスアドレス</param>
/// <remarks>
/// lcd_addressで指定されたデバイスアドレスででI2C LCDを初期化する。
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

	// 40msec 休止。
	delay(40);
	// Function Set。8bit bus mode, 2-line mode,normal font,normal instruction mode。
	writeByte(LCD_RS_CMD, 0x38);
	// Function Set。extension instruction modeへ。
	writeByte(LCD_RS_CMD, 0x39);
	// Internal OSC frequency(extension instruction mode)設定。
	writeByte(LCD_RS_CMD, 0x14);
	// Contrast set(extension instruction mode)。コントラスト値下位4bit設定。
	writeByte(LCD_RS_CMD, 0x70 | (LCD_CONTRAST & 0xF));
	// Power/ICON/Contrast set(extension instruction mode)。
	// アイコン On,booster On,コントラスト値上位2bit設定。
	writeByte(LCD_RS_CMD, 0x5c | ((LCD_CONTRAST >> 4) & 0x3));
	// Follower control。internal follower on, 
	writeByte(LCD_RS_CMD, 0x6c);
	// 時間待ち。
	delay(300);

	// Function Set。normal instruction mode。
	writeByte(LCD_RS_CMD, 0x38);
	// Display On/Off。Display Onに設定。
	writeByte(LCD_RS_CMD, 0x0c);
	// Clear Display。
	writeByte(LCD_RS_CMD, 0x0c);
	// 時間待ち。
	delay(2);
}

/// <summary>
/// I2C LCD に モード1バイト + データ1バイトを送信する。
/// </summary>
/// <param name="rs">コマンドモード（LCD_RS_CMD）または
/// データモード(LCD_RS_DATA）の選択。</param>
/// <param name="data">送信データ</param>
/// <remarks>
/// I2C LCD に モード（LCD_RS_CMD / LCD_RS_DATA）1バイトと
/// データを1バイト送信する。
/// </remarks>
void I2C_LCD::writeByte(unsigned char rs, unsigned char data)
{
	unsigned char buf[2];

	if (rs == LCD_RS_CMD || rs == LCD_RS_DATA)
	{
		// LCD_RS_CMD ならコマンドモード。LCD_RS_DATA ならデータモード。

		buf[0] = rs;
		buf[1] = data;
		if (write(_fd, buf, 2) != 2)
		{
			printf("Error writeing to i2c slave1\n");
		}
	}
	else
	{
		// rsの指定がLCD_RS_CMD,LCD_RS_DATA以外ならなにもしない。
	}
}

/// <summary>
/// I2C LCD をクリアして、カーソルをホームに戻す。
/// </summary>
void I2C_LCD::clear()
{
	writeByte(LCD_RS_CMD, LCD_CMD_CLEAR);
	delay(2);
	writeByte(LCD_RS_CMD, LCD_CMD_HOME);
	delay(2);
}

/// <summary>
/// I2C LCD のカーソル位置を設定する。
/// </summary>
/// <param name="col">カラム（桁）</param>
/// <param name="row">行</param>
void I2C_LCD::setCursor(unsigned char col, unsigned char row)
{
	unsigned char offset[] = { 0x00, 0x40 };

	if (row > 1)	row = 1;
	if (col > 15)	col = 15;

	writeByte(LCD_RS_CMD, 0x80 | (col + offset[row]));
}

/// <summary>
/// カーソル位置に1文字表示する。
/// </summary>
/// <param name="c">表示する文字</param>
void I2C_LCD::put(unsigned char c)
{
	writeByte(LCD_RS_DATA, c);
}

/// <summary>
/// カーソル位置から文字列を表示する。
/// </summary>
/// <param name="str">null(0x00)終端の文字配列</param>
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
/// アドレスとデータを指定して、アイコン表示設定を行う。
/// </summary>
/// <param name="address_4bit">アドレス（下位4bit）</param>
/// <param name="data_5bit">データ（下位5bit）</param>
void I2C_LCD::setIcon(unsigned char address_4bit, unsigned char data_5bit)
{
	// Function Set. IS=1にセット。extension instruction modeへ。
	writeByte(LCD_RS_CMD, 0x39);
	// Icon Addressを送信。
	writeByte(LCD_RS_CMD, 0x40 | (0x0f & address_4bit));
	// data送信。
	writeByte(LCD_RS_DATA, 0x1f & data_5bit);
	// Function Set. IS=0にセット。normal instruction modeへ。
	writeByte(LCD_RS_CMD, 0x38);
}

/// <summary>
/// 事前定義した定数でアイコンのOn/Offを設定する。
/// </summary>
/// <param name="icon_define">I2C_LCD.h中の#defineを参照</param>
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