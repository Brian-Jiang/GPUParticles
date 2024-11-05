.MODEL FLAT

.data

number DWORD 5
.code

testfunc PROC
; mov eax, number
add eax, 34
; mov number, eax
testfunc ENDP

END