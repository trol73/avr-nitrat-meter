#define ADC_BATTERY			0b11000100		// ADC4, правое выравнивание, ИОН-внутренний
#define ADC_PRODUCT			0b11000101		// ADC5, правое выравнивание, ИОН-внутренний

/**
 * Проведение замера продукта
 **/
//void meassure() {
//	ADMUX = 0b11000101;                      //11000101 Инициализация АЦП МК: ADC5,правое выравнивание,ИОН-внутренний
//	uint16_t dADC = 0;                   //Переменная для хранения измеренной величины
//	for (uint8_t i = 0; i < 10; i++) {		// Цикл из 10 замеров
//		wdt_reset();
//		while (bit_is_set(ADCSRA, ADSC));     //Ожидание завершения преобразования
//		wdt_reset();
//		cli();                               //Запрещение прерываний
//		ADCSRA = 0b11000100;                   //11000100 - Пуск АЦП (Кдел=16, f=125 кГц, однократн.)
//		while (bit_is_set(ADCSRA, ADSC));     //Ожидание завершения преобразования
//		wdt_reset();
//		dADC += ADCL;                      //Чтение младшего разряда АЦП
//		dADC += 256*ADCH;                //Чтение старшего разряда АЦП
//		sei();                               //Разрешение прерываний
//	}
//	ADCx10 = dADC;                           //Перезапись в глобальную переменную
//}



uint16_t meassureAdc(uint8_t channel, uint8_t count) {
	ADMUX = channel;
	uint16_t result = 0;						// Переменная для хранения измеренной величины
	for (uint8_t i = 0; i < count; i++) {
		wdt_reset();
		while (bit_is_set(ADCSRA, ADSC));     //Ожидание завершения преобразования
		cli();                               //Запрещение прерываний
		ADCSRA = 0b11000100;                   //11000100 - Пуск АЦП (Кдел=16, f=125 кГц, однократн.)
		while (bit_is_set(ADCSRA, ADSC));     //Ожидание завершения преобразования
//		result += ADCL;                      //Чтение младшего разряда АЦП
//		result += 256*ADCH;                //Чтение старшего разряда АЦП
		result += ADC;
		sei();                               //Разрешение прерываний
	}
	return result;
}


void meassure() {
	ADCx10 = meassureAdc(ADC_PRODUCT, 10);
}


static void meassureBattery() {
/*
	ACD=((Uбат/Кдел)/Vref)*1024)
	Vref=2.4В_2.56В_2.7В или (2.56В)
	Kдел=(Rдел_верхн+Rдел_нижн||Rвх_ацп_мк)/(Rдел_нижн||Rвх_ацп_мк) - коэф.деления делителя
	Kдел=(100к+100к||100M)/(100к||100M)=(100к+99,9к)/99,9к=2,001 => 2

	Измеренное напряжение для N замеров:
	Uбат=(ACD/N)*Vref*Кдел/1024

	Вычисление мВ:
	1) mVolt=(ACD/N)*(Vref/1024)*Kдел=(ACD/N)*(2500/1024)*2=(ACD/N)*(5000)/1024=
	=(ACD*5000)/(1024*N)=(ACD*5000)/20480 (для N=20)

	Вычисление мВ с подстроечной константой Vref:
	2) mVolt=(ACD/N)*(Vref/1024)*Kдел=(ACD*Vref)*2/(N*1024)=(ACD*Vref)/(N*512)=
	=(ACD*Vref)/10240 (для N=20)

	Для отображения 3-х разрядов в формате Х.ХХ В результат делится на 10 с округлением
	(прибавляется 5 перед делением)
	1) mVolt=((ACD*5000)+5)/204800  - 1 вариант (N=20)
	2) mVolt=((ACD*vref)+5)/102400  - 2 вариант (N=20)
*/
	uint16_t dADC = meassureAdc(ADC_BATTERY, 20);
	dADC += dADC + 10;                          //Коррекция последнего разряда (+ N/2)
	unsigned long mVolt = dADC;                            //Преобразование типа unsigned int в unsigned long

	mVolt = ((mVolt*vref)+5)/10240;          //2 вариант (N=20)
	Ubat_mV = mVolt;                         //Преобразование типа unsigned long в unsigned int
	dADC = mVolt/10;						// Преобразование типа unsigned long в unsigned int
	Ubat = dADC;
	batter = calcBatteryLevel(dADC);
}
