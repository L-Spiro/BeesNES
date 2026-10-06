	.inesprg 1
	.ineschr 2
	.inesmir 0
	.inesmap 4

	.bank	1
	.org	$f000

RESET:
	sei
	cld
	ldx	#$ff
	txs
	lda	#%01000000		; disable APU frame IRQ
	sta	$4017
	lda	#$00
	sta	$2000
	sta	$2001
	sta	$4010			; disable DMC IRQs
	stx	$e000
	stx	$a000

	bit	$2002			; vblank flag reset
.Vwait	bit	$2002
	bpl	.Vwait

	ldy	#$07
	sty	<$01
	sta	<$00
	tay
.clrmem	sta	[$00],y
	iny
	bne	.clrmem
	dec	<$01
	bpl	.clrmem
	sta	<$01

.Vwait2	bit	$2002
	bpl	.Vwait2

;-------------------------------------------------------------------------------

	ldx	#$3f
	stx	$2006
	ldx	#$00
	stx	$2006
.loadPal
	lda	PaletteData,x
	sta	$2007
	inx
	cpx	#$20
	bne	.loadPal

	ldy	#$20
	sty	$2006
	ldy	#$00
	sty	$2006
	ldx	#$04
	lda	#$ff
.loadBG
	sta	$2007
	iny
	bne	.loadBG
	dex
	bne	.loadBG

	ldx	#$23
	stx	$2006
	ldx	#$c0
	stx	$2006
	ldx	#$00
.loadBGAttr
	lda	BGAttrData,x
	sta	$2007
	inx
	cpx	#$20
	bne	.loadBGAttr

;------------------------------

	ldx	#$00
	ldy	#$ff
.setmem	lda	SpriteData,x
	sta	$0500,x
	sta	$0700,x
	inx
	bne	.setmem


	lda	#$02	; OAMADDR initial value
	sta	$04ff


	lda	#%10101000
	sta	<$30
	lda	#%10000000
	sta	$2000
	cli

.loop	jmp	.loop

;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------

	.org	$f100

NMI:
	lda	<$30
	sta	$2000		; Set PPUCTRL
	lda	#%00011110
	sta	$2001		; Set PPUMASK

	lda	#$00
	sta	$2003
	lda	#$07
	sta	$4014

;------------------------------

	bit	$2002

	lda	#$20
	sta	$2006
	lda	#$66
	sta	$2006
	ldx	#$0d
	ldy	#$00
	jsr	DrawDigits

	lda	#$18		; Set scroll
	sta	$2005
	lda	#$00
	sta	$2005

	lda	#$00
	sta	$c000		; Set IRQ
	sta	$c001
	sta	$e000
	sta	$e001

	lda	<$30
	and	#%00100000
	lsr	A
	lsr	A
	jsr	BankSet

	jsr	GetKey		; Get keystrokes

;------------------------------

	; Processing in response to keystrokes.
	lda	<$26
.right	lsr	A
	bcc	.left
	inc	<$40
.left	lsr	A
	bcc	.down
	dec	<$40
.down	lsr	A
	bcc	.up
	ldx	<$40
	inc	$0500,x
.up	lsr	A
	bcc	.start
	ldx	<$40
	dec	$0500,x
.start	lsr	A
	bcc	.select
	pha
	lda	<$30
	eor	#%00100000
	sta	<$30
	pla
.select	lsr	A
	bcc	.key_end
	lda	$04ff
	adc	#$00
	and	#$03
	sta	$04ff
.key_end

	ldy	#$0b	; If the cursor is out of range, return.
	lda	<$40
	bmi	*+8
	cmp	#$0c
	bcc	*+6
	ldy	#$00
	sty	<$40


	jsr 	SetSprData


	lda	<$40	; Move cursor
	asl	A
	asl	A
	asl	A
	asl	A
	clc
	adc	#$2c
	sta	$07ff

	lda	$04ff
	asl	A
	asl	A
	asl	A
	asl	A
	clc
	adc	#$28
	sta	$07f7
	clc
	adc	#$38
	sta	$07fb

	lda	#$ff
	ldy	$04ff
	beq	.end
	tya
	clc
	adc	#$03
	tax
	lda	$0500,x
	cpy	#$03
	bne	.end
	and	#%11100011
