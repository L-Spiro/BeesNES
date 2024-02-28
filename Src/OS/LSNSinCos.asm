.code

PUBLIC sincos
; void sincos( double _dAngle, double * _pdSin, double * _pdCos );

dStackAngle$ = 8

sincos PROC

	movsd QWORD PTR dStackAngle$[rsp], xmm0 ; argument angle is in xmm0, move it to the stack
	fld QWORD PTR dStackAngle$[rsp]         ; push angle onto the FPU stack where we can do FLOPs
	fsincos
	fstp QWORD PTR [r8]  ; store/pop cosine output argument 
	fstp QWORD PTR [rdx] ; store/pop sine output argument
	ret 0

sincos ENDP

END