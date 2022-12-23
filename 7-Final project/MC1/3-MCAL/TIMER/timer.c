 /******************************************************************************
 *
 * Timer Driver C file
 *
 * File Name: Timer.c
 *
 * Description:the Full timer driver C file
 *
 * Author: Mahmoud Tanany
 *
 *******************************************************************************/

#include "timer.h"
uint8 g_counter_Timer0=0 , g_seconds_Timer0 ,
g_counter_Timer1=0 , g_seconds_Timer1 ,
g_counter_Timer2=0 , g_seconds_Timer2;

static volatile void (*g_callBackPtr0)(void) = NULL_PTR;//pointer to hold the address of the callback function
static volatile void (*g_callBackPtr1)(void) = NULL_PTR;//pointer to hold the address of the callback function
static volatile void (*g_callBackPtr2)(void) = NULL_PTR;//pointer to hold the address of the callback function

//CallBack functions
void Timer0_setCallBack(void(*a_ptr)(void))
{

	g_callBackPtr0 = a_ptr;
}
void Timer1_setCallBack(void(*a_ptr)(void))
{

	g_callBackPtr1 = a_ptr;
}
void Timer2_setCallBack(void(*a_ptr)(void))
{

	g_callBackPtr2 = a_ptr;
}



//TIMER0 FUNCTIONS


//Initialization
void Timer0_init(const timer_ConfigType *config_ptr){

TCNT0=config_ptr->intial_value;

#if (Timer0_mode==1)
TCCR0&=~(1<<WGM01)&(1<<WGM00)&(1<<COM01)&(1<<COM00);//Normal Mode
TCCR0|=(1<<FOC0);//Non-PWM
TIMSK |= (1<<TOIE0);//Enable overflow intrrupt


#elif(Timer0_mode==2)
TCCR0&=~(1<<WGM00)&(1<<COM01)&(1<<COM00);//Compare Mode
TCCR0|=(1<<FOC0)|(1<<WGM01);//Non-PWM(FOC0=1)
OCR0=config_ptr->compare_value;//Compare Value
TIMSK |= (1<<OCIE0);

#endif

}

//Timer starting
void Timer0_start(const timer_ConfigType *config_ptr , uint8 Num_Of_Sec){
	g_seconds_Timer0=(Num_Of_Sec*4);
	TCCR0|=(TCCR0 & 0xF8) | (config_ptr->clock);//Starting the clock


}


//Timer Stopping
void Timer0_stop(const timer_ConfigType *config_ptr){

	TCCR0&=0xF8;//Stopping the clock



}
//Timer ISR
#if(Timer0_mode==1)
ISR(TIMER0_OVF_vect){
	if(g_callBackPtr0 != NULL_PTR)
		{

			(*g_callBackPtr0)();
		}
}
#else
ISR(TIMER0_COMP_vect){

	g_counter_Timer0++;
	if(g_counter_Timer0==g_seconds_Timer0){
	if(g_callBackPtr0 != NULL_PTR)
		{

			(*g_callBackPtr0)();
		}
	g_counter_Timer0=0;}
}
#endif
//TIMER1 FUNCTIONS
//Initialization

void Timer1_init(const timer_ConfigType *config_ptr){

TCNT1=config_ptr->intial_value;

#if(Timer1_mode==1)
TCCR1A=0x0C; //Normal mode //non-PWM
TCCR1B=~(1<<WGM13)&(1<<WGM12);//Normal mode//
TIMSK|=(1<<TOIE1);//Overflow interrupt enable

#elif(Timer1_mode==2)
TCCR1A=0x0C;//
TCCR1B&=~(1<<WGM13);
TCCR1B|=(1<<WGM12);//compare mode
TIMSK|=(1<<OCIE1A);//Compare interrupt enable
OCR1A=config_ptr->compare_value;//Compare value


#endif

}
//Timer starting
void Timer1_start(const timer_ConfigType *config_ptr , uint8 Num_Of_Sec){

	TCCR1B|=(TCCR1B & 0xF8) | (config_ptr->clock);// Start clock
}
//Timer stopping
void Timer1_stop(const timer_ConfigType *config_ptr){
	TCCR1B&=0xF8;//Stop clock
}
//Timer ISR
#if(Timer0_mode==1)
ISR(TIMER1_OVF_vect){
if(g_callBackPtr1 != NULL_PTR)
{     (*g_callBackPtr1)();
}
}

#else
ISR(TIMER1_COMPA_vect){
	g_counter_Timer1++;
		if(g_counter_Timer1==g_seconds_Timer1){
		if(g_callBackPtr1 != NULL_PTR)
			{

				(*g_callBackPtr1)();
			}
		g_counter_Timer1=0;}
	}
#endif

//TIMER2 FUNCTIONS
//Initialization

void Timer2_init(const timer_ConfigType *config_ptr){

TCNT2=config_ptr->intial_value;

#if (Timer2_mode==1)
TCCR2&=~(1<<WGM21)&(1<<WGM20)&(1<<COM21)&(1<<COM20);//Normal Mode
TCCR2|=(1<<FOC2);//Non-PWM
TIMSK |= (1<<TOIE2);//Enable overflow interrupt

#elif(Timer2_mode==2)
TCCR2&=~(1<<WGM20)&(1<<COM21)&(1<<COM20);//Compare Mode
TCCR2|=(1<<FOC0)|(1<<WGM21);
OCR2=config_ptr->compare_value;//Compare Value
TIMSK |= (1<<OCIE2);//Enable compare interrupt

#endif


}
//Timer Starting
void Timer2_start(const timer_ConfigType *config_ptr , uint8 Num_Of_Sec){
	g_seconds_Timer1=(Num_Of_Sec*4);
	TCCR2|=(TCCR1B & 0xF8) | (config_ptr->clock);


}
//Timer Stopping
void Timer2_stop(const timer_ConfigType *config_ptr){

	TCCR2=0xF8;//Stop clock

}
//Timer ISR
#if(Timer0_mode==1)
ISR(TIMER2_OVF_vect){
if(g_callBackPtr2 != NULL_PTR)
{

(*g_callBackPtr2)();
}
}
#else
ISR(TIMER2_COMP_vect){
	g_counter_Timer2++;
		if(g_counter_Timer2==g_seconds_Timer2){
		if(g_callBackPtr2 != NULL_PTR)
			{

				(*g_callBackPtr2)();
			}
		g_counter_Timer2=0;}
	}

#endif
