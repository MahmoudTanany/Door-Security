 /******************************************************************************
 *
 * Timer Driver H file
 *
 * File Name: Timer.h
 *
 * Description:the Full timer driver h file
 *
 * Author: Mahmoud Tanany
 *
 *******************************************************************************/

#ifndef TIMER2_H_
#define TIMER2_H_

#include "std_types.h"
#include "common_macros.h"
#include "micro_config.h"

#define number_of_overflows_per_second 4
#define Timer0_mode 2 //1 for normal mode , 2 for CTC mode
#define Timer1_mode 2 //1 for normal mode , 2 for CTC mode
#define Timer2_mode 2 //1 for normal mode , 2 for CTC mode

typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}timer_clock;


typedef struct
{
	timer_clock clock;
	uint16 intial_value;
	uint16 compare_value;

}timer_ConfigType;



void Timer0_init(const timer_ConfigType *config_ptr);
void Timer1_init(const timer_ConfigType *config_ptr);
void Timer2_init(const timer_ConfigType *config_ptr);

void Timer0_start(const timer_ConfigType *config_ptr , uint8 Num_Of_Sec);
void Timer1_start(const timer_ConfigType *config_ptr , uint8 Num_Of_Sec);
void Timer2_start(const timer_ConfigType *config_ptr , uint8 Num_Of_Sec);

void Timer0_stop(const timer_ConfigType *config_ptr);
void Timer1_stop(const timer_ConfigType *config_ptr);
void Timer2_stop(const timer_ConfigType *config_ptr);

void Timer0_setCallBack(void(*a_ptr)(void));
void Timer1_setCallBack(void(*a_ptr)(void));
void Timer2_setCallBack(void(*a_ptr)(void));

#endif /* TIMER2_H_ */
