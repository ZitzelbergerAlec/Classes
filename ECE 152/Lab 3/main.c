/**
@file main.c
@brief Lab 3 Starter Code
@version .01 
@mainpage Lab 3 Starter Code

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
#include <stdlib.h>
#include <string.h>
#include "ff.h"
#include "diskio.h"

/** Constants */
#define F_CPU 1000000UL
#define DEBUG 1

/// Success error code
#define ERR_NONE 0x00

/// Failed to mount SDC/MMC error code
#define ERR_FMOUNT 0x01

/// No SDC/MMC present error code
#define ERR_NODISK 0x02

/// Unable to initialize FAT file system error code
#define ERR_NOINIT 0x03

/// MMC/SDC write protected error code
#define ERR_PROTECTED 0x04

/** Global Variables */

/** Functions */

/*START ADC HELPER FUNCTIONS*/


//#include "adc.h"

unsigned char read_adc(uint8_t channel){

	unsigned char test;

	ADMUX = 0x60 | channel; // Set the channel to the one we want
	ADCSRA = 0b11000110; // Start a new sample.
	while ((ADCSRA & 0b00010000) == 0 ); // Wait for a Valid Sample
	ADCSRA |= 0b00010000; // Tell ADC you have the sample you want.
	ADCSRA |= 0b01000000; // Start a new sample.
	while ((ADCSRA & 0b00010000) == 0 ); // Wait for a Valid Sample
	ADCSRA |= 0b00010000; // Tell ADC you have the sample you want.
	
	test = ADCH; 
	ADCSRA = 0x00; // Disable the ADC

	return (test);
}

/*END ADC HELPER FUNCTIONS*/

/** The clearArray() function turns off all LEDS on the Wunderboard array. It accepts no inputs and returns nothing*/
void clearArray(void)
{
	PORTC = 0x00;
	PORTB |= (1 << PB6) | (1 << PB7);		/** Enable latches*/
	PORTB &= ~((1 << PB6) | (1 << PB7));	/** Disable latches*/
}

/** The initialize() function initializes all of the Data Direction Registers for the Wunderboard. Before making changes to DDRx registers, ensure that you have read the peripherals section of the Wunderboard user guide.*/
void initialize(void)
{
	/** Port A is the switches and buttons. They should always be inputs. ( 0 = Input and 1 = Output )*/
	DDRA=0b00000000;

	/** Port B has the LED Array color control, SD card, and audio-out on it. Leave DDRB alone. ( 0 = Input and 1 = Output )*/
	DDRB=0b11000111;

	/** Port C is for the 'row' of the LED array. They should always be outputs. ( 0 = Input and 1 = Output )*/
	DDRC=0b11111111;

	/** Port D has the Serial on it. Leave DDRB alone. ( 0 = Input and 1 = Output )*/
	DDRD=0b00000000;

	/** Port E has the LED Array Column control out on it. Leave DDRE alone. ( 0 = Input and 1 = Output )*/
	DDRE=0b00000111;

	/** Port F has the accelerometer and audio-in on it. Leave DDRF alone. ( 0 = Input and 1 = Output )*/
	DDRF=0b00000000;
}

unsigned char GetByteUART(){
	return(UDR1);
}


/**
 * @brief Initializes the SDC/MMC and mounts the FAT filesystem, if it exists.
 * 
 * Initializes the connected SDC/MMC and mounts the FAT filesystem, if it
 * exists. Returns error code on failure.
 * 
 * @param fs Pointer to FATFS structure
 * @return Returns ERR_FMOUNT, ERR_NODISK, ERR_NOINIT, or ERR_PROTECTED on error, or ERR_NONE on success.
 */

 
 uint8_t initializeFAT(FATFS* fs)
{
	DSTATUS driveStatus;
	
	// Mount and verify disk type
	if (f_mount(0, fs) != FR_OK)
	{
		// Report error
		return ERR_FMOUNT;
	}
	
	driveStatus = disk_initialize(0);
	
	// Verify that disk exists
	if (driveStatus & STA_NODISK)
	{
		// Report error
		return ERR_NODISK;
	}
	
	// Verify that disk is initialized
	if (driveStatus & STA_NOINIT)
	{
		// Report error
		return ERR_NOINIT;
	}
	
	// Verify that disk is not write protected
	if (driveStatus & STA_PROTECT)
	{
		// Report error
		return ERR_PROTECTED;
	}
	
	return ERR_NONE;
}

