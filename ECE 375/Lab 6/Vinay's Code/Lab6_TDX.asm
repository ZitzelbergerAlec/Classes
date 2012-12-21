/*
 * Lab6_TDX.asm
 *
 *  Created: 11/11/2012 5:18:27 PM
 *   Author: Vinay
 */ 


 .include "m128def.inc"			; Include definition file

;***********************************************************
;*	Internal Register Definitions and Constants
;***********************************************************
.def	mpr = r16				; Multi-Purpose Register
.def	command = r17
.def waitcnt    =       r20                             ; Wait Loop Counter
.def ilcnt              =       r18                             ; Inner Loop Counter
.def olcnt              =       r19   

.equ WTime = 10

.equ	EngEnR = 4				; Right Engine Enable Bit
.equ	EngEnL = 7				; Left Engine Enable Bit
.equ	EngDirR = 5				; Right Engine Direction Bit
.equ	EngDirL = 6				; Left Engine Direction Bit

; Use these commands between the remote and TekBot
; MSB = 1 thus:
; commands are shifted right by one and ORed with 0b10000000 = $80
.equ	MovFwd =  ($80|1<<(EngDirR-1)|1<<(EngDirL-1))	;0b10110000 Move Forwards Command
.equ	MovBck =  ($80|$00)								;0b10000000 Move Backwards Command
.equ	TurnR =   ($80|1<<(EngDirL-1))					;0b10100000 Turn Right Command
.equ	TurnL =   ($80|1<<(EngDirR-1))					;0b10010000 Turn Left Command
.equ	Halt =    ($80|1<<(EngEnR-1)|1<<(EngEnL-1))		;0b11001000 Halt Command
.equ	Freez =   ($80|$F8)
.equ	FreezCheat = 0b01010101

.equ	BotID = 0b00000010		;(Enter you group ID here (8bits)); Unique XD ID (MSB = 0)

;***********************************************************
;*	Start of Code Segment
;***********************************************************
.cseg							; Beginning of code segment

;-----------------------------------------------------------
; Interrupt Vectors
;-----------------------------------------------------------
.org	$0000					; Beginning of IVs
		rjmp 	INIT			; Reset interrupt


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
		; Initialize Port D for inputs
		ldi		mpr, $FF		; Initialize Port D for inputs
		out		PORTD, mpr		; with Tri-State
		ldi		mpr, $00		; Set Port D Directional Register
		out		DDRD, mpr		; for inputs

		ldi mpr, $00; Initialize Port B for output
        out PORTB, mpr  ;Outputs low
        ldi mpr, $FF
        out DDRB, mpr

	;USART1
		;Set baudrate at 2400bps
		ldi	mpr,	$03
		sts	UBRR1H,	mpr
		ldi	mpr, 	$40
		sts	UBRR1L,	mpr
		;Enable transmitter
		ldi	mpr,	(1<<TXEN1)
		sts	UCSR1B,	mpr
		ldi	mpr,	(1<<U2X1)
		sts	UCSR1A,	mpr
		;Set frame format: 8data, 2 stop bit
		ldi	mpr, 	(1<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)
		sts	UCSR1C,	mpr

	;Other


;-----------------------------------------------------------
; Main Program
;-----------------------------------------------------------
MAIN:
				in mpr, PIND
	
				;out PORTB, mpr

				cpi	mpr,	$FF
				breq	MAIN

				ldi		command,	$00

                sbrs	mpr, 7
                ldi		command,	MovBck
	
				sbrs	mpr, 6
                ldi		command,	MovFwd

                sbrs	mpr, 5
                ldi		command,	TurnL
 
                sbrs	mpr, 4
                ldi		command,	TurnR

				sbrs	mpr, 2
				rjmp	FREEZTEST
				
				sbrs	mpr, 1
				ldi		command,	Freez

                sbrs	mpr, 0
                ldi		command,	Halt

				rcall	SENDCMD


	rjmp	MAIN

;***********************************************************
;*	Functions and Subroutines
;***********************************************************

FREEZTEST:
	push mpr

	rcall CHKTRANS

	ldi mpr, FreezCheat
    sts UDR1, mpr

	;important wait for sig to send
	ldi waitcnt, WTime
    rcall Wait

	pop mpr
	ret

;''''''''''''''''''''''''''''''''''''''''''''
;''''''''''''''''''''''''''''''''''''''''''''

SENDCMD:
	push mpr

	rcall SendID
	rcall CHKTRANS

	mov mpr, command
    sts UDR1, mpr

	;important wait for sig to send
	ldi waitcnt, WTime
    rcall Wait

	pop mpr
	ret

;''''''''''''''''''''''''''''''''''''''
;''''''''''''''''''''''''''''''''''''''

SendID:
        push mpr
 
        rcall CHKTRANS
 
        ldi mpr, BotID
        sts UDR1, mpr
 
        pop mpr
        
		ret

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

;'''''''''''''''''''''''''''''''''''''
;'''''''''''''''''''''''''''''''''''''''

Wait:
        push    waitcnt                 ; Save wait register
        push    ilcnt                   ; Save ilcnt register
        push    olcnt                   ; Save olcnt register
 
Loop:  
        ldi             olcnt, 224              ; load olcnt register
OLoop: 
        ldi             ilcnt, 237              ; load ilcnt register
ILoop: 
        dec             ilcnt                   ; decrement ilcnt
        brne    ILoop                   ; Continue Inner Loop
        dec             olcnt           ; decrement olcnt
        brne    OLoop                   ; Continue Outer Loop
        dec             waitcnt         ; Decrement wait
        brne    Loop                    ; Continue Wait loop   
 
        pop             olcnt           ; Restore olcnt register
        pop             ilcnt           ; Restore ilcnt register
        pop             waitcnt         ; Restore wait register
        ret                             ; Return from subroutine

;***********************************************************
;*	Stored Program Data
;***********************************************************



;***********************************************************
;*	Additional Program Includes
;***********************************************************