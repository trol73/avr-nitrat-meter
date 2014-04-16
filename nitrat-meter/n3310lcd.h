/*******************************************************************************
****************Библиотека функций для ЖКИ Nokia 3310 (PCD8544)*****************
********************************************************************************
для оригинального дисплея от NOKIA 3310 и "китайской" версии дисплея
48x84 пикселей, 6x14 символов
********************************************************************************/
//
#include <avr/io.h>                      //Подключение системной библиотеки
#include <string.h>                      //Подключение библиотеки работы со строковыми функциями
#include <avr/pgmspace.h>                //Подключение библиотеки массивов
unsigned char china=0;                   //1 - работа по алгоритмам "китайского" дисплея, 0 - оригинального Nokia
//Конфигурация SPI-интерфейса
#define SPCR_val 0b01010000;             //Включение SPI,первый-старший разряд,Master,SCK положит.полярн.,fclc/4
#define SPSR_val 0b00000001;             //Удвоение частоты (fclc/2)
//
//Конфигурация выводов МК для управления ЖКИ
#define LCD_DC_PORT PORTB                //DC (команда-данные)
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
#define LCD_MOSI_PORT PORTB              //Выход данных SPI (соответствующая линия аппаратного SPI)
#define LCD_MOSI_DDR  DDRB
#define LCD_MOSI_PIN  5
//
#define LCD_CLK_PORT  PORTB              //Тактирование SPI (соответствующая линия аппаратного SPI)
#define LCD_CLK_DDR   DDRB
#define LCD_CLK_PIN   7
//
//************************************************************************************************************************
//Настройки контроллера дисплея и переменные для работы с ним
//************************************************************************************************************************
#define PIXEL_OFF 0                      //Режимы отображения пикселя - используются в графических функциях
#define PIXEL_ON  1
#define PIXEL_XOR  2
//Разрешение экрана
#define LCD_X_RES  84                    //По горизонтали
#define LCD_Y_RES  48                    //По вертикали
#define LCD_CACHSIZE  LCD_X_RES*LCD_Y_RES/8 //Размер кэша
//Разрешение контроллера ЖКИ (предполагаемое, но работает)
#define Cntr_X_RES      102              //По горизонтали
#define Cntr_Y_RES      64               //По вертикали
#define Cntr_buf_size   Cntr_X_RES*Cntr_Y_RES/8
//
#define LCD_CMD         0                //Команда
#define LCD_DATA        1                //Данные
//
/*Массив в flash-памяти для задания режима дисплея (mode)*/
const unsigned char mas_mode[] PROGMEM =
//static unsigned char __attribute__ ((progmem)) mas_mode[]=
{ 0b00001000, 0b00001001, 0b00001100, 0b00001101 };
//
//
unsigned char lcd_buf[15];               //Текстовый буфер для вывода на LCD
unsigned char power_down=0;              //Power-down control: 0 - chip is active, 1 - chip is in PD-mode
unsigned char addressing=0;              //Направление адресации: 0 - горизонтальная, 1- вертикальная
unsigned char temp_control=3;            //Температурный коэффициент, 0..3
unsigned char bias=3;//3;                    //Смещение, 0..7
unsigned char Vop=70;//70                   //Рабочее напрядение LCD, 0..127 (определяет контрастность)
unsigned char disp_config=2;             //Режим дисплея: 0 - blank, 1 - all on, 2 - normal, 3 - inverse
unsigned char LcdCache[LCD_CACHSIZE];    //Кэш-буфер в ОЗУ МК (84*48 бит = 504 байт)
unsigned int  LcdCacheIdx;               //Кэш-индекс
//Для китайского дисплея
unsigned char shift=5;                   //0..63 - сдвиг экрана вверх, в пикселах
unsigned char x_mirror=0;                //Зеркалирование по X: 0 - выкл., 1 - вкл.
unsigned char y_mirror=0;                //Зеркалирование по Y: 0 - выкл., 1 - вкл.
unsigned char SPI_invert=0;              //Порядок битов в SPI: 0 - MSB first, 1 - LSB first
//
/*-----Инициализация внешних функций-----*/
extern void tayms (unsigned int t);      //Пауза в мс (1...65535)
//************************************************************************************************************************
/*-------------------------------Инициализация внутренних функций для работы с ЖКИ--------------------------------------*/
//************************************************************************************************************************
void LcdSend (unsigned char data, unsigned char cmd);//Передача данных в ЖКИ
void LcdUpdate (void);                               //Копирование буфера в ОЗУ ЖКИ
void LcdClear (void);                                //Очистка дисплея
void LcdInit ( void );                               //Настройка SPI и дисплея
void LcdContrast (unsigned char contrast);           //Установка контраста от 0 до 127 (регулируется только у китайского дисплея)
void LcdMode (unsigned char mode);                   //Режимы дисплея: 0-blank(чистый), 1-all on(залит), 2-normal(норм.), 3-inverse(инверсн.)
void LcdPwrMode (void);                              //Инвертирует состояние вкл/выкл дисплея
void LcdImage (const unsigned char *imageData);      //Вывод изображения
void LcdPixel (unsigned char x, unsigned char y, unsigned char mode); //Управление пикселем с координатами (x, y), mode -> Off, On or Xor
void LcdLine (unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode); //Вывод линии
void LcdCircle(char x, char y, char radius, unsigned char mode);   //Вывод круга с координатами центра и радиусом
void LcdBatt(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char persent);//Вывод батарейки с заполнением ее на %
void LcdGotoXYFont (unsigned char x, unsigned char y);//Установка курсора в положение xy (x=0...5, y=0...13)
void clean_lcd_buf (void);                            //Очистка текстового буфера
void clean_cache (void);                              //Очистка КЭШ-буфера в ОЗУ МК
void LcdChrXY (unsigned char x, unsigned char y, unsigned char ch);//Функция вывода символа в позицию x, y
void LcdChr (unsigned char ch);                       //Вывод символа в текущей позиции
void LcdChrInv (unsigned char ch);                    //Вывод инвертированного символа в текущей позиции
void LcdChrBig (unsigned char ch);                    //Вывод символа на текущем месте (большой)
void LcdChrBold (unsigned char ch);                   //Вывод символа на текущем месте (большой и жирный)
void LcdStringXY (unsigned char x, unsigned char y);  //Вывод строки из буфера с позиции x, y
void LcdString (void);                                //Вывод строки из буфера с текущей позиции
void LcdStringInv (unsigned char x, unsigned char y); //Вывод инвертированной строки
void LcdStringBig (unsigned char x, unsigned char y); //Вывод большой строки в позиции x, y
void LcdStringBold (unsigned char x, unsigned char y);//Вывод большой и жирной строки позиции x, y
void LcdStr (unsigned char *data);                    //Вывод строки "..." с текущей позиции
void LcdStrInv (unsigned char *data);                 //Вывод инвертированной строки "..." с текущей позиции
void LcdStrBig(unsigned char *data);                  //Вывод большой строки "..." с текущей позиции
void LcdStrBold(unsigned char *data);                 //Вывод большой и жирной строки "..." с текущей позиции
void LcdBar(int x1, int y1, int x2, int y2, unsigned char persent);//Вывод прогресс-бара и заполнение его на "процент"
void LcdRect(unsigned char baseX, unsigned char baseY, unsigned char height, unsigned char width, unsigned char mode);//Функция рисования закрашенного прямоугольника
void LcdBarLine(unsigned char line, unsigned char persent);//Вывод прогресс-бара в указанной строке
void LcdSkalaXYFont(unsigned char x, unsigned char y, unsigned char w, unsigned char persent);//Функция вывода шкалы регулируемого параметра с высотой и координатами шрифта
//*************************************************************************************************************************
//
//Массив ASCII (кодировка CP1251)в flash-памяти МК (5х256=1280 байт)
const unsigned char FontLookup [1280] PROGMEM=
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
  0x18, 0x28, 0x48, 0x28, 0x18,          //161 A1 пустой треугольник вниз V
  0x08, 0x20, 0x48, 0x20, 0x08,          //162 A2 пустой треугольник пунктиром вниз V
  0x0C, 0x0A, 0x09, 0x0A, 0x0C,          //163 A3 пустой треугольник вверх ^
  0x14, 0x36, 0x77, 0x36, 0x14,          //164 A4 два закрашенных треугольника вверх+вниз
  0x00, 0x7F, 0x22, 0x14, 0x08,          //165 A5 пустой треугольник вправо |>
  0x00, 0x7F, 0x3E, 0x1C, 0x08,          //166 A6 закрашенный треугольник вправо |>
  0x00, 0x08, 0x14, 0x22, 0x7F,          //167 A7 пустой треугольник влево <|
  0x00, 0x08, 0x1C, 0x3E, 0x7F,          //168 A8 закрашенный треугольник влево <| (Ё)
  0x00, 0x00, 0x00, 0x00, 0x00,          //169 A9 пустое место вместо символа батарейки
  0x7E, 0x43, 0x43, 0x43, 0x7E,          //170 AA 0 бат
  0x7E, 0x63, 0x63, 0x63, 0x7E,          //171 AB 0,25 бат
  0x7E, 0x73, 0x73, 0x73, 0x7E,          //172 AC 0,5 бат
  0x7E, 0x7B, 0x7B, 0x7B, 0x7E,          //173 AD 0,75 бат
  0x7E, 0x7F, 0x7F, 0x7F, 0x7E,          //174 AE 1 бат
  0x38, 0x7F, 0x78, 0x7F, 0x38,          //175 AF сетевая вилка
  0x00, 0x06, 0x09, 0x09, 0x06,          //176 B0 °
  0x44, 0x44, 0x5F, 0x44, 0x44,          //177 B1 ±
  0x00, 0x41, 0x7F, 0x41, 0x00,          //178 B2 І
  0x00, 0x44, 0x7D, 0x40, 0x00,          //179 B3 і
  0x7C, 0x04, 0x04, 0x04, 0x06,          //180 B4 ґ
  0x7C, 0x20, 0x20, 0x20, 0x1C,          //181 B5 µ
  0x06, 0x7E, 0x02, 0x7E, 0x02,          //182 B6 ¶
  0x00, 0x00, 0x18, 0x18, 0x00,          //183 B7 •
  0x38, 0x55, 0x54, 0x55, 0x18,          //184 B8 ё
  0x7F, 0x04, 0x08, 0x10, 0x7F,          //185 B9 №
  0x38, 0x54, 0x54, 0x44, 0x28,          //186 BA є
  0x22, 0x14, 0x2A, 0x14, 0x08,          //187 BB »
  0x20, 0x40, 0x44, 0x3D, 0x00,          //188 BC ј
  0x46, 0x49, 0x49, 0x49, 0x31,          //189 BD Ѕ
  0x08, 0x54, 0x54, 0x54, 0x20,          //190 BE ѕ
  0x00, 0x45, 0x7C, 0x41, 0x00,          //191 BF ї
  0x7E, 0x11, 0x11, 0x11, 0x7E,          //192 C0 А
  0x7F, 0x49, 0x49, 0x49, 0x31,          //193 C1 Б
  0x7F, 0x49, 0x49, 0x49, 0x36,          //194 C2 В
  0x7F, 0x01, 0x01, 0x01, 0x03,          //195 C3 Г
  0x60, 0x3E, 0x21, 0x21, 0x7F,          //196 C4 Д
  0x7F, 0x49, 0x49, 0x49, 0x41,          //197 C5 Е
  0x77, 0x08, 0x7F, 0x08, 0x77,          //198 C6 Ж
  0x22, 0x41, 0x49, 0x49, 0x36,          //199 C7 З
  0x7F, 0x10, 0x08, 0x04, 0x7F,          //200 C8 И
  0x7F, 0x10, 0x09, 0x04, 0x7F,          //201 C9 И
  0x7F, 0x08, 0x14, 0x22, 0x41,          //202 CA К
  0x40, 0x3E, 0x01, 0x01, 0x7F,          //203 CB Л
  0x7F, 0x02, 0x0C, 0x02, 0x7F,          //204 CC М
  0x7F, 0x08, 0x08, 0x08, 0x7F,          //205 CD Н
  0x3E, 0x41, 0x41, 0x41, 0x3E,          //206 CE О
  0x7F, 0x01, 0x01, 0x01, 0x7F,          //207 CF П
  0x7F, 0x09, 0x09, 0x09, 0x06,          //208 D0 Р
  0x3E, 0x41, 0x41, 0x41, 0x22,          //209 D1 С
  0x01, 0x01, 0x7F, 0x01, 0x01,          //210 D2 Т
  0x27, 0x48, 0x48, 0x48, 0x3F,          //211 D3 У
  0x1E, 0x21, 0x7F, 0x21, 0x1E,          //212 D4 Ф
  0x63, 0x14, 0x08, 0x14, 0x63,          //213 D5 Х
  0x3F, 0x20, 0x20, 0x3F, 0x60,          //214 D6 Ц
  0x07, 0x08, 0x08, 0x08, 0x7F,          //215 D7 Ч
  0x7F, 0x40, 0x7F, 0x40, 0x7F,          //216 D8 Ш
  0x3F, 0x20, 0x3F, 0x20, 0x7F,          //217 D9 Щ
  0x01, 0x7F, 0x48, 0x48, 0x30,          //218 DA Ъ
  0x7F, 0x48, 0x30, 0x00, 0x7F,          //219 DB Ы
  0x00, 0x7F, 0x48, 0x48, 0x30,          //220 DC Ь
  0x22, 0x41, 0x49, 0x49, 0x3E,          //221 DD Э
  0x7F, 0x08, 0x3E, 0x41, 0x3E,          //222 DE Ю
  0x46, 0x29, 0x19, 0x09, 0x7F,          //223 DF Я
  0x20, 0x54, 0x54, 0x54, 0x78,          //224 E0 а
  0x3C, 0x4A, 0x4A, 0x4A, 0x30,          //225 E1 б
  0x7C, 0x54, 0x54, 0x28, 0x00,          //226 E2 в
  0x7C, 0x04, 0x04, 0x04, 0x0C,          //227 E3 г
  0x60, 0x38, 0x24, 0x24, 0x7C,          //228 E4 д
  0x38, 0x54, 0x54, 0x54, 0x18,          //229 E5 е
  0x6C, 0x10, 0x7C, 0x10, 0x6C,          //230 E6 ж
  0x00, 0x44, 0x54, 0x54, 0x28,          //231 E7 з
  0x7C, 0x20, 0x10, 0x08, 0x7C,          //232 E8 и
  0x7C, 0x21, 0x12, 0x09, 0x7C,          //233 E9 й
  0x7C, 0x10, 0x28, 0x44, 0x00,          //234 EA к
  0x40, 0x38, 0x04, 0x04, 0x7C,          //235 EB л
  0x7C, 0x08, 0x10, 0x08, 0x7C,          //236 EC м
  0x7C, 0x10, 0x10, 0x10, 0x7C,          //237 ED н
  0x38, 0x44, 0x44, 0x44, 0x38,          //238 EE о
  0x7C, 0x04, 0x04, 0x04, 0x7C,          //239 EF п
  0x7C, 0x14, 0x14, 0x14, 0x08,          //240 F0 р
  0x38, 0x44, 0x44, 0x44, 0x00,          //241 F1 с
  0x04, 0x04, 0x7C, 0x04, 0x04,          //242 F2 т
  0x0C, 0x50, 0x50, 0x50, 0x3C,          //243 F3 у
  0x08, 0x14, 0x7C, 0x14, 0x08,          //244 F4 ф
  0x44, 0x28, 0x10, 0x28, 0x44,          //245 F5 х
  0x3C, 0x20, 0x20, 0x3C, 0x60,          //246 F6 ц
  0x0C, 0x10, 0x10, 0x10, 0x7C,          //247 F7 ч
  0x7C, 0x40, 0x7C, 0x40, 0x7C,          //248 F8 ш
  0x3C, 0x20, 0x3C, 0x20, 0x7C,          //249 F9 щ
  0x04, 0x7C, 0x50, 0x50, 0x20,          //250 FA ъ
  0x7C, 0x50, 0x20, 0x00, 0x7C,          //251 FB ы
  0x00, 0x7C, 0x50, 0x50, 0x20,          //252 FC ь
  0x28, 0x44, 0x54, 0x54, 0x38,          //253 FD э
  0x7C, 0x10, 0x38, 0x44, 0x38,          //254 FE ю
  0x48, 0x54, 0x34, 0x14, 0x7C           //255 FF я
};
//
/*------Функция передачи данных в ЖКИ-------*/
void LcdSend (unsigned char data, unsigned char cmd)//0-команда, 1-данные
{ LCD_CE_PORT&=~_BV(LCD_CE_PIN);         //CE=0 (включение контроллера дисплея)
  if (cmd) LCD_DC_PORT|=_BV(LCD_DC_PIN); //Если данные - DC=1
  else LCD_DC_PORT&=~_BV(LCD_DC_PIN);    //Иначе, если команда - DC=0
  SPDR=data;                             //Передача данных в контроллер дисплея
  while ((SPSR&0x80)!=0x80);             //Ожидание, пока не очистится Tx регистр
  LCD_CE_PORT|=_BV(LCD_CE_PIN);          //CE=1 (выключение контроллера дисплея)
  return;
}
//
/*-----Функция инициализации SPI и дисплея-----*/
void LcdInit (void)
{ LCD_RST_PORT|=_BV(LCD_RST_PIN);        //RESET=1
//Настройка портов ввода/вывода
  LCD_RST_DDR|=_BV(LCD_RST_PIN);
  LCD_DC_DDR|=_BV(LCD_DC_PIN);
  LCD_CE_DDR|=_BV(LCD_CE_PIN);
  LCD_MOSI_DDR|=_BV(LCD_MOSI_PIN);
  LCD_CLK_DDR|=_BV(LCD_CLK_PIN);
//
  tayms(1);
//Инициализация SPI-интерфейса
  SPCR = SPCR_val;
  SPSR = SPSR_val;
//Формирование импульса сброса RESET
  LCD_RST_PORT&=~_BV(LCD_RST_PIN);       //RESET=0
  tayms(20);
  LCD_RST_PORT|=_BV(LCD_RST_PIN);        //RESET=1
//
  LCD_CE_PORT|=_BV(LCD_CE_PIN);          //CE=1 (выключение контроллера дисплея)
//Команды инициализации ЖКИ
  LcdSend(0x21, LCD_CMD);                //LCD Extended Commands
  LcdSend(0x04+temp_control, LCD_CMD);   //Set Temp coefficent
  if (china==1)                          //Если китайский дисплей
  { LcdSend(0x08|SPI_invert<<3, LCD_CMD);//Порядок битов в SPI
  }
  LcdSend(0x10+bias, LCD_CMD);           //LCD bias mode 1:48
  if (china==1)                          //Если китайский дисплей
  { LcdSend(0x40+shift, LCD_CMD);        //Первая строка выше экрана, отображение со второй
  }
  LcdSend(0x80+Vop, LCD_CMD);            //Set LCD Vop (Contrast)
  if (china==1)                          //Если китайский дисплей
  { LcdSend(0x20|x_mirror<<4|y_mirror<<3|power_down<<2, LCD_CMD);//LCD Standard Commands
  }
  LcdSend(0x20|power_down<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
  LcdSend(0x08|((disp_config<<1|disp_config)&0x05), LCD_CMD);//LCD mode
//
  LcdClear();                            //Очистка дисплея
  return;
}
//
/*-----Функция установки контрастности дисплея-----*/
void LcdContrast (unsigned char contrast)//Контраст = 0...127 (регулируется только у китайского дисплея)
{ if (contrast>127) return;
  LcdSend(0x21, LCD_CMD);                //LCD Extended Commands
  LcdSend(0x80|contrast, LCD_CMD);       //Set LCD Vop (Contrast)
  LcdSend(0x20, LCD_CMD);                //LCD Standard Commands, Horizontal addressing mode
  return;
}
//
/*-----Функция установки режима дисплея-----*/
void LcdMode (unsigned char mode)        //mode: 0 - blank, 1 - all on, 2 - normal, 3 - inverse
{ if (mode > 3) return;
/*Команда "sets display configuration" 00001D0E:
  00001000 - display blank
  00001001 - all display segments on
  00001100 - normal mode
  00001101 - inverse video mode
*/
  LcdSend(pgm_read_byte(mas_mode+mode), LCD_CMD); //LCD mode
  return;
}
//
/*-----Функция инверсии состояния вкл/выкл дисплея-----*/
void LcdPwrMode (void)
{ if (power_down==0) power_down=1;
  else power_down=0;
  if (china==1)                          //Если китайский дисплей
  { LcdSend(0x20|x_mirror<<4|y_mirror<<3|power_down<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
  }
  else LcdSend(0x20|0<<2|addressing<<1, LCD_CMD);
  return;
}
//
/*-----Функция выключения дисплея-----*/
void Lcd_off (void)
{ if (china==1)                          //Если китайский дисплей
  { LcdSend(0x20|x_mirror<<4|y_mirror<<3|1<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
  }
  else LcdSend( 0x20|1<<2|addressing<<1, LCD_CMD );
  return;
}
//
/*-----Функция включения дисплея-----*/
void Lcd_on (void)
{ if (china==1)                          //Если китайский дисплей
  { LcdSend(0x20|x_mirror<<4|y_mirror<<3|0<<2|addressing<<1, LCD_CMD);//LCD Standard Commands
  }
  else LcdSend(0x20|0<<2|addressing<<1, LCD_CMD);
  return;
}
//
/*-----Функция очистки дисплея-----*/
void LcdClear (void)
{ unsigned int i;
  for (i=0; i<LCD_CACHSIZE; i++) LcdCache[i]=0;//Запись кэш-памяти 0
  LcdUpdate();                           //Копирование буфера в ОЗУ дисплея
  return;
}
//
/*-----Функция вывода изображения-----*/
void LcdImage (const unsigned char *imageData)
{ unsigned int i;
  for (i=0; i<LCD_CACHSIZE; i++) LcdCache[i]=pgm_read_byte(imageData+i);//Загрузка данных
  LcdUpdate();                           //Копирование буфера в ОЗУ ЖКИ
  return;
}
//
/*-----Функция управления пикселем с координатами (x, y)-----*/
void LcdPixel (unsigned char x, unsigned char y, unsigned char mode)//mode: PIXEL_ON или 1, PIXEL_OFF или 0, PIXEL_XOR или 2
{ unsigned int index;
  unsigned char offset, data;
  if (x>LCD_X_RES) return;               //Если некорректные координаты X - выход
  if (y>LCD_Y_RES) return;               //Если некорректные координаты Y - выход
  index=(y/8)*84+x;                      //Вычисление номера байта в массиве памяти дисплея
  offset=y-((y/8)*8);                    //Вычисление номера бита в этом байте
  data=LcdCache[index];                  //Чтение байта по найденному индексу
  if (mode==PIXEL_OFF) data&=(~(0x01<<offset));//Редактирование бита в этом байте
  else if (mode==PIXEL_ON) data|=(0x01<<offset);
  else if (mode==PIXEL_XOR) data^=(0x01<<offset);
  LcdCache[index]=data;                  //Загрузка байта назад в кэш
  return;
}
//
/*-----Функция вывода линии между двумя точками с координатами (x1, y1) и (x2,y2) по Брезенхейму-----*/
void LcdLine (unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode)
{ signed int dy=0;
  signed int dx=0;
  signed int stepx=0;
  signed int stepy=0;
  signed int fraction=0;
  if ((x1>LCD_X_RES)||(x2>LCD_X_RES)||(y1>LCD_Y_RES)||(y2>LCD_Y_RES)) return;
//Calculate differential form
// dy   y2 - y1
// -- = -------
// dx   x2 - x1
  dy=y2-y1;
  dx=x2-x1;
  if (dy<0)                              //Если dy<0
  { dy=-dy;
    stepy=-1;
  }
  else stepy=1;
  if (dx<0)                              //Если dx<0
  { dx=-dx;
    stepx=-1;
  }
  else stepx=1;
  dy<<=1;
  dx<<=1;
  LcdPixel(x1,y1,mode);
  if (dx>dy)
  { fraction=dy-(dx>>1); 
    while (x1!=x2)
    { if (fraction>=0)
      { y1+=stepy;
        fraction-=dx;
      }
      x1+=stepx;
      fraction+=dy;  
      LcdPixel(x1,y1,mode);
    }
  }
  else
  { fraction=dx-(dy>>1);
    while (y1!=y2)
    { if (fraction>=0)
      { x1+=stepx;
        fraction-=dy;
      }
      y1+=stepy;
      fraction+=dx;
      LcdPixel(x1,y1,mode);
    }
  }
  return;
}
//
/*-----Функция рисования круга с координатами (x, y) и радиусом по Брезенхейму-----*/
void LcdCircle(char x, char y, char radius, unsigned char mode)
{ signed char xc=0;
  signed char yc=0;
  signed char p=0;
  if ((x>LCD_X_RES)||(y>LCD_Y_RES)) return;
  yc=radius;
  p=3-(radius<<1);
  while (xc<=yc)
  { LcdPixel(x+xc, y+yc, mode);
    LcdPixel(x+xc, y-yc, mode);
    LcdPixel(x-xc, y+yc, mode);
    LcdPixel(x-xc, y-yc, mode);
    LcdPixel(x+yc, y+xc, mode);
    LcdPixel(x+yc, y-xc, mode);
    LcdPixel(x-yc, y+xc, mode);
    LcdPixel(x-yc, y-xc, mode);
    if (p<0) p+=(xc++ <<2)+6;
    else p+=((xc++ -yc--)<<2)+10;
  }
  return;
}
//
/*-----Функция рисования батарейки с заполнением в %-------*/
void LcdBatt(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char persent)
{ /*
   y1 0
   
   y2 47
   
   x1=0...76            x2=7...83
   (x2-x1)>=8
   (y2-y1)>=4
  */
  unsigned char horizon_line, horizon_line2, i;
  unsigned char k=(x2-x1)/3;
  if(persent>100)return;
  LcdLine(x1,   y2,   x2,   y2,   1);    //Нижняя линия
  LcdLine(x2,   y1+2, x2,   y2,   1);    //Линия справа
  LcdLine(x1,   y1+2, x1,   y2,   1);    //Линия слева
  LcdLine(x1,   y1+2, x2,   y1+2, 1);    //Верхняя линия
  LcdLine(x1+k, y1+1, x2-k, y1+1, 1);    //Колпачок
  LcdLine(x1+k, y1,   x2-k, y1,   1);    //Колпачок
  horizon_line=persent*(y2-y1-5)/100;    //Уровень заливки
  for(i=0; i<horizon_line; i++) LcdLine(x1+2,y2-2-i,x2-2,y2-2-i,1);//Заливка горизонтальными линиями
  horizon_line2=(y2-y1-5);               //Верхний уровень для заливки
  for(i=horizon_line2; i>horizon_line; i--) LcdLine(x1+2,y2-2-i,x2-2,y2-2-i,0);//Очистка до верха
  return;
}
//
/*-----Функция рисования горизонтального прогресс-бара в указанной строке и заполнения его на "процент"------*/
void LcdBarLine(unsigned char line, unsigned char persent)
{ //line=0...42 - верхняя линия прогресс бара (высотой 5 пикселей)
  LcdBar(0, line, 83, line+4, persent);
  return;
}
//
/*-----Функция рисования прогресс-бара и заполнения его на "процент"-------*/
void LcdBar(int x1, int y1, int x2, int y2, unsigned char persent)
{ unsigned char line;
  unsigned char i=y1+2;
/* y1 0
   
   y2 47
   
   x1 0            x2 83
   (x2-x1)>=8
   (y2-y1)>=4
*/
  if(persent>100)return;
  LcdLine(x1+2,y2,x2-2,y2,1);            //Линия снизу
  LcdLine(x2-2,y1,x2-2,y2,1);            //Линия справа
  LcdLine(x1+2,y1,x1+2,y2,1);            //Линия слева
  LcdLine(x1+2,y1,x2-2,y1,1);            //Линия сверху
  LcdLine(x2-1,y1+1,x2-1,y2-1,1);        //Укороченная линия справа
  LcdLine(x1+1,y1+1,x1+1,y2-1,1);        //Укороченная линия слева
  LcdLine(x2,y1+2,x2,y2-2,1);            //Ещё укороченная линия справа
  LcdLine(x1,y1+2,x1,y2-2,1);            //Ещё укороченная линия слева
  line=persent*(x2-x1-8)/100;
  while(i<(y2-1))
  { LcdLine(x1+4,i,x2-4,     i,0);       //Очистка предыдущей заливки
    if (persent>0) LcdLine(x1+4,i,x1+4+line,i,1); //Заливка
    i++;
  }
  return;
}
//
/*-----Функция рисования закрашенного прямоугольника-------*/
void LcdRect(unsigned char baseX, unsigned char baseY, unsigned char height, unsigned char width, unsigned char mode)
{ /*
    baseX - левая граница (0...83)
    baseY - нижняя граница (0...47)
    height - высота в пикселах
    width - ширина в пикселах
  */
  unsigned char tmpIdxX, tmpIdxY, tmp;
  if ((baseX>LCD_X_RES)||(baseY>LCD_Y_RES)) return;
  if (height>baseY) tmp=0;
  else tmp=1+baseY-height;
  for (tmpIdxY=tmp; tmpIdxY<=baseY; tmpIdxY++)
  { for (tmpIdxX=baseX; tmpIdxX<(baseX+width); tmpIdxX++)
    { LcdPixel(tmpIdxX, tmpIdxY, mode);
    }
  }
  return;
}
//
/*-----Функция установки курсора в положение xy для символа (x=0...5, y=0...13)-----*/
void LcdGotoXYFont (unsigned char x, unsigned char y)
{ if (x<=13 && y<=5) LcdCacheIdx=(int)(y)*84 + (int)(x)*6;
  return;
}
//
/*-----Функция очистки текстового буфера-----*/
void clean_lcd_buf (void)
{ unsigned char i;
  for (i=0; i<14; i++) lcd_buf[i]=0;
  return;
}
//
/*-----Функция очистки КЭШ-буфера ОЗУ МК-----*/
void clean_cache (void)
{ unsigned int i;
  for (i=0; i<LCD_CACHSIZE; i++) LcdCache[i]=0;//Запись кэш-памяти 0
  return;
}
//
/*-----Функция вывода символа в позицию x, y-----*/
void LcdChrXY (unsigned char x, unsigned char y, unsigned char ch)
{ //Пример: LcdChrXY(x,y,'С'); или LcdChrXY(x,y,0...255);
  LcdGotoXYFont(x, y);                   //Установка позиции
  LcdChr(ch);                            //Вывод символа
  return;
}
//
/*-----Функция вывода символа в текущей позиции-----*/
void LcdChr (unsigned char ch)
{ //Пример: LcdChr('С'); или LcdChr(0...255);
  unsigned char i;
  unsigned int s=ch;                     //Преобразование типа переменной
  for (i=0; i<5; i++) LcdCache[LcdCacheIdx++]=pgm_read_byte(FontLookup+(s*5+i));//Выделение байта-столбика из символа и загрузка в массив - 5 раз
  LcdCache[LcdCacheIdx++]=0x00;          //Введение пробела между символами
  return;
}
//
/*-----Функция вывода инвертированного символа в текущей позиции-----*/
void LcdChrInv (unsigned char ch)
{ unsigned char i;
  unsigned int s=ch;                     //Преобразование типа переменной
  for (i=0; i<5; i++) LcdCache[LcdCacheIdx++]=~pgm_read_byte(FontLookup+(s*5+i));//Выделение байта-столбика из символа и загрузка в массив - 5 раз
  LcdCache[LcdCacheIdx++]=0xFF;          //Введение пробела между символами
  return;
}
//
/*-----Функция вывода большого и жирного символа в текущей позиции-----*/
void LcdChrBold (unsigned char ch)
{ unsigned char i;
  unsigned int s=ch;                     //Преобразование типа переменной
  unsigned char a=0, b=0, c=0;
  for ( i=0; i<5; i++ )
  { c=pgm_read_byte(FontLookup+(s*5+i)); //Выделение столбца из символа
  //"растяжка" столбца на два байта
    b=(c&0x01)*3;
    b|=(c&0x02)*6;
    b|=(c&0x04)*12;
    b|=(c&0x08)*24;
    c>>=4;
    a=(c&0x01)*3;
    a|=(c&0x02)*6;
    a|=(c&0x04)*12;
    a|=(c&0x08)*24;
    LcdCache[LcdCacheIdx]=b;             //Копирование байт в экранный буфер
    LcdCache[LcdCacheIdx+1]=b;           //Дублирование для получения жирного шрифта
    LcdCache[LcdCacheIdx+84]=a;
    LcdCache[LcdCacheIdx+85]=a;
    LcdCacheIdx=LcdCacheIdx+2;
  }
  LcdCache[LcdCacheIdx++]=0x00;          //Пробел между символами
  LcdCache[LcdCacheIdx++]=0x00;
  return;
}
//
/*-----Функция вывода большого символа в текущей позиции-----*/
void LcdChrBig(unsigned char ch)
{ unsigned char i;
  unsigned int s=ch;                     //Преобразование типа переменной
  unsigned char a=0, b=0, c=0;
  for (i=0; i<5; i++)
  { c=pgm_read_byte(FontLookup+(s*5+i)); //Выделение столбца из символа
  //"растяжка" столбца на два байта
    b=(c&0x01)*3;
    b|=(c&0x02)*6;
    b|=(c&0x04)*12;
    b|=(c&0x08)*24;
    c>>=4;
    a=(c&0x01)*3;
    a|=(c&0x02)*6;
    a|=(c&0x04)*12;
    a|=(c&0x08)*24;
    LcdCache[LcdCacheIdx]=b;
    LcdCache[LcdCacheIdx+84]=a;
    LcdCacheIdx=LcdCacheIdx+1;
  }
  LcdCache[LcdCacheIdx++]=0x00;
  return;
}
//
/*-----Функция вывода строки из буфера с текущей позиции-----*/
void LcdString (void)
{ unsigned char i;
  for (i=0; i<14; i++ ) if(lcd_buf[i]) LcdChr(lcd_buf[i]);
  clean_lcd_buf();
  return;
}
//
/*-----Функция вывода строки из буфера с позиции xy-----*/
void LcdStringXY (unsigned char x, unsigned char y)
{ unsigned char i;
  if ((x>13)||(y>5)) return;
  LcdGotoXYFont(x, y);
  for (i=0; i<15-x; i++ ) if(lcd_buf[i]) LcdChr(lcd_buf[i]);
  clean_lcd_buf();
  return;
}
//
/*-----Функция вывода инвертированной строки из буфера с позиции xy-----*/
void LcdStringInv (unsigned char x, unsigned char y)
{ unsigned char i;
  if ((x>13)||(y>5)) return;
  LcdGotoXYFont (x, y);
  for (i=0; i<15-x; i++ ) if (lcd_buf[i]) LcdChrInv (lcd_buf[i]);
  clean_lcd_buf(); 
  return;
}
//
/*-----Функция вывода большой строки из буфера с позиции xy-----*/
void LcdStringBig (unsigned char x, unsigned char y)
{ //y=0...4 (позиционирование по верхней строке)
  //x=0...13
  unsigned char i;
  if ((x>13)||(y>4)) return;
  LcdGotoXYFont(x, y);
  for (i=0; i<15-x; i++) if(lcd_buf[i]) LcdChrBig(lcd_buf[i]); 
  clean_lcd_buf();
  return;
}
//
/*-----Функция вывода большой и жирной строки из буфера с позиции xy-----*/
void LcdStringBold(unsigned char x, unsigned char y)
{ //y=0...4 (позиционирование по верхней строке)
  //x=0...13
  unsigned char i;
  if ((x>13)||(y>4)) return;
  LcdGotoXYFont(x, y);
  for (i=0; i<14-x; i++) if(lcd_buf[i]) LcdChrBold (lcd_buf[i]); 
  clean_lcd_buf();
  return;
}
//
/*-----Функция вывода строки с текущей позиции-----*/
void LcdStr (unsigned char *data)
{ while(*data) LcdChr(*(data++));
  return;
}
//
/*-----Функция вывода большой строки с текущей позиции-----*/
void LcdStrBig (unsigned char *data)
{ while(*data) LcdChrBig(*(data++));
  return;
}
//
/*-----Функция вывода большой и жирной строки с текущей позиции-----*/
void LcdStrBold (unsigned char *data)
{ while(*data) LcdChrBold(*(data++));
  return;
}
//
/*-----Функция вывода шкалы регулируемого параметра с высотой и координатами шрифта-----*/
void LcdSkalaXYFont (unsigned char x, unsigned char y, unsigned char w, unsigned char persent)
{ /*
  x=0...13 (координата слева)
  y=0...5 (номер строки)
  w=1...13 (ширина по х в символах)
  persent=0...100
*/
  unsigned char Xn=x*6;                  //Координата Xn=0...83 начала шкалы
  unsigned char Xw=w*6;                  //Ширина шкалыв пикселях
  unsigned char Xk=Xn+Xw;                //Координата Xk=0...83 конца шкалы
  unsigned char Y1=y*8;                  //Координата Y1=0...47 верха шрифта
  unsigned char Y2=Y1+8;                 //Координата Y1=0...47 низа шрифта
  unsigned char i=Xn;                    //Счетчик пикселей по горизонтали
//Предварительная очистка выводимой области
  while (i<=Xk)
  { LcdLine (i, Y1, i, Y2, 0);           //Режим Blank
    i++;
  }
//Вывод шкалы
  i=Xn;
  unsigned char Yp=Y1+3;                 //Координата y для вывода пикселя
  Y1=Y1+1;                               //Верхняя координата линии шкалы
  Y2=Y1+4;                               //Нижняя координата линии шкалы
  while (i<=Xk)
  { if ((i-Xn)*100/Xw<persent)           //Если вывод линий
    { LcdLine (i, Y1, i, Y2, 1);
    }
    else LcdPixel (i, Yp, 1);            //Иначе - вывод точки
    i=i+2;                               //Инкремент с шагом 2
  }
  return;
}
//
/*-----Функция копирования буфера в ОЗУ ЖКИ-----*/
void LcdUpdate (void)
{ unsigned int i;
  unsigned char j;
  LcdSend(0x80, LCD_CMD);                //x=0
  LcdSend(0x40, LCD_CMD);                //y=0
  if (china==1)                          //Если китайский дисплей - вывод пустой строки
  { for (j=Cntr_X_RES; j>0; j--) LcdSend(0, LCD_DATA);
  }
  for (i=0; i<LCD_CACHSIZE; i++)         //Вывод данных
  { LcdSend(LcdCache[i], LCD_DATA);
    if (china==1)                        //Если китайский дисплей - вывод пустой строки
    { if (++j==LCD_X_RES)
      { for (j=(Cntr_X_RES-LCD_X_RES); j>0; j--) LcdSend(0, LCD_DATA);
        j=0;
      }
    }
  }
  return;
}
