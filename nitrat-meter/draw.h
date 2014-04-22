void menu_charge (void);                    //������� ������ ���� �������
void menu_main (void);                   //������� ������ ���������� ����
void menu_product (void);                   //������� ������ ���� ������ ��������
void menu_izm (void);                    //������� ������ ���� ���������
void menu_nastr (void);                  //������� ������ ���� ��������
void menu_PDK (void);                    //������� ������ ���� ���� ���

/**
 * ������� ������ ����
 **/
void menu() {
	if (T_menu == 0) {
		return;                 //���� ��� �� ������ 0.26 ��� - �����
	}
	clean_cache();                         //������� ���-������ � ��� ��
	
	if (rezh == 0 && zar > 0) {               //���� ����� ���������� ������ � ������� - ��������� �������� �������
		menu_charge();                          //����� ���� �������� �������
	} else {
		switch (menu_cnt) {
			case 0:
				menu_main();     //����� ���������� ����
				break;
			case 1:
				menu_product();     //����� ���� ������ ��������
				break;
			case 2:
				menu_izm();      //����� ���� ���������
				break;
			case 3:
				menu_nastr();    //����� ���� ��������
				break;
			case 4:
				menu_PDK();      //����� ���� ���� ���
				break;

		}
	}
	wr_batt();                             //����� ������� ��������� � ������ ������� ����
	LcdUpdate();                           //����� ���������� � �������
	T_menu = 0;                              //����� �������� ���������� 0,26-���������� ���������
}

/**
 * ������� ������ ���������� ����
 */
void menu_main() {
	//����� �������
	lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, mnc_nach+1);                           //������ ������������ ������������ ������ |>
	//����� �������� ����
	lcd_OutStrXY(STR_MAIN_MENU_MEASSURE, 1, 1);
	lcd_OutStrXY(STR_MAIN_MENU_SETTINGS, 1, 2);
	lcd_OutStrXY(STR_MAIN_MENU_LIMITS, 1, 3);
	//����� ���������� ������������
	lcd_OutStrXY(STR_UAKK_V, 1, 5);
	lcd_OutInt(0, Ubat, 1, 2);                // ����� ���������� � �
	//lcd_OutStr(STR__V);						// ����� " �"
}
//
/*----������� ������ ���� ������ ��������----*/
void menu_product() {
	//����� �������� ���������
	lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 0, 0);
	lcd_OutStr(STR_PRODUCT_TITLE);
	//����� �������
	lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, 1+n_prod-sdvig_prod);
	// //����� 5 ����� �������� ���� �� ������� nadp_mPDK[31][12]
	for (uint8_t m = 0; m < 5; m++) {
		bool builtIn = m + sdvig_prod < 30;
		lcd_GotoXY(1, m+1);
		for (uint8_t i = 0; i < 8; i++) {
			// ���� �������� ���������� - �� flash
			if (builtIn) {
				lcd_OutChar(pgm_read_byte(&nadp_mPDK[m+sdvig_prod][i]));
			} else {    //����� - �� ��� <- EEPROM
				lcd_OutChar(mas_name[m+sdvig_prod-30][i]);
			}
		}
		lcd_OutChar(builtIn ? ' ' : '*');
	    lcd_OutStr(STR___);                       //����� 2-� ��������
	    lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);
	}
	//����� ������� �����-���� � ��������� ������� �������
	if (sdvig_prod > 0) {
		lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1);         //������ ������� ������������ ����� ^
	}
	if (sdvig_prod < 31) {
		lcd_OutCharXY(CHAR_ARROW_DOWN_EMPTY, 13, 5);        //������ ������� ������������ ���� V
	}
}
//
/*----������� ������ ���� ���������----*/
void menu_izm() {
	unsigned char i=0;                     //������� ��������
	unsigned char max=0;                   //�������� ��� ������ ����������
	uint8_t d = 0;                     //������������� ������
	unsigned int Gmax;                     //������������ �������� ������������ � ����*10
	unsigned int Gmin;                     //����������� �������� ������������ � ����*10
	unsigned int dat;                      //������������� ����� ��� ����������
	unsigned long datl;                    //������������� ����� ��� ����������

	uint16_t  Gx = clk_Gx10(ADCx10);                   //���������� ���������� ������������ � ����*10
	meassureOk = Gx != 0xFFFF;

	Gmax = clk_Gx10(eeprom_read_word(&mas_lim[0][n_prod]));//���������� ������������� �������� ������������ � ����*10
	Gmin = clk_Gx10(eeprom_read_word(&mas_lim[1][n_prod]));//���������� ������������ �������� ������������ � ����*10
	//����� �������-��������� �����
	lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 0, 0);
	//����� ������ �������� ������������ ��������
	if (meassureScreen != MEASSURE_SCREEN_EDIT_NAME || akt != 1) {
		while (i < 8) {                          //����� ������ �� 8 ��������
			if (n_prod < 30) {                    //���� �������� ���������� - �� flash
				lcd_OutChar(pgm_read_byte(&nadp_mPDK[n_prod][i]));
			} else {                              //����� - �������� �� ��� <- EEPROM
				lcd_OutChar(mas_name[n_prod-30][i]);
			}
			i++;
		}
	}
	//����� %
	if ( meassureScreen < 3 &&  meassureStatus != MEASSURE_STATUS_NONE) {             //���� ��� ����� � ������ ���� 0(�����),1(����.) ��� 2(����������)
		if ( Gmax != Gmin) {                     //���� Gmax=Gmin - �������� �� ���������
			if (Gx >= Gmin) {                     //���� % �������������
				datl = Gx;
				dat = ((datl-Gmin)*100)/(Gmax-Gmin);//���������� %
				if (dat < 999) {
					lcd_OutIntBuf(2, dat, 3, 0, 0);//���� <999 - ������ � ��������� ����� ����������� %
				} else {
					sprintf(lcd_buf, "???");    //����� - ����������� � ����� ������ "???"
				}
				LcdStringXY(9, 0);               //����� ������ �� ���������� ������ x=9, y=0
			} else {                               //�����, ���� % �������������
				datl = Gmin;                       //��������� ���� ���������� �� unsigned long
				dat = ((datl-Gx)*100)/(Gmax-Gmin); //���������� %
				if (dat < 99) {
					lcd_OutIntBuf(1, dat, 2, 0, 0);//���� <99 - ������ � ��������� ����� ����������� %
				} else {
					sprintf(lcd_buf, "??");     //����� - ����������� � ����� ������ "???"
				}
				lcd_OutChar('-');                     //���� % ������������� - ����� ����� "-"
				LcdStringXY(10, 0);              //����� ������ �� ���������� ������ x=9, y=0
			}
			lcd_OutChar('%');                       //����� ������� "%"
		}
	}
