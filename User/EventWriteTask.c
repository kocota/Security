#include "EventWriteTask.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"
//#include "m95.h"

extern osMutexId Fm25v02MutexHandle;
extern osThreadId EventWriteTaskHandle;
extern status_register_struct status_registers;
extern control_register_struct control_registers;


void ThreadEventWriteTask(void const * argument)
{
	uint8_t temp_data[30];
	uint16_t address_event_temp;
	uint8_t read_temp;

	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_write(ADDRESS_PROCESSED_EVENT_H_REG, 0x20);
	fm25v02_write(ADDRESS_PROCESSED_EVENT_L_REG, 0x00);
	osMutexRelease(Fm25v02MutexHandle);

	osMutexWait(Fm25v02MutexHandle, osWaitForever); // вычитывавем из памяти значение последнего события
	fm25v02_read(ADDRESS_LAST_EVENT_H_REG, &read_temp);
	status_registers.address_last_event_h_reg = read_temp;
	fm25v02_read(ADDRESS_LAST_EVENT_L_REG, &read_temp);
	status_registers.address_last_event_l_reg = read_temp;
	osMutexRelease(Fm25v02MutexHandle);

	address_event_temp = (((status_registers.address_last_event_h_reg)<<8)|(status_registers.address_last_event_l_reg&0x00FF));

	if( (address_event_temp < 0x2000) || (address_event_temp > 0x7FFF) )
	{
		osMutexWait(Fm25v02MutexHandle, osWaitForever);
		fm25v02_write(ADDRESS_LAST_EVENT_H_REG, 0x20);
		fm25v02_write(ADDRESS_LAST_EVENT_L_REG, 0x00);
		osMutexRelease(Fm25v02MutexHandle);
		//fm25v02_fast_write(ADDRESS_LAST_EVENT_H_REG, 0x20, 1);
		//fm25v02_fast_write(ADDRESS_LAST_EVENT_L_REG, 0x00, 1);
	}



	for(;;)
	{
		osThreadSuspend(EventWriteTaskHandle);

		osMutexWait(Fm25v02MutexHandle, osWaitForever); // вычитывавем из памяти значение последнего события
		fm25v02_read(ADDRESS_LAST_EVENT_H_REG, &read_temp);
		status_registers.address_last_event_h_reg = read_temp;
		fm25v02_read(ADDRESS_LAST_EVENT_L_REG, &read_temp);
		status_registers.address_last_event_l_reg = read_temp;
		osMutexRelease(Fm25v02MutexHandle);

		address_event_temp = (((status_registers.address_last_event_h_reg)<<8)|(status_registers.address_last_event_l_reg&0x00FF)); // считаем значение адреса последнего события

		if( (address_event_temp >= 0x2000) && (address_event_temp <= 0x7FFF) ) // проверяем, чтобы значение адреса события лежало в области памяти ведения протокола событий
		{

			osMutexWait(Fm25v02MutexHandle, osWaitForever);

			fm25v02_read(TIME_CURRENT_YEAR_REG, &temp_data[0]);
			fm25v02_read(TIME_CURRENT_MONTH_REG, &temp_data[1]);
			fm25v02_read(TIME_CURRENT_DAY_REG, &temp_data[2]);
			fm25v02_read(TIME_CURRENT_HOUR_REG, &temp_data[3]);
			fm25v02_read(TIME_CURRENT_MINUTE_REG, &temp_data[4]);
			fm25v02_read(TIME_CURRENT_SECOND_REG, &temp_data[5]);
			fm25v02_read(SYSTEM_STATUS_REG, &temp_data[6]);
			fm25v02_read(SECURITY_STATUS_REG, &temp_data[7]);
			fm25v02_read(STATUS_LOOP_REG, &temp_data[8]);
			fm25v02_read(ALARM_LOOP_REG, &temp_data[9]);
			fm25v02_read(ERROR_LOOP_REG, &temp_data[10]);
			fm25v02_read(IBUTTON_COMPLETE_0_REG, &temp_data[11]);
			fm25v02_read(IBUTTON_COMPLETE_1_REG, &temp_data[12]);
			fm25v02_read(IBUTTON_COMPLETE_2_REG, &temp_data[13]);
			fm25v02_read(IBUTTON_COMPLETE_3_REG, &temp_data[14]);
			fm25v02_read(IBUTTON_COMPLETE_4_REG, &temp_data[15]);
			fm25v02_read(IBUTTON_COMPLETE_5_REG, &temp_data[16]);
			fm25v02_read(IBUTTON_COMPLETE_6_REG, &temp_data[17]);
			fm25v02_read(IBUTTON_COMPLETE_7_REG, &temp_data[18]);
			fm25v02_read(POWER_ON_REG, &temp_data[19]);

			osMutexRelease(Fm25v02MutexHandle);

			/*
			temp_data[0] = status_registers.time_current_year_reg; // записываем текущие значения переменных для события во временный буфер
			temp_data[1] = status_registers.time_current_month_reg;
			temp_data[2] = status_registers.time_current_day_reg;
			temp_data[3] = status_registers.time_current_hour_reg;
			temp_data[4] = status_registers.time_current_minute_reg;
			temp_data[5] = status_registers.time_current_second_reg;
			temp_data[6] = status_registers.system_status_reg;
			temp_data[7] = status_registers.security_status_reg;
			temp_data[8] = status_registers.status_loop_reg;
			temp_data[9] = status_registers.alarm_loop_reg;
			temp_data[10] = status_registers.error_loop_reg;
			temp_data[11] = status_registers.ibutton_complite_0_reg;
			temp_data[12] = status_registers.ibutton_complite_1_reg;
			temp_data[13] = status_registers.ibutton_complite_2_reg;
			temp_data[14] = status_registers.ibutton_complite_3_reg;
			temp_data[15] = status_registers.ibutton_complite_4_reg;
			temp_data[16] = status_registers.ibutton_complite_5_reg;
			temp_data[17] = status_registers.ibutton_complite_6_reg;
			temp_data[18] = status_registers.ibutton_complite_7_reg;
			temp_data[19] = status_registers.power_on_reg;
			*/

			osMutexWait(Fm25v02MutexHandle, osWaitForever);
			fm25v02_fast_write(address_event_temp, &temp_data[0], 30); // переписываем текущие значения переменных для события в память
			osMutexRelease(Fm25v02MutexHandle);

			address_event_temp = address_event_temp + 30;

			if( address_event_temp > 0x7FFF )
			{
				address_event_temp = 0x2000;

				read_temp = (uint8_t)((address_event_temp>>8)&0x00FF);
				fm25v02_write(ADDRESS_LAST_EVENT_H_REG, read_temp);
				read_temp = (uint8_t)(address_event_temp&0x00FF);
				fm25v02_write(ADDRESS_LAST_EVENT_L_REG, read_temp);

			}
			else if( (address_event_temp >= 0x2000) && (address_event_temp <= 0x7FFF) )
			{
				read_temp = (uint8_t)((address_event_temp>>8)&0x00FF);
				fm25v02_write(ADDRESS_LAST_EVENT_H_REG, read_temp);
				read_temp = (uint8_t)(address_event_temp&0x00FF);
				fm25v02_write(ADDRESS_LAST_EVENT_L_REG, read_temp);
			}

		}
		else // здесь должно быть то, что нужно сделать если значение адреса не вошло в область памяти ведения протокола событий, например можно выставить соответствующий флаг и сделать запрос на сервер
		{

		}




		osDelay(1);
	}
}
