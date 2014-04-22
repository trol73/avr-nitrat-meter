#define CHAR_ARROW_DOWN_EMPTY			161							// ������ ������� ������������ ���� V
#define CHAR_ARROW_UP_EMPTY				163							// ������ ������� ������������ ����� ^
#define CHAR_ARROWS_UP_DOWN				164							// ������ ���� ����������� ������������� �����+����
#define CHAR_ARROW_RIGHT_EMPTY			165							// ������ ������� ������������ ������ |>
#define CHAR_ARROW_RIGHT_FILLED			166                         // ������ ������������ ������������ ������ |>
#define CHAR_ARROW_LEFT_EMPTY			167                         // ������ ������� ������������ ����� <|


// ������ �������� ���� ��� �� 12 ��������
const unsigned char PROGMEM nadp_mPDK[31][12]= {
	"�������   60",
	"�����     60",
	"�����    200",
	"�������� 300",
	"��������  60",
	"�����     60",
	"������  2000",
	"����      90",
	"�������� 900",
	"�������� 500",
	"�������  400",
	"���������250",
	"�������� 100",
	"��� ����. 80",
	"��� ���. 600",
	"�����     60",
	"�������� 400",
	"�������� 250",
	"���� ��. 200",
	"��������  60",
	"������   300",
	"����� ��.200",
	"������    60",
	"�������  250",
	"�����   1500",
	"������  1000",
	"�����   1200",
	"������  1400",
	"�����     60",
	"������    60",
	"������   200"
};

const uint8_t PROGMEM STR_MAIN_MENU_MEASSURE[] = "���������  \xA5";
const uint8_t PROGMEM STR_MAIN_MENU_SETTINGS[] = "���������  \xA5";
const uint8_t PROGMEM STR_MAIN_MENU_LIMITS[] = "����� ���  \xA5";
const uint8_t PROGMEM STR_PRODUCT_TITLE[] = " �������:";

const uint8_t PROGMEM STR_UAKK_V[] = "U���,� ";
const uint8_t PROGMEM STR___[] = "  ";
//const uint8_t PROGMEM STR__V[] = " �";
const uint8_t PROGMEM STR_SAVE[] = "���������";
const uint8_t PROGMEM STR_SAVED[] = "���������";

const uint8_t PROGMEM STR_MAX[] = "����.";
const uint8_t PROGMEM STR_MIN[] = "���.";
const uint8_t PROGMEM STR_MEASSURES_CNT[] = "�������";
const uint8_t PROGMEM STR_MEASSURES[] = "�����";
const uint8_t PROGMEM STR_MICRO_SM_BRACKET[] = "(����)";
const uint8_t PROGMEM STR_MICRO_SM[] = "����";


// ������ �������� �� 10 ��������
const unsigned char PROGMEM nadp_10[15][10]= {
	"��������� ",                          //0	!!
	"��������� ",                          //1	!!
	"����� ��� ",                          //2	!!
	" �������: ",                          //3	!!
	" ���������",                          //4
	"����������",                          //5
	"          ",                          //6	!!
	"��������� ",                          //7	!!
	"��������� ",                          //8	!!
	"��������  ",                          //9
	"����������",                          //10
	"���.������",                          //11
	"��������? ",                          //12
	"��     ���",                          //13
	"���.������",                          //14
};

/*������ �������� ��������� �� 11 ��������*/
const unsigned char PROGMEM nadpis_mes[9][11]= { 
	"           ",                         //0
	"�����������",                         //1
	" ��������! ",                         //2
	" �������!  ",                         //3
	"����������!",                         //4
	"���������� ",                         //5
	" �������!  ",                         //6
	"  �����    ",                         //7
	" ��������! ",                         //8
};
// ������ �������� �� 8 ��������
const unsigned char PROGMEM nadp_8[8][8]= { 
	"T �����.",                            //0
	"������� ",                            //1
	"������  ",                            //2
	"����    ",                            //3
	"��������",                            //4
	"������� ",                            //5	!!!
	"Gx,���� ",                            //6
	"����� ��",                            //7
};

// ������ �������� �� 6 ��������s
const unsigned char PROGMEM nadp_6[8][6]= {
	"����� ",                              //0	!!!
	"����. ",                              //1	!!!
	"���.  ",                              //2
	"U���. ",                              //3
	"U���,�",                              //4	!!!
	"Rx,�� ",                              //5
	"(����)",                              //6	!!!
	" ���� ",                              //7	!!!
};

// ������ �������� �� 4 ��������
const unsigned char PROGMEM nadp_4[15][4]= { 
	"����",                                //0
	"1���",                                //1
	"2���",                                //2
	"3���",                                //3
	"5���",                                //4
	" ---",                                //5
	" 10�",                                //6
	" 15�",                                //7
	" 20�",                                //8
	" 30�",                                //9
	" 45�",                                //10
	" 60�",                                //11
	"�,��",                                //12
	"����",                                //13
	"���.",                                //14
};

