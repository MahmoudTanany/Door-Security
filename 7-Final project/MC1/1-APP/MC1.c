 /******************************************************************************
 *
 * MC1 application file
 *
 * File Name: MC1.c
 *
 * Description:the application file for MC1
 *
 * Author: Mahmoud Tanany
 *
 *******************************************************************************/

#include "common_macros.h"
#include "micro_config.h"
#include "std_types.h"
#include "uart.h"
#include "timer.h"
#include "keypad.h"
#include "lcd.h"
#define F_CPU 8000000UL


#define MATCHED 0x01
#define UNMATCHED 0x00
#define LOGIN 0x11
#define CHANGE 0x22
#define NEW 0x33

uint8 i;
uint8 pass[3];
uint8 z;
uint8 repass[3];
uint8 choose[0];
timer_ConfigType g_config={F_CPU_64,0,255};



void login(void);
void signup(void);
void passconfig(void);
void start(void);
void close(void)
{

	Timer0_stop(&g_config);
	cli();
}




void main(void)
{
	//initializing UART
	UART_init();
	//initializing LCD
	LCD_init();
	//calling signup function to configure the password
	signup();
	//clearing the LCD
	LCD_clearScreen();
	//MC2 should send back after receiving the 2 entered passwords
	// if it received UNMATCHED will show you that not matched and send again to MC2 to be ready to recieve another two passwords
	while(UART_recieveByte()==UNMATCHED)
			{
				LCD_clearScreen();
				LCD_displayString("NOT MATCHED");
				LCD_clearScreen();
				UART_sendByte(UNMATCHED);
				signup();
			}
	//sending MATCHED to MC1 to be ready to move from configuring the password and be ready to login or to change password commands
	UART_sendByte(MATCHED);

while(1)
	{
	//while this MC is connected after configuration it will stay at the start function that will be described below
		start();
	}

}
//this function is to request a password from the user for opening the door
//this function send the password entered by the user to MC2
//this function when called will wait a MATCHED or UNMATCHED bytes from MC1 according to the password in the EEPROM
//if this MC received MATCHED it will display opening
//if received unmatched this function will request the password again
 void login(void)
{
	 LCD_clearScreen();//clearing LCD
	LCD_displayStringRowColumn(0, 1, "Enter Password");//requesting the password through LCD
	_delay_ms(50);
	LCD_goToRowColumn(1, 0);//moving the cursor of LCD to the next line

	for(i=0;i<=3;i++)//loop to recieve and send the password and to show stars instead of the numbers
		{
			pass[i]=KeyPad_getPressedKey();
			if((pass[i] >= 0) && (pass[i] <= 9))
				{
					_delay_ms(50);
					LCD_displayString("*");
					_delay_ms(50);
					UART_sendByte(pass[i]);
				}

		 }//now it sent the password entered and waiting to recieve MATCHED or UNMATCHED
		if(UART_recieveByte()==MATCHED)
			{
			//sending MATCHED to MC1 to move to the next step
				UART_sendByte(MATCHED);
				LCD_clearScreen();
				sei();
				Timer0_init(&g_config);
				Timer0_start(&g_config, 15);
				LCD_displayString("opening");
				_delay_ms(100);
				LCD_clearScreen();
				Timer0_setCallBack(close);


			}
			else
				{
				//if not it will send to MC1 to receive another password
					UART_sendByte(UNMATCHED);
					LCD_clearScreen();
					LCD_displayString("Wrong pass");
					login();

				}

}
 //this function is used when configuring the password, sending it to MC1
 //so it take from the user 2 passwords and sending them to MC1 to check
 void signup(void)
 {

	 LCD_displayStringRowColumn(0, 1, "Enter Password");
	 _delay_ms(50);
	 LCD_goToRowColumn(1, 0);

	 for(i=0;i<=3;i++)
	 {
	 	pass[i]=KeyPad_getPressedKey();
	 		if((pass[i] >= 0) && (pass[i] <= 9))
	 		{
	 			UART_sendByte(pass[i]);
	 			_delay_ms(50);
	 			LCD_displayString("*");


	 		}
	 }

	 _delay_ms(50);
	 LCD_clearScreen();
	 _delay_ms(100);

	 LCD_displayStringRowColumn(0, 1, "ReEnter Password");
	 LCD_goToRowColumn(1, 0);

	 for (z=0;z<=3;)
	 	{
	 		repass[z]=KeyPad_getPressedKey();
	 		_delay_ms(50);
	 			if((repass[z] >= 0) && (repass[z] <= 9))
	 				{
	 					_delay_ms(50);
	 					LCD_displayString("*");
	 					_delay_ms(50);
	 					UART_sendByte(repass[z]);

	 				}
	 			else{
	 					 _delay_ms(50);
	 					LCD_clearScreen();
	 					_delay_ms(50);
	 //							 passconfig();
	 				}
	 			z++;

	 		}
 }
//this function is to reconfigure a new password so it take the old password and 2 new passwords by calling another function which is signup
 //it will send each password (old and 2 new) to MC1 and wait to check and send back a response with MATCHED or UNMATCHED
void passconfig(void)
{
	LCD_displayStringRowColumn(0, 1, "enter old Pass");
		LCD_goToRowColumn(1, 0);

		for (i=0;i<=3;)
				{
					pass[i]=KeyPad_getPressedKey();
					_delay_ms(50);
					if((pass[i] >= 0) && (pass[i] <= 9))
						 {
							_delay_ms(50);
							LCD_displayString("*");
							_delay_ms(50);
							UART_sendByte(pass[i]);

						  }
					else
						  	  {
								 _delay_ms(50);
								 LCD_clearScreen();
								_delay_ms(50);
							   }
					i++;

				}

				_delay_ms(50);
				LCD_clearScreen();
				_delay_ms(100);
				if(UART_recieveByte()==MATCHED)
				{
					UART_sendByte(NEW);
					signup();
					if(UART_recieveByte()==MATCHED)
					{
						UART_sendByte(MATCHED);
						LCD_displayString("newpassdone");
					}
					else
					{
						UART_sendByte(UNMATCHED);
						passconfig();
					}
				}
				else
				{
					UART_sendByte(CHANGE);
					passconfig();
				}
}
//this function is the starting menu the user will choose = or + according to what he want
//if the  user pressed = it will go to the login function that will make the user enter a passwrod ,sending it to MC1 to check
//if user pressed + it will go to reconfiguring function that the user will enter old and new passwords

void start(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 1, "+ change pass");
		_delay_ms(50);
		LCD_goToRowColumn(1, 0);
		_delay_ms(50);
		LCD_displayStringRowColumn(1, 0, "= log in");
		_delay_ms(50);
		choose[0]=KeyPad_getPressedKey();



		if(choose[0]=='=')
			{
				LCD_clearScreen();
				_delay_ms(50);
				UART_sendByte(LOGIN);
				login();
			}
		else if(choose[0]=='+')
			{
				LCD_clearScreen();
				_delay_ms(50);
				UART_sendByte(CHANGE);
				passconfig();

			}

}
