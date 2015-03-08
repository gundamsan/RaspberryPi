/*
        wiring  BCM                     BCM  wiringPi
function  Pi    GPIO Name pin pin Name  GPIO    Pi  function
-------------------------------------------------------------
Vcc   <-+3V3          +3V3 1  2  +5V + 5V
SDA   <-   8  GPIO2   SDA1 3  4  +5V + 5V
SCL   <-   9  GPIO3   SCL1 5  6  GND          GND
           7  GPIO4   GCLK 7  8  TXD0  GPIO14  15
GND   -> GND          GND  9  10 RXD0  GPIO15  16
Enc1_A ->  0  GPIO17  GEN0 11 12 GEN1  GPIO18   1  <- Enc1_B
LED1_R <-  2  GPIO27  GEN2 13 14 GND          GND
LED1_G <-  3  GPIO22  GEN3 15 16 GEN4  GPIO23   4  -> LED1_B
        +3V3          +3V3 17 18 GEN5  GPIO24   5  -> LCD_BKL
          12  GPIO10  MOSI 19 20 GND          GND
          13  GPIO9   MISO 21 22 GEN6  GPIO25   6
          14  GPIO11  SCLK 23 24 CE0_N GPIO8   10
         GND           GND 25 26 CE1_N GPIO7   11
          30  EEPROM ID_SD 27 28 ID_SC EEPROM  31
Enc2_A -> 21  GPIO5        29 30 GND          GND
ENC2_B -> 22  GPIO6        31 32       GPIO12  26
LED2_R <- 23  GPIO13       33 34 GND          GND
LED2_G <- 24  GPIO19       35 36       GPIO16  27
LED2_B <- 25  GPIO26       37 38       GPIO20  28
         GND           GND 39 40       GPIO21  29
------------------------------------------------------------

LEDx_x�[�q��220��������RPi�̊e�[�q�ցBRPi�[�qLOW�œ_���B
Encorder�����A,B���M�����͂�RPi���ŃG�b�W�ω����荞�݂Ŋ֐��Ăяo���ցB
�`���l���p�G���R�[�_�� Enc1_A,B�œ���RGB LED��LED1_R,G,B�B
�{�����[���p�G���R�[�_�� Enc2_A,B�œ���RGB LED��LED2_R,G,B�B
I2C�ɂ̓X�g���i��I2C LCD��ڑ��B
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wiringPi.h"
#include "I2C_LDC.h"

FILE *fp;
char str[256];
int tune_A, tune_B, pre_tune_A, pre_tune_B, tune_num;
int vol_A, vol_B, pre_vol_A, pre_vol_B, volume;

I2C_LCD lcd;

// �`���[�j���O�i�`���l���j�p�G���R�[�_�G�b�W��A���G�b�W�ω����o���荞�݂ŃR�[�������֐��B
void click_tune_A(void)
{
	int _a;

	// WiringPi GPIO_0(11pin)�l�擾�B
	_a = digitalRead(0);
	// �l���ς�����ꍇ�A
	if (_a != tune_A)
	{
		// ��O��A�[�q�̒l���i�[�B
		if (tune_A == 1)
		{
			pre_tune_A = 1;
		}
		else
		{
			pre_tune_A = 0;
		}
		tune_A = _a;

		// A�[�q��B�[�q�̒��O�̒l��1�ŁA���̒l��0�Ȃ�
		if (tune_A == 0 && tune_B == 0 && pre_tune_A == 1 && pre_tune_B == 1)
		{
			printf("left %d\n", --tune_num);

			system("mpc prev > radio_status.txt");
			system("mpc play > radio_status.txt");
			
			system("mpc status > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 16, fp);
			fclose(fp);
			printf("%s\n", str);
			lcd.setCursor(0, 0);
			lcd.puts("                ");
			lcd.setCursor(0, 0);
			lcd.puts(str);

			switch (abs(tune_num % 3))
			{
			case 0:
				digitalWrite(2, LOW);
				digitalWrite(3, HIGH);
				digitalWrite(4, HIGH);
				break;
			case 1:
				digitalWrite(2, HIGH);
				digitalWrite(3, LOW);
				digitalWrite(4, HIGH);
				break;
			case 2:
				digitalWrite(2, HIGH);
				digitalWrite(3, HIGH);
				digitalWrite(4, LOW);
				break;
			default:
				break;
			}
		}
	}
}

// �`���[�j���O�i�`���l���j�p�G���R�[�_�G�b�W��B���G�b�W�ω����o���荞�݂ŃR�[�������֐��B
void click_tune_B(void)
{
	int _b;

	// WiringPi GPIO_1(12pin)�l�擾�B
	_b = digitalRead(1);
	if (_b != tune_B)
	{
		if (tune_B == 1)
		{
			pre_tune_B = 1;
		}
		else
		{
			pre_tune_B = 0;
		}
		tune_B = _b;
		if (tune_A == 0 && tune_B == 0 && pre_tune_A == 1 && pre_tune_B == 1)
		{
			printf("right %d\n", ++tune_num);

			system("mpc next > radio_status.txt");
			system("mpc play > radio_status.txt");

			system("mpc status > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 16, fp);
			fclose(fp);
			printf("%s\n", str);
			lcd.setCursor(0, 0);
			lcd.puts("                ");
			lcd.setCursor(0, 0);
			lcd.puts(str);

			switch (abs(tune_num % 3))
			{
			case 0:
				digitalWrite(2, LOW);
				digitalWrite(3, HIGH);
				digitalWrite(4, HIGH);
				break;
			case 1:
				digitalWrite(2, HIGH);
				digitalWrite(3, LOW);
				digitalWrite(4, HIGH);
				break;
			case 2:
				digitalWrite(2, HIGH);
				digitalWrite(3, HIGH);
				digitalWrite(4, LOW);
				break;
			default:
				break;
			}
		}
	}
}

// �{�����[���p�G���R�[�_�G�b�W��A���G�b�W�ω����o���荞�݂ŃR�[�������֐��B
void click_vol_A(void)
{
	int _a;

	// WiringPi GPIO_21(29pin)�l�擾�B
	_a = digitalRead(21);
	// �l���ς�����ꍇ�A
	if (_a != vol_A)
	{
		// ��O��A�[�q�̒l���i�[�B
		if (vol_A == 1)
		{
			pre_vol_A = 1;
		}
		else
		{
			pre_vol_A = 0;
		}
		vol_A = _a;

		// A�[�q��B�[�q�̒��O�̒l��1�ŁA���̒l��0�Ȃ�
		if (vol_A == 0 && vol_B == 0 && pre_vol_A == 1 && pre_vol_B == 1)
		{
			volume = volume - 1;
			if (volume > 100)	volume = 100;
			sprintf(str, "amixer set PCM %d%% > radio_status.txt", volume);
			system(str);
			system("amixer | egrep '\[.*%\]' > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 30, fp);
			sprintf(str, strchr(str, '['));
			fclose(fp);
			printf("Volume:%s\n", str);
			lcd.setCursor(4, 1);
			lcd.puts("    ");
			lcd.setCursor(4, 1);
			lcd.puts(str);

			switch (abs(volume % 3))
			{
			case 0:
				digitalWrite(23, LOW);
				digitalWrite(24, HIGH);
				digitalWrite(25, HIGH);
				break;
			case 1:
				digitalWrite(23, HIGH);
				digitalWrite(24, LOW);
				digitalWrite(25, HIGH);
				break;
			case 2:
				digitalWrite(23, HIGH);
				digitalWrite(24, HIGH);
				digitalWrite(25, LOW);
				break;
			default:
				break;
			}
		}
	}
}

// �{�����[���p�G���R�[�_�G�b�W��B���G�b�W�ω����o���荞�݂ŃR�[�������֐��B
void click_vol_B(void)
{
	int _b;

	// WiringPi GPIO_22(31pin)�l�擾�B
	_b = digitalRead(22);
	if (_b != vol_B)
	{
		if (vol_B == 1)
		{
			pre_vol_B = 1;
		}
		else
		{
			pre_vol_B = 0;
		}
		vol_B = _b;
		if (vol_A == 0 && vol_B == 0 && pre_vol_A == 1 && pre_vol_B == 1)
		{
			volume = volume + 1;
			if (volume < 0)	volume = 0;
			sprintf(str, "amixer set PCM %d%% > radio_status.txt", volume);
			system(str);
			system("amixer | egrep '\[.*%\]' > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 30, fp);
			sprintf(str, strchr(str, '['));
			fclose(fp);
			printf("Volume:%s\n", str);
			lcd.setCursor(4, 1);
			lcd.puts("    ");
			lcd.setCursor(4, 1);
			lcd.puts(str);

			switch (abs(volume % 3))
			{
			case 0:
				digitalWrite(23, LOW);
				digitalWrite(24, HIGH);
				digitalWrite(25, HIGH);
				break;
			case 1:
				digitalWrite(23, HIGH);
				digitalWrite(24, LOW);
				digitalWrite(25, HIGH);
				break;
			case 2:
				digitalWrite(23, HIGH);
				digitalWrite(24, HIGH);
				digitalWrite(25, LOW);
				break;
			default:
				break;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int setup = 0;
	int fd;
	char *i2cFileName = "/dev/i2c-1";

	// �ϐ��������B
	tune_A = 1;
	tune_B = 1;
	pre_tune_A = 0;
	pre_tune_B = 0;
	tune_num = 0;
	vol_A = 1;
	vol_B = 1;
	pre_vol_A = 0;
	pre_vol_B = 0;
	volume = 80;
	
	// wiringP�̏������B
	setup = wiringPiSetup();

	// GPIO_0(11pin�j���v���A�b�v���āA�G�b�W���o���荞�ݐݒ�B
	pullUpDnControl(0, PUD_UP);
	wiringPiISR(0, INT_EDGE_BOTH, click_tune_A);
	// GPIO_1(12pin�j���v���A�b�v���āA�G�b�W���o���荞�ݐݒ�B
	pullUpDnControl(1, PUD_UP);
	wiringPiISR(1, INT_EDGE_BOTH, click_tune_B);
	// GPIO_21(29pin�j���v���A�b�v���āA�G�b�W���o���荞�ݐݒ�B
	pullUpDnControl(21, PUD_UP);
	wiringPiISR(21, INT_EDGE_BOTH, click_vol_A);
	// GPIO_22(31pin�j���v���A�b�v���āA�G�b�W���o���荞�ݐݒ�B
	pullUpDnControl(22, PUD_UP);
	wiringPiISR(22, INT_EDGE_BOTH, click_vol_B);

	// GPIO_2,3,4,5,23,24,25(13,15,16,18,33,35,37pin)���v���A�b�v�_�E���I�t�ɂ��āA�o�͂ɐݒ�B
	pullUpDnControl(2, PUD_OFF);
	pinMode(2, OUTPUT);
	pullUpDnControl(3, PUD_OFF);
	pinMode(3, OUTPUT);
	pullUpDnControl(4, PUD_OFF);
	pinMode(4, OUTPUT);
	pullUpDnControl(5, PUD_OFF);
	pinMode(5, OUTPUT);
	pullUpDnControl(23, PUD_OFF);
	pinMode(23, OUTPUT);
	pullUpDnControl(24, PUD_OFF);
	pinMode(24, OUTPUT);
	pullUpDnControl(25, PUD_OFF);
	pinMode(25, OUTPUT);

	// �G���R�[�_���C�g�o�͂̏������B
	digitalWrite(2, LOW);
	digitalWrite(3, HIGH);
	digitalWrite(4, HIGH);
	digitalWrite(23, LOW);
	digitalWrite(24, HIGH);
	digitalWrite(25, HIGH);
	
	// LCD�o�b�N���C�gOn�B
	digitalWrite(5, HIGH);

	// I2C�|�[�g��Read/Write�����ŃI�[�v���B
	if ((fd = open(i2cFileName, O_RDWR)) < 0)
	{
		printf("Faild to open i2c port\n");
		exit(1);
	}

	// LCD�������B
	lcd.init(fd);
	lcd.clear();

	lcd.setCursor(0, 1);
	lcd.puts("Vol:");

	// mpc �Đ��B
	system("mpc clear > radio_status.txt");
	system("mpc load radio.m3u");
	system("mpc play > radio_status.txt");

	system("mpc status > radio_status.txt");
	fp = fopen("radio_status.txt", "r");
	fgets(str, 16, fp);
	fclose(fp);
	printf("%s\n", str);
	lcd.setCursor(0, 0);
	lcd.puts(str);
	
	system("amixer | egrep '\[.*%\]' > radio_status.txt");
	fp = fopen("radio_status.txt", "r");
	fgets(str, 30, fp);
	sprintf(str, strchr(str, '['));
	fclose(fp);
	printf("Volume:%s\n", str);
	lcd.setCursor(0, 1);
	lcd.puts("Vol:");
	lcd.puts(str);

	while (1)
	{
		switch (getchar())
		{
		case 'n':
			system("mpc next > radio_status.txt");
			system("mpc play > radio_status.txt");
			system("mpc status > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 16, fp);
			fclose(fp);
			printf("%s\n", str);
			lcd.setCursor(0, 0);
			lcd.puts("                ");
			lcd.setCursor(0, 0);
			lcd.puts(str);
			break;
		case 'p':
			system("mpc prev > radio_status.txt");
			system("mpc play > radio_status.txt");
			system("mpc status > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 16, fp);
			fclose(fp);
			printf("%s\n", str);
			lcd.setCursor(0, 0);
			lcd.puts("                ");
			lcd.setCursor(0, 0);
			lcd.puts(str);
			break;
		case 's':
			system("mpc play > radio_status.txt");
			system("mpc status > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 16, fp);
			fclose(fp);
			printf("%s\n", str);
			lcd.setCursor(0, 0);
			lcd.puts(str);
			break;
		case 'u':
			volume = volume + 5;
			if (volume > 100)	volume = 100;
			sprintf(str, "amixer set PCM %d%% > radio_status.txt", volume);
			system(str);
			system("amixer | egrep '\[.*%\]' > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 30, fp);
			sprintf(str, strchr(str, '['));
			fclose(fp);
			printf("Volume:%s\n", str);
			lcd.setCursor(4, 1);
			lcd.puts("    ");
			lcd.setCursor(4, 1);
			lcd.puts(str);
			break;
		case 'd':
			volume = volume - 5;
			if (volume < 0)	volume = 0;
			sprintf(str, "amixer set PCM %d%% > radio_status.txt", volume);
			system(str);
			system("amixer | egrep '\[.*%\]' > radio_status.txt");
			fp = fopen("radio_status.txt", "r");
			fgets(str, 30, fp);
			sprintf(str, strchr(str, '['));
			fclose(fp);
			printf("Volume:%s\n", str);
			lcd.setCursor(4, 1);
			lcd.puts("    ");
			lcd.setCursor(4, 1);
			lcd.puts(str);
			break;
		case 'e':
			system("mpc stop > radio_status.txt");
			return 0;
			break;
		default:
			break;
		}

		delay(1);
		
	}

	return 0;
}