/*������� ������� ���� ��������� mnc_izm:
  0-�����,1-���������,2-����������,3-������� �������.,4-���.����
*/
	if (meassureScreen < MEASSURE_SCREEN_STATISTIC) {                         //���� ����� ��� ����������
		//����� �������� ������ ������ ����
		if (meassureScreen == MEASSURE_SCREEN_MEASSURE) {
			lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, 1);
		} else {
			lcd_GotoXY(0, 2);
		}
		lcd_OutStr(STR_MEASSURES);
		if (meassureStatus == MEASSURE_STATUS_NONE) {
			lcd_OutStr(STR_MICRO_SM_BRACKET);	// ���� ��� �� ���� �� ������ ������ - ����� ������� "(����)"
		} else {                                //����� - ����� ������������ � ����
			if (!meassureOk) {
				sprintf(lcd_buf, "????.?");//���� ������� ������������ - ����������� ������ � �����
			} else {
				lcd_OutIntBuf(3, Gx, 4, 1, 0);   //����� - ����� � ����� ������������ � ����
			}
			LcdString();                       //����� ������ �� ������ � ������� �������
		}
		lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                         //������ ������� ������������ ������ |>
		//����� �������� ������ ������ ����
		//���� ����� ���������� ������ � ����� ��� �� �������� - ������ ������������ ������������ ������ |>
		if (meassureStatus == MEASSURE_STATUS_NOT_SAVED) {
			lcd_OutCharXY(MEASSURE_STATUS_NOT_SAVED, 0, 2);
		}
		if (meassureOk) {
			switch(meassureStatus) {
				case MEASSURE_STATUS_NOT_SAVED:
					lcd_OutStr(STR_SAVE);
					lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 2);      //���� ����� ��� �� �������� - ������ ������� ������������ ������ |>
					break;
				case MEASSURE_STATUS_SAVED:
					lcd_OutStr(STR_SAVED);
					break;
			}
		}

		//����� �������� ������� ������ ����
		lcd_OutStrXY(STR_MAX, 1, 3);
		if (!meassureOk) {
			LcdStr("????.?");
		} else {
			lcd_OutInt(3, Gmax, 4, 1);         //����� Gmax � ����
		}
		//����� �������� ��������� ������ ����
		lcd_OutStrXY(STR_MIN, 1, 4);
		if (!meassureOk) {
			LcdStr("????.?");
		} else {
			lcd_OutInt(3, Gmin, 4, 1);         //����� Gmin � ����
		}
		//����� �������� ����� ������ ����
		lcd_OutStrXY(STR_MEASSURES_CNT, 1, 5);
		lcd_GotoXY(8, 5);                 //x=8, y=5
		lcd_OutInt(4, eeprom_read_word(mas_Nzam+n_prod), 5, 0);//����� ����� ����������� ������� ��� ���������� ��������
		lcd_OutChar(CHAR_ARROW_DOWN_EMPTY);
	} else {
		if (meassureScreen == MEASSURE_SCREEN_STATISTIC) {
    //����� �������� ����������� �������� � ����
      if (meassureStatus != MEASSURE_STATUS_NONE) {                       //���� ��� �����
    	  if (Gx == 0xffff) {
    		  sprintf(lcd_buf, "????.?");//���� ������� ������������ - ����������� ������ � �����
    	  } else {
    		  lcd_OutIntBuf(3, Gx, 4, 1, 0); //����� ������������ � ����
    	  }
    	  LcdStringXY(1, 1);
    	  lcd_OutStr(STR_MICRO_SM);
      }
      // ����� ������� ���������
      lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1);
      lcd_OutCharXY(CHAR_ARROW_DOWN_EMPTY, 13, 3);
      // ����� ����� �� ������� skal_stat[]
      lcd_GotoXY(0, 5);
      //����� � ���-����� ��� ����� � �����
      for (i = 0; i < 84; i++) {
    	  LcdCache[LcdCacheIdx++] = pgm_read_byte(skal_stat+i);
      }
      // ����� ��������������� �������
      // ���������� ������������� �������� � ������� � ������ ������� ��� ������������� ��������
      max = 0;
      for (i = 0; i < 20; i++) {
    	  d = eeprom_read_byte(&mas_stat[n_prod][i]);//���������� �������� �� EEPROM
    	  if (d > max) {
    		  max = d;
    	  }
      }
      // ����� ����������
      if (max > 0) {                         //���� ������ ���� �� ���� �����
    	  for (i = 0; i < 20; i++) {
    		  dat = (22*eeprom_read_byte(&mas_stat[n_prod][i]))/max;//������������ ������ - 22 �������
    		  d = dat;
    		  if (d > 0) {
    			  LcdRect(12+i*3, 38, d, 3, 1);	//��������� ������������ �������������� (���: y=38)
    		  }
    	  }
      }
      //����� �������������� �������� ������ � ���� ���������-������� ��� ������
      if (meassureStatus != MEASSURE_STATUS_NONE && Gmax != Gmin) {         //���� ��� ����� � ������� Gmax � Gmin
    	  if (Gx >= Gmin) {                    //���� ����� ������ ��������
    		  datl = Gx;                       //��������� ���� ���������� �� unsigned long
    		  if (Gx == Gmax) {
    			  dat = 71;
    		  } else {
    			  dat = (datl-Gmin)*60 / (Gmax-Gmin) + 12;	//x=12...71, 60 �����
    		  }
    		  if (dat > 83) {
    			  d = 83;              //����������� ��� ������������
    		  } else {
    			  d = dat;
    		  }
    	  } else {                            //����� - ����� ����� ��������
    		  datl = Gmin;                     //��������� ���� ���������� �� unsigned long
    		  dat = (datl - Gx)*59/(Gmax-Gmin);
    		  if (dat > 12) {
    			  d = 0;
    		  } else {
    			  d = 12 - dat;
    		  }
    	  }
    	  LcdLine(d, 45, d, 47, PIXEL_XOR);          //����� ����������� ������������ �����
    	  if (d > 0) {
    		  LcdLine(d-1, 46, d-1, 47, PIXEL_XOR);	//����� ����� ������� �������� ������������ �����
    	  }
    	  if (d > 1) {
    		  LcdPixel(d-2, 47, PIXEL_XOR);     //����� ������� �����
    	  }
    	  if (d < 83) {
    		  LcdLine(d+1, 46, d+1, 47, PIXEL_XOR);//����� ����� ������� �������� ������������ �����
    	  }
    	  if (d < 82) {
    		  LcdPixel(d+2, 47, PIXEL_XOR);    //����� ������� �����
    	  }
      }
		} else {                                //�����, ���� ������ "�������� ����������" ��� "���.������"
			//��������� %
      lcd_GotoXY(9, 0);               //x=9, y=0
      LcdStr("    ");                    //4 �������
      if (akt==0)                        //���� �� �������� ������
      {//����� ��������
        i=2;
        if (n_prod>29) d=5;              //���� �������� �������� �� EEPROM - ����� �� ������ "���.������"
        else d=4;                        //����� - ������ "�������� ����������"
        while (i<d)
        { lcd_GotoXY(1, i);           //x=1, y=2...4
          LcdNadp_10(i+7);               //9-"��������  ", 10-"����������", 11-"���.������"
          i++;
        }
        if (d==5) lcd_OutChar('.');           //����� ����� ����� "���.������"
      //����� ������� �����
        lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, (meassureScreen-2)*2); //������ ������������ ������������ ������ |>
      //����� �������-���������� ������
        lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1);            //������ ������� ������������ ����� ^
        lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 3);            //������ ������� ������������ ������ |>
        if (d==5) lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 4);  //���� �������� �������� �� EEPROM - ������ ������� ������������ ������ |>
      }
      else                               //�����, ���� ������� �����-���� �� ������� "�������� ����������" ��� "���.������"
      { if (meassureScreen==3)                  //�����, ���� ������������� ������� ����������
        {//����� ��������
          lcd_GotoXY(3, 2);           //x=3, y=2
          LcdNadp_10(12);                //"��������? "
          lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 0, 4);           //������ ������� ������������ ����� ^
          lcd_OutChar(' ');                   //������
          LcdNadp_10(13);                //"��     ���"
          lcd_OutChar(' ');                   //������
          lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                   //������ ������� ������������ ������ |>
          d=167;                         //������ ������� ������������ ����� <| � ������� x=0, y=0
        }
        else                             //�����, ���� �������������� �������� �������� � EEPROM
        { d=' ';                         //������ ������� � ������� x=0, y=0
          if (n_nazv==0) d=CHAR_ARROW_LEFT_EMPTY;          //���� ������ ������������� ����� - ������ ������ ����������� ����� <| � ������� x=0, y=0
          else lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 1, 2);      //����� - ����� ������� ������������ ����� <| � ������� x=1, y=2
        //����� ��������
          lcd_GotoXY(1, 0);           //x=1, y=0
          lcd_OutChar(' ');                   //������
          LcdNadp_10(14);                //"���.������"
          lcd_OutChar('.');                   //�����
          lcd_GotoXY(3, 2);           //x=3, y=2
          i=0;
          while (i<8)                    //����� ������ �� 8 ��������
          { lcd_OutChar(mas_name[n_prod-30][i]);
            i++;
          }
          if (n_nazv<7)                  //���� �� ��������� ������������� �����
          { lcd_OutChar(' ');                 //������
            lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                 //������ ������� ������������ ������ |>
          }
        //����� ������� ��� ������������� ������
          lcd_OutCharXY(CHAR_ARROWS_UP_DOWN, 3+n_nazv, 3);    //������ ���� ����������� ������������� �����+����
        }
        lcd_OutCharXY(d, 0, 0);               //����� ������� � ������� x=0, y=0
      }
    }
  }
}
//
/*----������� ������ ���� ��������----*/
void menu_nastr () {
	unsigned char i=0;                     //������� ��������
  unsigned char m=0;                     //������� �� ����������� ������� nadp_mPDK[31][12]
  unsigned char y_kur=0;                 //�������������� ������� �� y
  unsigned char dat=0;                   //������
  unsigned int r=0;                      //�������������/������������
//0-����� �����.,1-�������,2-������,3-����,4-�������������,5-U���,6-Rx
//����� �������� ���������
  lcd_GotoXY(0, 0);                   //x=0, y=0
  if (akt==1) dat=32;                    //���� ������� ����� ���� - ������
  else dat=167;                          //����� - ������ ������� ������������ ����� <|
  lcd_OutChar(dat);                           //������ ������� ������������ ����� <| ��� ������
  LcdNadp_10(4);                         //����� ������� " ���������"
//����� ������ �������� ���� ��������
	if (mnc_nastr < 5) {                      //���� ������ �������� ���� ��������
		//���������� �������������� ������� �� y
		y_kur = 1 + mnc_nastr;
		//����� 5 ����� �������� ����
		while (m < 5) {
			lcd_GotoXY(1, m+1);             //��������� ��������� ��� ������ ������� ������� ������
      		LcdNadp_8(m);                      //����� ��������: "T �����.", "������� ", "������  ", "����    ", "��������"
    			//����� ������������ ����������
      		i = 0;                               //����� �������� �������� �������
      		if (m == 0) {                         //���� ����� ������� ���������
				LcdNadp_4(5+ligh_off);           //����� �������� " ---", " 10�", " 15�", " 20�", " 30�", " 45�", " 60�"
			} else {
				if (m==1) {                       //���� ����� ������� ���������
					LcdSkalaXYFont (9, 2, 3, yark*100/9);//����� ����� �� 10 ��������, ����������� �� "�������"
        			} else {
        				if (m==2) {                     //���� ����� �������
						LcdNadp_4(taym_off);         //����� �������� "����", "1���", "2���", "3���", "5���"
          			} else {
          				if (m==3) {                    //���� ����� �����
							LcdNadp_4(13+sound);       //����� "����" ��� "���."
            				} else {                        //����� - ������������� �������
            				lcd_OutInt(3, contrast, 4, 0);
						}
					}
				}
			}
			//����� ������� � ������ �������
      		if (akt == 1) {                        //���� ������� ����� ���� ��������
				if (m==mnc_nastr) dat=164;       //���� ������ ��������� ������ - ������ ���� ����������� ������������� �����+����
        			else dat=32;                     //����� - ������ �������
      		} else
      			dat=CHAR_ARROW_RIGHT_EMPTY;                      //����� - ������ ������� ������������ ������ |>
      		lcd_OutCharXY(dat, 13,m+1);                       //����� �������
      		m++;
		}
	}
//����� ������ �������� ���� ��������
  else {                                  //����� - ����� ������ �������� ���� ��������
	meassure();                             //���������� ������
  //���������� �������������� ������� �� y � ����������� ���� �������
    y_kur=mnc_nastr-3;
  //����� �������� ��������� "����������"
    lcd_GotoXY(0, 0);                 //x=0, y=0
    if (akt==1) dat=32;                  //���� ������� ����� ���� - ������
    else dat=163;                        //����� - ������ ������� ������������ ����� ^
    lcd_OutChar(dat);                         //������ ������� ������������ ����� ^ ��� ������
    lcd_OutChar(' ');                         //����� �������
    LcdNadp_10(5);                       //����� ������� "����������"
  //����� ������ ���������� ������������
    lcd_GotoXY(1, 2);                 //x=1, y=2
    LcdNadp_6(4);                        //"U���,�"
    lcd_OutInt(1, Ubat_mV, 2, 3);           //����� ���������� � X.XXX �
    if (akt==1)                          //���� ������� ����� ���� ��������
    { if (mnc_nastr==5) dat=164;         //���� ������� ������ ����� - ������ ���� ����������� ������������� �����+����
      else dat=32;                       //����� - ������ �������
    }
    else dat=165;                        //����� - ������ ������� ������������ ������ |>
    lcd_OutChar(dat);                         //����� �������
  //����� ������ ����������� �������������
    lcd_GotoXY(1, 3);                 //x=1, y=3
    LcdNadp_6(5);                        //����� ������� "Rx,�� "
    lcd_OutChar(' ');                         //����� �������
    r=clk_Rx(ADCx10);                    //���������� Rx
    if (r==0xffff) sprintf(lcd_buf, "?????");//���� ������� ������������ - ����������� ������ � �����
    else lcd_OutIntBuf(2, r, 5, 0, 0);      //����� - ����� ������������� � ����
    LcdStringXY(8, 3);
    if (akt==1)                          //���� ������� ����� ���� ��������
    { if (mnc_nastr==6) dat=164;         //���� ������� ������ ����� - ������ ���� ����������� ������������� �����+����
      else dat=32;                       //����� - ������ �������
    }
    else dat=165;                        //����� - ������ ������� ������������ ������ |>
    lcd_OutChar(dat);                         //����� �������
  //����� ������ ���������� ������������
    lcd_GotoXY(0, 5);                 //x=0, y=4
    LcdNadp_8(6);                        //����� ������� "Gx,���� "
    r=clk_Gx10(ADCx10);                  //���������� Gx
    if (r==0xffff) sprintf(lcd_buf, "????.?");//���� ������� ������������ - ����������� ������ � �����
    else lcd_OutIntBuf(3, r, 4, 1, 0);      //����� - ����� ������������ � ����
    LcdStringXY(8, 5);
  }
//����� �������
  lcd_GotoXY(0, y_kur);               //x=0, y=1...5
  if (akt==1) dat=167;                   //���� �������� ����� ���� - ������ ����������� ����� <|
  else dat=166;                          //����� - ����������� ����������� ������ |>
  lcd_OutChar(dat);                           //������ ������������ ������������ ������ |> ��� ������� ����� <|
}
//
/*----������� ������ ���� ���� ���----*/
void menu_PDK() {
	//����� �������� ��������� "����� ���,��"
	lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 0, 0);                   //������ ������� ������������ ����� <|, x=0, y=0
	LcdNadp_8(7);                          //����� ������� "����� ��"
	LcdNadp_4(12);                         //����� ������� "�,��"
	//����� �������
	lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, 1+mnc_PDK-sdvig_PDK); //x=0, y=1...5, c����� ������������ ������������ ������ |>
	//����� 5 ����� �������� ���� �� ������� nadp_mPDK[31][12]
	for (uint8_t m = 0; m < 5; m++) {
		lcd_GotoXY(1, m+1);               //��������� ��������� ��� ������ ������� ������� ������
		for (uint8_t i = 0; i < 12; i++) {
			lcd_OutChar(pgm_read_byte(&nadp_mPDK[m+sdvig_PDK][i]));
		}
	}
	//����� ������� �����-���� � ��������� ������� �������
	if (sdvig_PDK > 0) {
		lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1); //������ ������� ������������ ����� ^
	}
	if (sdvig_PDK < 26) {
		lcd_OutCharXY(CHAR_ARROW_DOWN_EMPTY, 13, 5);//������ ������� ������������ ���� V
	}
}
//
/*----������� ������ ���� ���������----*/
void menumess(uint8_t ms) {
	//ms=0 - "����������"
  //ms=1 - "����������� ��������!"
  //ms=2 - "����������� �������. ����������"
  //ms=3 - "����������� �������!"
  //ms=4 - "���������� �������"
  //ms=5 - "����� ��������!"
	clean_cache();                         //������� ���-������ � ��� ��
	for (uint8_t m = 0; m < 3; m++) {
		lcd_GotoXY(2, pgm_read_byte(&tab_mes[ms][m*2]));//��������� ��������� ��� ������ ������� ������� ������
		for (uint8_t i = 0; i < 11; i++) {
			lcd_OutChar(pgm_read_byte(&nadpis_mes[pgm_read_byte(&tab_mes[ms][m*2+1])][i]));
		}
	}
	wr_batt();                             //����� ������� ��������� � ������ ������� ����
	LcdUpdate();                           //����� ���������� � �������
}
//
/*----������� ������ ������� ��������� � ������ ������� ����----*/
void wr_batt() {
	if (zar == 1) {
		batter = 6;                  //���� ���� ������� ������� - ������ ������� �����
	}
	lcd_OutCharXY(169+batter, 13, 0);                    //����� ������� � ����� ��� ��
}
//
/*----������� ������ ���� �������----*/
void menu_charge() {
	unsigned char persent;                 //������� ���������� ������� �������
	//����� �������� ������� �������
	if (Ubat > 415) {
		persent = 100;             //U���>4,15� - 100%
	} else if (Ubat > 380) {
		persent = (Ubat-380)*25/35+75;//U���>3,8� - 75...100%
	} else if (Ubat > 360) {
		persent = (Ubat-360)*50/20+25;//U���>3,6� - 25...75%
	} else if (Ubat > 300) {
		persent = (Ubat-300)*25/60;//U���>3,0� - 0...25%
	} else {
		persent = 0;                        //U���<3,0� - 0%
	}
	LcdBatt(34, 2, 48, 28, persent);       //����� �������� ������� ������� � ����������� �� %
//����� ���������� �������
	lcd_OutIntBuf(0, Ubat, 1, 2, 0);
	LcdStringBold(1, 4);
	LcdStrBold(" �");
}
