.MODEL FLAT ;seems like MASM also allows , STDCALL in here and will mangle function names appropriately, but I don't use that


.data
; //extern vars
PUBLIC ?number_cpp_extern@@3HA
PUBLIC _number_c_extern

ALIGN 4
?number_cpp_extern@@3HA DWORD 5

ALIGN 16
_number_c_extern DWORD 7

.code

_testfunc_c_cdecl PROC ; this is c cdecl function
; mov eax, number
; add eax, 34
; mov number, eax
ret
_testfunc_c_cdecl ENDP


_testfunc_c_stdcall@0 PROC ; this is c stdcall function
ret
_testfunc_c_stdcall@0 ENDP


@testfunc_c_fastcall@0 PROC ; this is c fastcall function
ret

@testfunc_c_fastcall@0 ENDP

END