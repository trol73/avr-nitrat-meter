void LcdAnim (void);                     //Функция вывода аннимации в виде заполняющейся шкалы процесса замера
void lcd_OutInt(uint16_t val);
void lcd_OutIntXY(uint16_t val, uint8_t x, uint8_t y);
//void lcd_OutInt(bool gash, unsigned int data, uint8_t l, uint8_t r);//Функция вывода (r+l)-значн.числа типа unsigned int с гаш.незнач.нуля с текущей позиции
void lcd_OutIntBuf(bool gash, unsigned int data, uint8_t l, uint8_t r);//Функция вывода (r+l)-значн.числа типа unsigned int в буфер с гаш.незнач.нуля
void DrawBattery(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t persent);//Вывод батарейки с заполнением ее на %
void DrawProgressBar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t persent);//Вывод прогресс-бара и заполнение его на "процент"
void LcdBarLine(uint8_t line, uint8_t persent);//Вывод прогресс-бара в указанной строке

void lcd_OutFloat1(uint16_t val);


/*-----Функция вывода анимации в виде заполняющейся шкалы процесса замера-----*/
void LcdAnim() {
	// Стирание предыдущих надписей
	lcd_OutStrXY(STR_________, 6, 1);
	//Вывод анимации
	for (uint8_t per = 0; per < 100; per += 2) {
		DrawProgressBar(42, 9, 83, 13, per);          //Рисование прогресс-бара и заполнения его на "процент"
		lcd_Update();                         //Вывод информации в дисплей
	}
}
//
/**
 * Функция вывода (r+l)-значн.числа типа unsigned int с гаш.незнач.нуля с текущей позиции
*/
//void lcd_OutInt(bool gash, unsigned int data, uint8_t l, uint8_t r) {
/*gash=0 - нет гашения, 1 - гашение 1-го незнач.нуля слева,
  data - число; l - число выводимых цифр слева, r - справа от запятой)
*/
//	lcd_OutIntBuf(gash, data, l, r);
//	LcdString();                           //Вывод строки из текстового буфера с текущей позиции
//}

void lcd_OutInt(uint16_t val) {
	uint16_t num = 10000;
	bool started = false;

	while (num > 0) {
		uint8_t b = val / num;
		if (b > 0 || started || num == 1) {
			lcd_OutChar('0' + b);
			started = true;
		}
		val -= b * num;
		num /= 10;
	}
}


void lcd_OutIntXY(uint16_t val, uint8_t x, uint8_t y) {
	lcd_GotoXY(x, y);
	lcd_OutInt(val);
}
/**
 * Функция вывода (r+l)-значн.числа типа unsigned int в буфер с гаш.незнач.нуля
 *
	gash=0 - нет гашения, 1 - гашение 1-го незнач.нуля слева,
	data - число; l - число выводимых цифр слева, r - справа от запятой)
	act=false - обычная индикация, act=trus - с миганием активного пункта *
 **/
void lcd_OutIntBuf(bool gash, unsigned int data, uint8_t l, uint8_t r) {
	unsigned int i=0;                      //Счетчик выводимых разрядов
	uint8_t c=1;                     //Счетчик разрядов текстового буфера
	unsigned int x=1;                      //Степень множителя
	uint8_t  dat;                     //Вспомогательная переменная

	clean_lcd_buf();                       //Очистка текстового буфера

	for (i = 0; i < l + r - 1; i++) {
		x *= 10;                              //Вычисл. 10 в степени (l+r-1)
	}
	i = l + r - 1;                            //Счетчик выводимых разрядов
	//Вывод первого разряда
	if (((data / x)==0) && l > 1 && gash) {   //Если гашение незначащего нуля
		lcd_buf[0] = ' ';                      //Вывод пробела в текстовый буфер
	} else {
		lcd_buf[0] = data/x + 0x30;
		gash = 0;                              //Если была выведена цифра, дальнейшее гашение нулей не требуется
	}
	x /= 10;                                //Понижение степени множителя
	if (gash > 0) {
		gash--;                    //Декремент числа гасящих нулей
	}
	while (i != 0) {                          //Цикл вывода оставшихся разрядов
		if (i == r && r > 0) {                  //Если вывод разделительной точки
			lcd_buf[c++] = '.';
			r++;                               //Изменение r для исключения повтора
		} else {                                //Иначе вывод цифры
				dat = (data/x) % 10;                 //Вычислние цифры очередного разряда
				if (dat == 0 && gash > 0) {         //Если цифра=0 и нужно гашение
					dat = ' ';                       //Пробел
				} else {
					dat += 0x30;                  //Иначе вывод цифры очередного разряда
					gash=0;                        //Если была выведена цифра, дальнейшее гашение нулей не требуется
				}
				lcd_buf[c] = dat;                //Вывод байта данных в буфер

			x /= 10;                            //Понижение степени множителя
			if (gash > 0) {
				gash--;                //Декремент числа гасящих нулей
			}
			i--;                               //Декремент счетчика разрядов
			c++;                               //Инкремент счетчика разрядов текстового буфера
		}
	}
}



