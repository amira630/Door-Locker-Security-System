 /******************************************************************************
 *
 * Module: TIMER
 *
 * File Name: timer.c
 *
 * Description: Source file for the TIMER1 driver
 *
 * Author: Amira Atef
 *
 *******************************************************************************/

#include "timer.h"
#include "avr/io.h" /* To use the TIMER1 Registers */
#include <avr/interrupt.h> /* For TIMER1 ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

/* Interrupt Service Routine for timer1 normal mode */
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
		{
			/* Call the Call Back function in the application after the edge is detected */
			(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}
}

/* Interrupt Service Routine for timer1 compare mode */
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Function to initialise the Timer driver
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	TCNT1 = Config_Ptr->initial_value;		/* Set timer1 initial count */

	if(Config_Ptr->mode == CTC){
		OCR1A = Config_Ptr->compare_value;    /* Set the Compare value */

		/* Configure timer control register TCCR1A
		 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
		 * 2. FOC1A=1 FOC1B=0
		 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4)
		 */
		TCCR1A = (1<<FOC1A);
		/* Configure timer control register TCCR1B
		 * 1. CTC Mode WGM12=1 WGM13=0 (Mode Number 4)
		 * 2. Prescaler = F_CPU/?
		 */
		TCCR1B = (1<<WGM12) |(Config_Ptr->prescaler);
		TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */
	}
	else if(Config_Ptr->mode == NORMAL){
		TCCR1B |= (Config_Ptr->prescaler);/* The Prescaler */
		TIMSK |= (1<<TOIE1); /* Enable Timer1 Overflow Interrupt */
	}
}

/*
 * Description :
 * Function to disable the Timer1.
 */
void Timer1_deInit(void){
	/* Clear All Timer1/ICU Registers */
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
    TIMSK &= ~(1 << TOIE1);  /* Disable Timer1 overflow interrupt */
    TIMSK &= ~(1 << OCIE1A);  /* Disable Timer1 compare match interrupt */

    /* Reset the global pointer value */
    g_callBackPtr = NULL_PTR;
}

/*
 * Description :
 * Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void)){
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}
