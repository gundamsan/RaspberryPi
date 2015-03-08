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

LEDx_x端子は220Ωを入れてRPiの各端子へ。RPi端子LOWで点灯。
EncorderからのA,B相信号入力はRPi側でエッジ変化割り込みで関数呼び出しへ。
チャネル用エンコーダが Enc1_A,Bで内蔵RGB LEDがLED1_R,G,B。
ボリューム用エンコーダが Enc2_A,Bで内蔵RGB LEDがLED2_R,G,B。
I2CにはストリナのI2C LCDを接続。
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

// チューニング（チャネル）用エンコーダエッジのA相エッジ変化検出割り込みでコールされる関数。
void click_tune_A(void)
{
	int _a;

	// WiringPi GPIO_0(11pin)値取得。
	_a = digitalRead(0);
	// 値が変わった場合、
	if (_a != tune_A)
	{
		// 一つ前のA端子の値を格納。
		if (tune_A == 1)
		{
			pre_tune_A = 1;
		}
		else
		{
			pre_tune_A = 0;
		}
		tune_A = _a;

		// A端子とB端子の直前の値が1で、今の値が0なら
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

// チューニング（チャネル）用エンコーダエッジのB相エッジ変化検出割り込みでコールされる関数。
void click_tune_B(void)
{
	int _b;

	// WiringPi GPIO_1(12pin)値取得。
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

// ボリューム用エンコーダエッジのA相エッジ変化検出割り込みでコールされる関数。
void click_vol_A(void)
{
	int _a;

	// WiringPi GPIO_21(29pin)値取得。
	_a = digitalRead(21);
	// 値が変わった場合、
	if (_a != vol_A)
	{
		// 一つ前のA端子の値を格納。
		if (vol_A == 1)
		{
			pre_vol_A = 1;
		}
		else
		{
			pre_vol_A = 0;
		}
		vol_A = _a;

		// A端子とB端子の直前の値が1で、今の値が0なら
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

// ボリューム用エンコーダエッジのB相エッジ変化検出割り込みでコールされる関数。
void click_vol_B(void)
{
	int _b;

	// WiringPi GPIO_22(31pin)値取得。
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

	// 変数初期化。
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
	
	// wiringPの初期化。
	setup = wiringPiSetup();

	// GPIO_0(11pin）をプルアップして、エッジ検出割り込み設定。
	pullUpDnControl(0, PUD_UP);
	wiringPiISR(0, INT_EDGE_BOTH, click_tune_A);
	// GPIO_1(12pin）をプルアップして、エッジ検出割り込み設定。
	pullUpDnControl(1, PUD_UP);
	wiringPiISR(1, INT_EDGE_BOTH, click_tune_B);
	// GPIO_21(29pin）をプルアップして、エッジ検出割り込み設定。
	pullUpDnControl(21, PUD_UP);
	wiringPiISR(21, INT_EDGE_BOTH, click_vol_A);
	// GPIO_22(31pin）をプルアップして、エッジ検出割り込み設定。
	pullUpDnControl(22, PUD_UP);
	wiringPiISR(22, INT_EDGE_BOTH, click_vol_B);

	// GPIO_2,3,4,5,23,24,25(13,15,16,18,33,35,37pin)をプルアップダウンオフにして、出力に設定。
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

	// エンコーダライト出力の初期化。
	digitalWrite(2, LOW);
	digitalWrite(3, HIGH);
	digitalWrite(4, HIGH);
	digitalWrite(23, LOW);
	digitalWrite(24, HIGH);
	digitalWrite(25, HIGH);
	
	// LCDバックライトOn。
	digitalWrite(5, HIGH);

	// I2CポートをRead/Write属性でオープン。
	if ((fd = open(i2cFileName, O_RDWR)) < 0)
	{
		printf("Faild to open i2c port\n");
		exit(1);
	}

	// LCD初期化。
	lcd.init(fd);
	lcd.clear();

	lcd.setCursor(0, 1);
	lcd.puts("Vol:");

	// mpc 再生。
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