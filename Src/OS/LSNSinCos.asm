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

PUBLIC sincosf
; void sincosf( float _fAngle, float * _pfSin, float * _pfCos );

fStackAngle$ = 4

sincosf PROC

	movss DWORD PTR fStackAngle$[rsp], xmm0 ; argument angle is in xmm0, move it to the stack
	fld DWORD PTR fStackAngle$[rsp]         ; push angle onto the FPU stack where we can do FLOPs
	fsincos
	fstp DWORD PTR [r8]  ; store/pop cosine output argument 
	fstp DWORD PTR [rdx] ; store/pop sine output argument
	ret 0

sincosf ENDP

END
