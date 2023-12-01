/**************************************************************************************************
 Name        : Final_Project.c
 Author      : Amira Atef Ismaeil El Komy
 Description : Door Locker Security System - HMI_ECU
 Date        : 3/11/2023
 **************************************************************************************************/
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer.h"
#include <util/delay.h>
#include "avr/io.h"

uint8 i, done =0, failed = 0, op =0, matched, password_one[5]={0}, password_two[5]={0}, pass[5]={0} ;

/* global variable contain the ticks count of the timer */
uint8 g_tick = 0;

UART_ConfigType UART_Configs= {EIGHT, DISABLED, ONE, 9600};

Timer1_ConfigType TIMER1_Configs ={0, 23437, F_CPU_1024, CTC};

void Status(void){
	g_tick++;
}

void openDoorLCD(void){
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 1,"Door is ");
		LCD_displayStringRowColumn(1, 1,"Unlocking ");
		Timer1_init(&TIMER1_Configs);
		Timer1_setCallBack(Status);
		while(g_tick < 5);
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 1,"Door is");
		LCD_displayStringRowColumn(1, 1,"Unlocked ");
		while(g_tick < 6);
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 1,"Door is");
		LCD_displayStringRowColumn(1, 1,"Locking ");
		while(g_tick < 11);
		Timer1_deInit();
		g_tick =0;
		done=1;
}

void errorMssg(void){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 1,"Error!");
}
void failure(void){
	TIMER1_Configs.compare_value = 46874;
	errorMssg();
	Timer1_init(&TIMER1_Configs);
	Timer1_setCallBack(Status);
	while(g_tick <10);
	Timer1_deInit();
	g_tick = 0;
	failed =1;
	TIMER1_Configs.compare_value = 23437;
}

void enterPass(uint8 *pass){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0,"Plz enter pass:");
	LCD_moveCursor(1,1);
	for(i=0; i<5; i++){
		pass[i] = KEYPAD_getPressedKey();
		while(pass[i]> 9 || pass[i] < 0){
			_delay_ms(500);
			pass[i] = KEYPAD_getPressedKey();
		}
		LCD_displayCharacter('*');
		_delay_ms(500);
	}
	while(KEYPAD_getPressedKey() != '=');
	for(i=0; i<5; i++){
		UART_sendByte(pass[i]);
	}
}

void reenterPass(uint8 *pass){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0,"Plz re-enter the");
	LCD_displayStringRowColumn(1, 0,"same pass: ");
	for(i=0; i<5; i++){
		pass[i] = KEYPAD_getPressedKey();
		while(pass[i]> 9 || pass[i] < 0){
			_delay_ms(500);
			pass[i] = KEYPAD_getPressedKey();
		}
		LCD_displayCharacter('*');
		_delay_ms(500);
	}
	while(KEYPAD_getPressedKey() != '=');
	for(i=0; i<5; i++){
		UART_sendByte(pass[i]);
	}
}



void passSet(void){
	matched =0;
	while(matched != 0xFF){
		enterPass(password_one);
		reenterPass(password_two);
		matched=UART_receiveByte();
	}
}
void changePass(void){
	UART_sendByte('-');
	enterPass(pass);
	matched = UART_receiveByte();
	if(matched == 0xFF){
		passSet();
		done =1;
	}
	else {
		enterPass(pass);
		matched = UART_receiveByte();
		if(matched == 0xFF){
			passSet();
			done =1;
		}
		else{
			enterPass(pass);
			matched = UART_receiveByte();
			if(matched == 0xFF){
				passSet();
				done =1;
			}
			else{
				failure();
			}
		}
	}
}

void openDoor(void){
	UART_sendByte('+');
	enterPass(pass);
	matched = UART_receiveByte();
	if(matched == 0xFF)
		openDoorLCD();
	else{
		enterPass(pass);
		matched = UART_receiveByte();
		if(matched == 0xFF)
			openDoorLCD();
		else{
			enterPass(pass);
			matched = UART_receiveByte();
			if(matched == 0xFF)
				openDoorLCD();
			else
				failure();
		}
	}
}
void mainPage(void){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 1,"+ : Open Door");
	LCD_displayStringRowColumn(1, 1,"- : Change Pass");
	while(op != '+' && op != '-'){
		op = KEYPAD_getPressedKey();
		_delay_ms(200);
	}
		if(op == '+'){
			op=0;
			openDoor();
		}
		else if(op == '-'){
			op=0;
			changePass();
		}
}



int main(void){
	UART_init(&UART_Configs);
	LCD_init();
	passSet();
	/* Enable global interrupt */
	SREG |= (1<<7);
	while(1){
		mainPage();

		if(failed == 1){
			failed =0;
			continue;
		}
		if (done == 1){
			done =0;
			continue;
		}
	}
	return 0;
}

