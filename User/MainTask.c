#include "MainTask.h"
#include "modbus.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "fm25v02.h"
#include "SecurityTask.h"
#include "m95.h"

extern osTimerId Ring_Center_TimerHandle;
extern RTC_HandleTypeDef hrtc;
//extern osMutexId UartMutexHandle;
extern osThreadId LedTaskHandle;
extern osMutexId Fm25v02MutexHandle;
extern status_register_struct status_registers;
extern control_register_struct control_registers;

RTC_TimeTypeDef current_time;
RTC_DateTypeDef current_date;
RTC_TimeTypeDef set_time;
RTC_DateTypeDef set_date;

uint16_t status_registers_quantity = 58; // количество статусных регистров

volatile uint8_t security_control_temp = 0;
volatile uint8_t security_state_temp = 0;

//uint8_t address1[10] = {0, 0, 0, 3, 4, 1, 0, 0, 0, 0};
//uint8_t address1[10] = "SEND OK   ";

//uint8_t len;

//uint8_t asdf[3] = "END";

//uint32_t* a;



void ThreadMainTask(void const * argument)
{
	//read_status_registers();
	//read_control_registers();

	//osThreadResume(LedTaskHandle);

	osDelay(1000);


	for(;;)
	{
		//read_status_registers();
		//read_control_registers();

		if( HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0) == GPIO_PIN_SET ) // проверяем если есть наличие единицы на пине PFO микросхемы TPS3306-15
		{
			if(status_registers.power_on_reg == 0) // если основного питания до этого не было, записываем в регистр наличия питания 1
			{
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(POWER_ON_REG, 1);
				osMutexRelease(Fm25v02MutexHandle);
				/*
	  			osMutexWait(UartMutexHandle, osWaitForever);
	  			request_to_server();
	  			osMutexRelease(UartMutexHandle);
	  			*/
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(GPRS_CALL_REG, CALL_ON);
				osMutexRelease(Fm25v02MutexHandle);
				//osTimerStart(Ring_Center_TimerHandle, 1);
			}
		}
		else // если на пине PFO микросхемы TPS3306-15 нет наличия единицы
		{
			if(status_registers.power_on_reg == 1)
			{
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(POWER_ON_REG, 0);
				osMutexRelease(Fm25v02MutexHandle);
				/*
	  			osMutexWait(UartMutexHandle, osWaitForever);
	  			request_to_server();
	  			osMutexRelease(UartMutexHandle);
	  			*/
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(GPRS_CALL_REG, CALL_ON);
				osMutexRelease(Fm25v02MutexHandle);
				//osTimerStart(Ring_Center_TimerHandle, 1);
			}
		}

		switch(control_registers.security_control_reg) // проверяем значение переменной включения охранной сигнализации
		{

			case(DISABLE_FROM_SERVER):

				BUZ_OFF();

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(SECURITY_CONTROL_REG, SECURITY_CONTROL_DEFAULT);
				fm25v02_write(SECURITY_STATUS_REG, DISABLED_BY_SERVER);
				osMutexRelease(Fm25v02MutexHandle);

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(GPRS_CALL_REG, CALL_ON);
				osMutexRelease(Fm25v02MutexHandle);

			break;

			case(ENABLE_FROM_SERVER): // если в регистр поступила команда включить из сервера

				BUZ_OFF();

				security_control_temp = ENABLED_BY_SERVER;

				//security_state_temp = status_registers.security_status_reg;

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(SECURITY_STATUS_REG, ARMING_PROCESS);
				osMutexRelease(Fm25v02MutexHandle);

				//osMutexWait(Fm25v02MutexHandle, osWaitForever);
				//fm25v02_write(SECURITY_CONTROL_REG, SECURITY_CONTROL_DEFAULT);
				//fm25v02_write(SECURITY_STATUS_REG, ENABLED_BY_SERVER);
				//osMutexRelease(Fm25v02MutexHandle);

				//osMutexWait(Fm25v02MutexHandle, osWaitForever);
				//fm25v02_write(GPRS_CALL_REG, CALL_ON);
				//osMutexRelease(Fm25v02MutexHandle);

			break;

			case(DISABLE_FROM_IBUTTON): // если поступила команда выключить из сервера или с таблетки

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(SECURITY_CONTROL_REG, SECURITY_CONTROL_DEFAULT);
				fm25v02_write(SECURITY_STATUS_REG, DISABLED_BY_IBUTTON);
				osMutexRelease(Fm25v02MutexHandle);

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(GPRS_CALL_REG, CALL_ON);
				osMutexRelease(Fm25v02MutexHandle);

			break;

			case(ENABLE_FROM_IBUTTON): // если поступила команда включить с таблетки

				security_control_temp = ENABLED_BY_IBUTTON;

				//security_state_temp = status_registers.security_status_reg;

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(SECURITY_STATUS_REG, ARMING_PROCESS);
				osMutexRelease(Fm25v02MutexHandle);

				//osMutexWait(Fm25v02MutexHandle, osWaitForever);
				//fm25v02_write(SECURITY_CONTROL_REG, SECURITY_CONTROL_DEFAULT);
				//fm25v02_write(SECURITY_STATUS_REG, ENABLED_BY_IBUTTON);
				//osMutexRelease(Fm25v02MutexHandle);

				//osMutexWait(Fm25v02MutexHandle, osWaitForever);
				//fm25v02_write(GPRS_CALL_REG, CALL_ON);
				//osMutexRelease(Fm25v02MutexHandle);

			break;
		}

		switch(control_registers.time_update_reg) // проверяем значение переменной обновления времени
		{
			case(SET_TIME): // Если в регистр записана команда установить время

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(TIME_UPDATE_REG, SET_TIME_DEFAULT);
				osMutexRelease(Fm25v02MutexHandle);

				set_time.Hours = control_registers.time_hour_reg; // записываем в переменные структуры времени значения регистров управления временем
				set_time.Minutes = control_registers.time_minute_reg;
				set_time.Seconds = control_registers.time_seconds_reg;
				set_date.Date = control_registers.time_day_reg;
				set_date.Month = control_registers.time_month_reg;
				set_date.Year = control_registers.time_year_reg;
				set_date.WeekDay = control_registers.time_weekday_reg;

				set_time.Hours = RTC_ByteToBcd2(set_time.Hours); // переводим обычный формат времени в BCD
				set_time.Minutes = RTC_ByteToBcd2(set_time.Minutes);
				set_time.Seconds = RTC_ByteToBcd2(set_time.Seconds);
				set_date.Date = RTC_ByteToBcd2(set_date.Date);
				set_date.Month = RTC_ByteToBcd2(set_date.Month);
				set_date.Year = RTC_ByteToBcd2(set_date.Year);
				set_date.WeekDay = RTC_ByteToBcd2(set_date.WeekDay);

				HAL_RTC_SetTime(&hrtc, &set_time, RTC_FORMAT_BCD); // устанавливаем дату и время
				HAL_RTC_SetDate(&hrtc, &set_date, RTC_FORMAT_BCD);


			break;

			case(SET_TIME_DEFAULT): // Если в регистр не поступала команда установки времени

				HAL_RTC_GetTime(&hrtc, &current_time , RTC_FORMAT_BCD); // получаем значение даты и времени
				HAL_RTC_GetDate(&hrtc, &current_date , RTC_FORMAT_BCD);

				current_time.Hours = RTC_Bcd2ToByte(current_time.Hours); // переводим из BCD формата в обычный
				current_time.Minutes = RTC_Bcd2ToByte(current_time.Minutes);
				current_time.Seconds = RTC_Bcd2ToByte(current_time.Seconds);
				current_date.Date = RTC_Bcd2ToByte(current_date.Date);
				current_date.Month = RTC_Bcd2ToByte(current_date.Month);
				current_date.Year = RTC_Bcd2ToByte(current_date.Year);
				current_date.WeekDay = RTC_Bcd2ToByte(current_date.WeekDay);

				osMutexWait(Fm25v02MutexHandle, osWaitForever);

				fm25v02_write(TIME_CURRENT_HOUR_REG, current_time.Hours); // записываем в регистры значения даты и времени
				fm25v02_write(TIME_CURRENT_MINUTE_REG, current_time.Minutes);
				fm25v02_write(TIME_CURRENT_SECOND_REG, current_time.Seconds);
				fm25v02_write(TIME_CURRENT_DAY_REG, current_date.Date);
				fm25v02_write(TIME_CURRENT_MONTH_REG, current_date.Month);
				fm25v02_write(TIME_CURRENT_YEAR_REG, current_date.Year);
				fm25v02_write(TIME_CURRENT_WEEKDAY_REG, current_date.WeekDay);

				osMutexRelease(Fm25v02MutexHandle);

			break;

			/*
			default: // Если в регистр поступила любое другое число отличное от команды установки времени

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(TIME_UPDATE_REG, SET_TIME_DEFAULT);
				osMutexRelease(Fm25v02MutexHandle);

			break;
			*/
		}

		switch(control_registers.lamp_control_reg) // удаленная перезагрузка контроллера
		{
			case(1):
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(LAMP_CONTROL_REG, 0);
				osMutexRelease(Fm25v02MutexHandle);
				NVIC_SystemReset();
			break;
			/*
			default:
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(LAMP_CONTROL_REG, 0);
				osMutexRelease(Fm25v02MutexHandle);
			break;
			*/
		}

		switch(control_registers.alarm_loop_clear_reg) // сбросить сработавшие шлейфы
		{
			case(1):
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(ALARM_LOOP_CLEAR_REG, 0);
				fm25v02_write(ALARM_LOOP_REG, 0);
				osMutexRelease(Fm25v02MutexHandle);

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(GPRS_CALL_REG, CALL_ON);
				osMutexRelease(Fm25v02MutexHandle);
				//osTimerStart(Ring_Center_TimerHandle, 1);

			break;
			/*
			default:
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(ALARM_LOOP_CLEAR_REG, 0);
				osMutexRelease(Fm25v02MutexHandle);
			break;
			*/
		}

		switch(control_registers.false_loop_clear_reg) // сбросить неисправные шлейфы
		{
			case(1):
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(FALSE_LOOP_CLEAR_REG, 0);
				fm25v02_write(ERROR_LOOP_REG, 0);
				osMutexRelease(Fm25v02MutexHandle);

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(GPRS_CALL_REG, CALL_ON);
				osMutexRelease(Fm25v02MutexHandle);
				//osTimerStart(Ring_Center_TimerHandle, 1);

			break;
			/*
			default:
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(FALSE_LOOP_CLEAR_REG, 0);
				osMutexRelease(Fm25v02MutexHandle);
			break;
			*/
		}

		/*
		switch(status_registers.security_status_reg)
		{
			case(DOOR_OPEN_ALARM):

			break;
		}
		*/

		if( (control_registers.light_control_reg&0x01) != 0x00 )
		{
			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_SET);
		}
		else if( (control_registers.light_control_reg&0x01) == 0x00 )
		{
			HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, GPIO_PIN_RESET);
		}

		if( (control_registers.light_control_reg&0x02) != 0x00 )
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
		}
		else if( (control_registers.light_control_reg&0x02) == 0x00 )
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
		}

		if( (control_registers.light_control_reg&0x04) != 0x00 )
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		}
		else if( (control_registers.light_control_reg&0x04) == 0x00 )
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		}

		osDelay(1);
	}
}
