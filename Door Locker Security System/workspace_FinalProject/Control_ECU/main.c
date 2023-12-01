/**************************************************************************************************
 Name        : Final_Project.c
 Author      : Amira Atef Ismaeil El Komy
 Description : Door Locker Security System - Control_ECU
 Date        : 3/11/2023
 **************************************************************************************************/
#include "uart.h"
#include "timer.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include "dc_motor.h"
#include <util/delay.h>
#include "avr/io.h"

uint8 i, op, password_one[5]={0}, password_two[5]={0};
uint16 pass_address= 0x0311;

/* global variable contain the ticks count of the timer */
uint8 g_tick = 0;

UART_ConfigType UART_Configs= {EIGHT, DISABLED, ONE, 9600};
Timer1_ConfigType TIMER1_Configs ={0,23437, F_CPU_1024, CTC};

void receivePass(uint8 *pass){
	for (i =0 ; i<5; i++)
		pass[i]=UART_recieveByte();
}

boolean areMatched(const uint8 *pass1, const uint8 *pass2){
	for (i =0 ; i<5; i++){
		if(pass1[i] != pass2[i])
			return FALSE;
	}
	return TRUE;
}

void storePass(void){
	if(areMatched(password_one, password_two)){
		UART_sendByte(0xFF);
		for(i = 0; i<5; i++){
			EEPROM_writeByte(pass_address+i,password_one[i]);
			_delay_ms(10);
		}
	}
	else
		UART_sendByte(0);
}


void status(void){
	g_tick++;
}

void Door(void){
	UART_sendByte(0xFF);
	Timer1_init(&TIMER1_Configs);
	DcMotor_Rotate(CLOCKWISE, 100);
	Timer1_setCallBack(status);
	while(g_tick < 11){
		if(g_tick == 5)
			DcMotor_Rotate(STOP, 0);
		else if(g_tick == 6){
			DcMotor_Rotate(ANTICLOCKWISE, 100);
		}
	}
	Timer1_deInit();
	DcMotor_Rotate(STOP, 0);
	g_tick =0;
}

void Start(void){
	receivePass(password_two);
	for(i =0; i<5; i++)
		EEPROM_readByte(pass_address+i, password_one + i);
}

void failure(void){
	TIMER1_Configs.compare_value =46874;
	Buzzer_on();
	Timer1_init(&TIMER1_Configs);
	Timer1_setCallBack(status);
	while(g_tick < 10);
	Timer1_deInit();
	Buzzer_off();
	g_tick =0;
	TIMER1_Configs.compare_value = 23437;
}
void openDoor(void){
	Start();
	if(areMatched(password_one, password_two)){
		Door();
	}
	else{
		UART_sendByte(0);
		Start();
		if(areMatched(password_one, password_two)){
			Door();
		}
		else{
			UART_sendByte(0);
			Start();
			if(areMatched(password_one, password_two)){
				Door();
			}
			else{
				UART_sendByte(0);
				failure();
			}
		}
	}
}


void storing(void){
	receivePass(password_one);
	receivePass(password_two);
	storePass();
	while(!areMatched(password_one, password_two)){
		receivePass(password_one);
		receivePass(password_two);
		storePass();
	}
}

void changePass(){
	Start();
	if(areMatched(password_one, password_two)){
		UART_sendByte(0xFF);
		storing();
	}
	else {
		UART_sendByte(0);
		Start();
		if(areMatched(password_one, password_two)){
			UART_sendByte(0xFF);
			storing();
		}
		else {
			UART_sendByte(0);
			Start();
			if(areMatched(password_one, password_two)){
				UART_sendByte(0xFF);
				storing();
			}
			else {
				UART_sendByte(0);
				failure();
			}
		}
	}
}

int main(void){
	UART_init(&UART_Configs);
	DcMotor_Init();
	Buzzer_init();
	storing();
	/* Enable global interrupt */
	SREG |= (1<<7);
	while(1){
		op= UART_recieveByte();
		if(op== '+')
			openDoor();
		else if(op== '-')
			changePass();
	}
	return 0;
}
