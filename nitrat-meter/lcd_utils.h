void LcdAnim (void);                     //������� ������ ��������� � ���� ������������� ����� �������� ������
void lcd_OutInt(uint16_t val);
void lcd_OutIntXY(uint16_t val, uint8_t x, uint8_t y);
//void lcd_OutInt(bool gash, unsigned int data, uint8_t l, uint8_t r);//������� ������ (r+l)-�����.����� ���� unsigned int � ���.������.���� � ������� �������
void lcd_OutIntBuf(bool gash, unsigned int data, uint8_t l, uint8_t r);//������� ������ (r+l)-�����.����� ���� unsigned int � ����� � ���.������.����
void DrawBattery(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t persent);//����� ��������� � ����������� �� �� %
void DrawProgressBar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t persent);//����� ��������-���� � ���������� ��� �� "�������"
void LcdBarLine(uint8_t line, uint8_t persent);//����� ��������-���� � ��������� ������

void lcd_OutFloat1(uint16_t val);


/*-----������� ������ �������� � ���� ������������� ����� �������� ������-----*/
void LcdAnim() {
	// �������� ���������� ��������
	lcd_OutStrXY(STR_________, 6, 1);
	//����� ��������
	for (uint8_t per = 0; per < 100; per += 2) {
		DrawProgressBar(42, 9, 83, 13, per);          //��������� ��������-���� � ���������� ��� �� "�������"
		lcd_Update();                         //����� ���������� � �������
	}
}
//
/**
 * ������� ������ (r+l)-�����.����� ���� unsigned int � ���.������.���� � ������� �������
*/
//void lcd_OutInt(bool gash, unsigned int data, uint8_t l, uint8_t r) {
/*gash=0 - ��� �������, 1 - ������� 1-�� ������.���� �����,
  data - �����; l - ����� ��������� ���� �����, r - ������ �� �������)
*/
//	lcd_OutIntBuf(gash, data, l, r);
//	LcdString();                           //����� ������ �� ���������� ������ � ������� �������
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
 * ������� ������ (r+l)-�����.����� ���� unsigned int � ����� � ���.������.����
 *
	gash=0 - ��� �������, 1 - ������� 1-�� ������.���� �����,
	data - �����; l - ����� ��������� ���� �����, r - ������ �� �������)
	act=false - ������� ���������, act=trus - � �������� ��������� ������ *
 **/
void lcd_OutIntBuf(bool gash, unsigned int data, uint8_t l, uint8_t r) {
	unsigned int i=0;                      //������� ��������� ��������
	uint8_t c=1;                     //������� �������� ���������� ������
	unsigned int x=1;                      //������� ���������
	uint8_t  dat;                     //��������������� ����������

	clean_lcd_buf();                       //������� ���������� ������

	for (i = 0; i < l + r - 1; i++) {
		x *= 10;                              //������. 10 � ������� (l+r-1)
	}
	i = l + r - 1;                            //������� ��������� ��������
	//����� ������� �������
	if (((data / x)==0) && l > 1 && gash) {   //���� ������� ����������� ����
		lcd_buf[0] = ' ';                      //����� ������� � ��������� �����
	} else {
		lcd_buf[0] = data/x + 0x30;
		gash = 0;                              //���� ���� �������� �����, ���������� ������� ����� �� ���������
	}
	x /= 10;                                //��������� ������� ���������
	if (gash > 0) {
		gash--;                    //��������� ����� ������� �����
	}
	while (i != 0) {                          //���� ������ ���������� ��������
		if (i == r && r > 0) {                  //���� ����� �������������� �����
			lcd_buf[c++] = '.';
			r++;                               //��������� r ��� ���������� �������
		} else {                                //����� ����� �����
				dat = (data/x) % 10;                 //��������� ����� ���������� �������
				if (dat == 0 && gash > 0) {         //���� �����=0 � ����� �������
					dat = ' ';                       //������
				} else {
					dat += 0x30;                  //����� ����� ����� ���������� �������
					gash=0;                        //���� ���� �������� �����, ���������� ������� ����� �� ���������
				}
				lcd_buf[c] = dat;                //����� ����� ������ � �����

			x /= 10;                            //��������� ������� ���������
			if (gash > 0) {
				gash--;                //��������� ����� ������� �����
			}
			i--;                               //��������� �������� ��������
			c++;                               //��������� �������� �������� ���������� ������
		}
	}
}



