#define ADC_BATTERY			0b11000100		// ADC4, ������ ������������, ���-����������
#define ADC_PRODUCT			0b11000101		// ADC5, ������ ������������, ���-����������

/**
 * ���������� ������ ��������
 **/
//void meassure() {
//	ADMUX = 0b11000101;                      //11000101 ������������� ��� ��: ADC5,������ ������������,���-����������
//	uint16_t dADC = 0;                   //���������� ��� �������� ���������� ��������
//	for (uint8_t i = 0; i < 10; i++) {		// ���� �� 10 �������
//		wdt_reset();
//		while (bit_is_set(ADCSRA, ADSC));     //�������� ���������� ��������������
//		wdt_reset();
//		cli();                               //���������� ����������
//		ADCSRA = 0b11000100;                   //11000100 - ���� ��� (����=16, f=125 ���, ���������.)
//		while (bit_is_set(ADCSRA, ADSC));     //�������� ���������� ��������������
//		wdt_reset();
//		dADC += ADCL;                      //������ �������� ������� ���
//		dADC += 256*ADCH;                //������ �������� ������� ���
//		sei();                               //���������� ����������
//	}
//	ADCx10 = dADC;                           //���������� � ���������� ����������
//}



uint16_t meassureAdc(uint8_t channel, uint8_t count) {
	ADMUX = channel;
	uint16_t result = 0;						// ���������� ��� �������� ���������� ��������
	for (uint8_t i = 0; i < count; i++) {
		wdt_reset();
		while (bit_is_set(ADCSRA, ADSC));     //�������� ���������� ��������������
		cli();                               //���������� ����������
		ADCSRA = 0b11000100;                   //11000100 - ���� ��� (����=16, f=125 ���, ���������.)
		while (bit_is_set(ADCSRA, ADSC));     //�������� ���������� ��������������
//		result += ADCL;                      //������ �������� ������� ���
//		result += 256*ADCH;                //������ �������� ������� ���
		result += ADC;
		sei();                               //���������� ����������
	}
	return result;
}


void meassure() {
	ADCx10 = meassureAdc(ADC_PRODUCT, 10);
}


static void meassureBattery() {
/*
	ACD=((U���/����)/Vref)*1024)
	Vref=2.4�_2.56�_2.7� ��� (2.56�)
	K���=(R���_�����+R���_����||R��_���_��)/(R���_����||R��_���_��) - ����.������� ��������
	K���=(100�+100�||100M)/(100�||100M)=(100�+99,9�)/99,9�=2,001 => 2

	���������� ���������� ��� N �������:
	U���=(ACD/N)*Vref*����/1024

	���������� ��:
	1) mVolt=(ACD/N)*(Vref/1024)*K���=(ACD/N)*(2500/1024)*2=(ACD/N)*(5000)/1024=
	=(ACD*5000)/(1024*N)=(ACD*5000)/20480 (��� N=20)

	���������� �� � ������������ ���������� Vref:
	2) mVolt=(ACD/N)*(Vref/1024)*K���=(ACD*Vref)*2/(N*1024)=(ACD*Vref)/(N*512)=
	=(ACD*Vref)/10240 (��� N=20)

	��� ����������� 3-� �������� � ������� �.�� � ��������� ������� �� 10 � �����������
	(������������ 5 ����� ��������)
	1) mVolt=((ACD*5000)+5)/204800  - 1 ������� (N=20)
	2) mVolt=((ACD*vref)+5)/102400  - 2 ������� (N=20)
*/
	uint16_t dADC = meassureAdc(ADC_BATTERY, 20);
	dADC += dADC + 10;                          //��������� ���������� ������� (+ N/2)
	unsigned long mVolt = dADC;                            //�������������� ���� unsigned int � unsigned long

	mVolt = ((mVolt*vref)+5)/10240;          //2 ������� (N=20)
	Ubat_mV = mVolt;                         //�������������� ���� unsigned long � unsigned int
	dADC = mVolt/10;						// �������������� ���� unsigned long � unsigned int
	Ubat = dADC;
	batter = calcBatteryLevel(dADC);
}
