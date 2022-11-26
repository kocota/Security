#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"
#include "m95.h"
#include "gpio.h"

extern UART_HandleTypeDef huart3;
extern osSemaphoreId TransmissionStateHandle;
extern osSemaphoreId ReceiveStateHandle;
extern osMutexId UartMutexHandle;
extern osMutexId Fm25v02MutexHandle;
extern osTimerId Ring_Center_TimerHandle;
extern osThreadId CallRingCenterTaskHandle;

extern uint8_t modem_rx_data[256];

extern uint8_t level;

volatile uint8_t ip1 = 0; // стартовое значение ip адреса сервера
volatile uint8_t ip2 = 0; // стартовое значение ip адреса сервера
volatile uint8_t ip3 = 0; // стартовое значение ip адреса сервера
volatile uint8_t ip4 = 0;  // стартовое значение ip адреса сервера
volatile uint8_t port_high_reg = 0; // старший байт номера порта сервера
volatile uint8_t port_low_reg = 0;  // младший байт номера порта сервера
volatile uint16_t port = 0;   // номер порта сервера

uint8_t state;
uint8_t Version_H = 0;  // версия прошивки, старший байт
uint8_t Version_L = 14; // версия прошивки, младший байт
uint8_t id2[10]; // номер CCID симкарты
uint64_t id1[20];

uint8_t idh = 1;
uint8_t idl = 15;

volatile uint8_t request_state = 0;



