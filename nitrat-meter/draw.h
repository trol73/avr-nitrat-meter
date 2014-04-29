void menu_charge (void);                    //Функция вывода меню зарядки
void menu_main (void);                   //Функция вывода начального меню
void menu_product (void);                   //Функция вывода меню выбора продукта
void menu_izm (void);                    //Функция вывода меню измерения
void menu_nastr (void);                  //Функция вывода меню настроек
void menu_PDK (void);                    //Функция вывода меню норм ПДК

/**
 * Функция вывода меню
 **/
void menu() {
	if (T_menu == 0) {
		return;                 //Если еще не прошло 0.26 сек - выход
	}
	clean_cache();                         //Очистка КЭШ-буфера в ОЗУ МК
	
	if (rezh == 0 && zar > 0) {               //Если режим выключения работы и зарядки - индикация процесса зарядки
		menu_charge();                          //Вывод меню процесса зарядки
	} else {
		switch (menu_cnt) {
			case 0:
				menu_main();     //Вывод начального меню
				break;
			case 1:
				menu_product();     //Вывод меню выбора продукта
				break;
			case 2:
				menu_izm();      //Вывод меню измерения
				break;
			case 3:
				menu_nastr();    //Вывод меню настроек
				break;
			case 4:
				menu_PDK();      //Вывод меню норм ПДК
				break;

		}
	}
	wr_batt();                             //Вывод символа батарейки в правом верхнем углу
	lcd_Update();                           //Вывод информации в дисплей
	T_menu = 0;                              //Сброс признака прошедшего 0,26-секундного интервала
}

/**
 * Функция вывода начального меню
 */
