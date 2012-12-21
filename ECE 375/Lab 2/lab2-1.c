/*
This code will cause a TekBot connected to the AVR board to
move forward and when it touches an obsticle, it will reverse
and turn away from the obsticle and resume forward motion.

PORT MAP
Port B, Pin 4 -> Output -> Right Motor Enable
Port B, Pin 5 -> Output -> Right Motor Direction
Port B, Pin 7 -> Output -> Left Motor Enable
Port B, Pin 6 -> Output -> Left Motor Direction
Port D, Pin 1 -> Input -> Left Whisker
Port D, Pin 0 -> Input -> Right Whisker
*/

#define F_CPU 16000000
#include <avr/io.h> 
#include <util/delay.h> 
#include <stdio.h>

int main(void)
{

DDRB =0b11110000;	//Setup Port B for Input/Output
PORTB=0b11110000;	//Turn off both motors

while (1)	       					// Loop Forever
      {
	      PORTB=0b01100000;	//Move forward
		  if(!(PIND & 0b00000010)){ //Left whisker triggered
				_delay_ms(500);
				PORTB=0b00000000;	//Reverse
				_delay_ms(500);
				PORTB=0b00100000; //Turn Left
      			_delay_ms(1000);
		  } else if(!(PIND & 0b00000001)){ //Right whisker triggered 
				_delay_ms(500);
				PORTB=0b00000000;	//Reverse
				_delay_ms(500);
				PORTB=0b01000000; //Turn Right
      			_delay_ms(1000);
		  } else if(!(PIND & 0b00000011)){ //Both whiskers triggered 
				_delay_ms(500);
				PORTB=0b00000000;	//Reverse
				_delay_ms(500);
				PORTB=0b01000000; //Turn Right
      			_delay_ms(1000);		
		  }
      };
}

