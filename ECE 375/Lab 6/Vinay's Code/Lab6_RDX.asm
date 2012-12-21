/*
 * Lab6_RDX.asm
 *
 *  Created: 11/11/2012 5:18:04 PM
 *   Author: Vinay
 */ 


 .include "m128def.inc"			; Include definition file

;***********************************************************
;*	Internal Register Definitions and Constants
;***********************************************************
.def	mpr = r16				; Multi-Purpose Register
.def	waitcnt = r17
.def	ilcnt	= r18
.def	olcnt	= r19
.def	flag	= r20
.def	command = r21
.def	freezcnt = r22

.equ	WTime = 100
.equ	WskrR = 4				; Right Whisker Input Bit
.equ	WskrL = 5				; Left Whisker Input Bit
.equ	EngEnR = 4				; Right Engine Enable Bit
.equ	EngEnL = 7				; Left Engine Enable Bit
.equ	EngDirR = 5				; Right Engine Direction Bit
.equ	EngDirL = 6				; Left Engine Direction Bit

.equ	BotID = 0b00000010		;(Enter you group ID here (8bits)); Unique XD ID (MSB = 0)

;/////////////////////////////////////////////////////////////
;These macros are the values to make the TekBot Move.
;/////////////////////////////////////////////////////////////

.equ	MovFwd =  (1<<EngDirR|1<<EngDirL)	;0b01100000 Move Forwards Command
.equ	MovBck =  $00						;0b00000000 Move Backwards Command
.equ	TurnR =   (1<<EngDirL)				;0b01000000 Turn Right Command
.equ	TurnL =   (1<<EngDirR)				;0b00100000 Turn Left Command
.equ	Halt =    (1<<EngEnR|1<<EngEnL)		;0b10010000 Halt Command
.equ	FreezR =   ($80|$F8)
.equ	FreezT	=	0b01010101

;***********************************************************
;*	Start of Code Segment
;***********************************************************
.cseg							; Beginning of code segment

;-----------------------------------------------------------
; Interrupt Vectors
;-----------------------------------------------------------
.org	$0000					; Beginning of IVs
		rjmp 	INIT			; Reset interrupt

;Should have Interrupt vectors for:
;- Left wisker
.org	$0002	;INT0
	rcall	HitLeft
	reti

;- Right wisker
.org	$0004	;INT1
	rcall	HitRight
	reti

;- Left wisker
.org	$000A	;INT4
	rcall	HitLeft
	reti

;- Right wisker
.org	$000C	;INT5
	rcall	HitRight
	reti

;- USART receive
.org	$003C			; USART1 - this one is tied to the IR
	rcall	USARTrec
	reti

.org	$0046					; End of Interrupt Vectors

;-----------------------------------------------------------
; Program Initialization
;-----------------------------------------------------------
INIT:
	;Stack Pointer (VERY IMPORTANT!!!!)
	ldi		mpr,	LOW(RAMEND)
	out		SPL,	mpr
	ldi		mpr,	HIGH(RAMEND)
	out		SPH,	mpr

	;I/O Ports
		;PORT B (Motor/Wheels)
		ldi	mpr,	(1<<EngEnL)|(1<<EngEnR)|(1<<EngDirR)|(1<<EngDirL)	;select direction as output on all pins
		out	DDRB,	mpr			; set value in DDRB
		ldi	mpr,	$00			; set initial value to 0s
		out	PORTB,	mpr			; set PORTB value, PORT B pins should all be high
		; Initialize Port E for input
		ldi	mpr,	(0<<WskrL)|(0<<WskrR)	; Set Port E Directional Register
		out	DDRE,	mpr						; for input
		ldi	mpr,	(1<<WskrL)|(1<<WskrR)	; Activate pull-up regsiters
		out	PORTE,	mpr	

	
	;USART1
		;Set baudrate at 2400bps
		ldi	mpr,	$03
		sts	UBRR1H,	mpr
		ldi	mpr, 	$40
		sts	UBRR1L,	mpr
		;Enable receiver and enable receive interrupts
		ldi	mpr,	(1<<RXEN1)|(1<<RXCIE1)|(1<<TXEN1)
		sts	UCSR1B,	mpr
		ldi	mpr,	(1<<U2X1)
		sts	UCSR1A,	mpr
		;Set frame format: 8data bits, 2 stop bit
		ldi	mpr, 	(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)
		sts	UCSR1C,	mpr

	;External Interrupts
		;Set the External Interrupt Mask
		ldi	mpr,	(1<<INT0)|(1<<INT1)	
		out	EIMSK,	mpr

		;Set the Interrupt Sense Control to Rising Edge detection
		ldi	mpr,	$00
		out	EICRB,	mpr
		ldi	mpr,	(1<<ISC01)|(0<<ISC00)|(1<<ISC11)|(0<<ISC10)
		sts	EICRA,	mpr

	;Other
	sei

	; Initialize TekBot Halt Movement
	ldi		command, MovFwd		; Load Move Foward Command
	ldi		freezcnt,	3		; init freeze count
