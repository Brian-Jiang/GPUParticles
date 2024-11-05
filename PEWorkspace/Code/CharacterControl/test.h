extern "C"
{
	void __cdecl testfunc_c_cdecl(void);
	void __stdcall testfunc_c_stdcall(void);
	void __fastcall testfunc_c_fastcall(void);

	
}

extern int number_cpp_extern;

extern "C" extern int number_c_extern;
