;***********************************************************
;*
;*	Lab 4
;*
;*	Addition and Multiplication in Assembly Language
;*
;*	This was the skeleton file Lab 4 of ECE 375
;*
;***********************************************************
;*
;*	 Author: David Merrick
;*	   Date: 10/19/12
;*
;***********************************************************

.include "m128def.inc"			; Include definition file

;***********************************************************
;*	Internal Register Definitions and Constants
;***********************************************************
.def	mpr = r16				; Multipurpose register 
.def	rlo = r0				; Low byte of MUL result
.def	rhi = r1				; High byte of MUL result
.def	zero = r2				; Zero register, set to zero in INIT, useful for calculations
.def	A = r3					; An operand
.def	B = r4					; Another operand

.def	oloop = r17				; Outer Loop Counter
.def	iloop = r18				; Inner Loop Counter

.equ	addrA = $0100			; Beginning Address of Operand A data
.equ	addrB = $0103			; Beginning Address of Operand B data
.equ	LAddrP = $0106			; Beginning Address of Product Result
.equ	HAddrP = $010D			; End Address of Product Result

;my added variables
.equ    LAddrS = $0114			; Beginning address of sum result		
.equ	HAddrS = $0116			; End address of sum result

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
INIT:							; The initialization routine
		; Initialize Stack Pointer
		ldi mpr,	low(RAMEND)
		out SPL,	mpr	
		ldi mpr,	high(RAMEND)
		out SPH,	mpr

		; Move numbers A and B from program memory to data memory	
		; Initialize Z to point to program memory where number A is stored
		; and X to point to A's destination data memory address space.
		ldi ZL, low(A_BEG<<1)
		ldi ZH, high(A_BEG<<1)
		ldi XL, low(addrA)
		ldi XH, high(addrA)
		
		; Steps:
		; Load bytes of A into data memory
		; Load bytes of B into data memory
		; Clear out memory space of addition result
		; Clear out memory space of product result

LOADA:
	lpm mpr, Z+ ; Load byte of A into MPR
	st X+, mpr ; load this byte into appropriate data memory address space
	
	cpi ZL, low(A_END<<1) ; check if we're at the end of the string
	brne LOADA        ; and repeat if we're not
	
	cpi ZH, high(A_END<<1) ; check if we're at the end of the string
	brne LOADA        ; and repeat if we're not

	; point Z at number B's program memory address space
	; and Y at B's destination data memory address space

	ldi ZL, low(B_BEG<<1)
	ldi ZH, high(B_BEG<<1)
	ldi YL, low(addrB)
	ldi YH, high(addrB)
		
LOADB:
	lpm mpr, Z+ ; Load byte of B into MPR
	st Y+, mpr ; load this byte into approprate data memory address space		
	
	cpi ZL, low(B_END<<1)
	brne LOADB
	
	cpi ZH, high(B_END<<1)
	brne LOADB

	;Point X at the memory space where the sum will be stored in preparation to clear it
	ldi XL, low(LAddrS) 
	ldi XH, high(LAddrS)
CLEARSUM: ; Clear the memory space where the sum will be stored
	st X+, zero
	cpi XL, low(HAddrS) ; Repeat if the low byte of X is not pointed at the end of the address space
	brne CLEARSUM

	cpi XH, high(HAddrS)
	brne CLEARSUM

	; Clear the product memory space
	ldi XL, low(LAddrP)
	ldi XH, high(LAddrP)
CLEARPRODUCT:
	st X+, zero
	cpi XL, low(HAddrP)
	brne CLEARPRODUCT

	cpi XH, high(HAddrP)
	brne CLEARPRODUCT
	
	clc ; clear carry

	clr		zero			; Set the zero register to zero, maintain
								; these semantics, meaning, don't load anything
								; to it.

;-----------------------------------------------------------
; Main Program
;-----------------------------------------------------------
MAIN:							; The Main program
		; Setup the add funtion
		; Add the two 16-bit numbers
		rcall	ADD16			; Call the add function

		; Setup the multiply function
			
		;Clear product space	
			
			; Point X at data memory location where A will be stored
			ldi XL, low(addrA)
			ldi XH, high(addrA)
			
			; Point Y at data memory location where B will be stored
			ldi YL, low(addrB)
			ldi YH, high(addrB)
			
			; Point Z at data memory location where sum is be stored
			ldi ZL, low(LaddrS)
			ldi ZH, high(LaddrS)

