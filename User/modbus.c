#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"


extern osMutexId Fm25v02MutexHandle;
extern uint8_t Version_H;

status_register_struct status_registers;
control_register_struct control_registers;

//----функция чтения из памяти регистров статуса--------------------------
void read_status_registers(void)
{
	uint8_t status_reg_temp;
	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(VERSION_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.version_reg = status_reg_temp|(((uint16_t)Version_H)<<8); // костыль для чтения регистра версии прошивки
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(SECURITY_STATUS_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.security_status_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(STATUS_LOOP_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.status_loop_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ERROR_LOOP_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.error_loop_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ALARM_LOOP_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.alarm_loop_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CURRENT_YEAR_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.time_current_year_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CURRENT_MONTH_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.time_current_month_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CURRENT_DAY_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.time_current_day_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CURRENT_HOUR_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.time_current_hour_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CURRENT_MINUTE_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.time_current_minute_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CURRENT_SECOND_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.time_current_second_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CURRENT_WEEKDAY_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.time_current_weekday_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ADDRESS_PROCESSED_EVENT_H_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.address_processed_event_h_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ADDRESS_PROCESSED_EVENT_L_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.address_processed_event_l_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ADDRESS_LAST_EVENT_H_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.address_last_event_h_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ADDRESS_LAST_EVENT_L_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.address_last_event_l_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(SYSTEM_STATUS_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.system_status_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(POWER_ON_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.power_on_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ERROR_RTC_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.error_rtc_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_0_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_0_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_1_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_1_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_2_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_2_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_3_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_3_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_4_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_4_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_5_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_5_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_6_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_6_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IBUTTON_COMPLETE_7_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ibutton_complite_7_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_ERROR_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_error_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_CURRENT_A_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_current_a_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_CURRENT_B_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_current_b_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_CURRENT_C_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_current_c_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_CURRENT_MIL_A_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_current_mil_a_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_CURRENT_MIL_B_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_current_mil_b_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_CURRENT_MIL_C_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_current_mil_c_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_VOLT_A_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_volt_a_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_VOLT_B_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_volt_b_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_VOLT_C_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_volt_c_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_VOLT_MIL_A_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_volt_mil_a_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_VOLT_MIL_B_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_volt_mil_b_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_VOLT_MIL_C_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_volt_mil_c_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_POWER_A_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_power_a_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_POWER_B_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_power_b_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_POWER_C_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_power_c_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_POWER_MIL_A_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_power_mil_a_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_POWER_MIL_B_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_power_mil_b_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_POWER_MIL_C_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_power_mil_c_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_TOTAL_POWER_H_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_total_power_h_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_TOTAL_POWER_L_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_total_power_l_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CE_303_TOTAL_POWER_MIL_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.ce303_total_power_mil_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(SIGNAL_LEVEL_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.signal_level_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG1, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg1 = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG2, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg2 = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG3, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg3 = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG4, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg4 = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG5, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg5 = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG6, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg6 = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG7, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg7 = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ICCID_NUMBER_REG8, &status_reg_temp);
	osMutexRelease(Fm25v02MutexHandle);
	status_registers.iccid_number_reg8 = status_reg_temp;
}
//----------------------------------------------------------------

//----Функция чтения из памяти управляющих регистров--------------
void read_control_registers(void)
{
	uint8_t status_reg_temp;
	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(SECURITY_CONTROL_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.security_control_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(CONTROL_LOOP_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.control_loop_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(FILTER_TIME_LOOP_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.filter_time_loop_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(QUANTITY_FALSE_LOOP_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.quantity_false_loop_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_FALSE_LOOP_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_false_loop_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ALARM_LOOP_CLEAR_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.alarm_loop_clear_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(FALSE_LOOP_CLEAR_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.false_loop_clear_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(SECURITY_TIME_MAX_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.security_time_max_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_UPDATE_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_update_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_YEAR_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_year_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_MONTH_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_month_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_DAY_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_day_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_HOUR_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_hour_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_MINUTE_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_minute_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_SECONDS_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_seconds_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_WEEKDAY_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_weekday_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(MODBUS_IDLE_TIME_MAX_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.modbus_idle_time_max_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(TIME_CONNECTION_TEST_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.time_connection_test_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(EVENT_READ_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.event_read_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(EVENT_ADDRESS_HIGH_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.event_address_high_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(EVENT_ADDRESS_LOW_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.event_address_low_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(MODEM_RING_TRY_LOAD_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.modem_ring_try_load_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(MODEM_RING_PAUSE_LOAD_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.modem_ring_pause_load_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(MODEM_RING_PAUSE2_LOAD_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.modem_ring_pause2_load_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(RING_MINUTE_TIME_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.ring_minute_time_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(RING_HOUR_TIME_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.ring_hour_time_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ID_HIGH_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.id_high_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(ID_LOW_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.id_low_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(RESET_CONTROL_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.reset_control_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(METER_POLLING_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.meter_polling_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IP_1_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.ip1_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IP_2_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.ip2_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IP_3_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.ip3_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(IP_4_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.ip4_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(PORT_HIGH_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.port_high_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(PORT_LOW_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.port_low_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(METER_ID_HIGH_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.meter_id_high_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(METER_ID_LOW_REG, &status_reg_temp);
	//osMutexRelease(Fm25v02MutexHandle);
	control_registers.meter_id_low_reg = status_reg_temp;
	//osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(GPRS_CALL_REG, &status_reg_temp);
	control_registers.gprs_call_reg = status_reg_temp;
	//osMutexRelease(Fm25v02MutexHandle);
	fm25v02_read(LIGHT_CONTROL_REG, &status_reg_temp);
	control_registers.light_control_reg = status_reg_temp;
	//osMutexRelease(Fm25v02MutexHandle);
	fm25v02_read(MUTE_REG, &status_reg_temp);
	osMutexRelease(Fm25v02MutexHandle);
	control_registers.mute_reg = status_reg_temp;
}
//----------------------------------------------------------------


//-----Блок расчета CRC16 для Modbus---------------------------------------------------------------

const unsigned char MB_Slave_aucCRCHi[] =
{
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

const unsigned char MB_Slave_aucCRCLo[] =
{
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
  0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
  0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
  0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
  0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
  0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
  0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
  0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
  0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
  0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
  0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
  0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
  0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
  0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
  0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

//------------------------------------------------------------------------------------------------------



//--------------------- Процедура расчета CRC16 -------------------------------------

 unsigned int CRC16( unsigned char * pucFrame, unsigned int usLen ){
  unsigned char MB_Slave_ucCRCHi = 0xFF;
  unsigned char MB_Slave_ucCRCLo = 0xFF;
  int iIndex;
  //
  while( usLen-- ){
    iIndex = MB_Slave_ucCRCLo ^ *( pucFrame++ );
    MB_Slave_ucCRCLo = MB_Slave_ucCRCHi ^ MB_Slave_aucCRCHi[iIndex];
    MB_Slave_ucCRCHi = MB_Slave_aucCRCLo[iIndex];
  }
  return MB_Slave_ucCRCHi << 8 | MB_Slave_ucCRCLo;
}

//------------------------------------------------------------------------------------