//	lcd_OutIntBuf(2, dat, 3, 0, 0)		0..999 %
//  lcd_OutIntBuf(1, dat, 2, 0, 0); 	0..99%
//  lcd_OutIntBuf(3, Gx, 4, 1, 0);   //����� - ����� � ����� ������������ � ����
//  lcd_OutIntBuf(3, Gx, 4, 1, 0); //����� ������������ � ����
//  lcd_OutIntBuf(2, r, 5, 0, 0);      //����� - ����� ������������� � ����
//  lcd_OutIntBuf(3, r, 4, 1, 0);      //����� - ����� ������������ � ����
//  lcd_OutIntBuf(0, Ubat, 1, 2, 0);


/*-----������� ��������� ��������� � ����������� � %-------*/
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
  lcd_DrawLine(x1,   y2,   x2,   y2,   PIXEL_ON);    //������ �����
  lcd_DrawLine(x2,   y1+2, x2,   y2,   PIXEL_ON);    //����� ������
  lcd_DrawLine(x1,   y1+2, x1,   y2,   PIXEL_ON);    //����� �����
  lcd_DrawLine(x1,   y1+2, x2,   y1+2, PIXEL_ON);    //������� �����
  lcd_DrawLine(x1+k, y1+1, x2-k, y1+1, PIXEL_ON);    //��������
  lcd_DrawLine(x1+k, y1,   x2-k, y1,   PIXEL_ON);    //��������
  horizon_line=persent*(y2-y1-5)/100;    //������� �������
	for(i=0; i<horizon_line; i++)
		lcd_DrawLine(x1+2, y2-2-i, x2-2, y2-2-i, PIXEL_ON);//������� ��������������� �������
	horizon_line2 = (y2-y1-5);               //������� ������� ��� �������
	for (i = horizon_line2; i > horizon_line; i--)
		lcd_DrawLine(x1+2, y2-2-i, x2-2, y2-2-i, PIXEL_OFF);//������� �� �����
}

/*-----������� ��������� ��������-���� � ���������� ��� �� "�������"-------*/
void DrawProgressBar(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t persent) {
#ifdef DISPLAY_CHECK_ARGS
	if (persent > 100) {
		return;
	}
#endif
	lcd_DrawLineH(y2, x1+2, x2-2, PIXEL_ON);	// ����� �����
	lcd_DrawLineV(x2-2, y1, y2, PIXEL_ON);		// ����� ������
	lcd_DrawLineV(x1+2, y1, y2, PIXEL_ON);		// ����� �����
	lcd_DrawLineH(y1, x1+2, x2-2, PIXEL_ON);	// ����� ������

	lcd_DrawLineV(x2-1, y1+1, y2-1, PIXEL_ON);	// ����������� ����� ������
	lcd_DrawLineV(x1+1, y1+1, y2-1, PIXEL_ON);	// ����������� ����� �����
	lcd_DrawLineV(x2, y1+2, y2-2, PIXEL_ON);	// ��� ����������� ����� ������
	lcd_DrawLineV(x1, y1+2, y2-2, PIXEL_ON);	// ��� ����������� ����� �����

	// ������ ������������� �������
	uint16_t line = ((uint16_t)persent)*(x2-x1-8)/100;
	uint8_t fillWidth = (uint8_t)line;
	uint8_t emptyWidth = x2 - x1 - 8 - fillWidth;
	uint8_t height = y2-y1-4;
	lcd_DrawRect(x1+4, y1+2, fillWidth, height, PIXEL_ON);
	lcd_DrawRect(x1+4 + fillWidth, y1+2, emptyWidth, height, PIXEL_OFF);
}

/*-----������� ��������� ��������������� ��������-���� � ��������� ������ � ���������� ��� �� "�������"------*/
void LcdBarLine(unsigned char line, unsigned char persent) {
	//line=0...42 - ������� ����� �������� ���� (������� 5 ��������)
	DrawProgressBar(0, line, 83, line+4, persent);
}

void lcd_OutFloat1(uint16_t val) {
	lcd_OutInt(val / 10);
	lcd_OutChar('.');
	lcd_OutChar(val % 10);
}

