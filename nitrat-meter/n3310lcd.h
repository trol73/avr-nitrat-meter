/*******************************************************************************
****************���������� ������� ��� ��� Nokia 3310 (PCD8544)*****************
********************************************************************************
��� ������������� ������� �� NOKIA 3310 � "���������" ������ �������
48x84 ��������, 6x14 ��������
********************************************************************************/

#define DISPLAY_CHINA		0			// 1 - ������ �� ���������� "����������" �������, 0 - ������������� Nokia
#define DISPLAY_CHECK_ARGS	0			// 1 - ��������� �������� ������������ ����������

#define DISPLAY_MODE_BLANK	0b00001000	// display blank
#define DISPLAY_MODE_FILL 	0b00001001	// all display segments on
#define DISPLAY_MODE_NORMAL	0b00001100	// normal mode
#define DISPLAY_MODE_INVERT	0b00001101	// inverse video mode

//
#include <avr/io.h>                      //����������� ��������� ����������
#include <string.h>                      //����������� ���������� ������ �� ���������� ���������
#include <avr/pgmspace.h>                //����������� ���������� ��������

//������������ SPI-����������
#define SPCR_VAL 0b01010000;             //��������� SPI,������-������� ������,Master,SCK �������.������.,fclc/4
#define SPSR_VAL 0b00000001;             //�������� ������� (fclc/2)
//
//������������ ������� �� ��� ���������� ���
#define LCD_DC_PORT PORTB                //DC (�������-������)
#define LCD_DC_DDR  DDRB
#define LCD_DC_PIN  4
//
#define LCD_CE_PORT  PORTD               //Chip-Select
#define LCD_CE_DDR   DDRD
#define LCD_CE_PIN   4
//
#define LCD_RST_PORT PORTD               //RESET
#define LCD_RST_DDR  DDRD
#define LCD_RST_PIN  5
//
#define LCD_MOSI_PORT PORTB              //����� ������ SPI (��������������� ����� ����������� SPI)
#define LCD_MOSI_DDR  DDRB
#define LCD_MOSI_PIN  5
//
#define LCD_CLK_PORT  PORTB              //������������ SPI (��������������� ����� ����������� SPI)
#define LCD_CLK_DDR   DDRB
#define LCD_CLK_PIN   7
//
//************************************************************************************************************************
//��������� ����������� ������� � ���������� ��� ������ � ���
//************************************************************************************************************************
#define PIXEL_OFF	0                      //������ ����������� ������� - ������������ � ����������� ��������
#define PIXEL_ON	1
#define PIXEL_XOR	2
//���������� ������
#define LCD_X_RES  84                    //�� �����������
#define LCD_Y_RES  48                    //�� ���������
#define LCD_CACHSIZE  (LCD_X_RES*LCD_Y_RES/8) //������ ����
//���������� ����������� ��� (��������������, �� ��������)
#define Cntr_X_RES      102              //�� �����������
#define Cntr_Y_RES      64               //�� ���������
#define Cntr_buf_size   Cntr_X_RES*Cntr_Y_RES/8
//
#define LCD_CMD         0                //�������
#define LCD_DATA        1                //������
//
//
//
uint8_t lcd_buf[15];               //��������� ����� ��� ������ �� LCD
uint8_t power_down=0;              //Power-down control: 0 - chip is active, 1 - chip is in PD-mode
uint8_t addressing=0;              //����������� ���������: 0 - ��������������, 1- ������������
uint8_t temp_control=3;            //������������� �����������, 0..3
uint8_t bias=3;//3;                    //��������, 0..7
uint8_t Vop=70;//70                   //������� ���������� LCD, 0..127 (���������� �������������)
uint8_t disp_config=2;             //����� �������: 0 - blank, 1 - all on, 2 - normal, 3 - inverse
uint8_t LcdCache[LCD_CACHSIZE];    //���-����� � ��� �� (84*48 ��� = 504 ����)
unsigned int  LcdCacheIdx;               //���-������
//��� ���������� �������
uint8_t shift=5;                   //0..63 - ����� ������ �����, � ��������
uint8_t x_mirror=0;                //�������������� �� X: 0 - ����., 1 - ���.
uint8_t y_mirror=0;                //�������������� �� Y: 0 - ����., 1 - ���.
uint8_t SPI_invert=0;              //������� ����� � SPI: 0 - MSB first, 1 - LSB first
//
/*-----������������� ������� �������-----*/
//extern void tayms (unsigned int t);      //����� � �� (1...65535)
//************************************************************************************************************************
/*-------------------------------������������� ���������� ������� ��� ������ � ���--------------------------------------*/
//************************************************************************************************************************
void LcdSend(uint8_t data, uint8_t cmd);//�������� ������ � ���
void lcd_Update();                               //����������� ������ � ��� ���
void lcd_Clear();                                //������� �������
void lcd_Init();                               //��������� SPI � �������
void lcd_SetContrast(uint8_t contrast);           //��������� ��������� �� 0 �� 127 (������������ ������ � ���������� �������)
void LcdMode(uint8_t mode);                   //������ �������: 0-blank(������), 1-all on(�����), 2-normal(����.), 3-inverse(�������.)
void LcdPwrMode();                              //����������� ��������� ���/���� �������
void lcd_DrawImage(const uint8_t *imageData);      //����� �����������
void lcd_DrawPixel(uint8_t x, uint8_t y, uint8_t mode); //���������� �������� � ������������ (x, y), mode -> Off, On or Xor
void lcd_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode); //����� �����
void lcd_DrawLineH(uint8_t y, uint8_t x1, uint8_t x2, uint8_t mode); //����� �������������� �����
void lcd_DrawLineV(uint8_t x, uint8_t y1, uint8_t y2, uint8_t mode); //����� ������������ �����

