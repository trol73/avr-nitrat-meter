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
	lcd_Update();                           //����� ���������� � �������
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
					lcd_OutIntBuf(2, dat, 3, 0);//���� <999 - ������ � ��������� ����� ����������� %
				} else {
					sprintf(lcd_buf, "???");    //����� - ����������� � ����� ������ "???"
				}
				LcdStringXY(9, 0);               //����� ������ �� ���������� ������ x=9, y=0
			} else {                               //�����, ���� % �������������
				datl = Gmin;                       //��������� ���� ���������� �� unsigned long
				dat = ((datl-Gx)*100)/(Gmax-Gmin); //���������� %
				if (dat < 99) {
					lcd_OutIntBuf(1, dat, 2, 0);//���� <99 - ������ � ��������� ����� ����������� %
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
				lcd_OutIntBuf(3, Gx, 4, 1);   //����� - ����� � ����� ������������ � ����
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
	} else if (meassureScreen == MEASSURE_SCREEN_STATISTIC) {
    //����� �������� ����������� �������� � ����
      if (meassureStatus != MEASSURE_STATUS_NONE) {                       //���� ��� �����
    	  if (Gx == 0xffff) {
    		  sprintf(lcd_buf, "????.?");//���� ������� ������������ - ����������� ������ � �����
    	  } else {
    		  lcd_OutIntBuf(3, Gx, 4, 1); //����� ������������ � ����
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
    			  lcd_DrawRect(12+i*3, 38, d, 3, 1);	//��������� ������������ �������������� (���: y=38)
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
    	  lcd_DrawLine(d, 45, d, 47, PIXEL_XOR);          //����� ����������� ������������ �����
    	  if (d > 0) {
    		  lcd_DrawLine(d-1, 46, d-1, 47, PIXEL_XOR);	//����� ����� ������� �������� ������������ �����
    	  }
    	  if (d > 1) {
    		  lcd_DrawPixel(d-2, 47, PIXEL_XOR);     //����� ������� �����
    	  }
    	  if (d < 83) {
    		  lcd_DrawLine(d+1, 46, d+1, 47, PIXEL_XOR);//����� ����� ������� �������� ������������ �����
    	  }
    	  if (d < 82) {
    		  lcd_DrawPixel(d+2, 47, PIXEL_XOR);    //����� ������� �����
    	  }
      }
		} else {                                //�����, ���� ������ "�������� ����������" ��� "���.������"
			//��������� %
			lcd_OutStrXY(STR_____, 9, 0);	//4 �������
			if (akt == 0) {                        //���� �� �������� ������
				//����� ��������
				lcd_OutStrXY(STR_CLEAR, 1, 2);
				lcd_OutStrXY(STR_STATISTIC, 1, 3);
				//���� �������� �������� �� EEPROM - ����� �� ������ "���.������"
				if (n_prod > 29) {
					lcd_OutStrXY(STR_EDIT_NAME, 1, 4);
					lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 4);  //���� �������� �������� �� EEPROM - ������ ������� ������������ ������ |>
				}
				//����� ������� �����
				lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, (meassureScreen-2)*2); //������ ������������ ������������ ������ |>
				//����� �������-���������� ������
				lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1);            //������ ������� ������������ ����� ^
				lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 3);            //������ ������� ������������ ������ |>

			} else if (meassureScreen==3) {//�����, ���� ������������� ������� ����������
				// ����� ��������
				lcd_OutStrXY(STR_CLEAR_QUESTION, 3, 2);
				lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 0, 4);           //������ ������� ������������ ����� ^
				lcd_OutStr(STR_YES_NO);
				lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                   //������ ������� ������������ ������ |>
				d = CHAR_ARROW_LEFT_EMPTY;                         //������ ������� ������������ ����� <| � ������� x=0, y=0
			} else {                            //�����, ���� �������������� �������� �������� � EEPROM
				d = ' ';                         //������ ������� � ������� x=0, y=0
				if (n_nazv == 0) d = CHAR_ARROW_LEFT_EMPTY;          //���� ������ ������������� ����� - ������ ������ ����������� ����� <| � ������� x=0, y=0
				else lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 1, 2);      //����� - ����� ������� ������������ ����� <| � ������� x=1, y=2
				//����� ��������
				lcd_OutCharXY(' ', 1, 0);                   //������
				lcd_OutStr(STR_EDIT_NAME_UP);
				lcd_OutChar('.');                   //�����
				lcd_GotoXY(3, 2);           //x=3, y=2
		i=0;
          while (i<8)                    //����� ������ �� 8 ��������
          { lcd_OutChar(mas_name[n_prod-30][i]);
            i++;
          }
          if (n_nazv < 7) {                  //���� �� ��������� ������������� �����
        	  lcd_OutChar(' ');                 //������
        	  lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                 //������ ������� ������������ ������ |>
          }
        //����� ������� ��� ������������� ������
          lcd_OutCharXY(CHAR_ARROWS_UP_DOWN, 3+n_nazv, 3);    //������ ���� ����������� ������������� �����+����
        }
        lcd_OutCharXY(d, 0, 0);               //����� ������� � ������� x=0, y=0
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

	lcd_OutCharXY(akt == 1 ? ' ' : CHAR_ARROW_LEFT_EMPTY, 0, 0);
	lcd_OutStr(STR_SETTINGS_TITLE);