.end	sta	$07f0

;------------------------------

	inc	<$00

	rti

;-------------------------------------------------------------------------------

	.org	$f200

IRQ:
	sta	$e000		; IRQ disable

	ldx	#$14
.loop	dex
	bne	.loop

	jsr	Set2003Loop

	rti

;------------------------------

Set2003Loop:
	ldx	#$ed
.loop
	jsr	Set2003
	cpx	#$ff
	beq	.end

	lda	#17
	sec
.wait	sbc	#1
	bne	.wait

	jsr	Set2003
	cpx	#$ff
	beq	.end

	lda	#17
	sec
.wait2	sbc	#1
	bne	.wait2

	jsr	Set2003
	cpx	#$ff
	beq	.end

	lda	#15
	sec
.wait3	sbc	#1
	bne	.wait3
	nop
	nop
	nop

	jmp	.loop

.end
	lda	$06fe
	sta	$06ff

	rts


Set2003:
	lda	$06ff
	sta	$2003
	dex
	rts

;-------------------------------------------------------------------------------

SetSprData:
	ldx	#$00
	ldy	$04ff
	lda	<$24
	and	#%11000000
	beq	*+7
	ldx	$04ff
	ldy	#$00
	sty	$06fe

	ldy	#$00
.loop	txa
	and	#%00000011
	cmp	#$02
	bne	*+7
	lda	#%11100011	; Reproduce non-existent bits of attribute bytes
	jmp	.sub+2
.sub	lda	#%11111111
	and	$0500,x
	sta	$0700,y
	inx
	iny
	tya
	and	#%00000011
	bne	.loop
	bit	<$24
	bpl	*+4
	tya
	tax
	cpy	#$0c
	bcc	.loop
	rts

;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------

	.org	$f300

BankTable:
	.db $08,$0a,$0c,$0d,$0e,$0f,$00,$00	;8x8
	.db $00,$02,$04,$05,$06,$07,$00,$00	;8x16

BankSet:
	clc
	adc	#$07
	tax
	ldy	#$07
.loop
	lda	BankTable,x
	sty	$8000
	sta	$8001
	dex
	dey
	bpl	.loop
	rts

;-------------------------------------------------------------------------------

GetKey:
	lda	<$24
	pha
	lda	#$01
	sta	<$24
	sta	$4016
	lsr	A
	sta	$4016
.loop
	lda	$4016
	lsr	A
	rol	<$24
	bcc	.loop
	pla
	eor	<$24
	and	<$24
	sta	<$25

	sta	<$26
	bne	.end
	lda	<$24
	beq	.end
	ldx	<$27
	cpx	#$10
	bcs	*+5
	inc	<$27
	rts
	sta	<$26
	rts
.end	lda	#$00
	sta	<$27
	rts

;-------------------------------------------------------------------------------

DrawDigits:
.loop	lda	$04ff,y
	lsr	A
	lsr	A
	lsr	A
	lsr	A
	sta	$2007
	lda	$04ff,y
	and	#%00001111
	sta	$2007
	iny
	dex
	bne	.loop
	rts

;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------

	.org	$e800
PaletteData:
	.db	$0f,$0f,$00,$30,$0f,$0f,$00,$10,$0f,$0f,$00,$00,$0f,$0f,$00,$24
	.db	$0f,$00,$10,$30,$0f,$11,$21,$30,$0f,$15,$25,$30,$0f,$1a,$2a,$30

BGAttrData:
	.db	$ff,$ff,$00,$00,$55,$55,$00,$00

	.org	$e900
SpriteData:
	.db	$00,$00,$60,$01, $60,$58,$60,$6c, $60,$64,$00,$78
	.org	$e9f0
	.db	$ff,$f9,$00,$f0, $0f,$fb,$00,$ff, $0f,$fb,$40,$ff, $0f,$fd,$00,$ff	;cursor

	.org	$fffa

	.dw	NMI,RESET,IRQ

	.bank	2
	.org	$0000

	.incbin	"sprite_evaluation_test.chr"
