#include "GetCurrentTask.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "modbus.h"
#include "fm25v02.h"

extern ADC_HandleTypeDef hadc1;
//extern ADC_HandleTypeDef hadc1;
//extern osThreadId LedTaskHandle;
extern osMutexId Fm25v02MutexHandle;
extern status_register_struct status_registers;
extern control_register_struct control_registers;

uint32_t data_in[3];


void ThreadGetCurrentTask(void const * argument)
{
	uint8_t overcurrent_phase_a_state = 0;
	uint8_t overcurrent_phase_b_state = 0;
	uint8_t overcurrent_phase_c_state = 0;
	uint8_t temp_h;
	uint8_t temp_l;


	for(;;)
	{
		switch(control_registers.lighting_switching_reg) // проверяем включена ли охранная сигнаизация
		{
			case(ALARM_ON): // если сигнализация включена, то делаем измерения тока

				HAL_ADCEx_InjectedStart(&hadc1);

				while( HAL_ADCEx_InjectedPollForConversion(&hadc1, 1) != HAL_OK )
				{
					//LED5_TOGGLE();
				}

				data_in[0] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
				data_in[1] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
				data_in[2] = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);

				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_write(2*CURRENT_PHASE_A_REG, 0x00);
				fm25v02_write(2*CURRENT_PHASE_A_REG+1, (uint8_t)(data_in[0]*100/4095));
				fm25v02_write(2*CURRENT_PHASE_B_REG, 0x00);
				fm25v02_write(2*CURRENT_PHASE_B_REG+1, (uint8_t)(data_in[1]*100/4095));
				fm25v02_write(2*CURRENT_PHASE_C_REG, 0x00);
				fm25v02_write(2*CURRENT_PHASE_C_REG+1, (uint8_t)(data_in[2]*100/4095));
				osMutexRelease(Fm25v02MutexHandle);

				status_registers.current_phase_a_reg = data_in[0]*100/4095;
				status_registers.current_phase_b_reg = data_in[1]*100/4095;
				status_registers.current_phase_c_reg = data_in[2]*100/4095;

				if(status_registers.current_phase_a_reg > control_registers.max_current_phase_a) // проверяем если значение тока превысило максимальное значение тока фазы А
				{
					if(overcurrent_phase_a_state<10)
					{
						overcurrent_phase_a_state++;

					}
					else if(overcurrent_phase_a_state==10)
					{
						overcurrent_phase_a_state++;
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*LIGHTING_ALARM_REG, &temp_h);
						fm25v02_read(2*LIGHTING_ALARM_REG+1, &temp_l);
						temp_l = temp_l|0x80;
						fm25v02_write(2*LIGHTING_ALARM_REG+1, temp_l);
						osMutexRelease(Fm25v02MutexHandle);
						status_registers.lighting_alarm_reg = (((uint16_t)temp_h)<<8)|temp_l; // если превышение тока длилось 10 циклов, то выставляем бит превышения тока фазы А в регистр аварий
						//LED6_ON();
					}

				}
				else
				{
					overcurrent_phase_a_state = 0;
					//LED6_OFF();
				}

				if(status_registers.current_phase_b_reg > control_registers.max_current_phase_b) // проверяем если значение тока превысило максимальное значение тока фазы В
				{
					if(overcurrent_phase_b_state<10)
					{
						overcurrent_phase_b_state++;

					}
					else if(overcurrent_phase_b_state==10)
					{
						overcurrent_phase_b_state++;
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*LIGHTING_ALARM_REG, &temp_h);
						fm25v02_read(2*LIGHTING_ALARM_REG+1, &temp_l);
						temp_h = temp_h|0x01;
						fm25v02_write(2*LIGHTING_ALARM_REG, temp_h);
						osMutexRelease(Fm25v02MutexHandle);
						status_registers.lighting_alarm_reg = (((uint16_t)temp_h)<<8)|temp_l; // если превышение тока длилось 10 циклов, то выставляем бит превышения тока фазы В в регистр аварий
						//LED7_ON();
					}

				}
				else
				{
					overcurrent_phase_b_state = 0;
					//LED7_OFF();
				}

				if(status_registers.current_phase_c_reg > control_registers.max_current_phase_c) // проверяем если значение тока превысило максимальное значение тока фазы С
				{
					if(overcurrent_phase_c_state<10)
					{
						overcurrent_phase_c_state++;

					}
					else if(overcurrent_phase_c_state==10)
					{
						overcurrent_phase_c_state++;
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*LIGHTING_ALARM_REG, &temp_h);
						fm25v02_read(2*LIGHTING_ALARM_REG+1, &temp_l);
						temp_h = temp_h|0x02;
						fm25v02_write(2*LIGHTING_ALARM_REG, temp_h);
						osMutexRelease(Fm25v02MutexHandle);
						status_registers.lighting_alarm_reg = (((uint16_t)temp_h)<<8)|temp_l; // если превышение тока длилось 10 циклов, то выставляем бит превышения тока фазы А в регистр аварий
						//LED7_ON();
					}

				}
				else
				{
					overcurrent_phase_c_state = 0;
					//LED7_OFF();
				}



			break;

		}

		osDelay(10);
	}
}