void lcd_DrawCircle(char x, char y, char radius, uint8_t mode);   //����� ����� � ������������ ������ � ��������
void lcd_GotoXY(uint8_t x, uint8_t y);//��������� ������� � ��������� xy (x=0...5, y=0...13)
void clean_lcd_buf();                            //������� ���������� ������
void clean_cache();                              //������� ���-������ � ��� ��
void lcd_OutCharXY(uint8_t ch, uint8_t x, uint8_t y);//������� ������ ������� � ������� x, y
void lcd_OutChar(uint8_t ch);                       //����� ������� � ������� �������
void LcdChrInv(uint8_t ch);                    //����� ���������������� ������� � ������� �������
void LcdChrBig(uint8_t ch);                    //����� ������� �� ������� ����� (�������)
void LcdChrBold(uint8_t ch);                   //����� ������� �� ������� ����� (������� � ������)
void LcdStringXY(uint8_t x, uint8_t y);  //����� ������ �� ������ � ������� x, y
void LcdString();                                //����� ������ �� ������ � ������� �������
void LcdStringInv(uint8_t x, uint8_t y); //����� ��������������� ������
void LcdStringBig(uint8_t x, uint8_t y); //����� ������� ������ � ������� x, y
void lcd_OutStr(PGM_P s);				// ����� ������
void lcd_OutStrXY(PGM_P s, uint8_t x, uint8_t y);				// ����� ������

//void lcd_outData(PGM_P s);			// ����� ������ ������
void LcdStringBold(uint8_t x, uint8_t y);//����� ������� � ������ ������ ������� x, y
void LcdStr(unsigned char *data);                    //����� ������ "..." � ������� �������
void LcdStrInv (unsigned char *data);                 //����� ��������������� ������ "..." � ������� �������
void LcdStrBig(unsigned char *data);                  //����� ������� ������ "..." � ������� �������
void LcdStrBold(unsigned char *data);                 //����� ������� � ������ ������ "..." � ������� �������
void lcd_DrawRect(uint8_t baseX, uint8_t baseY, uint8_t width, uint8_t height, uint8_t mode);//������� ��������� ������������ ��������������
void LcdSkalaXYFont(uint8_t x, uint8_t y, uint8_t w, uint8_t persent);//������� ������ ����� ������������� ��������� � ������� � ������������ ������
//*************************************************************************************************************************
//
//������ ASCII (��������� CP1251)� flash-������ �� (5�256=1280 ����)
const uint8_t FontLookup [1280] PROGMEM=
{ 0x00, 0x00, 0x00, 0x00, 0x00,          //  0 00
  0x00, 0x00, 0x5F, 0x00, 0x00,          //  1 01
  0x00, 0x07, 0x00, 0x07, 0x00,          //  2 02
  0x14, 0x7F, 0x14, 0x7F, 0x14,          //  3 03
  0x24, 0x2A, 0x7F, 0x2A, 0x12,          //  4 04
  0x23, 0x13, 0x08, 0x64, 0x62,          //  5 05
  0x36, 0x49, 0x55, 0x22, 0x50,          //  6 06
  0x00, 0x05, 0x03, 0x00, 0x00,          //  7 07
  0x00, 0x1C, 0x22, 0x41, 0x00,          //  8 08
  0x00, 0x41, 0x22, 0x1C, 0x00,          //  9 09
  0x14, 0x08, 0x3E, 0x08, 0x14,          // 10 0A
  0x08, 0x08, 0x3E, 0x08, 0x08,          // 11 0B
  0x00, 0x50, 0x30, 0x00, 0x00,          // 12 0C
  0x08, 0x08, 0x08, 0x08, 0x08,          // 13 0D
  0x00, 0x60, 0x60, 0x00, 0x00,          // 14 0E
  0x20, 0x10, 0x08, 0x04, 0x02,          // 15 0F
  0x00, 0x00, 0x00, 0x00, 0x00,          // 16 10
  0x00, 0x00, 0x5F, 0x00, 0x00,          // 17 11
  0x00, 0x07, 0x00, 0x07, 0x00,          // 18 12
  0x14, 0x7F, 0x14, 0x7F, 0x14,          // 19 13
  0x24, 0x2A, 0x7F, 0x2A, 0x12,          // 20 14
  0x23, 0x13, 0x08, 0x64, 0x62,          // 21 15
  0x36, 0x49, 0x55, 0x22, 0x50,          // 22 16
  0x00, 0x05, 0x03, 0x00, 0x00,          // 23 17
  0x00, 0x1C, 0x22, 0x41, 0x00,          // 24 18
  0x00, 0x41, 0x22, 0x1C, 0x00,          // 25 19
  0x14, 0x08, 0x3E, 0x08, 0x14,          // 26 1A
  0x08, 0x08, 0x3E, 0x08, 0x08,          // 27 1B
  0x00, 0x50, 0x30, 0x00, 0x00,          // 28 1C
  0x08, 0x08, 0x08, 0x08, 0x08,          // 29 1D
  0x00, 0x60, 0x60, 0x00, 0x00,          // 30 1E
  0x20, 0x10, 0x08, 0x04, 0x02,          // 31 1F
  0x00, 0x00, 0x00, 0x00, 0x00,          // 32 20 space
  0x00, 0x00, 0x5F, 0x00, 0x00,          // 33 21 ! 
  0x00, 0x07, 0x00, 0x07, 0x00,          // 34 22 " 
  0x14, 0x7F, 0x14, 0x7F, 0x14,          // 35 23 # 
  0x24, 0x2A, 0x7F, 0x2A, 0x12,          // 36 24 $ 
  0x23, 0x13, 0x08, 0x64, 0x62,          // 37 25 % 
  0x36, 0x49, 0x55, 0x22, 0x50,          // 38 26 & 
  0x00, 0x05, 0x03, 0x00, 0x00,          // 39 27 '
  0x00, 0x1C, 0x22, 0x41, 0x00,          // 40 28 ( 
  0x00, 0x41, 0x22, 0x1C, 0x00,          // 41 29 )
  0x14, 0x08, 0x3E, 0x08, 0x14,          // 42 2a * 
  0x08, 0x08, 0x3E, 0x08, 0x08,          // 43 2b + 
  0x00, 0x50, 0x30, 0x00, 0x00,          // 44 2c , 
  0x08, 0x08, 0x08, 0x08, 0x08,          // 45 2d - 
  0x00, 0x60, 0x60, 0x00, 0x00,          // 46 2e . 
  0x20, 0x10, 0x08, 0x04, 0x02,          // 47 2f / 
  0x3E, 0x51, 0x49, 0x45, 0x3E,          // 48 30 0
  0x00, 0x42, 0x7F, 0x40, 0x00,          // 49 31 1
  0x42, 0x61, 0x51, 0x49, 0x46,          // 50 32 2
  0x21, 0x41, 0x45, 0x4B, 0x31,          // 51 33 3
  0x18, 0x14, 0x12, 0x7F, 0x10,          // 51 34 4
  0x27, 0x45, 0x45, 0x45, 0x39,          // 53 35 5
  0x3C, 0x4A, 0x49, 0x49, 0x30,          // 54 36 6
  0x01, 0x71, 0x09, 0x05, 0x03,          // 55 37 7
  0x36, 0x49, 0x49, 0x49, 0x36,          // 56 38 8
  0x06, 0x49, 0x49, 0x29, 0x1E,          // 57 39 9
  0x00, 0x36, 0x36, 0x00, 0x00,          // 58 3a :
  0x00, 0x56, 0x36, 0x00, 0x00,          // 59 3b ;
  0x08, 0x14, 0x22, 0x41, 0x00,          // 60 3c <
  0x14, 0x14, 0x14, 0x14, 0x14,          // 61 3d =
  0x00, 0x41, 0x22, 0x14, 0x08,          // 62 3e >
  0x02, 0x01, 0x51, 0x09, 0x06,          // 63 3f ?
  0x32, 0x49, 0x79, 0x41, 0x3E,          // 64 40 @
  0x7E, 0x11, 0x11, 0x11, 0x7E,          // 41 A
  0x7F, 0x49, 0x49, 0x49, 0x36,          // 42 B
  0x3E, 0x41, 0x41, 0x41, 0x22,          // 43 C
  0x7F, 0x41, 0x41, 0x22, 0x1C,          // 44 D
  0x7F, 0x49, 0x49, 0x49, 0x41,          // 45 E
  0x7F, 0x09, 0x09, 0x09, 0x01,          // 46 F
  0x3E, 0x41, 0x49, 0x49, 0x7A,          // 47 G
  0x7F, 0x08, 0x08, 0x08, 0x7F,          // 48 H
  0x00, 0x41, 0x7F, 0x41, 0x00,          // 49 I
  0x20, 0x40, 0x41, 0x3F, 0x01,          // 4a J
  0x7F, 0x08, 0x14, 0x22, 0x41,          // 4b K
  0x7F, 0x40, 0x40, 0x40, 0x40,          // 4c L
  0x7F, 0x02, 0x0C, 0x02, 0x7F,          // 4d M
  0x7F, 0x04, 0x08, 0x10, 0x7F,          // 4e N
  0x3E, 0x41, 0x41, 0x41, 0x3E,          // 4f O
  0x7F, 0x09, 0x09, 0x09, 0x06,          // 50 P
  0x3E, 0x41, 0x51, 0x21, 0x5E,          // 51 Q
  0x7F, 0x09, 0x19, 0x29, 0x46,          // 52 R
  0x46, 0x49, 0x49, 0x49, 0x31,          // 53 S
  0x01, 0x01, 0x7F, 0x01, 0x01,          // 54 T
  0x3F, 0x40, 0x40, 0x40, 0x3F,          // 55 U
  0x1F, 0x20, 0x40, 0x20, 0x1F,          // 56 V
  0x3F, 0x40, 0x38, 0x40, 0x3F,          // 57 W
  0x63, 0x14, 0x08, 0x14, 0x63,          // 58 X
  0x07, 0x08, 0x70, 0x08, 0x07,          // 59 Y
  0x61, 0x51, 0x49, 0x45, 0x43,          // 5a Z
  0x00, 0x7F, 0x41, 0x41, 0x00,          // 5b [
  0x02, 0x04, 0x08, 0x10, 0x20,          // 5c Yen Currency Sign 
  0x00, 0x41, 0x41, 0x7F, 0x00,          // 5d ]
  0x04, 0x02, 0x01, 0x02, 0x04,          // 5e ^
  0x40, 0x40, 0x40, 0x40, 0x40,          // 5f _
  0x00, 0x01, 0x02, 0x04, 0x00,          // 60 `
  0x20, 0x54, 0x54, 0x54, 0x78,          // 61 a
  0x7F, 0x48, 0x44, 0x44, 0x38,          // 62 b
  0x38, 0x44, 0x44, 0x44, 0x20,          // 63 c
  0x38, 0x44, 0x44, 0x48, 0x7F,          // 64 d
  0x38, 0x54, 0x54, 0x54, 0x18,          // 65 e
  0x08, 0x7E, 0x09, 0x01, 0x02,          // 66 f
  0x0C, 0x52, 0x52, 0x52, 0x3E,          // 67 g
  0x7F, 0x08, 0x04, 0x04, 0x78,          // 68 h
  0x00, 0x44, 0x7D, 0x40, 0x00,          // 69 i
  0x20, 0x40, 0x44, 0x3D, 0x00,          // 6a j
  0x7F, 0x10, 0x28, 0x44, 0x00,          // 6b k
  0x00, 0x41, 0x7F, 0x40, 0x00,          // 6c l
  0x7C, 0x04, 0x18, 0x04, 0x78,          // 6d m
  0x7C, 0x08, 0x04, 0x04, 0x78,          // 6e n
  0x38, 0x44, 0x44, 0x44, 0x38,          // 6f o
  0x7C, 0x14, 0x14, 0x14, 0x08,          // 70 p
  0x08, 0x14, 0x14, 0x18, 0x7C,          // 71 q
  0x7C, 0x08, 0x04, 0x04, 0x08,          // 72 r
  0x08, 0x54, 0x54, 0x54, 0x20,          // 73 s
  0x04, 0x3F, 0x44, 0x40, 0x20,          // 74 t
  0x3C, 0x40, 0x40, 0x20, 0x7C,          // 75 u
  0x1C, 0x20, 0x40, 0x20, 0x1C,          // 76 v
  0x3C, 0x40, 0x30, 0x40, 0x3C,          // 77 w
  0x44, 0x28, 0x10, 0x28, 0x44,          // 78 x
  0x0C, 0x50, 0x50, 0x50, 0x3C,          // 79 y
  0x44, 0x64, 0x54, 0x4C, 0x44,          // 7a z
  0x00, 0x08, 0x36, 0x41, 0x00,          // 7b <
  0x00, 0x00, 0x7F, 0x00, 0x00,          // 7c |
  0x00, 0x41, 0x36, 0x08, 0x00,          // 7d >
  0x10, 0x08, 0x08, 0x10, 0x08,          // 7e Right Arrow ->
  0x78, 0x46, 0x41, 0x46, 0x78,          // 7f Left Arrow <-
  0x00, 0x00, 0x00, 0x00, 0x00,          // 80
  0x00, 0x00, 0x5F, 0x00, 0x00,          // 81
  0x00, 0x07, 0x00, 0x07, 0x00,          // 82
  0x14, 0x7F, 0x14, 0x7F, 0x14,          // 83
  0x24, 0x2A, 0x7F, 0x2A, 0x12,          // 84
  0x23, 0x13, 0x08, 0x64, 0x62,          // 85
  0x36, 0x49, 0x55, 0x22, 0x50,          // 86
  0x00, 0x05, 0x03, 0x00, 0x00,          // 87
  0x00, 0x1C, 0x22, 0x41, 0x00,          // 88
  0x00, 0x41, 0x22, 0x1C, 0x00,          // 89
  0x14, 0x08, 0x3E, 0x08, 0x14,          // 8A
  0x08, 0x08, 0x3E, 0x08, 0x08,          // 8B
  0x00, 0x50, 0x30, 0x00, 0x00,          // 8C
  0x08, 0x08, 0x08, 0x08, 0x08,          // 8D
  0x00, 0x60, 0x60, 0x00, 0x00,          // 8E
  0x20, 0x10, 0x08, 0x04, 0x02,          // 8F
  0x00, 0x00, 0x00, 0x00, 0x00,          // 90
  0x00, 0x00, 0x5F, 0x00, 0x00,          // 91
  0x00, 0x07, 0x00, 0x07, 0x00,          // 92
  0x14, 0x7F, 0x14, 0x7F, 0x14,          // 93
  0x24, 0x2A, 0x7F, 0x2A, 0x12,          // 94
  0x23, 0x13, 0x08, 0x64, 0x62,          // 95
  0x36, 0x49, 0x55, 0x22, 0x50,          // 96
  0x00, 0x05, 0x03, 0x00, 0x00,          // 97
  0x00, 0x1C, 0x22, 0x41, 0x00,          // 98
  0x00, 0x41, 0x22, 0x1C, 0x00,          // 99
  0x14, 0x08, 0x3E, 0x08, 0x14,          // 9A
  0x08, 0x08, 0x3E, 0x08, 0x08,          // 9B
  0x00, 0x50, 0x30, 0x00, 0x00,          // 9C
  0x08, 0x08, 0x08, 0x08, 0x08,          // 9D
  0x00, 0x60, 0x60, 0x00, 0x00,          // 9E
  0x20, 0x10, 0x08, 0x04, 0x02,          //159 9F
  0x00, 0x00, 0x00, 0x00, 0x00,          //160 A0
  0x18, 0x28, 0x48, 0x28, 0x18,          //161 A1 ������ ����������� ���� V
  0x08, 0x20, 0x48, 0x20, 0x08,          //162 A2 ������ ����������� ��������� ���� V
  0x0C, 0x0A, 0x09, 0x0A, 0x0C,          //163 A3 ������ ����������� ����� ^
  0x14, 0x36, 0x77, 0x36, 0x14,          //164 A4 ��� ����������� ������������ �����+����
  0x00, 0x7F, 0x22, 0x14, 0x08,          //165 A5 ������ ����������� ������ |>
  0x00, 0x7F, 0x3E, 0x1C, 0x08,          //166 A6 ����������� ����������� ������ |>
  0x00, 0x08, 0x14, 0x22, 0x7F,          //167 A7 ������ ����������� ����� <|
  0x00, 0x08, 0x1C, 0x3E, 0x7F,          //168 A8 ����������� ����������� ����� <| (�)
  0x00, 0x00, 0x00, 0x00, 0x00,          //169 A9 ������ ����� ������ ������� ���������
  0x7E, 0x43, 0x43, 0x43, 0x7E,          //170 AA 0 ���
  0x7E, 0x63, 0x63, 0x63, 0x7E,          //171 AB 0,25 ���
  0x7E, 0x73, 0x73, 0x73, 0x7E,          //172 AC 0,5 ���
  0x7E, 0x7B, 0x7B, 0x7B, 0x7E,          //173 AD 0,75 ���
  0x7E, 0x7F, 0x7F, 0x7F, 0x7E,          //174 AE 1 ���
  0x38, 0x7F, 0x78, 0x7F, 0x38,          //175 AF ������� �����
  0x00, 0x06, 0x09, 0x09, 0x06,          //176 B0 �
  0x44, 0x44, 0x5F, 0x44, 0x44,          //177 B1 �
  0x00, 0x41, 0x7F, 0x41, 0x00,          //178 B2 �
  0x00, 0x44, 0x7D, 0x40, 0x00,          //179 B3 �
  0x7C, 0x04, 0x04, 0x04, 0x06,          //180 B4 �
  0x7C, 0x20, 0x20, 0x20, 0x1C,          //181 B5 �
  0x06, 0x7E, 0x02, 0x7E, 0x02,          //182 B6 �
  0x00, 0x00, 0x18, 0x18, 0x00,          //183 B7 �
  0x38, 0x55, 0x54, 0x55, 0x18,          //184 B8 �
  0x7F, 0x04, 0x08, 0x10, 0x7F,          //185 B9 �
  0x38, 0x54, 0x54, 0x44, 0x28,          //186 BA �
  0x22, 0x14, 0x2A, 0x14, 0x08,          //187 BB �
  0x20, 0x40, 0x44, 0x3D, 0x00,          //188 BC �
  0x46, 0x49, 0x49, 0x49, 0x31,          //189 BD �
  0x08, 0x54, 0x54, 0x54, 0x20,          //190 BE �
  0x00, 0x45, 0x7C, 0x41, 0x00,          //191 BF �
  0x7E, 0x11, 0x11, 0x11, 0x7E,          //192 C0 �
  0x7F, 0x49, 0x49, 0x49, 0x31,          //193 C1 �
  0x7F, 0x49, 0x49, 0x49, 0x36,          //194 C2 �
  0x7F, 0x01, 0x01, 0x01, 0x03,          //195 C3 �
  0x60, 0x3E, 0x21, 0x21, 0x7F,          //196 C4 �
  0x7F, 0x49, 0x49, 0x49, 0x41,          //197 C5 �
  0x77, 0x08, 0x7F, 0x08, 0x77,          //198 C6 �
  0x22, 0x41, 0x49, 0x49, 0x36,          //199 C7 �
  0x7F, 0x10, 0x08, 0x04, 0x7F,          //200 C8 �
  0x7F, 0x10, 0x09, 0x04, 0x7F,          //201 C9 �
  0x7F, 0x08, 0x14, 0x22, 0x41,          //202 CA �
  0x40, 0x3E, 0x01, 0x01, 0x7F,          //203 CB �
  0x7F, 0x02, 0x0C, 0x02, 0x7F,          //204 CC �
  0x7F, 0x08, 0x08, 0x08, 0x7F,          //205 CD �
  0x3E, 0x41, 0x41, 0x41, 0x3E,          //206 CE �
  0x7F, 0x01, 0x01, 0x01, 0x7F,          //207 CF �
  0x7F, 0x09, 0x09, 0x09, 0x06,          //208 D0 �
  0x3E, 0x41, 0x41, 0x41, 0x22,          //209 D1 �
  0x01, 0x01, 0x7F, 0x01, 0x01,          //210 D2 �
  0x27, 0x48, 0x48, 0x48, 0x3F,          //211 D3 �
  0x1E, 0x21, 0x7F, 0x21, 0x1E,          //212 D4 �
  0x63, 0x14, 0x08, 0x14, 0x63,          //213 D5 �
  0x3F, 0x20, 0x20, 0x3F, 0x60,          //214 D6 �
  0x07, 0x08, 0x08, 0x08, 0x7F,          //215 D7 �
  0x7F, 0x40, 0x7F, 0x40, 0x7F,          //216 D8 �
  0x3F, 0x20, 0x3F, 0x20, 0x7F,          //217 D9 �
  0x01, 0x7F, 0x48, 0x48, 0x30,          //218 DA �
  0x7F, 0x48, 0x30, 0x00, 0x7F,          //219 DB �
  0x00, 0x7F, 0x48, 0x48, 0x30,          //220 DC �
  0x22, 0x41, 0x49, 0x49, 0x3E,          //221 DD �
  0x7F, 0x08, 0x3E, 0x41, 0x3E,          //222 DE �
  0x46, 0x29, 0x19, 0x09, 0x7F,          //223 DF �
  0x20, 0x54, 0x54, 0x54, 0x78,          //224 E0 �
  0x3C, 0x4A, 0x4A, 0x4A, 0x30,          //225 E1 �
  0x7C, 0x54, 0x54, 0x28, 0x00,          //226 E2 �
  0x7C, 0x04, 0x04, 0x04, 0x0C,          //227 E3 �
  0x60, 0x38, 0x24, 0x24, 0x7C,          //228 E4 �
  0x38, 0x54, 0x54, 0x54, 0x18,          //229 E5 �
  0x6C, 0x10, 0x7C, 0x10, 0x6C,          //230 E6 �
  0x00, 0x44, 0x54, 0x54, 0x28,          //231 E7 �
  0x7C, 0x20, 0x10, 0x08, 0x7C,          //232 E8 �
  0x7C, 0x21, 0x12, 0x09, 0x7C,          //233 E9 �
  0x7C, 0x10, 0x28, 0x44, 0x00,          //234 EA �
  0x40, 0x38, 0x04, 0x04, 0x7C,          //235 EB �
  0x7C, 0x08, 0x10, 0x08, 0x7C,          //236 EC �
  0x7C, 0x10, 0x10, 0x10, 0x7C,          //237 ED �
  0x38, 0x44, 0x44, 0x44, 0x38,          //238 EE �
  0x7C, 0x04, 0x04, 0x04, 0x7C,          //239 EF �
  0x7C, 0x14, 0x14, 0x14, 0x08,          //240 F0 �
  0x38, 0x44, 0x44, 0x44, 0x00,          //241 F1 �
  0x04, 0x04, 0x7C, 0x04, 0x04,          //242 F2 �
  0x0C, 0x50, 0x50, 0x50, 0x3C,          //243 F3 �
  0x08, 0x14, 0x7C, 0x14, 0x08,          //244 F4 �
  0x44, 0x28, 0x10, 0x28, 0x44,          //245 F5 �
  0x3C, 0x20, 0x20, 0x3C, 0x60,          //246 F6 �
  0x0C, 0x10, 0x10, 0x10, 0x7C,          //247 F7 �
  0x7C, 0x40, 0x7C, 0x40, 0x7C,          //248 F8 �
  0x3C, 0x20, 0x3C, 0x20, 0x7C,          //249 F9 �
  0x04, 0x7C, 0x50, 0x50, 0x20,          //250 FA �
  0x7C, 0x50, 0x20, 0x00, 0x7C,          //251 FB �
  0x00, 0x7C, 0x50, 0x50, 0x20,          //252 FC �
  0x28, 0x44, 0x54, 0x54, 0x38,          //253 FD �
  0x7C, 0x10, 0x38, 0x44, 0x38,          //254 FE �
  0x48, 0x54, 0x34, 0x14, 0x7C           //255 FF �
};
//
/*------������� �������� ������ � ���-------*/
void LcdSend(uint8_t data, uint8_t cmd) {//0-�������, 1-������
	LCD_CE_PORT &= ~_BV(LCD_CE_PIN);         //CE=0 (��������� ����������� �������)
	if (cmd) {
		LCD_DC_PORT |= _BV(LCD_DC_PIN); //���� ������ - DC=1
	} else {
		LCD_DC_PORT &= ~_BV(LCD_DC_PIN);    //�����, ���� ������� - DC=0
	}
	SPDR = data;                             //�������� ������ � ���������� �������
	while ((SPSR & 0x80) != 0x80);             //��������, ���� �� ��������� Tx �������
	LCD_CE_PORT |= _BV(LCD_CE_PIN);          //CE=1 (���������� ����������� �������)
}
//
/*-----������� ������������� SPI � �������-----*/
void lcd_Init() {
	LCD_RST_PORT |= _BV(LCD_RST_PIN);        //RESET=1
	// ��������� ������ �����/������
	LCD_RST_DDR |= _BV(LCD_RST_PIN);
	LCD_DC_DDR |= _BV(LCD_DC_PIN);
	LCD_CE_DDR |= _BV(LCD_CE_PIN);
	LCD_MOSI_DDR |= _BV(LCD_MOSI_PIN);
	LCD_CLK_DDR |= _BV(LCD_CLK_PIN);

	_delay_ms(1);
	// ������������� SPI-����������
	SPCR = SPCR_VAL;
	SPSR = SPSR_VAL;
	// ������������ �������� ������ RESET
	LCD_RST_PORT &= ~_BV(LCD_RST_PIN);       //RESET=0
	_delay_ms(20);
	LCD_RST_PORT |= _BV(LCD_RST_PIN);        //RESET=1

	LCD_CE_PORT |= _BV(LCD_CE_PIN);          //CE=1 (���������� ����������� �������)
	// ������� ������������� ���
	LcdSend(0x21, LCD_CMD);                //LCD Extended Commands
	LcdSend(0x04+temp_control, LCD_CMD);   //Set Temp coefficent

#if DISPLAY_CHINA
	LcdSend(0x08 | SPI_invert<<3, LCD_CMD);//������� ����� � SPI
#endif

	LcdSend(0x10+bias, LCD_CMD);           //LCD bias mode 1:48
#if DISPLAY_CHINA
	LcdSend(0x40+shift, LCD_CMD);        //������ ������ ���� ������, ����������� �� ������
#endif

	LcdSend(0x80+Vop, LCD_CMD);            //Set LCD Vop (Contrast)

#if DISPLAY_CHINA
	LcdSend(0x20|x_mirror<<4|y_mirror<<3|power_down<<2, LCD_CMD);//LCD Standard Commands
#endif

	LcdSend(0x20|power_down<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
	LcdSend(0x08|((disp_config<<1|disp_config)&0x05), LCD_CMD);//LCD mode

	lcd_Clear();                            //������� �������
}
//
/*-----������� ��������� ������������� �������-----*/
void lcd_SetContrast (uint8_t contrast) {//�������� = 0...127 (������������ ������ � ���������� �������)
#ifdef DISPLAY_CHECK_ARGS
	if (contrast > 127)
		return;
#endif
	LcdSend(0x21, LCD_CMD);                //LCD Extended Commands
	LcdSend(0x80|contrast, LCD_CMD);       //Set LCD Vop (Contrast)
	LcdSend(0x20, LCD_CMD);                //LCD Standard Commands, Horizontal addressing mode
}
//
/*-----������� ��������� ������ �������-----*/
void LcdMode(uint8_t mode) {       // mode: DISPLAY_MODE_xxx
	LcdSend(pgm_read_byte(mode), LCD_CMD);
}
//
/*-----������� �������� ��������� ���/���� �������-----*/
void LcdPwrMode() {
	power_down = 1 - power_down;

#if DISPLAY_CHINA
	LcdSend(0x20|x_mirror<<4|y_mirror<<3|power_down<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
#else
	LcdSend(0x20|0<<2|addressing<<1, LCD_CMD);
#endif
}
//
/*-----������� ���������� �������-----*/
void Lcd_off() {
#if DISPLAY_CHINA
	LcdSend(0x20|x_mirror<<4|y_mirror<<3|1<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
#else
	LcdSend( 0x20|1<<2|addressing<<1, LCD_CMD );
#endif
}
//
/*-----������� ��������� �������-----*/
void Lcd_on() {
#if DISPLAY_CHINA
	LcdSend(0x20|x_mirror<<4|y_mirror<<3|0<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
#else
	LcdSend(0x20|0<<2|addressing<<1, LCD_CMD);
#endif
}
//
/*-----������� ������� �������-----*/
void lcd_Clear() {
	clean_cache();
	lcd_Update();                           //����������� ������ � ��� �������
}
//
/*-----������� ������ �����������-----*/
void lcd_DrawImage(const uint8_t *imageData) {
	for (uint16_t i = 0; i < LCD_CACHSIZE; i++)
		LcdCache[i] = pgm_read_byte(imageData+i); //�������� ������
	lcd_Update();                           //����������� ������ � ��� ���
}
//
/*-----������� ���������� �������� � ������������ (x, y)-----*/
void lcd_DrawPixel (uint8_t x, uint8_t y, uint8_t mode) {//mode: PIXEL_ON ��� 1, PIXEL_OFF ��� 0, PIXEL_XOR ��� 2
#ifdef DISPLAY_CHECK_ARGS
	if (x > LCD_X_RES || y > LCD_Y_RES)
		return;               //���� ������������ ���������� Y - �����
#endif
	uint16_t index = (y/8)*84+x;                      //���������� ������ ����� � ������� ������ �������
	uint8_t offset = y - ((y/8)*8);                    //���������� ������ ���� � ���� �����
	uint8_t data = LcdCache[index];                  //������ ����� �� ���������� �������
	switch (mode) {
		case PIXEL_OFF:
			data &= ~_BV(offset);
			break;
		case PIXEL_ON:
			data |= ~_BV(offset);
			break;
		case PIXEL_XOR:
			data ^= ~_BV(offset);
			break;
	}
	LcdCache[index] = data;                  //�������� ����� ����� � ���
}
//
/*-----������� ������ ����� ����� ����� ������� � ������������ (x1, y1) � (x2,y2) �� �����������-----*/
void lcd_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode) {
#ifdef DISPLAY_CHECK_ARGS
	if (x1 > LCD_X_RES || x2 > LCD_X_RES || y1 > LCD_Y_RES || y2 > LCD_Y_RES) {
		return;
	}
#endif
//Calculate differential form
// dy   y2 - y1
// -- = -------
// dx   x2 - x1
	int dy = y2 - y1;
	int dx = x2 - x1;
	signed int stepX, stepY;

	if (dy < 0) {
		dy = -dy;
		stepY = -1;
	} else {
		stepY = 1;
	}
	if (dx < 0) {
		dx = -dx;
		stepX = -1;
	} else {
		stepX = 1;
	}
	dy <<= 1;
	dx <<= 1;
	lcd_DrawPixel(x1, y1, mode);
	signed int fraction = 0;
	if (dx > dy) {
		fraction = dy - (dx >> 1);
		while (x1 != x2) {
			if (fraction >= 0) {
				y1 += stepY;
				fraction-=dx;
			}
			x1 += stepX;
			fraction += dy;
			lcd_DrawPixel(x1, y1, mode);
		}
	} else {
		fraction = dx - (dy>>1);
		while (y1 != y2) {
			if (fraction >= 0) {
				x1 += stepX;
				fraction -= dy;
			}
			y1 += stepY;
			fraction += dx;
			lcd_DrawPixel(x1, y1, mode);
		}
	}
}

/**
 * ������� ��������� �������������� �����
 */
void lcd_DrawLineH(uint8_t y, uint8_t x1, uint8_t x2, uint8_t mode) {
	for (uint8_t x = x1; x <= x2; x++) {
		lcd_DrawPixel(x, y, mode);
	}
}

/**
 * ������� ��������� ������������ �����
 */
void lcd_DrawLineV(uint8_t x, uint8_t y1, uint8_t y2, uint8_t mode) {
	for (uint8_t y = y1; y <= y2; y++) {
		lcd_DrawPixel(x, y, mode);
	}
}


//
/**
 * ������� ��������� ����� � ������������ (x, y) � �������� �� �����������
 **/
void lcd_DrawCircle(char x, char y, char radius, uint8_t mode){
#ifdef DISPLAY_CHECK_ARGS
	if (x > LCD_X_RES || y > LCD_Y_RES) {
		return;
	}
#endif
	signed char yc = radius;
	signed char p = 3-(radius << 1);
	signed char xc = 0;
	while (xc <= yc) {
		lcd_DrawPixel(x+xc, y+yc, mode);
		lcd_DrawPixel(x+xc, y-yc, mode);
		lcd_DrawPixel(x-xc, y+yc, mode);
		lcd_DrawPixel(x-xc, y-yc, mode);
		lcd_DrawPixel(x+yc, y+xc, mode);
		lcd_DrawPixel(x+yc, y-xc, mode);
		lcd_DrawPixel(x-yc, y+xc, mode);
		lcd_DrawPixel(x-yc, y-xc, mode);
		if (p < 0) {
			p += (xc++ <<2) + 6;
		} else {
			p += ((xc++ -yc--)<<2) + 10;
		}
	}
}
//
/*-----������� ��������� ������������ ��������������-------*/
void lcd_DrawRect(uint8_t baseX, uint8_t baseY, uint8_t height, uint8_t width, uint8_t mode) {
	for (uint8_t x = baseX; x <= baseX + width; x++) {
		for (uint8_t y = baseY; y <= baseY + height; y++) {
			lcd_DrawPixel(x, y, mode);
		}
	}
}
//
/*-----������� ��������� ������� � ��������� xy ��� ������� (x=0...5, y=0...13)-----*/
void lcd_GotoXY(uint8_t x, uint8_t y) {
#ifdef DISPLAY_CHECK_ARGS
	if (x > 13 || y > 5) {
		return;
	}
#endif
	LcdCacheIdx = (int)(y)*84 + (int)(x)*6;
}
//
/*-----������� ������� ���������� ������-----*/
void clean_lcd_buf() {
	for (uint8_t i = 0; i < 14; i++)
		lcd_buf[i] = 0;
}
//
/*-----������� ������� ���-������ ��� ��-----*/
void clean_cache() {
	for (uint16_t i = 0; i < LCD_CACHSIZE; i++) {
		LcdCache[i] = 0;
	}
}
//
/*-----������� ������ ������� � ������� x, y-----*/
void lcd_OutCharXY(uint8_t ch, uint8_t x, uint8_t y) {
	lcd_GotoXY(x, y);
	lcd_OutChar(ch);
}
//
/*-----������� ������ ������� � ������� �������-----*/
void lcd_OutChar(uint8_t ch) {
	//������: LcdChr('�'); ��� LcdChr(0...255);
	uint16_t s = ch*5;                     //�������������� ���� ����������
	for (uint8_t i = 0; i < 5; i++) {
		LcdCache[LcdCacheIdx++] = pgm_read_byte(FontLookup+s+i);//��������� �����-�������� �� ������� � �������� � ������ - 5 ���
	}
	LcdCache[LcdCacheIdx++] = 0x00;          //�������� ������� ����� ���������
}
//
/*-----������� ������ ���������������� ������� � ������� �������-----*/
void LcdChrInv(uint8_t ch) {
	uint16_t s = ch*5;                     //�������������� ���� ����������
	for (uint8_t i = 0; i < 5; i++) {
  		LcdCache[LcdCacheIdx++] = ~pgm_read_byte(FontLookup+(s+i));//��������� �����-�������� �� ������� � �������� � ������ - 5 ���
	}
	LcdCache[LcdCacheIdx++] = 0xFF;          //�������� ������� ����� ���������
}
//
/*-----������� ������ �������� � ������� ������� � ������� �������-----*/
void LcdChrBold (uint8_t ch) {
	uint16_t s = ch*5;                     //�������������� ���� ����������
	for (uint8_t i = 0; i < 5; i++) {
		uint8_t c = pgm_read_byte(FontLookup+(s+i)); //��������� ������� �� �������
		//"��������" ������� �� ��� �����
		uint8_t b = (c & 0x01)*3;
		b |= (c & 0x02)*6;
		b |= (c & 0x04)*12;
		b |= (c & 0x08)*24;
		c >>= 4;
		uint8_t a = (c & 0x01)*3;
		a |= (c & 0x02)*6;
		a |= (c & 0x04)*12;
		a |= (c & 0x08)*24;
		LcdCache[LcdCacheIdx] = b;             //����������� ���� � �������� �����
		LcdCache[LcdCacheIdx+1] = b;           //������������ ��� ��������� ������� ������
		LcdCache[LcdCacheIdx+84] = a;
		LcdCache[LcdCacheIdx+85] = a;
		LcdCacheIdx = LcdCacheIdx + 2;
	}
	LcdCache[LcdCacheIdx++] = 0x00;          //������ ����� ���������
	LcdCache[LcdCacheIdx++] = 0x00;
}
//
/*-----������� ������ �������� ������� � ������� �������-----*/
void LcdChrBig(uint8_t ch) {
	uint16_t s = ch*5;                     //�������������� ���� ����������
	for (uint8_t i = 0; i < 5; i++) {
		uint8_t c = pgm_read_byte(FontLookup+(s*5+i)); //��������� ������� �� �������
		//"��������" ������� �� ��� �����
		uint8_t b = (c & 0x01)*3;
		b |= (c & 0x02)*6;
		b |= (c & 0x04)*12;
		b |= (c & 0x08)*24;
		c >>= 4;
		uint8_t a = (c & 0x01)*3;
		a |= (c & 0x02)*6;
		a |= (c & 0x04)*12;
		a |= (c & 0x08)*24;
		LcdCache[LcdCacheIdx] = b;
		LcdCache[LcdCacheIdx+84] = a;
		LcdCacheIdx = LcdCacheIdx+1;
	}
	LcdCache[LcdCacheIdx++] = 0x00;
}
//
/*-----������� ������ ������ �� ������ � ������� �������-----*/
void LcdString() {
	for (uint8_t i = 0; i < 14; i++) {
		if (lcd_buf[i]) {
			lcd_OutChar(lcd_buf[i]);
		}
	}
	clean_lcd_buf();
}
//
/*-----������� ������ ������ �� ������ � ������� xy-----*/
void LcdStringXY (uint8_t x, uint8_t y) {
#ifdef DISPLAY_CHECK_ARGS
	if (x > 13 || y > 5) {
		return;
	}
#endif
	lcd_GotoXY(x, y);
	for (uint8_t i = 0; i < 15-x; i++) {
		if (lcd_buf[i]) {
			lcd_OutChar(lcd_buf[i]);
		}
	}
	clean_lcd_buf();
}
//
/*-----������� ������ ��������������� ������ �� ������ � ������� xy-----*/
void LcdStringInv (uint8_t x, uint8_t y) {
#ifdef DISPLAY_CHECK_ARGS
	if (x > 13 || y > 5) {
		return;
	}
#endif
	lcd_GotoXY(x, y);
	for (uint8_t i = 0; i < 15-x; i++) {
		if (lcd_buf[i]) {
			LcdChrInv(lcd_buf[i]);
		}
	}
	clean_lcd_buf();
}
//
/*-----������� ������ ������� ������ �� ������ � ������� xy-----*/
void LcdStringBig (uint8_t x, uint8_t y) {
	//y=0...4 (���������������� �� ������� ������)
	//x=0...13
#ifdef DISPLAY_CHECK_ARGS
	if (x > 13 || y > 4) {
		return;
	}
#endif
	lcd_GotoXY(x, y);
	for (uint8_t i = 0; i < 15-x; i++) {
		if (lcd_buf[i]) {
			LcdChrBig(lcd_buf[i]);
		}
	}
	clean_lcd_buf();
}
//
/*-----������� ������ ������� � ������ ������ �� ������ � ������� xy-----*/
void LcdStringBold(uint8_t x, uint8_t y) {
	// y=0...4 (���������������� �� ������� ������)
	// x=0...13
#ifdef DISPLAY_CHECK_ARGS
	if (x > 13 || y > 4) {
		return;
	}
#endif
	lcd_GotoXY(x, y);
	for (uint8_t i = 0; i < 14-x; i++) {
		if (lcd_buf[i]) {
			LcdChrBold(lcd_buf[i]);
		}
	}
	clean_lcd_buf();
}
//
/*-----������� ������ ������ � ������� �������-----*/
void LcdStr(unsigned char *data) {
	while (*data) {
		lcd_OutChar(*(data++));
	}
}
//
/*-----������� ������ ������� ������ � ������� �������-----*/
void LcdStrBig (unsigned char *data) {
	while (*data) {
		LcdChrBig(*(data++));
	}
}
//
/*-----������� ������ ������� � ������ ������ � ������� �������-----*/
void LcdStrBold (unsigned char *data)
{ while(*data) LcdChrBold(*(data++));
}
//
/*-----������� ������ ����� ������������� ��������� � ������� � ������������ ������-----*/
void LcdSkalaXYFont (uint8_t x, uint8_t y, uint8_t w, uint8_t persent) {
/*
  x=0...13 (���������� �����)
  y=0...5 (����� ������)
  w=1...13 (������ �� � � ��������)
  persent=0...100
*/
	uint8_t Xn = x*6;                  //���������� Xn=0...83 ������ �����
	uint8_t Xw = w*6;                  //������ ������ ��������
	uint8_t Xk = Xn + Xw;                //���������� Xk=0...83 ����� �����
	uint8_t Y1 = y*8;                  //���������� Y1=0...47 ����� ������
	uint8_t Y2 = Y1+8;                 //���������� Y1=0...47 ���� ������
	uint8_t i;                    //������� �������� �� �����������
//��������������� ������� ��������� �������
	for (i = Xn; i <= Xk; i++) {
		lcd_DrawLine(i, Y1, i, Y2, PIXEL_OFF);           //����� Blank
	}
	//����� �����
//	i = Xn;
	uint8_t Yp = Y1 + 3;                 //���������� y ��� ������ �������
	Y1 = Y1 + 1;                               //������� ���������� ����� �����
	Y2 = Y1 + 4;                               //������ ���������� ����� �����
	for (i = Xn; i <= Xk; i += 2) {
		if ( (i-Xn)*100/Xw < persent) {          //���� ����� �����
			lcd_DrawLine(i, Y1, i, Y2, PIXEL_ON);
		} else {
			lcd_DrawPixel(i, Yp, PIXEL_ON);            //����� - ����� �����
		}
	}
}
//
/*-----������� ����������� ������ � ��� ���-----*/
void lcd_Update() {
	LcdSend(0x80, LCD_CMD);                //x=0
	LcdSend(0x40, LCD_CMD);                //y=0

#if DISPLAY_CHINA
	uint8_t j;
	for (j = Cntr_X_RES; j > 0; j--)
		LcdSend(0, LCD_DATA);
#endif
	for (uint16_t i = 0; i < LCD_CACHSIZE; i++) {         //����� ������
		LcdSend(LcdCache[i], LCD_DATA);
#if DISPLAY_CHINA
		if (++j == LCD_X_RES) {
			for (j = (Cntr_X_RES-LCD_X_RES); j>0; j--) {
				LcdSend(0, LCD_DATA);
			}
			j=0;
      	}
#endif
	}
}



// ����� ������
void lcd_OutStr(PGM_P str) {
	for (;;) {
		uint8_t c = pgm_read_byte_near(str++);
		if (c == 0) {
			break;
		}
		lcd_OutChar(c);
	}
}

void lcd_OutStrXY(PGM_P str, uint8_t x, uint8_t y) {
	lcd_GotoXY(x, y);
	lcd_OutStr(str);
}

/**
 * ����������� ����:
 * 	\n	      ������� �� ��������� ������
 * 	0 1 x y    ������� � �������
 * 	0 0		 ����� ����� ������
 */
//void lcd_outData(PGM_P s) {			// ����� ������ ������
//	for (;;) {
//		uint8_t c = pgm_read_byte_near(str++);
//		if (c == 0) {
//			break;
//		}
//		LcdChr(c);
//	}
//}
