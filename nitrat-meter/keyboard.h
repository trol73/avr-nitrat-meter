/*----����� ������----*/
void opros(void)
{ if (knopka() > 2)                        //���� ������ ����� ������, ����� "���./����."
	{ _delay_ms(30);                           //����� ������������
	}
	else return;
	if (knopka()>2)                        //���� ������ ����� ������, ����� "���./����."
	{ schpow=0;                            //����� �������� 0,262144 ��������� ����������
		if (light==0)                        //���� ��������� ���� ��������� �� �������
		{ light=1;                           //��������� �������� ���������� ���������
			highlight();                        //���������� ����������
		}
		if (rezh==0)                         //���� ��� ������� ����������
		{ rezh=1;                            //��������� �������� ������
			beep(1); beep(4);                //������ 2-� �������� �������� �������� ��� ��������� (3, 3.5 ���)
			T_menu=1;                          //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
			while (knopka()!=0)                //���� ������ �������� ������
			{ batt();                          //��������� ���������� ������������ � ���������� ��������
				menu();                          //����� ���� ������ 0.26 ���
			}
		}
	}
	else return;
	unsigned char dat;                     //��������������� ����������
	unsigned char t=100;                   //������ ����������� ������� ������ � ��/2
	unsigned char bip=255;                 //���������� ���� ��������� ������������� (255-��� �����)
	register unsigned char nkn=knopka();   //����������� ���������� ��� ���� ������� ������
	//������
	if (nkn==6)                            //���� ������ ������ ������
	{ b=0;                                 //��������� �������� ������� ������� ������
		bip=255;                             //�� ��������� - ��� �����
		if (mnc_nach==0)                     //���� � ��������� ���� ������ ����� "���������"
		{ if (menu_cnt<2)                    //���� ��� �� ���� "�����"
			{ menu_cnt++;                      //������� � ���������� ������
				bip=4;                           //1 �������� ���� 0,1 ���, 3,5 ���
			}
			else                               //�����, ���� ���� "�����" (menu_cnt=2)
			{ if (mnc_izm==0)                  //���� ����� "�����"
				{ TCCR0=0b01101001;              //���� �.��.0 ��������� ������� (FastPWM, OC2=0 ��� ������.,OC2=1 ��� TOP=255,F=2���/1/256=7812,5��
					beep(1);                      //1 �������� ���� 0,1 ���, 3 ���
					LcdAnim();                     //����� �������� � ���� ������������� ����� �������� ������ - ����� ��� ��������� ���������� ���
					meassure();                       //���������� ������
					T_menu=1;                      //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
					zam=3;                         //������� ������������ (�� ������������) ������
					menu();                        //����� ����, ��� �� ���������� ���������� Gx, Rx � korr_zam
					if (korr_zam==0)               //���� ����� ���������
					{ beep(6);                    //2 �������� ����� 0,1 ���, 2,5 ���
					}
					else                           //�����, ���� ����� ��������
					{ beep(7);                    //2 �������� ����� 0,1 ���, 3,5 ���
					}
					while (knopka()==6)            //���� ������ ������ ������ - ����������� ����� � ����� ����
					{ batt();                      //��������� ���������� ������������ ������ 0.26 ���
						if (T_menu==1) meassure();      //���� ������ 0.26 ��� - ���������� ������ ��������
						menu();                      //����� ���� ������ 0.26 ���
						if (knopka()!=6)             //���� ������ ������ ��������
						{ if (korr_zam==0)           //���� ����� ���������
							{ beep(6);                //2 �������� ����� 0,1 ���, 2,5 ���
							}
							else                       //�����, ���� ����� ��������
							{ beep(7);                //2 �������� ����� 0,1 ���, 3,5 ���
							}
							break;                     //��������� ����� �� �����
						}
					}
					//�������� ���������� ����� ������
					while (knopka()==7)            //���� ������ �����-���� ������
					{ batt();                      //��������� ���������� ������������ ������ 0.26 ���
						menu();                      //����� ���� ������ 0.26 ���
					}
					T_menu=1;                      //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
					zam=1;                         //������� ������������, �� �� ������������ ������
					TCCR0=0b01101000;              //���� �.��.0 ��������� �������
				}
				else
				{ if (mnc_izm==1)                //���� ����� "���������"
					{ if ((zam==1)&&(korr_zam==1)) //���� �� ����������� � ���������� �����
						{ saveMeasure();             //���������� ����������� ������ �������� � EEPROM
							zam=2;                     //������� ������������ ������
							mnc_izm=0;                 //����������� ������� �� ����� �����
							menumess(5);               //����� ��������� "����� ��������!"
							beep(1);                  //1 �������� ���� 0,1 ���, 3 ���
							beep(4);                  //1 �������� ���� 0,1 ���, 3,5 ���
							_delay_ms(600);                //����� ��� ����������� ��������� ���������
						}
					}
					else
					{ if (mnc_izm==3)              //���� ����� "�������� ����������"
						{ if (akt==0)
							{ akt=1;                   //���� ����� �� ����������� - �������������
							}
							else akt=0;                //����� - ���������������
							bip=4;                     //1 �������� ���� 0,1 ���, 3,5 ���
						}
						else
						{ if (mnc_izm==4)            //���� ����� "�������������� ��������"
							{ if (akt==0)              //���� ����� �� �����������
								{ akt=1;                 //�������������
									n_nazv=0;              //����� �������� �������� �������� (������ �����)
									T_menu=1;              //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
									menu();
									beep(4);              //1 �������� ���� 0,1 ���, 3,5 ���
									ozhid(6);              //����.������.��.������ (����� ���������� ������������ � ����� ���� ������ 0,26�)
								}
								else                     //�����, ���� ����� �����������
								{ if (n_nazv<8)
									{ while (knopka()==6)  //���� ������ ������ ������
										{ if (n_nazv<7)
											{ n_nazv++;        //��������� �������� ���� ����������������� �������� ��������
												bip=4;           //1 �������� ���� 0,1 ���, 3,5 ���
											}
											else bip=5;        //1 �������� ���� 0,1 ���, 2,5 ���
											T_menu=1;          //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
											batt();            //��������� ���������� ������������ ������ 0.26 ���
											menu();            //����� ����
											if (b==0) beep(bip);//���� ������ ������� ������ � ����� ����������� - ����
											bip=255;           //������� ���������� �����
											if (autopovt(6, 150)==1) break;//���� ��� ����������� ������ �������� - �����
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{ if (mnc_nach==1)                   //�����, ���� ������ ����� "���������"
			{ if (menu_cnt==0)                 //���� ��������� ����
				{ menu_cnt=3;                    //������� � ���� "���������"
					bip=4;                         //1 �������� ���� 0,1 ���, 3,5 ���
				}
				else
				{ if (menu_cnt==3)               //�����, ���� ���� "���������"
					{ if (akt==0)                  //���� ����� ���� "���������" �� �������
						{ akt=1;                     //��������� �������� ������ ���� "���������"
							bip=4;                     //1 �������� ���� 0,1 ���, 3,5 ���
						}
					}
				}
			}
			else
			{ if (mnc_nach==2)                 //�����, ���� ������ ����� "����� ���"
				{ if (menu_cnt==0)               //���� ��������� ����
					{ menu_cnt=4;                  //������� � ���� "����� ���"
						bip=4;                       //1 �������� ���� 0,1 ���, 3,5 ���
					}
				}
			}
		}
		beep(bip);                          //����
		T_menu=1;                            //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
		menu();                              //����� ����
		ozhid(6);                            //����.������.��.������ (����� ���������� ������������ � ����� ���� ������ 0,26�)
	}
	//
	//�����
	if (nkn==5)                            //���� ������ ������ �����
	{ b=0;                                 //��������� �������� ������� ������� ������
		bip=255;                             //�� ��������� - ��� �����
		if (mnc_nach==0)                     //���� � ��������� ���� ������ ����� "���������"
		{ if (menu_cnt>0)                    //���� ��� �� ��������� ����
			{ if (mnc_izm<3)
				{ menu_cnt--;                    //���� ������ �����, ��������� ��� ���������� - ������� � ���� ������ ��������
					bip=4;                         //1 �������� ���� 0,1 ���, 3,5 ���
				}
				else
				{ if (mnc_izm==3)                //���� ����� "�������� ����������"
					{ if (akt==1)                  //���� ����� �����������
						{ akt=0;                     //���������������
						}
						else menu_cnt--;             //����� - ������� � ���� ������ ��������
						bip=4;                       //1 �������� ���� 0,1 ���, 3,5 ���
					}
					else
					{ if (mnc_izm==4)              //���� ����� "�������������� ��������"
						{ if (akt==1)                //���� ����� �����������
							{ if (n_nazv>0)            //���� �� ��������� ������������� �����
								{ if (n_nazv>0)          //���� �������������� �������
									{ while (knopka()==5)  //���� ������ ������ �����
										{ if (n_nazv>0)
											{ n_nazv--;        //��������� �������� ���� ����������������� �������� ��������
											}
											T_menu=1;          //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
											batt();            //��������� ���������� ������������ ������ 0.26 ���
											menu();            //����� ����
											if (b==0) beep(4);//���� ������ ������� ������ � ����� ����������� - 1 �������� ���� 0,1 ���, 3,5 ���
											if (autopovt(5, 150)==1) break;//���� ��� ����������� ������ �������� - �����
										}
									}
								}
								else
								{ akt=0;                 //�����, ���� ��������� ����� - ���������������
									bip=4;                 //1 �������� ���� 0,1 ���, 3,5 ���
								}
							}
							else                       //�����, ���� ����� �������������
							{ menu_cnt--;
								bip=4;                   //1 �������� ���� 0,1 ���, 3,5 ���
							}
						}
					}
				}
			}
		}
		else
		{ if (mnc_nach==1)                   //�����, ���� ������ ����� "���������"
			{ if (menu_cnt==3)                 //���� ���� "���������"
				{ if (akt==1)                    //���� ����� ���� "���������" �������
					{ akt=0;                       //����������� �������� ������ ���� "���������"
						bip=4;                       //1 �������� ���� 0,1 ���, 3,5 ���
					}
					else
					{ if (mnc_nastr<5)             //�����, ���� �� ���� "����������"
						{ menu_cnt=0;                //������� � ���������� ����
							bip=4;                     //1 �������� ���� 0,1 ���, 3,5 ���
						}
					}
				}
			}
			else
			{ if (mnc_nach==2)                 //�����, ���� ������ ����� "����� ���"
				{ if (menu_cnt==4)               //���� ���� "����� ���"
					{ menu_cnt=0;                  //������� � ���������� ����
						bip=4;                       //1 �������� ���� 0,1 ���, 3,5 ���
					}
				}
			}
		}
		beep(bip);                          //����
		T_menu=1;                            //1 - ������� ���������� ��������� 0,26 ��� ��� ������ ����
		menu();                              //����� ����
		ozhid(5);                            //����.������.��. ����� (����� ���������� ������������ � ����� ���� ������ 0,26�)
	}
	//
	//����
	if (nkn==4)                            //���� ������ ������ ����
	{ b=0;                                 //��������� �������� ������� ������� ������
		bip=255;                             //�� ��������� - ��� �����
		//���� ���� ������ ������ ����
		while (knopka()==4)
		{ if (menu_cnt==0)                   //���� ��������� ����
			{ if (mnc_nach<2)                  //���� ������� ���������� ���� <2 (0-���������, 1-���������, 2-����� ���)
				{ mnc_nach++;
					bip=4;                         //1 �������� ���� 0,1 ���, 3,5 ���
				}
				else                             //�����, ���� ������� ���������� ���� =2
				{ ozhid(4);                      //�������� ���������� ������ ���� � ������� ����
				}
			}
			else
			{ if (menu_cnt==1)                 //���� ���� ������ ��������
				{ if (n_prod<35)                 //���� ������� �������� <35
					{ n_prod++;
						if ((n_prod<30)&&(mnc_izm==4)) mnc_izm=3;//���� �������� �������� �� flash � ����� ���.����.->������� �������.
						if ((n_prod-sdvig_prod)>4)
						{ sdvig_prod++;
						}
						zam=0;                       //������� ��� �� ������������ ������ ���������� ��������
						if (mnc_izm==1) mnc_izm=0;   //���� ��� ����� "���������" - ��������� �� ����� "�����"
						bip=4;                       //1 �������� ���� 0,1 ���, 3.5 ���
					}
				}
				else
				{ if (menu_cnt==2)               //���� ���� ������ ��������
					{ if (mnc_izm<3)               //���� ��� �� ����� "�������� ����������"
						{ mnc_izm++;
							if (((zam!=1)||(korr_zam==0))&&(mnc_izm==1))//���� ������ ���������
							{ mnc_izm++;               //"��������������" ����� ����� ���������
							}
							bip=4;                     //1 �������� ���� 0,1 ���, 3.5 ���
						}
						else                         //�����, ���� ����� "�������� ����������" ��� "�������������� ��������"
						{ if (akt==0)                //���� ��������� ����� �� �������
							{ if ((mnc_izm==3)&&(n_prod>=30))//���� ����� "�������� ����������" � ������ ������� � ��������� �� EEPROM
								{ mnc_izm++;             //������� � ������ "�������������� ��������"
									bip=4;                 //1 �������� ���� 0,1 ���, 3.5 ���
								}
							}
							else                       //�����, ���� ��������� ����� �������
							{ if (mnc_izm==4)          //���� ����� "�������������� ��������"
								{ dat=mas_name[n_prod-30][n_nazv];//������ �������������� ������� �� ������� � ��� ���������������� �������� ��������� �� 8 ��������
									if (dat==255) dat=32;  //������� �� ����� "�" � " " (32...57 - ������� � �����)
									else if (dat==57) dat=192;//������� �� ����� "0" � ����� "�" (192-255 - ������� �����)
									else dat++;            //������� � ���������� �� ������� �������
									mas_name[n_prod-30][n_nazv]=dat;//������ ������� � ������ ���
									bip=1;                 //1 �������� ���� 0,1 ���, 3 ���
								}
							}
						}
					}
					else
					{ if (menu_cnt==3)             //���� ���� "���������"  mnc_nastr: 0-����� �����.,1-�������,2-������,3-����,4-�������������,5-U���,6-Rx
						{ if (akt==0)                //���� ��������� ����� �� �������
							{ if ((mnc_nastr!=4)&&(mnc_nastr<6))//���� �� ����� ������������� � ��� �� ����� Rx
								{ mnc_nastr++;
									bip=4;                 //1 �������� ���� 0,1 ���, 3.5 ���
								}
								else                     //�����
								{ if (mnc_nastr==4)      //���� ����� ������������� - ������� � ��������� � ���� ����������
									{ b=0;
										while (b<25) {        //���� � ������� 1,5 ��� ���� �������� ������ ����
											_delay_ms(60);
											batt();            //��������� ���������� ������������
											menu();            //����� ����
											if (knopka()!=4) return;
											b++;
										}
										mnc_nastr++;         //������� � ������ Rx
										TCCR0=0b01101001;    //���� �.��.��������� ������� (FastPWM, OC2=0 ��� ������.,OC2=1 ��� TOP=255,F=2���/1/256=7812,5��
										b=1;                 //������� ���������� �����
										beep(4);            //1 �������� ���� 0,1 ���, 3.5 ���
										ozhid(4);            //�������� ���������� ������ ���� � ������� ����
									}
								}
							}//������� ���� ��������: 0-����� �����.,1-�������,2-������,3-����,4-�������������,5-U���,6-Rx
							else                       //�����, ���� ��������� ����� ���� ��������� ��� ���������� �������
							{ if (mnc_nastr==0)        //���� ����� ��������� ������� ���������
								{ if (ligh_off==0) ligh_off=6;//����� ����.�����.�� ������� (0-����.;1-10�;2-15�;3-20�;4-30�;5-45�;6-60�)
									else ligh_off--;
									bip=1;                 //1 �������� ���� 0,1 ���, 3 ���
								}
								else
								{ if (mnc_nastr==1)      //���� ����� ��������� ������� ���������
									{ if (yark>0)
										{ yark--;            //������� ��������� (0...10 -> I=0...10��)
											highlight();        //���������� �������� ���������
											bip=1;             //1 �������� ���� 0,1 ���, 3 ���
										}
										else  bip=5;         //1 �������� ���� 0,1 ���, 2.5 ���
									}
									else
									{ if  (mnc_nastr==2)   //���� ����� ��������� ������� ����������
										{ if (taym_off==0) taym_off=4;//������ ���������� (0-����.;1-1���;2-2���;3-3���;4-5���)
											else taym_off--;
											bip=1;             //1 �������� ���� 0,1 ���, 3 ���
										}
										else
										{ if (mnc_nastr==3)  //���� ����� ���./����. �����
											{ if (sound==1) sound=0;
												else sound=1;    //���������� ������ (0-����., 1-���.)
												bip=1;           //1 �������� ���� 0,1 ���, 3 ���
											}
											else
											{ if (mnc_nastr==4)//���� ����� ����������� �������������
												{ if (contrast>0)//������������� �������: 0...100
													{ contrast--;
														LcdContrast(contrast+SDV_CONTR);//��������� ������������� �������
														bip=1;       //1 �������� ���� 0,1 ���, 3 ���
													}
													else bip=5;    //1 �������� ���� 0,1 ���, 2.5 ���
												}
												else
												{ if (mnc_nastr==5)//���� ����� ���������� U���
													{ if (vref>2200)//������� ���������� ����������� ��� ��� �� � �� (�� ���.29 (AREF) ��) ��� �����.�������
														{ vref--;
															t=0;
															T_bat=1;   //��������� �������� ���������� ��������� 0.26 ��� ��� ������ ���������� ������������
															bip=1;     //1 �������� ���� 0,1 ���, 3 ���
														}
														else bip=5;  //1 �������� ���� 0,1 ���, 2,5 ���
													}
													else
													{ if (mnc_nastr==6)//���� ����� ���������� Rx
														{ if (Uref<2800)//������� ���������� ����������� ��� ��� �� � �� (�� ���.29 (AREF) ��) ��� �����.������������
															{ Uref++;
																t=0;
																meassure(); //���������� ������ �������������
																bip=1;   //1 �������� ���� 0,1 ���, 3 ���
															}
															else bip=5;//1 �������� ���� 0,1 ���, 2.5 ���
														}
													}
												}
											}
										}
									}
								}
							}
						}
						else
						{ if (menu_cnt==4)           //���� ���� "����� ���"
							{ if (mnc_PDK<30)          //���� ������� ���� ��� �������� <30
								{ mnc_PDK++;
									if ((mnc_PDK-sdvig_PDK)>4)
									{ sdvig_PDK++;
									}
									bip=4;                 //1 �������� ���� 0,1 ���, 3.5 ���
								}
							}
						}
					}
				}
			}
			batt();                            //��������� ���������� ������������
			T_menu=1;                          //��������� �������� ���������� ��������� 0,26 ��� ��� ������ ����
			menu();                            //����� ���� ��� ��������� ���������
			if (b==0) beep(bip);              //���� ������ ������� ������ � ����� ����������� - ����
			if (autopovt(4, t)==1) break;      //���� ��� ����������� ������ ���� �������� - �����
		}
	}
	//
	//�����
	if (nkn==3)                            //���� ������ ������ �����
	{ b=0;                                 //��������� �������� ������� ������� ������
		bip=255;                             //�� ��������� - ��� �����
		//���� ���� ������ ������ �����
		while (knopka()==3)
		{ if (menu_cnt==0)                   //���� ��������� ����
			{ if (mnc_nach>0)                  //���� ������� ���������� ���� >0 (0-���������, 1-���������, 2-����� ���)
				{ mnc_nach--;
					bip=4;                         //1 �������� ���� 0,1 ���, 3,5 ���
				}
				else                             //�����, ���� ������� ���������� ���� =2
				{ ozhid(3);                      //�������� ���������� ������ ���� � ������� ����
				}
			}
			else
			{ if (menu_cnt==1)                 //���� ���� ������ ��������
				{ if (n_prod>0)                  //���� ������� �������� >0
					{ n_prod--;
						if ((n_prod<30)&&(mnc_izm==4)) mnc_izm=3;//���� �������� �������� �� flash � ����� ���.����.->������� �������.
						if (n_prod<sdvig_prod)
						{ sdvig_prod=n_prod;
						}
						zam=0;                       //������� ��� �� ������������ ������ ���������� ��������
						if (mnc_izm==1) mnc_izm=0;   //���� ��� ����� "���������" - ��������� �� ����� "�����"
						bip=4;                       //1 �������� ���� 0,1 ���, 3,5 ���
					}
				}
				else
				{ if (menu_cnt==2)               //���� ���� ������ ��������
					{ if (mnc_izm<3)               //���� �� ������ "�������� ����������" � "�������������� ��������"
						{ if (mnc_izm>0)
							{ mnc_izm--;
								bip=4;                   //1 �������� ���� 0,1 ���, 3,5 ���
							}
							if (((zam!=1)||(korr_zam==0))&&(mnc_izm==1))//���� ������ ���������
							{ mnc_izm--;               //"��������������" ����� ����� ���������
								bip=4;                   //1 �������� ���� 0,1 ���, 3,5 ���
							}
						}
						else                         //�����, ���� ������ "�������� ����������" ��� "�������������� ��������"
						{ if (akt==0)                //���� ��������� ����� �� �������
							{ mnc_izm--;               //������� � ������ ����
								bip=4;                   //1 �������� ���� 0,1 ���, 3,5 ���
							}
							else                       //�����, ���� ��������� ����� �������
							{ if (mnc_izm==3)          //���� ����� "�������� ����������"
								{ eeprom_write_word(mas_Nzam+n_prod, 0);//������ 0 ������� � EEPROM
									eeprom_write_word(&mas_lim[1][n_prod], 0);//������ ������������ �������� ������ ���
									eeprom_write_word(&mas_lim[0][n_prod], 0);//������ ������������� �������� ������ ���
									dat=0;
									while (dat<20)
									{ eeprom_write_byte(&mas_stat[n_prod][dat], 0);//���������� ������� ���������� ������
										dat++;
									}
									menumess(4);           //����� ��������� "���������� �������"
									beep(1);              //1 �������� ���� 0,1 ���, 3 ���
									beep(4);              //1 �������� ���� 0,1 ���, 3,5 ���
									_delay_ms(1500);           //����� 1,5 ��� ��� ����������� ��������� ���������
									akt=0;                 //������ ���������� � ������
									T_menu=1;              //��������� �������� ���������� ��������� 0,26 ��� ��� ������ ����
									if (zam==2) zam=1;     //���� ��� ����������� ����� - ������� �� ������������ ������
									ozhid(3);              //�������� ���������� ������ ����� � ������� ����
								}
								else
								{ if (mnc_izm==4)        //���� ����� "�������������� ��������"
									{ dat=mas_name[n_prod-30][n_nazv];//������ �������������� ������� �� ������� � ��� ���������������� �������� ��������� �� 8 ��������
										if (dat==32) dat=255;//������� �� " " � ����� "�" (32...57 - ������� � �����)
										else if (dat==192) dat=57;//������� �� ����� "�" � ����� "0" (192-255 - ������� �����)
										else dat--;          //������� � ���������� �� ������� �������
										mas_name[n_prod-30][n_nazv]=dat;//������ ������� � ������ ���
										bip=1;               //1 �������� ���� 0,1 ���, 3 ���
									}
								}
							}
						}
					}
					else
					{ if (menu_cnt==3)             //���� ���� "���������"  mnc_nastr: 0-����� �����.,1-�������,2-������,3-����,4-�������������,5-U���,6-Rx
						{ if (akt==0)                //���� ��������� ����� �� �������
							{ if ((mnc_nastr!=5)&&(mnc_nastr>0))//���� �� ����� U��� � ��� �� ����� ����� �����.
								{ mnc_nastr--;
									if (mnc_nastr==5)      //���� ����� U���
									{ beep(4);            //1 �������� ���� 0,1 ���, 3,5 ���
										ozhid(3);            //�������� ���������� ������ ����� � ������� ����
									}
									else bip=4;            //1 �������� ���� 0,1 ���, 3,5 ���
								}
								else                     //�����
								{ if (mnc_nastr==5)      //���� ����� U���
									{ mnc_nastr--;         //������� � ������ ����
										beep(4);            //1 �������� ���� 0,1 ���, 3,5 ���
										TCCR0=0b01101000;    //���� �.��.0 ��������� �������
										ozhid(3);            //�������� ���������� ������ ����� � ������� ����
									}
								}
							}//������� ���� ��������: 0-����� �����.,1-�������,2-������,3-����,4-�������������,5-U���,6-Rx
							else                       //�����, ���� ��������� ����� ���� ��������� ��� ���������� �������
							{ if (mnc_nastr==0)        //���� ����� ��������� ������� ���������
								{ if (ligh_off==6) ligh_off=0;//����� ����.�����.�� ������� (0-����.;1-10�;2-15�;3-20�;4-30�;5-45�;6-60�)
									else ligh_off++;
									bip=1;                 //1 �������� ���� 0,1 ���, 3 ���
								}
								else
								{ if (mnc_nastr==1)      //���� ����� ��������� ������� ���������
									{ if (yark<10)
										{ yark++;            //������� ��������� (0...10 -> I=0...10��)
											highlight();        //���������� �������� ���������
											bip=1;             //1 �������� ���� 0,1 ���, 3 ���
										}
										else bip=5;          //1 �������� ���� 0,1 ���, 2,5 ���
									}
									else
									{ if  (mnc_nastr==2)   //���� ����� ��������� ������� ����������
										{ if (taym_off==4) taym_off=0;//������ ���������� (0-����.;1-1���;2-2���;3-3���;4-5���)
											else taym_off++;
											bip=1;             //1 �������� ���� 0,1 ���, 3 ���
										}
										else
										{ if (mnc_nastr==3)  //���� ����� ���./����. �����
											{ if (sound==1) sound=0;
												else sound=1;    //���������� ������ (0-����., 1-���.)
												bip=1;           //1 �������� ���� 0,1 ���, 3 ���
											}
											else
											{ if (mnc_nastr==4)//���� ����� ����������� �������������
												{ if (contrast<100)//������������� �������: 0...100
													{ contrast++;
														LcdContrast(contrast+SDV_CONTR);//��������� ������������� �������
														bip=1;       //1 �������� ���� 0,1 ���, 3 ���
													}
													else bip=5;    //1 �������� ���� 0,1 ���, 2,5 ���
												}
												else
												{ if (mnc_nastr==5)//���� ����� ���������� U���
													{ if (vref<2800)//������� ���������� ����������� ��� ��� �� � �� (�� ���.29 (AREF) ��) ��� �����.�������
														{ vref++;
															t=0;
															T_bat=1;   //��������� �������� ���������� ��������� 0.26 ��� ��� ������ ���������� ������������
															bip=1;     //1 �������� ���� 0,1 ���, 3 ���
														}
														else  bip=5; //1 �������� ���� 0,1 ���, 2.5 ���
													}
													else
													{ if (mnc_nastr==6)//���� ����� ���������� Rx
														{ if (Uref>2200)//������� ���������� ����������� ��� ��� �� � �� (�� ���.29 (AREF) ��) ��� �����.������������
															{ Uref--;
																t=0;
																meassure(); //���������� ������ �������������
																bip=1;   //1 �������� ���� 0,1 ���, 3 ���
															}
															else bip=5;//1 �������� ���� 0,1 ���, 2,5 ���
														}
													}
												}
											}
										}
									}
								}
							}
						}
						else
						{ if (menu_cnt==4)           //���� ���� "����� ���"
							{ if (mnc_PDK>0)           //���� ������� ���� ��� �������� >0
								{ mnc_PDK--;
									if (mnc_PDK<sdvig_PDK)
									{ sdvig_PDK=mnc_PDK;
									}
									bip=4;                 //1 �������� ���� 0,1 ���, 3.5 ���
								}
							}
						}
					}
				}
			}
			batt();                            //��������� ���������� ������������
			T_menu=1;                          //��������� �������� ���������� ��������� 0,26 ��� ��� ������ ����
			menu();                            //����� ���� ��� ��������� ���������
			if (b==0) beep(bip);              //���� ������ ������� ������ � ����� ����������� - ����
			if (autopovt(3, t)==1) break;      //���� ��� ����������� ������ ����� �������� - �����
		}
	}
	//
	schpow=0;                              //����� �������� 0,262144 ��������� ����������
	return;
}



//
/*----����������� ������� ������----*/
unsigned char knopka (void)
{
	//0 - ������ ��� ������
	//1 - ������ ������ ������ ���./����.
	//2 - ������ ������ ���./����. ������������ � ������� ��������
	//3 - ������ ������ �����
	//4 - ������ ������ ����
	//5 - ������ ������ �����
	//6 - ������ ������ ������
	//7 - ������ ������������ ��������� ������ (�����, ����, �����, ������)
	unsigned char kn=0;                    //���������� �������� ���.��.
	register unsigned char p=PIND&0b00001111;
	if (bit_is_clear(PINB,ONOFF))          //���� ������ ������ "���./����."
	{ if (p==0b00001111) kn=1;             //���� ��������� ������ ������ - kn=1
		else kn=2;                           //���� ������ �����-���� ��� ������ - kn=2
	}
	else                                   //�����, ���� ������ "���./����." ������
	{ if (p==0b00001110) kn=3;             //���� ������ ������ �����
		else if (p==0b00001011) kn=4;        //���� ������ ������ ����
		else if (p==0b00000111) kn=5;        //���� ������ ������ �����
		else if (p==0b00001101) kn=6;        //���� ������ ������ ������
		else if (p!=0b00001111) kn=7;        //���� ������ ������������ ��������� ������ (�����, ����, �����, ������)
		else if (p==0b00001111) kn=0;        //���� ��� ������ ������ - kn=0
	}
	return(kn);
}
//
/*----�������� ���������� ������ (����� ����)----*/
void ozhid (unsigned char kn)
{ while (knopka()==kn)                   //���� ������ �������� ������
	{ batt();                              //��������� ���������� ������������ � ���������� ��������
		menu();                              //����� ���� ������ 0.26 ���
	}
	return;
}
//
/*----������� ����������� ������� ������----*/
unsigned char autopovt(unsigned char k, unsigned char t)
{ if (knopka()==k)                       //���� ������ ��� ��� ������
	{ b++;
		if (b==1)                            //���� ������ �������
		{ while (b<15) {
			_delay_ms(60);
			if (knopka()!=k) return(1);      //���� ������ �������� � �������
			b++;                             //0,9 ��� - ����� � "1"
		}
	}
	else                                 //����� - ����� ���������
	{ b=1;
		tayms(t*2);                        //������ �����������
	}
}                                      //����� ����� ������
else return(1);                        //����� ���� ������ �� ������ - ����� � "1"
return(0);                             //����� � "0" ��� ������� ������
}