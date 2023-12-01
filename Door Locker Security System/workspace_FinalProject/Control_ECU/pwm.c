/******************************************************************************
 *
 * Module: PWM
 *
 * File Name: pwm.c
 *
 * Description: Source file for the AVR PWM driver
 *
 * Author: Amira Atef
 *
 *******************************************************************************/
#include "pwm.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "avr/io.h" /* To use the IO Ports Registers */
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * The function responsible for trigger the Timer0 with the PWM Mode.
 * Setup the PWM mode with Non-Inverting.
 * Setup the prescaler with F_CPU/8.
 * Setup the compare value based on the required input duty cycle
 * Setup the direction for OC0 as output pin through the GPIO driver.
 * The generated PWM signal frequency will be 500Hz to control the DC Motor speed.
 */
void PWM_Timer0_Start(uint8 duty_cycle){
	TCNT0= 0;
	OCR0 = duty_cycle ;
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT);
	TCCR0 = (1<< WGM00) | (1<<WGM01)| (1<<COM01)|(1<<CS01);
}
