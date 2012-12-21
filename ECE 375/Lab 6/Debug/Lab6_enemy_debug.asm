;***********************************************************
;*
;*	Remote Receiver
;*
;*	Recieves instruction from transmitter and executes
;*
;*	This is the RECEIVE skeleton file for Lab 6 of ECE 375
;*
;***********************************************************
;*
;*	 Author: Garrett Clay, David Merrick
;*	 Date: 2012-11-12
;*
;***********************************************************

.include "m128def.inc"			; Include definition file

;***********************************************************
;*	Internal Register Definitions and Constants
;***********************************************************
.def	mpr = r16				; Multi-Purpose Register
.def	waitcnt  = r17
.def	ilcnt	 = r18
.def	olcnt	 = r19
;.def	flag	 = r26
.def 	freezcnt = r21			; Counts number of freezes
.def 	trany    = r22			; Transmit register
;.def	freezeflag = r23
.def	debug	  = r24

.equ	WTime = 100
.equ	WskrR = 0				; Right Whisker Input Bit
.equ	WskrL = 1				; Left Whisker Input Bit
.equ	EngEnR = 4				; Right Engine Enable Bit
.equ	EngEnL = 7				; Left Engine Enable Bit
.equ	EngDirR = 5				; Right Engine Direction Bit
.equ	EngDirL = 6				; Left Engine Direction Bit

.equ	BotID = 0b00000010 ;(Enter you group ID here (8bits)); Unique XD ID (MSB = 0)

;/////////////////////////////////////////////////////////////
;These macros are the values to make the TekBot Move.
;/////////////////////////////////////////////////////////////

.equ	MovFwd =  (1<<EngDirR|1<<EngDirL)	;0b01100000 Move Forwards Command
.equ	MovBck =  ($00)				;0b00000000 Move Backwards Command
.equ	TurnR =   (1<<EngDirL)			;0b01000000 Turn Right Command
.equ	TurnL =   (1<<EngDirR)			;0b00100000 Turn Left Command
.equ	Halt =    (1<<EngEnR|1<<EngEnL)		;0b10010000 Halt Command
.equ	Freez =   $F0		;0b11111000 Freeze Command, but after rol'ing becomes 0b11110000
.equ	flagmem = $0100
.equ	freezeflagmem = $0102


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


;- Right wisker
.org	$0004
rcall	HitRight
reti

;- Left wisker
.org	$0006
rcall	HitLeft
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
	ldi	mpr,	LOW(RAMEND)
	out	SPL,	R16
	ldi	mpr,	HIGH(RAMEND)
	out	SPH,	R16

	;I/O Ports
	;PORT B
;	ldi	mpr,	(1<<EngEnL)|(1<<EngEnR)|(1<<EngDirR)|(1<<EngDirL)	;select direction as output on all pins
	ldi	mpr,	$FF
	out	DDRB,	mpr			; set value in DDRB
	ldi	mpr,	$00			; set initial value to high on all pins
	out	PORTB,	mpr			; set PORTB value, PORT B pins should all be high

	;PORT D
	ldi	mpr,	$FF
	out	PORTD,	mpr
	ldi	mpr,	$00
	out	DDRD,	mpr

	;USART1
	;Set baudrate at 2400bps U2X set to 1. clk = 16 Mhz. UBRR = 832
	ldi	mpr,	$03
	sts	UBRR1H,	mpr
	ldi	mpr, 	$40
	sts	UBRR1L,	mpr

	;Enable receiver and enable receive interrupts. Enable transmitter
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
	ldi	mpr,	(0<<ISC10|1<<ISC11|0<<ISC20|1<<ISC21)
	sts	EICRA,	mpr

	; Initialize freeze count to zero
	ldi 	freezcnt, $00
;	ldi		waitcnt, 10 	; Initialize waitcnt to 10 ms
;	ldi freezeflag, $01
	ldi mpr, 0b11001100	

	;Other
	ldi	XL,	LOW(flagmem)
	ldi	XH,	HIGH(flagmem)

	ldi	YL,	LOW(freezeflagmem)
	ldi	YH,	HIGH(freezeflagmem)

	sei


;-----------------------------------------------------------
; Main Program
;-----------------------------------------------------------
MAIN:

		rjmp	MAIN

;***********************************************************
;*	Functions and Subroutines
;***********************************************************
HitRight:
	ldi waitcnt, 10
	ldi mpr, $FF
	rcall Wait
	
	/*
	; save variables by pushing them to the stack
	push	mpr
	push	waitcnt
	in		mpr,	SREG
	push	mpr

	; execute function
	; move backwards
	ldi	mpr,		MovBck
	out	PORTB,		mpr
	ldi	waitcnt,	WTime
	rcall	Wait

	; turn left
	ldi		mpr,		TurnL
	out		PORTB,		mpr
	ldi		waitcnt,	WTime
	rcall	Wait

	; restore variables
	pop	mpr
	out	SREG,		mpr
	pop	waitcnt
	pop	mpr
	*/
	ret

;------------------------------
;------------------------------
HitLeft:
	ldi waitcnt, 10
	ldi mpr, $FF
	rcall Wait

/*
	; save variables by pushing them to the stack
	push	mpr
	push	waitcnt
	in		mpr,	SREG
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
*/
	ret

;-------------------------------
;-------------------------------
USARTrec:	
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $01
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG
	
	; save variables
	push	mpr
	push	waitcnt
	in		mpr,	SREG
	push	mpr
	push	freezcnt

	lds		mpr,	UDR1

	;DEBUG
		out PORTB, mpr
		rcall Wait
	;DEBUG

	cpi		mpr,	BotID
	breq	FLAGSET			; set BotID check flag to 1 meaning ID was correct

	cpi		mpr,	BotID
	brne	FLAGNSET		; set BotID check flag to 0 meaning ID was incorrect

