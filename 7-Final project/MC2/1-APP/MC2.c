 /******************************************************************************
 *
 * MC2 application file
 *
 * File Name: MC2.c
 *
 * Description:the application file for MC2
 *
 * Author: Mahmoud Tanany
 *
 *******************************************************************************/
#include "micro_config.h"
#include "common_macros.h"
#include "timer.h"
#include "external_eeprom.h"
#include "uart.h"
#include "std_types.h"
#include "i2c.h"
#define F_CPU 8000000UL

#define READY 0x10
#define MATCHED 0x01
#define UNMATCHED 0x00
#define LOGIN 0x11
#define CHANGE 0x22
#define NEW 0x33

uint8 save;
uint16 x,y=0;
uint8 pass[3];
uint8 repass[3];

void matched(void);
void unmatched(void);
void recievepass(void);
void checkpass(void);
void login (void);
void Motorstop(void);
void Motorstart(void);
void buzzerstart(void);
void buzzerstop(void);
void motortoggle(void);
void motorreverse(void);
timer_ConfigType g_config={F_CPU_64,0,255};



void main(void)
{
	//configure the output pins for the buzzer and motor
	SET_BIT(DDRC,2);
	SET_BIT(DDRC,3);
	SET_BIT(DDRA,0);

	UART_init();
	EEPROM_init();

//while MC2 not recieved matched from MC1 it will recieve and check another passwords
	do{
		recievepass();
		checkpass();
	}
	//when the user enters 2 matched passwords the password will be inserted in the EEPROM
	while(UART_recieveByte()==UNMATCHED);
	EEPROM_writeArray(0, pass, 4);


	while (1)
	{
		//while this MC is connected it will wait in this switch case for MC1
		//to send the route that will help his MC to by sync. with MC1
		switch(UART_recieveByte())
		{
		//if recieved login from MC1 this MC will receive a password and compare it with the password inserted in EEPROM
		//if the password matched it will send MATCHED to MC1
		case LOGIN:
		login();
		EEPROM_readArray(0, repass, 4);
		checkpass();
		if(UART_recieveByte()==MATCHED)
		{
			//if the password is matched the door will open for 15 seconds
			Motorstart();
			Timer0_init(&g_config);
			sei();
			Timer0_start(&g_config, 15);
			Timer0_setCallBack(Motorstop);

			motorreverse();
			Timer2_init(&g_config);
			sei();
			Timer2_start(&g_config, 15);
			Timer2_setCallBack(motortoggle);
		}
		else
		{
			//if the password is not matched it will try to take it 3 times from the user if the 3 times are not MATCHED it will trigger buzzer
			login();
					EEPROM_readArray(0, repass, 4);
					checkpass();
					if(UART_recieveByte()==MATCHED)
					{
						sei();
						Motorstart();
						Timer0_init(&g_config);
						Timer0_start(&g_config, 15);
						Timer0_setCallBack(Motorstop);
						_delay_ms(2000);

						motorreverse();
						Timer2_init(&g_config);
						sei();
						Timer2_start(&g_config, 15);
						Timer2_setCallBack(motortoggle);
					}
					else
					{
						login();
								EEPROM_readArray(0, repass, 4);
								checkpass();
								if(UART_recieveByte()==MATCHED)
								{
									sei();
									Motorstart();
									Timer0_init(&g_config);
									Timer0_start(&g_config, 15);
									Timer0_setCallBack(Motorstop);
									motorreverse();
									sei();
									Timer2_init(&g_config);
									Timer2_start(&g_config, 15);
									Timer2_setCallBack(motortoggle);


								}
								else{

									buzzerstart();
									Timer2_init(&g_config);
									Timer2_start(&g_config, 20);
									Timer2_setCallBack(buzzerstop);

								}
					}
		}

		break;
		//the second case is the user wants to change the password this MC will receive
		//the old password to check if it is like the one insetrted in the EEPROM or nor then
		//receive the new password 2 times
		case CHANGE:
			login();
			EEPROM_readArray(0, repass, 4);
			checkpass();
			break;

		case NEW:
			recievepass();
			checkpass();
			if(UART_recieveByte()==MATCHED)
			{
				EEPROM_writeArray(0, pass, 4);
			}
			else
			{
				login();
				EEPROM_readArray(0, repass, 4);
				checkpass();
			}
			break;

	}
	}
}
//this function is to send matched byte to MC1
void matched(void)
{
	UART_sendByte(MATCHED);
}
//this function is to send unmatched byte to MC1
void unmatched(void)
{
	UART_sendByte(UNMATCHED);
}
//this function is to recieve a password for two times from MC1 in configuration and when needed to change the password
void recievepass(void)
{
	_delay_ms(50);
	for(x=0;x<=3;)
				{
					pass[x]=UART_recieveByte();
					x++;
				}

				for(y=0;y<=3;)
				{
					repass[y]=UART_recieveByte();
					y++;
				}
}
//this function checks if the 2 passwords are the same or not
void checkpass(void)
{
	if(pass[0]==repass[0] && pass[1]==repass[1] && pass[2]==repass[2] && pass[3]==repass[3] )
		{
			matched();
		}
		else
		{
			unmatched();
		}
}
//this function is to receive only one password from MC1
void login (void)
{
for(x=0;x<=3;)
			{
				pass[x]=UART_recieveByte();
				x++;
			}
}
//the following 4 function is to stop and start the door openning and buzzer start and stop
void Motorstop(void)
{
	Timer0_stop(&g_config);
	cli();
	CLEAR_BIT(PORTC,2);

}
void Motorstart(void)
{
	SET_BIT(PORTC,2);
}
void buzzerstart(void)
{
	SET_BIT(PORTA,0);
}
void buzzerstop(void)
{
	Timer2_stop(&g_config);
	CLEAR_BIT(PORTA,0);
}
void motortoggle(void)
{
	Timer2_stop(&g_config);
	CLEAR_BIT(PORTC,3);
	cli();
}
void motorreverse(void)
{
	SET_BIT(PORTC,3);
}
