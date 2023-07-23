/*
 * rtc_bu9873.h
 *
 *  Created on: Jul 19, 2023
 *      Author: Patsaoglou P.
 */

#ifndef INC_BU9873_CONFIG_H_
#define INC_BU9873_CONFIG_H_

#include "stm32f1xx_hal.h"

#define BU9873_DEV_ADDRESS (0x32 << 1)

// Important data register address definitions
#define SECOND_COUNTER_ADD 0x00
#define MINUTE_COUNTER_ADD 0x01
#define HOUR_COUNTER_ADD 0x02
#define WEEKDAY_COUNTER_ADD 0x03
#define DAY_COUNTER_ADD 0x04
#define MONTH_COUNTER_ADD 0x05
#define YEAR_COUNTER_ADD 0x06
#define ALARMA_MINUTE_COUNTER_ADD 0x08
#define ALARMA_HOUR_COUNTER_ADD 0x09
#define ALARMA_WEEKDAY_COUNTER_ADD 0x0A

// Important configuration register address definitions
#define CONTROL_REGISTER_1 0x0E
#define CONTROL_REGISTER_2 0x0F

// Important configuration data definitions
// For CONTROL_REGISTER_1
#define ALARMA_ENABLE 0x80

// For CONTROL_REGISTER_2
#define CLOCK_ENABLE 0x00
#define TIME_TYPE 0x20
#define ALARMA_FLG 0x02

enum rtc_errors{
	RTC_OK,
	RTC_ERROR
};

typedef uint8_t rtc_error;

typedef struct{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
}Time;

typedef struct{
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t weekday;
}Date;

typedef struct{
	I2C_HandleTypeDef *i2c;

	GPIO_TypeDef *rtc_int_port;
	uint16_t rtc_int_pin;

	uint8_t cr1_init_value;
	uint8_t cr2_init_value;

	Time time;
	Date date;
}Rtc;


rtc_error rtc_init(Rtc *rtc, I2C_HandleTypeDef *i2c, GPIO_TypeDef *rtc_int_port, uint16_t rtc_int_pin);

rtc_error set_time(Rtc *rtc, uint8_t second, uint8_t minute, uint8_t hour);
rtc_error set_date(Rtc *rtc, uint8_t weekday, uint8_t day, uint8_t month, uint8_t year);

rtc_error get_time(Rtc *rtc);
rtc_error get_date(Rtc *rtc);

rtc_error set_alarm_A(Rtc *rtc, uint8_t minute, uint8_t hour, uint8_t weekdays);
rtc_error reset_alarm_A(Rtc *rtc);
rtc_error disable_alarm_A(Rtc *rtc);

rtc_error get_alarm_pin(Rtc *rtc);

uint8_t  binary_to_bcd(uint8_t binary);
uint8_t  bcd_to_binary(uint8_t bcd);

#endif /* INC_BU9873_CONFIG_H_ */