void ThreadM95Task(void const * argument)
{

	osSemaphoreWait(TransmissionStateHandle, osWaitForever); // обнуляем семафор, при создании семафора его значение равно 1
	osSemaphoreWait(ReceiveStateHandle, osWaitForever); // обнуляем семафор, при создании семафора его значение равно 1

	if(AT()==AT_ERROR)
	{
		m95_power_on();
		while(AT()==AT_ERROR)
		{

		}
	}
	else if(AT()==AT_OK)
	{
		m95_power_off();
		HAL_Delay(12000);
		m95_power_on();
		while(AT()==AT_ERROR)
		{

		}
	}


	//----Обнуление регистров IP адреса и порта сервера, обнуление ID устройства------
	// Для записи регистров раскоментировать строки и прошить контроллер
	/*
	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	uint8_t data0 = 0;
	fm25v02_fast_write(ID_HIGH_REG, &data0, 1);
	fm25v02_fast_write(ID_LOW_REG, &data0, 1);

	fm25v02_fast_write(IP_1_REG, &data0, 1);
	fm25v02_fast_write(IP_2_REG, &data0, 1);
	fm25v02_fast_write(IP_3_REG, &data0, 1);
	fm25v02_fast_write(IP_4_REG, &data0, 1);
	fm25v02_fast_write(PORT_HIGH_REG, &data0, 1);
	fm25v02_fast_write(PORT_LOW_REG, &data0, 1);
	osMutexRelease(Fm25v02MutexHandle);
	*/

	/*
	// сервер освещения Михаил, неопределено 239
	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_write(ID_HIGH_REG, 0);
	fm25v02_write(ID_LOW_REG, 239);

	fm25v02_write(IP_1_REG, 195);
	fm25v02_write(IP_2_REG, 208);
	fm25v02_write(IP_3_REG, 163);
	fm25v02_write(IP_4_REG, 67);
	fm25v02_write(PORT_HIGH_REG, 136);
	fm25v02_write(PORT_LOW_REG, 254);
	osMutexRelease(Fm25v02MutexHandle);
	*/


	// сервер освещения Главный, неопределено 239

	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_write(2*LIGHTING_SWITCHING_REG, 0x00);
	fm25v02_write(2*LIGHTING_SWITCHING_REG+1, 0x01);
	fm25v02_write(2*ID_HIGH_REG, 0);
	fm25v02_write(2*ID_HIGH_REG+1, 0);
	fm25v02_write(2*ID_LOW_REG, 0);
	fm25v02_write(2*ID_LOW_REG+1, 239);

	fm25v02_write(2*IP_1_REG, 0);
	fm25v02_write(2*IP_1_REG+1, 213);
	fm25v02_write(2*IP_2_REG, 0);
	fm25v02_write(2*IP_2_REG+1, 183);
	fm25v02_write(2*IP_3_REG, 0);
	fm25v02_write(2*IP_3_REG+1, 101);
	fm25v02_write(2*IP_4_REG, 0);
	fm25v02_write(2*IP_4_REG+1, 177);
	fm25v02_write(2*PORT_HIGH_REG, 0);
	fm25v02_write(2*PORT_HIGH_REG+1, 136);
	fm25v02_write(2*PORT_LOW_REG, 0);
	fm25v02_write(2*PORT_LOW_REG+1, 255);
	osMutexRelease(Fm25v02MutexHandle);


	// сервер сигнализации резерв. Не определено 271
	/*
	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_write(ID_HIGH_REG, 1);
	fm25v02_write(ID_LOW_REG, 15);

	fm25v02_write(IP_1_REG, 195);
	fm25v02_write(IP_2_REG, 208);
	fm25v02_write(IP_3_REG, 163);
	fm25v02_write(IP_4_REG, 67);
	fm25v02_write(PORT_HIGH_REG, 136);
	fm25v02_write(PORT_LOW_REG, 234);
	osMutexRelease(Fm25v02MutexHandle);
	*/

	/*
	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_write(MONTH_LIGHTING_OFF_REG, 11);
	fm25v02_write(DAY_LIGHTING_OFF_REG, 24);
	fm25v02_write(HOUR_LIGHTING_OFF_REG, 14);
	fm25v02_write(MINUTE_LIGHTING_OFF_REG, 52);

	fm25v02_write(MONTH_LIGHTING_ON_REG, 11);
	fm25v02_write(DAY_LIGHTING_ON_REG, 24);
	fm25v02_write(HOUR_LIGHTING_ON_REG, 14);
	fm25v02_write(MINUTE_LIGHTING_ON_REG, 54);

	fm25v02_write(CE_303_CURRENT_A_REG, 11);
	fm25v02_write(CE_303_CURRENT_B_REG, 12);
	fm25v02_write(CE_303_CURRENT_C_REG, 13);
	fm25v02_write(CE_303_CURRENT_MIL_A_REG, 21);
	fm25v02_write(CE_303_CURRENT_MIL_B_REG, 22);
	fm25v02_write(CE_303_CURRENT_MIL_C_REG, 23);
	fm25v02_write(CE_303_VOLT_A_REG, 21);
	fm25v02_write(CE_303_VOLT_B_REG, 22);
	fm25v02_write(CE_303_VOLT_C_REG, 23);
	fm25v02_write(CE_303_VOLT_MIL_A_REG, 31);
	fm25v02_write(CE_303_VOLT_MIL_B_REG, 32);
	fm25v02_write(CE_303_VOLT_MIL_C_REG, 33);
	fm25v02_write(CE_303_POWER_A_REG, 31);
	fm25v02_write(CE_303_POWER_B_REG, 32);
	fm25v02_write(CE_303_POWER_C_REG, 33);
	fm25v02_write(CE_303_POWER_MIL_A_REG, 41);
	fm25v02_write(CE_303_POWER_MIL_B_REG, 42);
	fm25v02_write(CE_303_POWER_MIL_C_REG, 43);

	fm25v02_write(LIGHT_CONTROL_REG, 56);
	fm25v02_write(LIGHTING_STATUS_REG, 128);

	osMutexRelease(Fm25v02MutexHandle);
	*/

	/*
	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_write(2*VERSION_REG, 0x01);
	fm25v02_write(2*VERSION_REG+1, 0x0E);
	fm25v02_write(2*ID_HIGH_REG, 0x00);
	fm25v02_write(2*ID_HIGH_REG+1, 0x01);
	fm25v02_write(2*ID_LOW_REG, 0x00);
	fm25v02_write(2*ID_LOW_REG+1, 0x0F);
	fm25v02_write(2*IP_1_REG, 0);
	fm25v02_write(2*IP_1_REG+1, 0);
	fm25v02_write(2*IP_2_REG, 0);
	fm25v02_write(2*IP_2_REG+1, 0);
	fm25v02_write(2*IP_3_REG, 0);
	fm25v02_write(2*IP_3_REG+1, 0);
	fm25v02_write(2*IP_4_REG, 0);
	fm25v02_write(2*IP_4_REG+1, 0);
	fm25v02_write(2*PORT_HIGH_REG, 0);
	fm25v02_write(2*PORT_HIGH_REG+1, 0);
	fm25v02_write(2*PORT_LOW_REG, 0);
	fm25v02_write(2*PORT_LOW_REG+1, 0);
	fm25v02_write(2*POWER_ON_REG, 0);
	fm25v02_write(2*POWER_ON_REG+1, 0);
	fm25v02_write(2*STATUS_LOOP_REG, 0);
	fm25v02_write(2*STATUS_LOOP_REG+1, 0);
	osMutexRelease(Fm25v02MutexHandle);
	*/

	//---------------------------------------------------------------------------------------------------------

	osMutexWait(Fm25v02MutexHandle, osWaitForever);

	//fm25v02_fast_write(VERSION_REG, &Version_L, 1); // записываем в память версию прошивки

	fm25v02_fast_read(2*IP_1_REG+1, &ip1, 1); // читаем значение IP адреса сервера из памяти
	fm25v02_fast_read(2*IP_2_REG+1, &ip2, 1);
	fm25v02_fast_read(2*IP_3_REG+1, &ip3, 1);
	fm25v02_fast_read(2*IP_4_REG+1, &ip4, 1);
	fm25v02_fast_read(2*PORT_HIGH_REG+1, &port_high_reg, 1); // читаем значение старшего байта порта сервера
	fm25v02_fast_read(2*PORT_LOW_REG+1, &port_low_reg, 1); // читаем занчение младшего байта порта сервера

	osMutexRelease(Fm25v02MutexHandle);

	port = (((uint16_t)port_high_reg)<<8)|((uint16_t)port_low_reg); // вычисляем общее значение регистра порта

	if ( (ip1==0)&&(ip2==0)&&(ip3==0)&&(ip4==0)&&(port==0) ) // Если значения ip адреса сервера и его номера порта при инициализации нулевые, то выставляем их значения по умолчанию
	{
		// сервер сигнализации
		ip1 = 195;    // значение по умолчанию
		ip2 = 208;    // значение по умолчанию
		ip3 = 163;    // значение по умолчанию
		ip4 = 67;     // значение по умолчанию
		port = 35050; // значение по умолчанию

		// сервер освещения
		//ip1 = 195;    // значение по умолчанию
		//ip2 = 208;    // значение по умолчанию
		//ip3 = 163;    // значение по умолчанию
		//ip4 = 67;     // значение по умолчанию
		//port = 35070; // значение по умолчанию
	}



	for(;;)
	{
		osMutexWait(UartMutexHandle, osWaitForever);

		if(AT()==AT_ERROR) // два раза проверяем, есть ли ответ на команду АТ, если нет, включаем питание
		{
			if(AT()==AT_ERROR)
			{
				m95_power_on();
			}
		}

		if( ATE0() == AT_OK )
		{

		}

		osMutexRelease(UartMutexHandle);

		osMutexWait(UartMutexHandle, osWaitForever);

		switch(AT_QISTATE())
		{
			case IP_INITIAL:

				LED1_OFF();
				if( AT_COPS() == AT_OK )
				{

				}
				if(	AT_QCCID(&id2[0], &id1[0]) == AT_OK ) // читаем CCID сим-карты
				{
					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					//fm25v02_fast_write(ICCID_NUMBER_REG1, &id2[0], 8); // записываем в регистры CCID сим-карты
					fm25v02_write(2*ICCID_NUMBER_REG1, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG1+1, id2[0]);
					fm25v02_write(2*ICCID_NUMBER_REG2, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG2+1, id2[1]);
					fm25v02_write(2*ICCID_NUMBER_REG3, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG3+1, id2[2]);
					fm25v02_write(2*ICCID_NUMBER_REG4, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG4+1, id2[3]);
					fm25v02_write(2*ICCID_NUMBER_REG5, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG5+1, id2[4]);
					fm25v02_write(2*ICCID_NUMBER_REG6, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG6+1, id2[5]);
					fm25v02_write(2*ICCID_NUMBER_REG7, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG7+1, id2[6]);
					fm25v02_write(2*ICCID_NUMBER_REG8, 0x00);
					fm25v02_write(2*ICCID_NUMBER_REG8+1, id2[7]);
					osMutexRelease(Fm25v02MutexHandle);
				}

				if(AT_QIREGAPP("mts.internet.ru", "mts", "mts") == AT_OK)
				{

				}

			break;

			case IP_START:

				LED1_OFF();
				if(AT_QIACT()!=AT_OK)
				{

				}

			break;

			case IP_IND:

				LED1_OFF();
				if(AT_QIDEACT()!=AT_OK)
				{

				}

			break;

			case IP_GPRSACT:

				LED1_OFF();
				if( AT_QIOPEN("TCP", ip1, ip2, ip3, ip4, port) == AT_OK )
				{

				}
				else
				{
					LED1_OFF();
				}

			break;

			case IP_CLOSE:
				osThreadSuspend(CallRingCenterTaskHandle);
				LED1_OFF();
				if( AT_QIOPEN("TCP", ip1 , ip2, ip3, ip4, port) == AT_OK )
				{

				}
				else
				{

				}

			break;

			case PDP_DEACT:

				LED1_OFF();
				if(AT_QIACT()!=AT_OK)
				{

				}
			break;

			case CONNECT_OK: // Если соединение установлено

				osThreadResume(CallRingCenterTaskHandle); // пробуждаем процесс запроса к серверу
				LED1_ON();
				if( AT_COPS() == AT_OK )
				{

				}
				if( AT_CSQ(&level) == AT_OK )
				{
					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*SIGNAL_LEVEL_REG, 0x00);
					fm25v02_write(2*SIGNAL_LEVEL_REG+1, level);
					osMutexRelease(Fm25v02MutexHandle);
				}
				if( request_state == 0)
				{
					request_state = 1;
					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*GPRS_CALL_REG, 0x00);
					fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
					osMutexRelease(Fm25v02MutexHandle);
				}

			break;

			case AT_ERROR:

			break;

			default:

			break;

		}

		osMutexRelease(UartMutexHandle);

		// образцы АТ команд
		/*
		if(AT_QIFGCNT(0) == AT_OK){}
		if(AT_QIMUX(0) == AT_OK){}
		if(AT_QIMODE(0) == AT_OK){}
		if(AT_QIHEAD(1) == AT_OK){}
		if(AT_QISHOWPT(0) == AT_OK){}
		*/

		osDelay(1000);

	}
}
