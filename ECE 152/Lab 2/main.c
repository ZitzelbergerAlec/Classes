David Merrick
10/12/2011
ECE 152 LAB 2

/**
@file main.c
@brief Lab 2 Starter Code
@version .01 
@mainpage Lab 2 Starter Code

@section intro Code Overview

@section hw Hardware Pin Out
Port A:
A0 - A3 : Push Buttons
A4 - A7 : Slide Switches 

Port B:
B0 - B3 : SPI (SD Card)
B4		: Nothing
B5		: Audio Out
B6 		: Red Enable 
B7 		: Green Enable

Port C:
C0 - C7 : LED Array (Row)

Port D:
D0 - D1 : Nothing
D2		: Serial RX
D3		: Serial TX
D4 - D7	: Nothing

Port E:
E0 - E2 : LED Array (Column)
E3		: USB (UID)
E4 - E5	: Nothing
E6		: Relay
E7		: Nothing

Port F:
F0 		: ADC Channel 0
F1 		: ADC Channel 1
F2 		: ADC Channel 2
F3 		: ADC Channel 3
F4		: ADC Channel 4 (Audio In)
F5 		: ADC Channel 5 (Accel X Axis)
F6 		: ADC Channel 6 (Accel Y Axis)
F7 		: ADC Channel 7 (Accel Z Axis (if installed))

*/

/** Includes */
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

/** Constants */
#define F_CPU 1000000U
#define DEBUG 1
#ifdef DEBUG
#endif

/** Global Variables */

/** Functions */

/** The initialize() function initializes all of the Data Direction Registers for the Wunderboard. Before making changes to DDRx registers, ensure that you have read the peripherals section of the Wunderboard user guide.*/
void initialize (void) {
	/** Port A is the switches and buttons. They should always be inputs. ( 0 = Input and 1 = Output )*/
	DDRA=0b00000000;

	/** Port B has the LED Array color control, SD card, and audio-out on it. Leave DDRB alone. ( 0 = Input and 1 = Output )*/
	DDRB=0b11000000;

	/** Port C is for the 'row' of the LED array. They should always be outputs. ( 0 = Input and 1 = Output )*/
	DDRC=0b11111111;

	/** Port D has the Serial on it. Leave DDRB alone. ( 0 = Input and 1 = Output )*/
	DDRD=0b00000000;

	/** Port E has the LED Array Column control out on it. Leave DDRE alone. ( 0 = Input and 1 = Output )*/
	DDRE=0b00000111;

	/** Port F has the accelerometer and audio-in on it. Leave DDRF alone. ( 0 = Input and 1 = Output )*/
	DDRF=0b00000000;
}

/** The clearArray() function turns off all LEDS on the Wunderboard array. It accepts no inputs and returns nothing*/
void clearArray(void)
{
	PORTC = 0x00;
	PORTB |= (1 << PB6) | (1 << PB7);		/** Enable latches*/
	PORTB &= ~((1 << PB6) | (1 << PB7));	/** Disable latches*/
}
#ifdef DEBUG
/** This function needs to setup the variables used by the UART to enable the UART and tramsmit at 9600bps. This 
function should always return 0. Remember, by defualt the Wunderboard runs at 1mHz for its system clock.*/
unsigned char InitializeUART(){

	/* Set baud rate */
	UBRR1H = 0b00000000;
	UBRR1L = 0b00001100;
	/* Set the U2X1 bit */
	UCSR1A = 0b00000010;
	/* Enable transmitter */
	UCSR1B = 0b00001000;
	/* Set frame format: 8data, 1stop bit */
	UCSR1C = 0b00000110;
	if(UBRR1H == 0 && UBRR1L == 0b00001100  && UCSR1A == 0b00000010 && UCSR1B == 0b00001000 && UCSR1C == 0b00000110){
		return 0;
	}
	else{
		return 1;
	}

}
#endif
#ifdef DEBUG
/** This function needs to write a single byte to the UART. It must check that the UART is ready for a new byte 
and return a 1 if the byte was not sent. 
@param [in] data This is the data byte to be sent.
@return The function returns a 1 or error and 0 on successful completion.*/

unsigned char SendByteUART (unsigned char data){
	if(UCSR1A & (1<<UDRE1)){ //If the UDR1 register is empty, send the data
		UDR1=data;
		return 0;
	}
	else
	{
		return 1; //UDR1 is not empty, so did not send the data. Return 1
	}


}
#endif

/** This function needs to writes a string to the UART. It must check that the UART is ready for a new byte and 
return a 1 if the string was not sent. 
@param [in] data This is a pointer to the data to be sent.
@return The function returns a 1 or error and 0 on successful completion.*/
#ifdef DEBUG
unsigned char SendStringUART(unsigned char *data){	
			char x = strlen(data);
			int i;
			for(i=0;i<x;i++){
				while(SendByteUART(data[i])); //If the SendByteUART returns 1, meaning it did not send the data, run the loop again until successful	
			}
			return 0;
		
}
		

#endif
	/** Main Function */

int main (void) {	
	
	/** Local Varibles */	
	unsigned char buf[20];
		initialize();
		clearArray();
		#ifdef DEBUG
		InitializeUART();
		#endif
				
		PORTB=0b01000000; //red
		PORTC=0b11111111;
			
		while(1);
	}
