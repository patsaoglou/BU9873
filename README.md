In this repository, i share a STM32 Firmware driver for the BU9873 RTC IC. I also supply the datasheet document provided by the manufacturer with notes i did that were important for the initialization of the RTC IC. You are also going to find instructions to use the code with a initialization example.

Note: if there is an issue with the code, let me know!

Firmware driver for the BU9873 RTC IC:
       - rtc_init()
       - set_time()
       - set_date()
       - get_time()
       - get_date()
       - set_alarm_A()
       - reset_alarm_A()
       - disable_alarm_A()
       - get_alarm_pin() (Alarm pin check in a polling mode)
       - binary_to_bcd()
       - bcd_to_binary()
 
Instructions:
 
1) Initialize RTC by passing the Rtc, I2C_HandleTypeDef and GPIO(PORT/PIN) struct addresses to rtc_init()
 
2) Pass Rtc struct address, second, minute and hour uint8_t values
   to set_time() and initialize IC Time registers
 
3) Pass Rtc struct address, day, month, year and weekday struct values
   to set_date() and initialize IC Date registers
 
4) Use get_time() to Write current time IC values to Time struct
   and access the values using the Rtc.Time.X where X is sec/min/hour values
 
5) Use get_date() to Write current date IC values to Date struct
   and access the values using the Rtc.Date.X where X is day/month/year/weekday values
 
6) Use set_alarm() to enable alarm and write desired time values to alarm IC registers
 
7) Use reset_alarm() to reset alarm FLG reg that triggers the interrupt pin
 
8) Use disable_alarm() to disable alarm completely
 
9) Use get_alarm_pin() to read interrupt pin (Use function in polling mode)
 
10) binary_to_bcd() and bcd_to_binary() are used for data conversion (IC needs BCD values)
 
NOTE: set_alarm_A() weekdays parameter is 7bit truth table like number from 0b0000000
(no alarm) to 0b1111111 (every day alarm).
 
Example: to set alarm for TUE and FRI, set weekday parameter to 0b0100100

Example Initialization:
       
       rtc_init(&rtc, &hi2c1, RTC_INT_GPIO_Port, RTC_INT_Pin);
    
       set_time(&rtc, 40, 59, 23);
       set_date(&rtc, 2, 1, 1, 1);
       set_alarm_A(&rtc, 0, 0, 0b001110);
    
       while (1)
       {    	  
    	   get_time(&rtc);
    	   get_date(&rtc);
    
    	   if (get_alarm_pin(&rtc) == 1){
    		   HAL_Delay(5000);
    		   reset_alarm_A(&rtc);
    	   }       
       }
        
      
    