void menu_main() {
	//Вывод курсора
	lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, mnc_nach+1);                           //Символ закрашенного треугольника вправо |>
	//Вывод надписей меню
	lcd_OutStrXY(STR_MAIN_MENU_MEASSURE, 1, 1);
	lcd_OutStrXY(STR_MAIN_MENU_SETTINGS, 1, 2);
	lcd_OutStrXY(STR_MAIN_MENU_LIMITS, 1, 3);
	//Вывод напряжения аккумулятора
	lcd_OutStrXY(STR_UAKK_V, 1, 5);
	lcd_OutInt(0, Ubat, 1, 2);                // Вывод напряжения в В
	//lcd_OutStr(STR__V);						// Вывод " В"
}
//
/*----Функция вывода меню выбора продукта----*/
void menu_product() {
	//Вывод верхнего заголовка
	lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 0, 0);
	lcd_OutStr(STR_PRODUCT_TITLE);
	//Вывод курсора
	lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, 1+n_prod-sdvig_prod);
	// //Вывод 5 строк надписей меню из массива nadp_mPDK[31][12]
	for (uint8_t m = 0; m < 5; m++) {
		bool builtIn = m + sdvig_prod < 30;
		lcd_GotoXY(1, m+1);
		for (uint8_t i = 0; i < 8; i++) {
			// Если названия неизменные - из flash
			if (builtIn) {
				lcd_OutChar(pgm_read_byte(&nadp_mPDK[m+sdvig_prod][i]));
			} else {    //Иначе - из ОЗУ <- EEPROM
				lcd_OutChar(mas_name[m+sdvig_prod-30][i]);
			}
		}
		lcd_OutChar(builtIn ? ' ' : '*');
	    lcd_OutStr(STR___);                       //Вывод 2-х пробелов
	    lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);
	}
	//Вывод стрелок вверх-вниз в последней колонке дисплея
	if (sdvig_prod > 0) {
		lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1);         //Символ пустого треугольника вверх ^
	}
	if (sdvig_prod < 31) {
		lcd_OutCharXY(CHAR_ARROW_DOWN_EMPTY, 13, 5);        //Символ пустого треугольника вниз V
	}
}
//
/*----Функция вывода меню измерения----*/
void menu_izm() {
	unsigned char i=0;                     //Счетчик символов
	unsigned char max=0;                   //Максимум для вывода статистики
	uint8_t d = 0;                     //Промежуточные данные
	unsigned int Gmax;                     //Максимальное значение проводимости в мкСм*10
	unsigned int Gmin;                     //Минимальное значение проводимости в мкСм*10
	unsigned int dat;                      //Промежуточные даные для вычислений
	unsigned long datl;                    //Промежуточные даные для вычислений

	uint16_t  Gx = clk_Gx10(ADCx10);                   //Вычисление измеренной проводимости в мкСм*10
	meassureOk = Gx != 0xFFFF;

	Gmax = clk_Gx10(eeprom_read_word(&mas_lim[0][n_prod]));//Вычисление максимального значения проводимости в мкСм*10
	Gmin = clk_Gx10(eeprom_read_word(&mas_lim[1][n_prod]));//Вычисление минимального значения проводимости в мкСм*10
	//Вывод стрелки-указателя влево
	lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 0, 0);
	//Вывод строки названия проверяемого продукта
	if (meassureScreen != MEASSURE_SCREEN_EDIT_NAME || akt != 1) {
		while (i < 8) {                          //Вывод строки из 8 символов
			if (n_prod < 30) {                    //Если названия неизменные - из flash
				lcd_OutChar(pgm_read_byte(&nadp_mPDK[n_prod][i]));
			} else {                              //Иначе - название из ОЗУ <- EEPROM
				lcd_OutChar(mas_name[n_prod-30][i]);
			}
			i++;
		}
	}
	//Вывод %
	if ( meassureScreen < 3 &&  meassureStatus != MEASSURE_STATUS_NONE) {             //Если был замер и пункты меню 0(замер),1(сохр.) или 2(статистика)
		if ( Gmax != Gmin) {                     //Если Gmax=Gmin - проценты не выводятся
			if (Gx >= Gmin) {                     //Если % положительные
				datl = Gx;
				dat = ((datl-Gmin)*100)/(Gmax-Gmin);//Вычисление %
				if (dat < 999) {
					lcd_OutIntBuf(2, dat, 3, 0);//Если <999 - запись в текстовый буфер вычисленных %
				} else {
					sprintf(lcd_buf, "???");    //Иначе - копирование в буфер строки "???"
				}
				LcdStringXY(9, 0);               //Вывод строки из текстового буфера x=9, y=0
			} else {                               //Иначе, если % отрицательные
				datl = Gmin;                       //Изменение типа переменной на unsigned long
				dat = ((datl-Gx)*100)/(Gmax-Gmin); //Вычисление %
				if (dat < 99) {
					lcd_OutIntBuf(1, dat, 2, 0);//Если <99 - запись в текстовый буфер вычисленных %
				} else {
					sprintf(lcd_buf, "??");     //Иначе - копирование в буфер строки "???"
				}
				lcd_OutChar('-');                     //Если % отрицательные - вывод знака "-"
				LcdStringXY(10, 0);              //Вывод строки из текстового буфера x=9, y=0
			}
			lcd_OutChar('%');                       //Вывод символа "%"
		}
	}
