#ifndef _I2C_LCD_H_
#define _I2C_LCD_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "wiringPi.h"

#define LCD_ADDRESS (0x3e)	// 0b0111110
#define LCD_CONTRAST 100

#define LCD_RS_CMD (0x00)
#define LCD_RS_DATA (0x40)
#define LCD_CMD_CLEAR (0x01)
#define LCD_CMD_HOME (0x03)

#define LCD_ICON_ANT_ON (0x01)
#define LCD_ICON_ANT_OFF (0x00)
#define LCD_ICON_WAVE_ON (0x21)
#define LCD_ICON_WAVE_OFF (0x20)
#define LCD_ICON_LOCK_ON (0x46)
#define LCD_ICON_LOCK_OFF (0x40)
#define LCD_ICON_BATT_FULL (0x68)
#define LCD_ICON_BATT_MID (0x67)
#define LCD_ICON_BATT_LESS (0x66)
#define LCD_ICON_BATT_EMP (0x69)
#define LCD_ICON_BATT_OFF (0x60)

class I2C_LCD
{
private:
	int _fd;
public:
	I2C_LCD();
	void init(int fd);
	void init(int fd, int lcd_address);
	void writeByte(unsigned char rs, unsigned char data);
	void clear();
	void setCursor(unsigned char col, unsigned char row);
	void put(unsigned char c); 
	void puts(char *str);
	void setIcon(unsigned char address_4bit, unsigned char data_5bit);
	void setIcon(unsigned char icon_define);
};

#endif