LOADSUM: ; load the sum of A and B we've already found into data memory
		ld mpr, Z+ ; Store a byte of the sum into the MPR
		st X+, mpr ; Store this into A and B memory locations
		st Y+, mpr
		
		cpi ZL, low(HAddrS)
		brne LOADSUM

		cpi ZH, high(HAddrS)
		brne LOADSUM

		; Multiply two 24-bit numbers
		rcall	MUL24			; Call the multiply function

DONE:	
	rjmp	DONE			; Create an infinite while loop to signify the 
								; end of the program.

;***********************************************************
;*	Functions and Subroutines
;***********************************************************

;-----------------------------------------------------------
; Func: ADD16
; Desc: Adds two 16-bit numbers and generates a 24-bit number
;		where the high byte of the result contains the carry
;		out bit.
;-----------------------------------------------------------
ADD16:
		; Save variable by pushing them to the stack
		push A
		push B
		push XL
		push XH
		push YL
		push YH
		push ZL
		push ZH
		push zero

		clr zero

		; Set X, Y, and Z to point to A, B, and our result space, respectively

		; Point X at address space for A
		ldi XL, low(addrA)
		ldi XH, high(addrA)
		
		; Point Y at address space for B
		ldi YL, low(addrB)
		ldi YH, high(addrB)
		
		; Point Z at address space for sum
		ldi ZL, low(LaddrS)
		ldi ZH, high(LAddrS)

		; Load A and B into A and B registers so we can work with them
		ld A, X+ 
		ld B, Y+

		add A, B ; Perform the addition
		st Z+, A ; Store the sum and increment Z so we can store the carry bit

		ld A, X
		ld B, Y

		adc A, B

		st Z+, A

		clr A ; Set A to zero
		adc zero, A ; Take care of the carry. Add with carry will produce a 1 if carry is 1 and 0 otherwise.

		st Z, A ; Store the carry bit into the 3rd byte

		; Execute the function here
		
		; Restore variable by popping them from the stack in reverse order\
		pop zero
		pop ZH
		pop ZL
		pop YH
		pop YL
		pop XH
		pop XL
		pop B
		pop A
		
		ret						; End a function with RET

;-----------------------------------------------------------
; Func: MUL24
; Desc: Multiplies two 24-bit numbers and generates a 48-bit 
;		result.
;-----------------------------------------------------------
MUL24:
		push 	A				; Save A register
		push	B				; Save B register
		push	rhi				; Save rhi register
		push	rlo				; Save rlo register
		push	zero			; Save zero register
		push	XH				; Save X-ptr
		push	XL
		push	YH				; Save Y-ptr
		push	YL				
		push	ZH				; Save Z-ptr
		push	ZL
		push	oloop			; Save counters
		push	iloop				

		clr		zero			; Maintain zero semantics

		; Set Y to beginning address of B
		ldi		YL, low(addrB)	; Load low byte
		ldi		YH, high(addrB)	; Load high byte

		; Set Z to begginning address of resulting Product
		ldi		ZL, low(LAddrP)	; Load low byte
		ldi		ZH, high(LAddrP); Load high byte

		; Begin outer for loop
		ldi		oloop, 3		; Load counter
MUL24_OLOOP:
		; Set X to beginning address of A
		ldi		XL, low(addrA)	; Load low byte
		ldi		XH, high(addrA)	; Load high byte

		; Begin inner for loop
		ldi		iloop, 3		; Load counter