JUMP:
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $03
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG

	ld		mpr,	X
	cpi		mpr,	$01
	breq	RECcmd
	
	cpi mpr, $01
	brne POPVAR
	
FLAGSET:
	
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $02
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG

	ldi mpr, $00
	
	ldi		mpr, 	$01
	st		X,	mpr		; correct BotID

	rjmp	JUMP

FLAGNSET:
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $F2
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG


	ldi		mpr,	$00		; not correct BotID
	st		X,		mpr
	rjmp	JUMP	

RECcmd:
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $4
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG

	ldi		mpr,	$00		; not correct BotID
	st		X,		mpr

	lds		mpr,	UDR1

	cpi		mpr, 	0b01010101 ; To respond to freeze commands from other robots.
	breq 	FREEZETAG	

	clc ; Clear the carry bit
	rol mpr	; Shift everything left so the command will parse/compare correctly
	
	cpi		mpr,	MovFwd
	breq	FWD
	cpi		mpr,	MovBck
	breq	BCK	
	cpi		mpr, 	TurnR
	breq	R
	cpi		mpr, 	TurnL
	breq	L	
	cpi		mpr,	Halt
	breq	H
	cpi		mpr,	Freez
	breq	SENDFR
	

	rjmp	POPVAR			; return to pop variables


POPVAR:
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $FE
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG

	; return variables
	pop 	freezcnt
	pop		mpr
	out		SREG,	mpr
	pop		waitcnt
	pop		mpr
	
	ret						; only want one return. return increments PC



FREEZETAG: ; Responds to being frozen called 			from USARTrec
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $F3
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG

	ldi mpr, $00
	
	ld		mpr,	Y
	cpi		mpr,	$01
	breq POPVAR					; If freeze flag is set, do not respond
	
	pop freezcnt	
	inc freezcnt ; This is not incrementing. Why?
	push freezcnt

	cpi freezcnt, $03
	breq STUCK ; If the robot has been frozen 3 times, jump into infinite loop
	
	out PORTB, freezcnt
	
	ldi waitcnt, 250 ; Freeze for 5 seconds
	rcall Wait
	ldi mpr, $00
	out PORTB, mpr
	
	rjmp POPVAR

FWD:
	ldi mpr, $00

	ldi 	mpr, MovFwd
	out 	PORTB, mpr
	rcall	Wait
	ldi mpr, $00
	out PORTB, mpr
	rjmp	POPVAR

BCK:
	ldi mpr, $00

	ldi mpr, MovBck
	out PORTB, mpr
	rcall	Wait
	ldi mpr, $00
	out PORTB, mpr
	rjmp	POPVAR

R:
	ldi mpr, $00

	ldi mpr, TurnR
	out PORTB, mpr
	rcall	Wait
	ldi mpr, $00
	out PORTB, mpr
	rjmp	POPVAR

L:
	ldi mpr, $00

	ldi mpr, TurnL
	out PORTB, mpr
	rcall	Wait
	ldi mpr, $00
	out PORTB, mpr
	rjmp	POPVAR

H:
	ldi mpr, Halt
	out PORTB, mpr
	rcall	Wait
	ldi mpr, $00
	out PORTB, mpr
	rjmp	POPVAR

SENDFR: ; Sends the freeze signal to other robots
	;DEBUG
		ldi waitcnt, 50
		ldi debug, $FC
		out PORTB, debug
		rcall Wait
		ldi debug, $00
		out PORTB, mpr
	;DEBUG

	ldi mpr, $00

	ldi mpr, $01
	st	Y,	mpr			; set freezeflag meaning send freeze command

	ldi waitcnt, 50
	ldi trany, 0b00000010		; freeze Robot 2
	rcall USARTtran
	
	ldi trany, 0b01010101		; freeze command to others
	rcall USARTtran
	rcall Wait

	ldi mpr, $03		; set freezeflag so can respond to kill shot
	st	Y,	mpr			; 

	rjmp POPVAR

STUCK:
	ldi mpr, $FF
	out PORTB, mpr
	ldi waitcnt, 250
	rcall Wait
	rjmp STUCK


USARTtran:
	;ldi mpr, $00
	
;	lds		mpr,	UCSR1A		; load status of USART1
;	sbrs 	mpr,	UDRE1		; check UDRE1 to see if set
;	rjmp	USARTtran			; loop until UDRE1 is set
	
;	sts		UDR1,	trany
	ret

;----------------------------------------------------------------
; Sub:	Wait
; Desc:	A wait loop that is 16 + 159975*waitcnt cycles or roughly 
;		waitcnt*10ms.  Just initialize wait for the specific amount 
;		of time in 10ms intervals. Here is the general eqaution
;		for the number of clock cycles in the wait loop:
;			((3 * ilcnt + 3) * olcnt + 3) * waitcnt + 13 + call
;----------------------------------------------------------------
Wait:
		push	waitcnt			; Save wait register
		push	ilcnt			; Save ilcnt register
		push	olcnt			; Save olcnt register

Loop:	ldi		olcnt, 224		; load olcnt register
OLoop:	ldi		ilcnt, 237		; load ilcnt register
ILoop:	dec		ilcnt			; decrement ilcnt
		brne	ILoop			; Continue Inner Loop
		dec		olcnt		; decrement olcnt
		brne	OLoop			; Continue Outer Loop
		dec		waitcnt		; Decrement wait 
		brne	Loop			; Continue Wait loop	

		pop		olcnt		; Restore olcnt register
		pop		ilcnt		; Restore ilcnt register
		pop		waitcnt		; Restore wait register
		ret				; Return from subroutine