;-----------------------------------------------------------
; Main Program
;-----------------------------------------------------------
MAIN:
	out		PORTB, command		; Send command to motors

	rjmp	MAIN

;***********************************************************
;*	Functions and Subroutines
;***********************************************************
USARTrec:
	cli
	; save variables
	push	mpr
	push	waitcnt
	in		mpr,	SREG
	push	mpr

	rcall	GETSIG

	;check for recieved freeze from other bot
	cpi		mpr,	FreezT
	breq	FREEZEBOT

	cpi		mpr,	BotID
	;breq	FLAGSET			; set BotID check flag to 1 meaning ID was correct
	brne	POPVAR		; BotID was incorrect, skip command

	rcall	GETSIG

	;from transmitter, send freeze.
	cpi		mpr,	FreezR
	breq	FREEZESEND

	sbrc	mpr, 7	;if mpr signal is command signal set the command to "command"
	rjmp	SETCMD

POPVAR:
	; return variables
	pop		mpr
	out		SREG,	mpr
	pop		waitcnt
	pop		mpr
	sei
	ret						; only want one return. return increments PC

SETCMD:
	clc				; Clear the carry bit
	rol		mpr		; Shift everything left so the command will parse/compare correctly
	
	mov		command,	mpr
	rjmp	POPVAR

FREEZEBOT:
	push	mpr

	ldi		mpr, Halt
	out		PORTB, mpr

	dec		freezcnt
	breq	FREEZEFOREVER

	ldi		waitcnt, 200
    rcall Wait
	rcall Wait

	pop		mpr

	rjmp	POPVAR

;''''''''''''''''''''''''''''''

FREEZEFOREVER:

	ldi		mpr, Halt
	out		PORTB, mpr

	rjump FREEZEFOREVER

;''''''''''''''''''''''''''''''

FREEZESEND:
	push mpr

	rcall CHKTRANS

	ldi	mpr,	FreezT
	sts	UDR1,	mpr

	;important wait for sig to send
	ldi waitcnt, 5
    rcall Wait

	; important so it doesnt freeze it self.
	rcall GETSIG

	pop	mpr

	rjmp POPVAR

	
;''''''''''''''''''''''''''''''''''''
;''''''''''''''''''''''''''''''''''''

CHKTRANS:
        push mpr
EmptyL:
        lds mpr, UCSR1A
        sbrs mpr, UDRE1
        rjmp EmptyL
       
        pop mpr
        ret

;-------------------------------
;-------------------------------

GETSIG:
	lds		mpr,	UCSR1A
	sbrs	mpr,	RXC1
	rjmp	GETSIG
	
	lds		mpr,	UDR1

	ret

;-------------------------------
;-------------------------------

HitRight:
	; save variables by pushing them to the stack
	push	mpr
	push	waitcnt
	in	    mpr,	SREG
	push	mpr

	; execute function
	; move backwards
	ldi		mpr,	MovBck
	out		PORTB,	mpr
	ldi		waitcnt,WTime
	rcall	Wait

	; turn left
	ldi		mpr,	TurnL
	out		PORTB,	mpr
	ldi		waitcnt,WTime
	rcall	Wait

	; restore variables
	pop		mpr
	out		SREG,	mpr
	pop		waitcnt
	pop		mpr

	ret

;------------------------------
;------------------------------
HitLeft:
	; save variables by pushing them to the stack
	push	mpr
	push	waitcnt
	in	mpr,		SREG
	push	mpr

	; execute function
	; move backwards
	ldi	mpr,		MovBck
	out	PORTB,		mpr
	ldi	waitcnt,	WTime
	rcall	Wait

	; turn left
	ldi	mpr,		TurnR
	out	PORTB,		mpr
	ldi	waitcnt,	WTime
	rcall	Wait

	; restor variables
	pop	mpr
	out	SREG,		mpr
	pop	waitcnt
	pop	mpr

	ret

;--------------------------------
;--------------------------------
Wait:
	; save variables
	push	waitcnt
	push	ilcnt
	push	olcnt

	; execute function

Loop:	ldi	olcnt,	224
OLoop:	ldi	ilcnt,	237
ILoop:	dec	ilcnt
	brne	ILoop
	dec	olcnt
	brne	Oloop
	dec	waitcnt
	brne	Loop

	; restore variables
	pop	olcnt
	pop	ilcnt
	pop	waitcnt

	ret

;***********************************************************
;*	Stored Program Data
;***********************************************************



;***********************************************************
;*	Additional Program Includes
;***********************************************************