MUL24_ILOOP:
		ld		A, X+			; Get byte of A operand
		ld		B, Y			; Get byte of B operand
		mul		A,B				; Multiply A and B
		ld		A, Z+			; Get a result byte from memory
		ld		B, Z+			; Get the next result byte from memory
		add		rlo, A			; rlo <= rlo + A
		adc		rhi, B			; rhi <= rhi + B + carry
		ld		A, Z			; Get a third byte from the result
		adc		A, zero			; Add carry to A
		st		Z, A			; Store third byte to memory
		st		-Z, rhi			; Store second byte to memory
		st		-Z, rlo			; Store third byte to memory
		adiw	ZH:ZL, 1		; Z <= Z + 1			
		dec		iloop			; Decrement counter
		brne	MUL24_ILOOP		; Loop if iLoop != 0
		; End inner for loop

		sbiw	ZH:ZL, 2		; Z <= Z - 2
		adiw	YH:YL, 1		; Y <= Y + 1
		dec		oloop			; Decrement counter
		brne	MUL24_OLOOP		; Loop if oLoop != 0
		; End outer for loop
		 		
		pop		iloop			; Restore all registers in reverves order
		pop		oloop
		pop		ZL				
		pop		ZH
		pop		YL
		pop		YH
		pop		XL
		pop		XH
		pop		zero
		pop		rlo
		pop		rhi
		pop		B
		pop		A
		ret						; End a function with RET
;-----------------------------------------------------------
; Func: MUL16
; Desc: An example function that multiplies two 16-bit numbers
;			A - Operand A is gathered from address $0101:$0100
;			B - Operand B is gathered from address $0103:$0102
;			Res - Result is stored in address 
;					$0107:$0106:$0105:$0104
;		You will need to make sure that Res is cleared before
;		calling this function.
;-----------------------------------------------------------
MUL16:
		push 	A				; Save A register
		push	B				; Save B register
		push	rhi				; Save rhi register
		push	rlo				; Save rlo register
		push	zero			; Save zero register
		push	XH				; Save X-ptr
		push	XL
		push	YH				; Save Y-ptr
		push	YL				
		push	ZH				; Save Z-ptr
		push	ZL
		push	oloop			; Save counters
		push	iloop				

		clr		zero			; Maintain zero semantics

		; Set Y to beginning address of B
		ldi		YL, low(addrB)	; Load low byte
		ldi		YH, high(addrB)	; Load high byte

		; Set Z to begginning address of resulting Product
		ldi		ZL, low(LAddrP)	; Load low byte
		ldi		ZH, high(LAddrP); Load high byte

		; Begin outer for loop
		ldi		oloop, 2		; Load counter
MUL16_OLOOP:
		; Set X to beginning address of A
		ldi		XL, low(addrA)	; Load low byte
		ldi		XH, high(addrA)	; Load high byte

		; Begin inner for loop
		ldi		iloop, 2		; Load counter
MUL16_ILOOP:
		ld		A, X+			; Get byte of A operand
		ld		B, Y			; Get byte of B operand
		mul		A,B				; Multiply A and B
		ld		A, Z+			; Get a result byte from memory
		ld		B, Z+			; Get the next result byte from memory
		add		rlo, A			; rlo <= rlo + A
		adc		rhi, B			; rhi <= rhi + B + carry
		ld		A, Z			; Get a third byte from the result
		adc		A, zero			; Add carry to A
		st		Z, A			; Store third byte to memory
		st		-Z, rhi			; Store second byte to memory
		st		-Z, rlo			; Store third byte to memory
		adiw	ZH:ZL, 1		; Z <= Z + 1			
		dec		iloop			; Decrement counter
		brne	MUL16_ILOOP		; Loop if iLoop != 0
		; End inner for loop

		sbiw	ZH:ZL, 1		; Z <= Z - 1
		adiw	YH:YL, 1		; Y <= Y + 1
		dec		oloop			; Decrement counter
		brne	MUL16_OLOOP		; Loop if oLoop != 0
		; End outer for loop
		 		
		pop		iloop			; Restore all registers in reverves order
		pop		oloop
		pop		ZL				
		pop		ZH
		pop		YL
		pop		YH
		pop		XL
		pop		XH
		pop		zero
		pop		rlo
		pop		rhi
		pop		B
		pop		A
		ret						; End a function with RET

;-----------------------------------------------------------
; Func: Template function header
; Desc: Cut and paste this and fill in the info at the 
;		beginning of your functions
;-----------------------------------------------------------
FUNC:							; Begin a function with a label
		; Save variable by pushing them to the stack

		; Execute the function here
		
		; Restore variable by popping them from the stack in reverse order\
		ret						; End a function with RET


;***********************************************************
;*	Stored Program Data
;***********************************************************

A_BEG:
	.DB 3
A_END:
B_BEG:
	.DB 3
B_END:

;***********************************************************
;*	Additional Program Includes
;***********************************************************
; There are no additional file includes for this program