//����� ������ �������� ���� ��������
	if (settingsScreen < SETTINGS_SCREEN_BATTERY) {                      //���� ������ �������� ���� ��������
		//���������� �������������� ������� �� y
		y_kur = 1 + settingsScreen;
		//����� 5 ����� �������� ����
		lcd_OutStrXY(STR_SETTINGS_TIME, 1, 1);
		lcd_OutStr(STR_TIME_VALUES[ligh_off]);

		lcd_OutStrXY(STR_SETTINGS_BRIGHTNESS, 1, 2);
		LcdSkalaXYFont (9, 2, 3, yark*100/9);//����� ����� �� 10 ��������, ����������� �� "�������"

		lcd_OutStrXY(STR_SETTINGS_TIMER, 1, 3);
		lcd_OutStr(STR_TIMER_VALUES[taym_off]);		//����� �������� "����", "1���", "2���", "3���", "5���"

		lcd_OutStrXY(STR_SETTINGS_SOUND, 1, 4);
		lcd_OutStr(sound ? STR_ON : STR_OFF);		//����� "����" ��� "���."

		lcd_OutStrXY(STR_SETTINGS_CONTRAST, 1, 5);
		lcd_OutInt(3, contrast, 4, 0);

		//����� ������� � ������ �������
		for (i = 0; i < 5; i++) {
			if (akt == 1) {                        //���� ������� ����� ���� ��������
				//���� ������ ��������� ������ - ������ ���� ����������� ������������� �����+����
				dat = i == settingsScreen ? CHAR_ARROWS_UP_DOWN : ' ';
			} else {
				dat = CHAR_ARROW_RIGHT_EMPTY;                      //����� - ������ ������� ������������ ������ |>
			}
			lcd_OutCharXY(dat, 13, i+1);                       //����� �������
		}
	} else {                                  //����� - ����� ������ �������� ���� ��������
		meassure();                             //���������� ������
		//���������� �������������� ������� �� y � ����������� ���� �������
		y_kur = settingsScreen - 3;
		//����� �������� ��������� "����������"
		lcd_OutCharXY(akt == 1 ? ' ' : CHAR_ARROW_UP_EMPTY, 0, 0);                         //������ ������� ������������ ����� ^ ��� ������
		lcd_OutChar(' ');                         //����� �������
		lcd_OutStr(STR_CALIBRATE);				//����� ������� "����������"
		//����� ������ ���������� ������������
		lcd_OutStrXY(STR_UAKK_V, 1, 2);		//"U���,�"
		lcd_OutInt(1, Ubat_mV, 2, 3);           //����� ���������� � X.XXX �
		if (akt==1) {                         //���� ������� ����� ���� ��������
			if (settingsScreen==SETTINGS_SCREEN_BATTERY) dat=CHAR_ARROWS_UP_DOWN;         //���� ������� ������ ����� - ������ ���� ����������� ������������� �����+����
			else dat = ' ';                       //����� - ������ �������
		} else dat=CHAR_ARROW_RIGHT_EMPTY;                        //����� - ������ ������� ������������ ������ |>
		lcd_OutChar(dat);                         //����� �������

		//����� ������ ����������� �������������
		lcd_OutStrXY(STR_RX_OHM, 1, 3);
		lcd_OutChar(' ');                         //����� �������
		r = clk_Rx(ADCx10);                    //���������� Rx
		if (r == 0xffff) {
			sprintf(lcd_buf, "?????");//���� ������� ������������ - ����������� ������ � �����
		} else {
			lcd_OutIntBuf(2, r, 5, 0);      //����� - ����� ������������� � ����
		}
		if (akt == 1) {                         //���� ������� ����� ���� ��������
			if (settingsScreen==6) dat=CHAR_ARROWS_UP_DOWN;         //���� ������� ������ ����� - ������ ���� ����������� ������������� �����+����
			else dat = ' ';                       //����� - ������ �������
		} else dat=CHAR_ARROW_RIGHT_EMPTY;                        //����� - ������ ������� ������������ ������ |>
		lcd_OutCharXY(dat, 8, 3);                         //����� �������
		//����� ������ ���������� ������������
		lcd_OutStrXY(STR_GX_MKSM, 0, 5);		// ����� ������� "Gx,���� "
		r = clk_Gx10(ADCx10);                  //���������� Gx
		if (r == 0xffff) sprintf(lcd_buf, "????.?");//���� ������� ������������ - ����������� ������ � �����
		else lcd_OutIntBuf(3, r, 4, 1);      //����� - ����� ������������ � ����
		LcdStringXY(8, 5);
	}
	//����� �������
	lcd_OutCharXY(akt == 1 ? CHAR_ARROW_LEFT_EMPTY : CHAR_ARROW_RIGHT_FILLED, 0, y_kur);	//������ ������������ ������������ ������ |> ��� ������� ����� <|
}
//
/*----������� ������ ���� ���� ���----*/
void menu_PDK() {
	//����� �������� ��������� "����� ���,��"
	lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 0, 0);                   //������ ������� ������������ ����� <|, x=0, y=0
	lcd_OutStr(STR_LIMITS);					//����� ������� "����� ��"
	lcd_OutStr(STR_K_MG);					//����� ������� "�,��"
	//����� �������
	lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, 1+mnc_PDK-sdvig_PDK); //x=0, y=1...5, c����� ������������ ������������ ������ |>
	//����� 5 ����� �������� ���� �� ������� nadp_mPDK[31][12]
	for (uint8_t m = 0; m < 5; m++) {
		lcd_GotoXY(1, m+1);               //��������� ��������� ��� ������ ������� ������� ������
		for (uint8_t i = 0; i < 12; i++) {
			lcd_OutChar(pgm_read_byte(&nadp_mPDK[m + sdvig_PDK][i]));
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
void drawMessage(uint8_t ms) {
	clean_cache();                         //������� ���-������ � ��� ��
	switch (ms) {
		case MSG_POWER_OFF:
			lcd_OutStrXY(STR_SHUTTING_DOWN, 2, 2);
			break;
		case MSG_BATTERY_LOW:
			lcd_OutStrXY(STR_BATTERY, 2, 1);
			lcd_OutStrXY(STR_DISCHARGED, 3, 2);
			lcd_OutStrXY(STR_SHUTTING_DOWN, 2, 4);
			break;
		case MSG_BATTERY_FULL_OFF:
			lcd_OutStrXY(STR_BATTERY, 2, 1);
			lcd_OutStrXY(STR_CHARGED, 3, 2);
			lcd_OutStrXY(STR_SHUTTING_DOWN, 2, 4);
			break;
		case MSG_BATTERY_CHARGED:
			lcd_OutStrXY(STR_BATTERY, 2, 2);
			lcd_OutStrXY(STR_CHARGED, 3, 3);
			break;
		case MSG_STATISTIC_CLEAN:
			lcd_OutStrXY(STR_STATISTIC_IS, 2, 2);
			lcd_OutStrXY(STR_CLEANED, 3, 3);
			break;
		case MSG_MEASSURE_STORED:
			lcd_OutStrXY(STR_RESULT, 3, 3);
			lcd_OutStrXY(STR_STORED, 3, 4);
			break;
	}
	wr_batt();                             //����� ������� ��������� � ������ ������� ����
	lcd_Update();                           //����� ���������� � �������
}
//
/*----������� ������ ������� ��������� � ������ ������� ����----*/
void wr_batt() {
	if (zar == 1) {
		batter = 6;                  //���� ���� ������� ������� - ������ ������� �����
	}
	lcd_OutCharXY(CHAR_BATTERY_0 + batter, 13, 0);                    //����� ������� � ����� ��� ��
}

uint8_t calcBatteryPercent(uint16_t u) {
	if (u > 415) {
		return 100;
	} else if (u > 380) {
		return (u-380)*25/35 + 75;	// U��� > 3,8� -> 75...100%
	} else if (u > 360) {
		return (u-360)*50/20+25;	// U��� > 3,6� - 25...75%
	} else if (u > 300) {
		return (u-300)*25/60;		// U��� > 3,0� - 0...25%
	}
	return 0;
}
//
/*----������� ������ ���� �������----*/
void menu_charge() {
	uint8_t percent = calcBatteryPercent(Ubat);
	DrawBattery(34, 2, 48, 28, percent);       //����� �������� ������� ������� � ����������� �� %
	// ����� ���������� �������
	lcd_OutIntBuf(0, Ubat, 1, 2);
	LcdStringBold(1, 4);
	LcdStrBold(" �");
}
