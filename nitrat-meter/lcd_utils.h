void LcdAnim (void);                     //������� ������ ��������� � ���� ������������� ����� �������� ������
void lcd_OutInt(bool gash, unsigned int data, uint8_t l, uint8_t r);//������� ������ (r+l)-�����.����� ���� unsigned int � ���.������.���� � ������� �������
void lcd_OutIntBuf(bool gash, unsigned int data, uint8_t l, uint8_t r, bool act);//������� ������ (r+l)-�����.����� ���� unsigned int � ����� � ���.������.����
void LcdNadp_10 (unsigned char n);       //������� ������ ������� �� 10 �������� �� ������� nadp_10 � ������� �������
void LcdNadp_8 (unsigned char n);        //������� ������ ������� �� 8 �������� �� ������� nadp_8 � ������� �������
void LcdNadp_6 (unsigned char n);        //������� ������ ������� �� 6 �������� �� ������� nadp_6 � ������� �������
void LcdNadp_4 (unsigned char n);        //������� ������ ������� �� 4 �������� �� ������� nadp_4 � ������� �������


/*-----������� ������ ������� �� 10 �������� �� ������� nadp_10 � ������� �������----*/
void LcdNadp_10 (unsigned char n)
//n-����� ������ � �������
{ unsigned char i=0;                     //������� �������� �������
  while (i<10)                           //����� ������ �����������
  { lcd_OutChar(pgm_read_byte(&nadp_10[n][i]));
    i++;
  }
  return;
}
//
/*-----������� ������ ������� �� 8 �������� �� ������� nadp_8 � ������� �������----*/
void LcdNadp_8 (unsigned char n)
//n-����� ������ � �������
{ unsigned char i=0;                     //������� �������� �������
  while (i<8)                            //����� ������ �����������
  { lcd_OutChar(pgm_read_byte(&nadp_8[n][i]));
    i++;
  }
  return;
}
//
/*-----������� ������ ������� �� 6 �������� �� ������� nadp_6 � ������� �������----*/
void LcdNadp_6 (unsigned char n)
//n-����� ������ � �������
{ unsigned char i=0;                     //������� �������� �������
  while (i<6)                            //����� ������ �����������
  { lcd_OutChar(pgm_read_byte(&nadp_6[n][i]));
    i++;
  }
  return;
}
//
/*-----������� ������ ������� �� 4 �������� �� ������� nadp_4 � ������� �������----*/
void LcdNadp_4 (unsigned char n)
//n-����� ������ � �������
{ unsigned char i=0;                     //������� �������� �������
  while (i<4)                            //����� ������ �����������
  { lcd_OutChar(pgm_read_byte(&nadp_4[n][i]));
    i++;
  }
}
//
/*-----������� ������ �������� � ���� ������������� ����� �������� ������-----*/
void LcdAnim() {
	unsigned char per=0;                   //������� ��������� ����������
	//�������� ���������� ��������
	lcd_GotoXY(6, 1);                   //x=6, y=1
	LcdStr("        ");                    //8 ��������
	//����� ��������
	for (uint8_t per = 0; per < 100; per += 2) {
		LcdBar(42, 9, 83, 13, per);          //��������� ��������-���� � ���������� ��� �� "�������"
		LcdUpdate();                         //����� ���������� � �������
	}
}
//
/**
 * ������� ������ (r+l)-�����.����� ���� unsigned int � ���.������.���� � ������� �������
*/
void lcd_OutInt(bool gash, unsigned int data, uint8_t l, uint8_t r) {
/*gash=0 - ��� �������, 1 - ������� 1-�� ������.���� �����,
  data - �����; l - ����� ��������� ���� �����, r - ������ �� �������)
*/
	lcd_OutIntBuf(gash, data, l, r, 0);
	LcdString();                           //����� ������ �� ���������� ������ � ������� �������
}

/**
 * ������� ������ (r+l)-�����.����� ���� unsigned int � ����� � ���.������.����
 *
	gash=0 - ��� �������, 1 - ������� 1-�� ������.���� �����,
	data - �����; l - ����� ��������� ���� �����, r - ������ �� �������)
	act=false - ������� ���������, act=trus - � �������� ��������� ������ *
 **/
void lcd_OutIntBuf(bool gash, unsigned int data, uint8_t l, uint8_t r, bool act)
/*
*/
{ unsigned int i=0;                      //������� ��������� ��������
  unsigned char c=1;                     //������� �������� ���������� ������
  unsigned int x=1;                      //������� ���������
  unsigned char mig=1;                   //������� ������� �������
  unsigned char dat;                     //��������������� ����������
  clean_lcd_buf();                       //������� ���������� ������
  if ((b==0)&&(t_026sec>1)&&(act==1))    //���� ��������� ������� ������� � ���� ������� ������� ���.������
  { mig=0;                               //������� ���� �������
  }
  for (i=0; i<(l+r-1); i++)
  { x=x*10;                              //������. 10 � ������� (l+r-1)
  }
  i=l+r-1;                               //������� ��������� ��������
//����� ������� �������
  if (((data/x)==0)&&(l>1)&&(gash>0))    //���� ������� ����������� ����
  { lcd_buf[0]=' ';                      //����� ������� � ��������� �����
  }
  else
  { if (mig==0)                          //���� ��������� ������� ������� � ���� ������� �������
    { lcd_buf[0]=' ';                    //����� ������� " "
    }
    else lcd_buf[0]=data/x+0x30;         //����� - ����� ������ ����� �������� �������
    gash=0;                              //���� ���� �������� �����, ���������� ������� ����� �� ���������
  }
  x=x/10;                                //��������� ������� ���������
  if (gash>0) gash--;                    //��������� ����� ������� �����
  while (i!=0)                           //���� ������ ���������� ��������
  { if ((i==r)&&(r>0))                   //���� ����� �������������� �����
    { if (mig==0)                        //���� ��������� ������� ������� � ���� ������� �������
      { dat=' ';                         //����� ������� " "
      }
      else
      { dat='.';                         //����� ����� �������������� �������
      }
      lcd_buf[c]=dat;                    //����� ����� ������ � �����
      c++;                               //��������� �������� �������� ���������� ������
      r++;                               //��������� r ��� ���������� �������
    }
    else                                 //����� ����� �����
    { if (mig==0)                        //���� ��������� ������� ������� � ���� ������� �������
      { lcd_buf[c]=' ';                  //����� ������� " "
      }
      else
      { dat=(data/x)%10;                 //��������� ����� ���������� �������
        if ((dat==0)&&(gash>0))          //���� �����=0 � ����� �������
        { dat=' ';                       //������
        }
        else
        { dat=dat+0x30;                  //����� ����� ����� ���������� �������
          gash=0;                        //���� ���� �������� �����, ���������� ������� ����� �� ���������
        }
        lcd_buf[c]=(dat);                //����� ����� ������ � �����
      }
      x=x/10;                            //��������� ������� ���������
      if (gash>0) gash--;                //��������� ����� ������� �����
      i--;                               //��������� �������� ��������
      c++;                               //��������� �������� �������� ���������� ������
    }
  }
}
