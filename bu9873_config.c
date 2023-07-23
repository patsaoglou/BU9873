/*
 * bu9873_rtc.c
 *
 *  Created on: Jul 19, 2023
 *      Author: Patsaoglou P.
 *
 *      Firmware driver for the BU9873 RTC IC:
 *      - rtc_init()
 *      - set_time()
 *      - set_date()
 *      - get_time()
 *      - get_date()
 *      - set_alarm_A()
 *      - reset_alarm_A()
 *      - disable_alarm_A()
 *      - get_alarm_pin() (Alarm pin check in a polling mode)
 *      - binary_to_bcd()
 *      - bcd_to_binary()
 *
 *      Instructions:
 *
 *      1) Initialize RTC by passing the Rtc, I2C_HandleTypeDef and GPIO(PORT/PIN) struct addresses to rtc_init()
 *
 *      2) Pass Rtc struct address, second, minute and hour uint8_t values
 *         to set_time() and initialize IC Time registers
 *
 *      3) Pass Rtc struct address, day, month, year and weekday struct values
 *         to set_date() and initialize IC Date registers
 *
 *      4) Use get_time() to Write current time IC values to Time struct
 *         and access the values using the Rtc.Time.X where X is sec/min/hour values
 *
 *      5) Use get_date() to Write current date IC values to Date struct
 *         and access the values using the Rtc.Date.X where X is day/month/year/weekday values
 *
 *      6) Use set_alarm() to enable alarm and write desired time values to alarm IC registers
 *
 *      7) Use reset_alarm() to reset alarm FLG reg that triggers the interrupt pin
 *
 *		8) Use disable_alarm() to disable alarm completely
 *
 *		9) Use get_alarm_pin() to read interrupt pin (Use function in polling mode)
 *
 *		10) binary_to_bcd() and bcd_to_binary() are used for data conversion (IC needs BCD values)
 *
 *		NOTE: set_alarm_A() weekdays parameter is 7bit truth table like number from 0b0000000
 *		(no alarm) to 0b1111111 (every day alarm).
 *
 *			Example: to set alarm for TUE and FRI, set weekday parameter to 0b0100100
 */

#include "bu9873_config.h"

rtc_error rtc_init(Rtc *rtc, I2C_HandleTypeDef *i2c, GPIO_TypeDef *rtc_int_port, uint16_t rtc_int_pin ){
	rtc->i2c = i2c;
	rtc->rtc_int_port = rtc_int_port;
	rtc->rtc_int_pin = rtc_int_pin;

	uint8_t data = 0x00;

	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS,  CONTROL_REGISTER_1 << 4, 1, &data, 1, 100)){
		return RTC_ERROR;
	}

	data = 0x20;

	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS, CONTROL_REGISTER_2 << 4, 1, &data, 1, 100)){
			return RTC_ERROR;
	}

	rtc->cr1_init_value = 0x00;
	rtc->cr2_init_value = TIME_TYPE | CLOCK_ENABLE;

	return RTC_OK;
}

rtc_error set_time(Rtc *rtc, uint8_t second, uint8_t minute, uint8_t hour){
	uint8_t time_data[3];

	rtc->time.second = second;
	rtc->time.minute = minute;
	rtc->time.hour = hour;

	time_data[0] = binary_to_bcd(second);
	time_data[1] = binary_to_bcd(minute);
	time_data[2] = binary_to_bcd(hour);

	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS, SECOND_COUNTER_ADD << 4, 1, time_data, 3, 100)){
		return RTC_ERROR;
	}

	return RTC_OK;
}

rtc_error set_date(Rtc *rtc, uint8_t weekday, uint8_t day, uint8_t month, uint8_t year){
	uint8_t date_data[4];

	rtc->date.weekday = weekday;
	rtc->date.day = day;
	rtc->date.month = month;
	rtc->date.year = year;

	date_data[0] = binary_to_bcd(weekday);
	date_data[1] = binary_to_bcd(day);
	date_data[2] = binary_to_bcd(month);
	date_data[3] = binary_to_bcd(year);

	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS, WEEKDAY_COUNTER_ADD << 4, 1, date_data, 4, 100)){
		return RTC_ERROR;
	}

	return RTC_OK;
}

rtc_error get_time(Rtc *rtc){
	uint8_t time_data[3];

	if (HAL_OK != HAL_I2C_Mem_Read(rtc->i2c, BU9873_DEV_ADDRESS, SECOND_COUNTER_ADD << 4, 1, time_data, 3, 100)){
		return RTC_ERROR;
	}

	rtc->time.second = bcd_to_binary(time_data[0]);
	rtc->time.minute = bcd_to_binary(time_data[1]);
	rtc->time.hour = bcd_to_binary(time_data[2]);

	return RTC_OK;
}

rtc_error get_date(Rtc *rtc){
	uint8_t time_data[4];

	if (HAL_OK != HAL_I2C_Mem_Read(rtc->i2c, BU9873_DEV_ADDRESS, WEEKDAY_COUNTER_ADD << 4, 1, time_data, 4, 100)){
		return RTC_ERROR;
	}

	rtc->date.weekday = bcd_to_binary(time_data[0]);
	rtc->date.day = bcd_to_binary(time_data[1]);
	rtc->date.month = bcd_to_binary(time_data[2]);
	rtc->date.year = bcd_to_binary(time_data[3]);

	return RTC_OK;
}

rtc_error set_alarm_A(Rtc *rtc, uint8_t minute, uint8_t hour, uint8_t weekdays){
	uint8_t alarm_data[3];

	alarm_data[0] = binary_to_bcd(minute);
	alarm_data[1] = binary_to_bcd(hour);
	alarm_data[2] = weekdays;

	rtc->cr1_init_value = rtc->cr1_init_value | ALARMA_ENABLE;

	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS, CONTROL_REGISTER_1 << 4, 1, &rtc->cr1_init_value, 1, 100)){
		return RTC_ERROR;
	}

	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS, ALARMA_MINUTE_COUNTER_ADD << 4, 1, alarm_data, 3, 100)){
		return RTC_ERROR;
	}

	return RTC_OK;
}

rtc_error reset_alarm_A(Rtc *rtc){
	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS, CONTROL_REGISTER_2 << 4, 1, &rtc->cr2_init_value, 1, 100)){
		return RTC_ERROR;
	}

	return RTC_OK;
}


rtc_error disable_alarm_A(Rtc *rtc){
	rtc->cr1_init_value = rtc->cr1_init_value & ~ALARMA_ENABLE;

	if (HAL_OK != HAL_I2C_Mem_Write(rtc->i2c, BU9873_DEV_ADDRESS, CONTROL_REGISTER_1 << 4, 1, &rtc->cr1_init_value, 1, 100)){
		return RTC_ERROR;
	}

	return RTC_OK;
}

uint8_t get_alarm_pin(Rtc *rtc){
	if( HAL_GPIO_ReadPin(rtc->rtc_int_port, rtc->rtc_int_pin) == GPIO_PIN_RESET){
		return 1;
	}

	return 0;
}

// These binary to bcd conversions can only
// be used with two digit numbers / 0 to 99 (Time in our application)
uint8_t  binary_to_bcd(uint8_t binary){
	if (binary>=10){
		uint8_t bcd = binary/10;
		bcd = bcd<<4;
		bcd = bcd | (binary % 10);
		return bcd;
	}

	return binary;
}

uint8_t  bcd_to_binary(uint8_t bcd){
	uint8_t dec = (bcd >> 4) * 10;
	dec += bcd & 0x0F;

	return dec;
}