/*Счетчик пунктов меню измерений mnc_izm:
  0-замер,1-сохранить,2-статистика,3-очистка статист.,4-ред.назв
*/
	if (meassureScreen < MEASSURE_SCREEN_STATISTIC) {                         //Если замер или сохранение
		//Вывод надписей первой строки меню
		if (meassureScreen == MEASSURE_SCREEN_MEASSURE) {
			lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, 1);
		} else {
			lcd_GotoXY(0, 2);
		}
		lcd_OutStr(STR_MEASSURES);
		if (meassureStatus == MEASSURE_STATUS_NONE) {
			lcd_OutStr(STR_MICRO_SM_BRACKET);	// Если еще не было ни одного замера - вывод надписи "(мкСм)"
		} else {                                //Иначе - вывод проводимости в мкСм
			if (!meassureOk) {
				sprintf(lcd_buf, "????.?");//Если признак переполнения - копирование строки в буфер
			} else {
				lcd_OutIntBuf(3, Gx, 4, 1);   //Иначе - вывод в буфер проводимости в мкСм
			}
			LcdString();                       //Вывод строки из буфера с текущей позиции
		}
		lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                         //Символ пустого треугольника вправо |>
		//Вывод надписей второй строки меню
		//Если пункт сохранения замера и замер ещё не сохранен - символ закрашенного треугольника вправо |>
		if (meassureStatus == MEASSURE_STATUS_NOT_SAVED) {
			lcd_OutCharXY(MEASSURE_STATUS_NOT_SAVED, 0, 2);
		}
		if (meassureOk) {
			switch(meassureStatus) {
				case MEASSURE_STATUS_NOT_SAVED:
					lcd_OutStr(STR_SAVE);
					lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 2);      //Если замер ещё не сохранен - символ пустого треугольника вправо |>
					break;
				case MEASSURE_STATUS_SAVED:
					lcd_OutStr(STR_SAVED);
					break;
			}
		}

		//Вывод надписей третьей строки меню
		lcd_OutStrXY(STR_MAX, 1, 3);
		if (!meassureOk) {
			LcdStr("????.?");
		} else {
			lcd_OutInt(3, Gmax, 4, 1);         //Вывод Gmax в мкСм
		}
		//Вывод надписей четвертой строки меню
		lcd_OutStrXY(STR_MIN, 1, 4);
		if (!meassureOk) {
			LcdStr("????.?");
		} else {
			lcd_OutInt(3, Gmin, 4, 1);         //Вывод Gmin в мкСм
		}
		//Вывод надписей пятой строки меню
		lcd_OutStrXY(STR_MEASSURES_CNT, 1, 5);
		lcd_GotoXY(8, 5);                 //x=8, y=5
		lcd_OutInt(4, eeprom_read_word(mas_Nzam+n_prod), 5, 0);//Вывод числа выполненных замеров для выбранного продукта
		lcd_OutChar(CHAR_ARROW_DOWN_EMPTY);
	} else if (meassureScreen == MEASSURE_SCREEN_STATISTIC) {
    //Вывод текущего измеренного значения в мкСм
      if (meassureStatus != MEASSURE_STATUS_NONE) {                       //Если был замер
    	  if (Gx == 0xffff) {
    		  sprintf(lcd_buf, "????.?");//Если признак переполнения - копирование строки в буфер
    	  } else {
    		  lcd_OutIntBuf(3, Gx, 4, 1); //Вывод проводимости в мкСм
    	  }
    	  LcdStringXY(1, 1);
    	  lcd_OutStr(STR_MICRO_SM);
      }
      // Вывод стрелок прокрутки
      lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1);
      lcd_OutCharXY(CHAR_ARROW_DOWN_EMPTY, 13, 3);
      // Вывод шкалы из массива skal_stat[]
      lcd_GotoXY(0, 5);
      //Вывод в кэш-буфер ОЗУ байта в цикле
      for (i = 0; i < 84; i++) {
    	  LcdCache[LcdCacheIdx++] = pgm_read_byte(skal_stat+i);
      }
      // Вывод статистического графика
      // Нахождение максимального значения в ячейках с числом замеров для отображаемого продукта
      max = 0;
      for (i = 0; i < 20; i++) {
    	  d = eeprom_read_byte(&mas_stat[n_prod][i]);//Считывание значения из EEPROM
    	  if (d > max) {
    		  max = d;
    	  }
      }
      // Вывод статистики
      if (max > 0) {                         //Если найден хотя бы один замер
    	  for (i = 0; i < 20; i++) {
    		  dat = (22*eeprom_read_byte(&mas_stat[n_prod][i]))/max;//Максимальная высота - 22 пикселя
    		  d = dat;
    		  if (d > 0) {
    			  lcd_DrawRect(12+i*3, 38, d, 3, 1);	//Рисование закрашенного прямоугольника (низ: y=38)
    		  }
    	  }
      }
      //Вывод местоположения текущего замера в виде указателя-стрелки под шкалой
      if (meassureStatus != MEASSURE_STATUS_NONE && Gmax != Gmin) {         //Если был замер и имеются Gmax и Gmin
    	  if (Gx >= Gmin) {                    //Если вывод правее минимума
    		  datl = Gx;                       //Изменение типа переменной на unsigned long
    		  if (Gx == Gmax) {
    			  dat = 71;
    		  } else {
    			  dat = (datl-Gmin)*60 / (Gmax-Gmin) + 12;	//x=12...71, 60 ячеек
    		  }
    		  if (dat > 83) {
    			  d = 83;              //Ограничение при зашкаливании
    		  } else {
    			  d = dat;
    		  }
    	  } else {                            //Иначе - вывод левее минимума
    		  datl = Gmin;                     //Изменение типа переменной на unsigned long
    		  dat = (datl - Gx)*59/(Gmax-Gmin);
    		  if (dat > 12) {
    			  d = 0;
    		  } else {
    			  d = 12 - dat;
    		  }
    	  }
    	  lcd_DrawLine(d, 45, d, 47, PIXEL_XOR);          //Вывод центральной вертикальной линии
    	  if (d > 0) {
    		  lcd_DrawLine(d-1, 46, d-1, 47, PIXEL_XOR);	//Вывод левой боковой короткой вертикальной линии
    	  }
    	  if (d > 1) {
    		  lcd_DrawPixel(d-2, 47, PIXEL_XOR);     //Вывод пикселя слева
    	  }
    	  if (d < 83) {
    		  lcd_DrawLine(d+1, 46, d+1, 47, PIXEL_XOR);//Вывод левой боковой короткой вертикальной линии
    	  }
    	  if (d < 82) {
    		  lcd_DrawPixel(d+2, 47, PIXEL_XOR);    //Вывод пикселя слева
    	  }
      }
		} else {                                //Иначе, если пункты "Очистить статистику" или "Ред.назван"
			//Затирание %
			lcd_OutStrXY(STR_____, 9, 0);	//4 пробела
			if (akt == 0) {                        //Если не активные пункты
				//Вывод надписей
				lcd_OutStrXY(STR_CLEAR, 1, 2);
				lcd_OutStrXY(STR_STATISTIC, 1, 3);
				//Если название продукта из EEPROM - вывод до строки "Ред.назван"
				if (n_prod > 29) {
					lcd_OutStrXY(STR_EDIT_NAME, 1, 4);
					lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 4);  //Если название продукта из EEPROM - символ пустого треугольника вправо |>
				}
				//Вывод курсора слева
				lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, (meassureScreen-2)*2); //Символ закрашенного треугольника вправо |>
				//Вывод стрелок-указателей справа
				lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1);            //Символ пустого треугольника вверх ^
				lcd_OutCharXY(CHAR_ARROW_RIGHT_EMPTY, 13, 3);            //Символ пустого треугольника вправо |>

			} else if (meassureScreen==3) {//Иначе, если подтверждение очистки статистики
				// Вывод надписей
				lcd_OutStrXY(STR_CLEAR_QUESTION, 3, 2);
				lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 0, 4);           //Символ пустого треугольника вверх ^
				lcd_OutStr(STR_YES_NO);
				lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                   //Символ пустого треугольника вправо |>
				d = CHAR_ARROW_LEFT_EMPTY;                         //Символ пустого треугольника влево <| в позиции x=0, y=0
			} else {                            //Иначе, если редактирование названия продукта в EEPROM
				d = ' ';                         //Символ пробела в позиции x=0, y=0
				if (n_nazv == 0) d = CHAR_ARROW_LEFT_EMPTY;          //Если первая редактируемая буква - символ пустой треугольник влево <| в позиции x=0, y=0
				else lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 1, 2);      //Иначе - вывод пустого треугольника влево <| в позиции x=1, y=2
				//Вывод надписей
				lcd_OutCharXY(' ', 1, 0);                   //Пробел
				lcd_OutStr(STR_EDIT_NAME_UP);
				lcd_OutChar('.');                   //Точка
				lcd_GotoXY(3, 2);           //x=3, y=2
		i=0;
          while (i<8)                    //Вывод строки из 8 символов
          { lcd_OutChar(mas_name[n_prod-30][i]);
            i++;
          }
          if (n_nazv < 7) {                  //Если не последняя редактируемая буква
        	  lcd_OutChar(' ');                 //Пробел
        	  lcd_OutChar(CHAR_ARROW_RIGHT_EMPTY);                 //Символ пустого треугольника вправо |>
          }
        //Вывод курсора под редактируемой буквой
          lcd_OutCharXY(CHAR_ARROWS_UP_DOWN, 3+n_nazv, 3);    //Символ двух закрашенных треугольников вверх+вниз
        }
        lcd_OutCharXY(d, 0, 0);               //Вывод символа в позиции x=0, y=0
    }
}
//
/*----Функция вывода меню настроек----*/
void menu_nastr () {
	unsigned char i=0;                     //Счетчик символов
	unsigned char m=0;                     //Счетчик по горизонтали массива nadp_mPDK[31][12]
	unsigned char y_kur=0;                 //Местоположение курсора по y
	unsigned char dat=0;                   //Данные
	unsigned int r=0;                      //Сопротивление/проводимость
//0-время подсв.,1-яркость,2-таймер,3-звук,4-контрастность,5-Uакк,6-Rx
//Вывод верхнего заголовка

	lcd_OutCharXY(akt == 1 ? ' ' : CHAR_ARROW_LEFT_EMPTY, 0, 0);
	lcd_OutStr(STR_SETTINGS_TITLE);
//Вывод первой страницы меню настроек
	if (settingsScreen < SETTINGS_SCREEN_BATTERY) {                      //Если первая страница меню настроек
		//Вычисление местоположения курсора по y
		y_kur = 1 + settingsScreen;
		//Вывод 5 строк надписей меню
		lcd_OutStrXY(STR_SETTINGS_TIME, 1, 1);
		lcd_OutStr(STR_TIME_VALUES[ligh_off]);

		lcd_OutStrXY(STR_SETTINGS_BRIGHTNESS, 1, 2);
		LcdSkalaXYFont (9, 2, 3, yark*100/9);//Вывод шкалы из 10 черточек, заполненной на "процент"

		lcd_OutStrXY(STR_SETTINGS_TIMER, 1, 3);
		lcd_OutStr(STR_TIMER_VALUES[taym_off]);		//Вывод надписей "Выкл", "1мин", "2мин", "3мин", "5мин"

		lcd_OutStrXY(STR_SETTINGS_SOUND, 1, 4);
		lcd_OutStr(sound ? STR_ON : STR_OFF);		//Вывод "Выкл" или "Вкл."

		lcd_OutStrXY(STR_SETTINGS_CONTRAST, 1, 5);
		lcd_OutInt(3, contrast, 4, 0);

		//Вывод значков в правой колонке
		for (i = 0; i < 5; i++) {
			if (akt == 1) {                        //Если активен пункт меню настроек
				//Если строка активного пункта - символ двух закрашенных треугольников вверх+вниз
				dat = i == settingsScreen ? CHAR_ARROWS_UP_DOWN : ' ';
			} else {
				dat = CHAR_ARROW_RIGHT_EMPTY;                      //Иначе - символ пустого треугольника вправо |>
			}
			lcd_OutCharXY(dat, 13, i+1);                       //Вывод символа
		}
	} else {                                  //Иначе - вывод второй страницы меню настроек
		meassure();                             //Проведение замера
		//Вычисление местоположения курсора по y и определение вида курсора
		y_kur = settingsScreen - 3;
		//Вывод верхнего заголовка "КАЛИБРОВКА"
		lcd_OutCharXY(akt == 1 ? ' ' : CHAR_ARROW_UP_EMPTY, 0, 0);                         //Символ пустого треугольника вверх ^ или пробел
		lcd_OutChar(' ');                         //Вывод пробела
		lcd_OutStr(STR_CALIBRATE);				//Вывод надписи "КАЛИБРОВКА"
		//Вывод пункта напряжения аккумулятора
		lcd_OutStrXY(STR_UAKK_V, 1, 2);		//"Uакк,В"
		lcd_OutInt(1, Ubat_mV, 2, 3);           //Вывод напряжения в X.XXX В
		if (akt==1) {                         //Если активен пункт меню настроек
			if (settingsScreen==SETTINGS_SCREEN_BATTERY) dat=CHAR_ARROWS_UP_DOWN;         //Если активен данный пункт - символ двух закрашенных треугольников вверх+вниз
			else dat = ' ';                       //Иначе - символ пробела
		} else dat=CHAR_ARROW_RIGHT_EMPTY;                        //Иначе - символ пустого треугольника вправо |>
		lcd_OutChar(dat);                         //Вывод символа

		//Вывод пункта измеренного сопротивления
		lcd_OutStrXY(STR_RX_OHM, 1, 3);
		lcd_OutChar(' ');                         //Вывод пробела
		r = clk_Rx(ADCx10);                    //Вычисление Rx
		if (r == 0xffff) {
			sprintf(lcd_buf, "?????");//Если признак переполнения - копирование строки в буфер
		} else {
			lcd_OutIntBuf(2, r, 5, 0);      //Иначе - вывод сопротивления в Омах
		}
		if (akt == 1) {                         //Если активен пункт меню настроек
			if (settingsScreen==6) dat=CHAR_ARROWS_UP_DOWN;         //Если активен данный пункт - символ двух закрашенных треугольников вверх+вниз
			else dat = ' ';                       //Иначе - символ пробела
		} else dat=CHAR_ARROW_RIGHT_EMPTY;                        //Иначе - символ пустого треугольника вправо |>
		lcd_OutCharXY(dat, 8, 3);                         //Вывод символа
		//Вывод пункта измеренной проводимости
		lcd_OutStrXY(STR_GX_MKSM, 0, 5);		// Вывод надписи "Gx,мкСм "
		r = clk_Gx10(ADCx10);                  //Вычисление Gx
		if (r == 0xffff) sprintf(lcd_buf, "????.?");//Если признак переполнения - копирование строки в буфер
		else lcd_OutIntBuf(3, r, 4, 1);      //Иначе - вывод проводимости в мкСм
		LcdStringXY(8, 5);
	}
	//Вывод курсора
	lcd_OutCharXY(akt == 1 ? CHAR_ARROW_LEFT_EMPTY : CHAR_ARROW_RIGHT_FILLED, 0, y_kur);	//Символ закрашенного треугольника вправо |> или пустого влево <|
}
//
/*----Функция вывода меню норм ПДК----*/
void menu_PDK() {
	//Вывод верхнего заголовка "Нормы ПДК,мг"
	lcd_OutCharXY(CHAR_ARROW_LEFT_EMPTY, 0, 0);                   //Символ пустого треугольника влево <|, x=0, y=0
	lcd_OutStr(STR_LIMITS);					//Вывод надписи "Нормы ПД"
	lcd_OutStr(STR_K_MG);					//Вывод надписи "К,мг"
	//Вывод курсора
	lcd_OutCharXY(CHAR_ARROW_RIGHT_FILLED, 0, 1+mnc_PDK-sdvig_PDK); //x=0, y=1...5, cимвол закрашенного треугольника вправо |>
	//Вывод 5 строк надписей меню из массива nadp_mPDK[31][12]
	for (uint8_t m = 0; m < 5; m++) {
		lcd_GotoXY(1, m+1);               //Установка координат для вывода первого символа строки
		for (uint8_t i = 0; i < 12; i++) {
			lcd_OutChar(pgm_read_byte(&nadp_mPDK[m + sdvig_PDK][i]));
		}
	}
	//Вывод стрелок вверх-вниз в последней колонке дисплея
	if (sdvig_PDK > 0) {
		lcd_OutCharXY(CHAR_ARROW_UP_EMPTY, 13, 1); //Символ пустого треугольника вверх ^
	}
	if (sdvig_PDK < 26) {
		lcd_OutCharXY(CHAR_ARROW_DOWN_EMPTY, 13, 5);//Символ пустого треугольника вниз V
	}
}
//
/*----Функция вывода меню сообщений----*/
void drawMessage(uint8_t ms) {
	clean_cache();                         //Очистка КЭШ-буфера в ОЗУ МК
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
	wr_batt();                             //Вывод символа батарейки в правом верхнем углу
	lcd_Update();                           //Вывод информации в дисплей
}
//
/*----Функция вывода символа батарейки в правом верхнем углу----*/
void wr_batt() {
	if (zar == 1) {
		batter = 6;                  //Если идет процесс зарядки - символ сетевой вилки
	}
	lcd_OutCharXY(CHAR_BATTERY_0 + batter, 13, 0);                    //Вывод символа в буфер ОЗУ МК
}

uint8_t calcBatteryPercent(uint16_t u) {
	if (u > 415) {
		return 100;
	} else if (u > 380) {
		return (u-380)*25/35 + 75;	// Uбат > 3,8В -> 75...100%
	} else if (u > 360) {
		return (u-360)*50/20+25;	// Uбат > 3,6В - 25...75%
	} else if (u > 300) {
		return (u-300)*25/60;		// Uбат > 3,0В - 0...25%
	}
	return 0;
}
//
/*----Функция вывода меню зарядки----*/
void menu_charge() {
	uint8_t percent = calcBatteryPercent(Ubat);
	DrawBattery(34, 2, 48, 28, percent);       //Вывод большого символа батареи с заполнением на %
	// Вывод напряжения батареи
	lcd_OutIntBuf(0, Ubat, 1, 2);
	LcdStringBold(1, 4);
	LcdStrBold(" В");
}
