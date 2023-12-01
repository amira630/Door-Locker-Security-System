/******************************************************************************
 *
 * Module: DC-Motor
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the AVR DC-Motor driver
 *
 * Author: Amira Atef
 *
 *******************************************************************************/
#include "dc_motor.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "gpio.h"
#include "pwm.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * The Function responsible for setup the direction for the two
 * motor pins through the GPIO driver.
 * Stop at the DC-Motor at the beginning through the GPIO driver.
 */
void DcMotor_Init(void){
	GPIO_setupPinDirection(DC_MOTOR_INPUT1_PORT_ID, DC_MOTOR_INPUT1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_INPUT2_PORT_ID, DC_MOTOR_INPUT2_PIN_ID, PIN_OUTPUT);

	GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID, DC_MOTOR_INPUT1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID, DC_MOTOR_INPUT2_PIN_ID, LOGIC_LOW);
}

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or
 * stop the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the
 * required speed value.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	if(state == STOP){
		GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID, DC_MOTOR_INPUT1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID, DC_MOTOR_INPUT2_PIN_ID, LOGIC_LOW);
	}
	else if(state == CLOCKWISE){
		GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID, DC_MOTOR_INPUT1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID, DC_MOTOR_INPUT2_PIN_ID, LOGIC_HIGH);
	}
	else {
		GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID, DC_MOTOR_INPUT1_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID, DC_MOTOR_INPUT2_PIN_ID, LOGIC_LOW);
	}
	PWM_Timer0_Start( (uint16)(255 *speed)/100 );
}

