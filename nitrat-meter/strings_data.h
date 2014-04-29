#define CHAR_ARROW_DOWN_EMPTY			161							// Символ пустого треугольника вниз V
#define CHAR_ARROW_UP_EMPTY				163							// Символ пустого треугольника вверх ^
#define CHAR_ARROWS_UP_DOWN				164							// Символ двух закрашенных треугольников вверх+вниз
#define CHAR_ARROW_RIGHT_EMPTY			165							// Символ пустого треугольника вправо |>
#define CHAR_ARROW_RIGHT_FILLED			166                         // Символ закрашенного треугольника вправо |>
#define CHAR_ARROW_LEFT_EMPTY			167                         // Символ пустого треугольника влево <|
#define CHAR_BATTERY_0					169

// Массив надписей меню ПДК из 12 символов
const char PROGMEM nadp_mPDK[31][12]= {
	"Абрикос   60",
	"Арбуз     60",
	"Банан    200",
	"Баклажан 300",
	"Виноград  60",
	"Груша     60",
	"Зелень  2000",
	"Дыня      90",
	"КапустаР 900",
	"КапустаП 500",
	"Кабачок  400",
	"Картофель250",
	"Клубника 100",
	"Лук репч. 80",
	"Лук зел. 600",
	"Манго     60",
	"МорковьР 400",
	"МорковьП 250",
	"Мясо св. 200",
	"Нектарин  60",
	"Огурец   300",
	"Перец сл.200",
	"Персик    60",
	"Помидор  250",
	"Редис   1500",
	"Редька  1000",
	"Салат   1200",
	"Свекла  1400",
	"Хурма     60",
	"Яблоко    60",
	"Прочее   200"
};

const char PROGMEM STR_MAIN_MENU_MEASSURE[] = "Измерение  \xA5";
const char PROGMEM STR_MAIN_MENU_SETTINGS[] = "Настройки  \xA5";
const char PROGMEM STR_MAIN_MENU_LIMITS[] = "Нормы ПДК  \xA5";
const char PROGMEM STR_PRODUCT_TITLE[] = " ПРОДУКТ:";
const char PROGMEM STR_SETTINGS_TITLE[] = " НАСТРОЙКИ";
const char PROGMEM STR_CALIBRATE[] = "КАЛИБРОВКА";

const char PROGMEM STR_UAKK_V[] = "Uакк,В ";
const char PROGMEM STR___[] = "  ";
const char PROGMEM STR_____[] = "    ";
const char PROGMEM STR_________[] = "        ";
//const uint8_t PROGMEM STR__V[] = " В";
const char PROGMEM STR_SAVE[] = "Сохранить";
const char PROGMEM STR_SAVED[] = "Сохранено";
const char PROGMEM STR_CLEAR[] = "Очистить";
const char PROGMEM STR_STATISTIC[] = "статистику";
const char PROGMEM STR_EDIT_NAME[] = "Ред.назван.";
const char PROGMEM STR_CLEAR_QUESTION[] = "Очистить?";
const char PROGMEM STR_YES_NO[] = " Да     Нет ";
const char PROGMEM STR_EDIT_NAME_UP[] = "РЕД.НАЗВАН";


const char PROGMEM STR_MAX[] = "Макс.";
const char PROGMEM STR_MIN[] = "Мин.";
const char PROGMEM STR_MEASSURES_CNT[] = "Замеров";
const char PROGMEM STR_MEASSURES[] = "Замер";
const char PROGMEM STR_MICRO_SM_BRACKET[] = "(мкСм)";
const char PROGMEM STR_MICRO_SM[] = "мкСм";

const char PROGMEM STR_SETTINGS_TIME[] =       "T подсв. ";
const char PROGMEM STR_SETTINGS_BRIGHTNESS[] = "Яркость";
const char PROGMEM STR_SETTINGS_TIMER[] =      "Таймер  ";
const char PROGMEM STR_SETTINGS_SOUND[] =      "Звук    ";
const char PROGMEM STR_SETTINGS_CONTRAST[] =   "Контраст";
const char PROGMEM STR_ON[] = "Вкл.";
const char PROGMEM STR_OFF[] = "Выкл";
const char PROGMEM STR_RX_OHM[] = "Rx,Ом";
const char PROGMEM STR_GX_MKSM[] = "Gx,мкСм";
const char PROGMEM STR_LIMITS[] = "Нормы ПД";
const char PROGMEM STR_K_MG[] = "К,мг";

const char PROGMEM STR_SHUTTING_DOWN[] = "Выключение!";
const char PROGMEM STR_BATTERY[] = "Аккумулятор";
const char PROGMEM STR_DISCHARGED[] = "разряжен!";
const char PROGMEM STR_CHARGED[] = "разряжен!";
const char PROGMEM STR_STATISTIC_IS[] = "Статистика";
const char PROGMEM STR_CLEANED[] = "очищена!";
const char PROGMEM STR_RESULT[] = "Замер";
const char PROGMEM STR_STORED[] = "сохранён!";


const char PROGMEM STR_TIME_VALUES[7][4] = {
		"---\x00",
		"10с\x00",
		"15с\x00",
		"20с\x00",
		"30с\x00",
		"45с\x00",
		"60с\x00"
};

const char PROGMEM STR_TIMER_VALUES[5][5] = {
		"Выкл\x00",
		"1мин\x00",
		"2мин\x00",
		"3мин\x00",
		"5мин\x00"
};