#if DEBUG == 1
/** This function needs to setup the variables used by the UART to enable the UART and tramsmit at 9600bps. This 
function should always return 0. Remember, by defualt the Wunderboard runs at 1mHz for its system clock.*/
unsigned char initializeUART(void)
{
	/* Set baud rate */
	UBRR1H =0 ;
	UBRR1L =12 ;
	
	/* Set the U2X1 bit */
	UCSR1A = 0b00000010;
	
	/* Enable transmitter */
	UCSR1B = 00010000;
	
	/* Set frame format: 8data, 1stop bit */
	UCSR1C = 00000110;
}

/** This function needs to write a single byte to the UART. It must check that the UART is ready for a new byte 
and return a 1 if the byte was not sent. 
@param [in] data This is the data byte to be sent.
@return The function returns a 1 or error and 0 on successful completion.*/
unsigned char sendByteUART(uint8_t data)
{
}

/** This function needs to writes a string to the UART. It must check that the UART is ready for a new byte and 
return a 1 if the string was not sent. 
@param [in] str This is a pointer to the data to be sent.
@return The function returns a 1 or error and 0 on successful completion.*/
unsigned char sendStringUART(char* str)
{
}
#else
#define initializeUART()
#define sendByteUART( data)
#define sendStringUART(str)
#endif // DEBUG

/** This function needs to setup the variables used by TIMER0 Compare Match (CTC) mode with 
a base clock frequency of clk/1024. This function should return a 1 if it fails and a 0 if it 
does not. Remember, by default the Wunderboard runs at 1mHz for its system clock.
@return This function returns a 1 is unsuccessful, else return 0.*/
unsigned char initializeTIMER0(void)
{
	/* Set the CTC mode */
	TCCR0A = 0b00000010;
	
	/* Set the Clock Frequency */
	TCCR0B =0b00000101 ;
	
	/* Set initial count value */
	OCR0A = 0b00000000;
	return (0);
}

/** This function checks if TIMER0 has elapsed. 
@return This function should return a 1 if the timer has elapsed, else return 0*/
unsigned char checkTIMER0(void)
{
	if (TIFR0 & 1<<OCF0A){
		TIFR0 = TIFR0 | 1<<OCF0A; //Timer elapsed. Set the output compare flag to 1
		return (1);
	}
	return (0);
}

/** This function takes two values, clock and count. The value of count should be copied into OCR0A and the value of clock should be used to set CS02:0. The TCNT0 variable should also be reset to 0 so that the new timer rate starts from 0.  
@param [in] clock Insert Comment
@param [in] count Insert Comment
@return The function returns a 1 or error and 0 on successful completion.*/
unsigned char setTIMER0(unsigned char clock, unsigned char count)
{
	OCR0A = count;
	TCCR0B = TCCR0B | clock;
	TCNT0=0b00000000;
	
	return(0);
}

/** Main Function */
int main (void)
{
	/** Local Varibles */
	FATFS fs;
	FIL log;
	unsigned char recv;
	enum states {idle, running, sample, transmit} state;
	state = idle; //set initial state to idle
	
	initialize();
	
	// Initialize TIMER/COUNTER0
	initializeTIMER0();
	
	// Initialize file system, check for errors
	if(initializeFAT(&fs) == ERR_NONE){ 
	
		// Open file for writing, create the file if it does not exist, truncate existing data, check for errors
		f_open(&log, "/log.txt", FA_WRITE | FA_CREATE_ALWAYS);
		
		// Set TIMER/COUNTER0 period, check for errors
		setTIMER0(5, 255);
		PORTB=0b01000000;
		PORTC=0b11111111;
		
		// While switch A7 is on
		while (PINA & (1 << PA7))
		{
			switch(state){
				case idle:
					recv=GetByteUART();
					while(recv != 's')
						recv=GetByteUART();
					if(recv == 's')
						state = running;
					break;
				case running:
						while(recv != 's'){
							recv=GetByteUART();
							// If TIMER/COUNTER0 has elapsed (checkTimer)
							if(checkTIMER0()){
								state = sample;
								break;
							}
						}
						if(recv == 's')
							state = idle;
					break;
				case sample:
					// Read accelerometer data, write to file, check for errors (PORTF is the accelerometer)
					//read_adc(5); //5 is x, 6 is y
					f_write(&log, read_adc(5), 8, 8);
					state = transmit;
					break;
				case transmit:
					state = idle;
					break;
				default:
					state = idle;
					break;
			
			}
		}
		// Close the file and unmount the file system, check for errors
	} //else {//there were errors mounting the filesystem. do stuff
}