//	lcd_OutIntBuf(2, dat, 3, 0, 0)		0..999 %
//  lcd_OutIntBuf(1, dat, 2, 0, 0); 	0..99%
//  lcd_OutIntBuf(3, Gx, 4, 1, 0);   //Иначе - вывод в буфер проводимости в мкСм
//  lcd_OutIntBuf(3, Gx, 4, 1, 0); //Вывод проводимости в мкСм
//  lcd_OutIntBuf(2, r, 5, 0, 0);      //Иначе - вывод сопротивления в Омах
//  lcd_OutIntBuf(3, r, 4, 1, 0);      //Иначе - вывод проводимости в мкСм
//  lcd_OutIntBuf(0, Ubat, 1, 2, 0);


/*-----Функция рисования батарейки с заполнением в %-------*/
void DrawBattery(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char persent)
{ /*
   y1 0

   y2 47

   x1=0...76            x2=7...83
   (x2-x1)>=8
   (y2-y1)>=4
  */
  unsigned char horizon_line, horizon_line2, i;
  unsigned char k=(x2-x1)/3;
#ifdef DISPLAY_CHECK_ARGS
	if (persent > 100) {
		return;
	}
#endif
  lcd_DrawLine(x1,   y2,   x2,   y2,   PIXEL_ON);    //Нижняя линия
  lcd_DrawLine(x2,   y1+2, x2,   y2,   PIXEL_ON);    //Линия справа
  lcd_DrawLine(x1,   y1+2, x1,   y2,   PIXEL_ON);    //Линия слева
  lcd_DrawLine(x1,   y1+2, x2,   y1+2, PIXEL_ON);    //Верхняя линия
  lcd_DrawLine(x1+k, y1+1, x2-k, y1+1, PIXEL_ON);    //Колпачок
  lcd_DrawLine(x1+k, y1,   x2-k, y1,   PIXEL_ON);    //Колпачок
  horizon_line=persent*(y2-y1-5)/100;    //Уровень заливки
	for(i=0; i<horizon_line; i++)
		lcd_DrawLine(x1+2, y2-2-i, x2-2, y2-2-i, PIXEL_ON);//Заливка горизонтальными линиями
	horizon_line2 = (y2-y1-5);               //Верхний уровень для заливки
	for (i = horizon_line2; i > horizon_line; i--)
		lcd_DrawLine(x1+2, y2-2-i, x2-2, y2-2-i, PIXEL_OFF);//Очистка до верха
}

/*-----Функция рисования прогресс-бара и заполнения его на "процент"-------*/
void DrawProgressBar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t persent) {
#ifdef DISPLAY_CHECK_ARGS
	if (persent > 100) {
		return;
	}
#endif
	lcd_DrawLineH(y2, x1+2, x2-2, PIXEL_ON);	// Линия снизу
	lcd_DrawLineV(x2-2, y1, y2, PIXEL_ON);		// Линия справа
	lcd_DrawLineV(x1+2, y1, y2, PIXEL_ON);		// Линия слева
	lcd_DrawLineH(y1, x1+2, x2-2, PIXEL_ON);	// Линия сверху

	lcd_DrawLineV(x2-1, y1+1, y2-1, PIXEL_ON);	// Укороченная линия справа
	lcd_DrawLineV(x1+1, y1+1, y2-1, PIXEL_ON);	// Укороченная линия слева
	lcd_DrawLineV(x2, y1+2, y2-2, PIXEL_ON);	// Ещё укороченная линия справа
	lcd_DrawLineV(x1, y1+2, y2-2, PIXEL_ON);	// Ещё укороченная линия слева

	// ширина закрашиваемой области
	uint16_t line = ((uint16_t)persent)*(x2-x1-8)/100;
	uint8_t fillWidth = (uint8_t)line;
	uint8_t emptyWidth = x2 - x1 - 8 - fillWidth;
	uint8_t height = y2-y1-4;
	lcd_DrawRect(x1+4, y1+2, fillWidth, height, PIXEL_ON);
	lcd_DrawRect(x1+4 + fillWidth, y1+2, emptyWidth, height, PIXEL_OFF);
}

/*-----Функция рисования горизонтального прогресс-бара в указанной строке и заполнения его на "процент"------*/
void LcdBarLine(unsigned char line, unsigned char persent) {
	//line=0...42 - верхняя линия прогресс бара (высотой 5 пикселей)
	DrawProgressBar(0, line, 83, line+4, persent);
}

void lcd_OutFloat1(uint16_t val) {
	lcd_OutInt(val / 10);
	lcd_OutChar('.');
	lcd_OutChar(val % 10);
}

