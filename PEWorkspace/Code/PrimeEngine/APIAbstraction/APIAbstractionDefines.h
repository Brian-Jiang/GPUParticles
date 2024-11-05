#ifndef __PYENGINE_2_0_APIABSTRACTIONDEFINES_H__
#define __PYENGINE_2_0_APIABSTRACTIONDEFINES_H__
// This controls which source files are compiled

// API defines, only one can be enabled for this class
// Note this will only be effective if there are no preset #define's by configuration
// for example Debug_DX11 configuration #defines APIABSTRACTION_D3D11

// Possisble abstraction combinations:

// APIABSTRACTION_D3D9 - PC DX9

// APIABSTRACTION_OGL AND APIABSTRACTION_GLPC - PC OpenGL

// APIABSTRACTION_OGL AND APIABSTRACTION_IOS iOS

#ifndef PE_64_BIT
#define PE_64_BIT 0
#endif

#ifndef APIABSTRACTION_D3D11
#	define APIABSTRACTION_D3D11 0
#endif

#ifndef APIABSTRACTION_D3D9
#	define APIABSTRACTION_D3D9 0
#endif

#ifndef APIABSTRACTION_IOS
#	define APIABSTRACTION_IOS 0
#endif

#ifndef APIABSTRACTION_GLPC
#	define APIABSTRACTION_GLPC 0
#endif



#	define OGL_USE_VERTEX_BUFFER_ARRAYS 0



#if APIABSTRACTION_IOS || APIABSTRACTION_GLPC || APIABSTRACTION_PS3
#	define APIABSTRACTION_OGL 1
#endif

#ifndef APIABSTRACTION_OGL
#	define APIABSTRACTION_OGL 0
#endif

#ifndef APIABSTRACTION_PS3
#	define APIABSTRACTION_PS3 0
#endif


#if APIABSTRACTION_D3D9
	#define APIABSTRACTION_D3D9_PC 1
#endif

#ifndef APIABSTRACTION_D3D9_PC
	#define APIABSTRACTION_D3D9_PC 0
#endif

#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
	#define APIABSTRACTION_PC 1
#endif

#ifndef APIABSTRACTION_PC
	#define APIABSTRACTION_PC 0
#endif

#if APIABSTRACTION_D3D9
	#define D3D9_USE_RENDER_TO_SURFACE 0// set to 1 to use for older cards
#endif

// Other important defines
#define NOMINMAX

//define to disable vsync and allow for more detailed gpu draw call measurement

#define PE_ENABLE_GPU_PROFILING 0 //(!APIABSTRACTION_OGL)


#define PE_DETAILED_GPU_PROFILING 1

#if APIABSTRACTION_D3D11
	#define API_CHOOSE_DX11_DX9_OGL(dx11, dx9, ogl) (dx11)
#elif APIABSTRACTION_D3D9
	#define API_CHOOSE_DX11_DX9_OGL(dx11, dx9, ogl) (dx9)
#else
	#define API_CHOOSE_DX11_DX9_OGL(dx11, dx9, ogl) (ogl)
#endif




#if APIABSTRACTION_D3D11
#define API_CHOOSE_DX11_DX9_OGL_PSVITA_NO_PAREN(dx11, dx9, ogl, vita) dx11
#elif APIABSTRACTION_D3D9
#define API_CHOOSE_DX11_DX9_OGL_PSVITA_NO_PAREN(dx11, dx9, ogl, vita) dx9
#elif PE_PLAT_IS_PSVITA
#define API_CHOOSE_DX11_DX9_OGL_PSVITA_NO_PAREN(dx11, dx9, ogl, vita) vita
#else
#define API_CHOOSE_DX11_DX9_OGL_PSVITA_NO_PAREN(dx11, dx9, ogl, vita) ogl
#endif

#define API_CHOOSE_DX11_DX9_OGL_PSVITA(dx11, dx9, ogl, vita) ( API_CHOOSE_DX11_DX9_OGL_PSVITA_NO_PAREN(dx11, dx9, ogl, vita) )


#if APIABSTRACTION_D3D11
#define API_CHOOSE_DX11_DX9_OGL_IOS_PSVITA(dx11, dx9, ogl, ios, vita) (dx11)
#elif APIABSTRACTION_D3D9
#define API_CHOOSE_DX11_DX9_OGL_IOS_PSVITA(dx11, dx9, ogl, ios, vita) (dx9)
#elif PE_PLAT_IS_PSVITA
#define API_CHOOSE_DX11_DX9_OGL_IOS_PSVITA(dx11, dx9, ogl, ios, vita) (vita)
#elif PE_PLAT_IS_IOS
#define API_CHOOSE_DX11_DX9_OGL_IOS_PSVITA(dx11, dx9, ogl, ios, vita) (ios)
#else
#define API_CHOOSE_DX11_DX9_OGL_IOS_PSVITA(dx11, dx9, ogl, ios, vita) (ogl)
#endif


#if APIABSTRACTION_D3D11
	#define API_CHOOSE_TYPE_DX11_DX9_OGL(dx11, dx9, ogl) dx11
#elif APIABSTRACTION_D3D9
	#define API_CHOOSE_TYPE_DX11_DX9_OGL(dx11, dx9, ogl) dx9
#else
	#define API_CHOOSE_TYPE_DX11_DX9_OGL(dx11, dx9, ogl) ogl
#endif

#if APIABSTRACTION_OGL
	#define API_CHOOSE_DX_OGL(dx, ogl)  ogl
#else
	#define API_CHOOSE_DX_OGL(dx, ogl)  dx
#endif

#if APIABSTRACTION_D3D11
#define API_CHOOSE_DX11_ELSE(dx11, notdx11)  dx11
#else
#define API_CHOOSE_DX11_ELSE(dx11, notdx11)  notdx11
#endif

#if PE_PLAT_IS_PSVITA
#define API_CHOOSE_PS3_PSVITA_ELSE(ps3, psvita, _else) (psvita)
#elif APIABSTRACTION_PS3
#define API_CHOOSE_PS3_PSVITA_ELSE(ps3, psvita, _else) (ps3)
#else
#define API_CHOOSE_PS3_PSVITA_ELSE(ps3, psvita, _else) (_else)
#endif


#if APIABSTRACTION_D3D11
	#define API_CHOOSE_DX11_DX9(dx11, dx9) (dx11)
#elif APIABSTRACTION_D3D9
	#define API_CHOOSE_DX11_DX9(dx11, dx9) (dx9)
#endif

#ifdef __cplusplus
//could put osme templated code here..
#endif


#define PE_DX11_USE_STRUCTURED_BUFFER_INSTEAD_OF_TBUFFER 1


#define PE_API_BIT_D3D9 0x0001
#define PE_API_BIT_D3D11 0x0002
#define PE_API_BIT_GL 0x0004
#define PE_PLAT_BIT_WIN32 0x0100
#define PE_PLAT_BIT_XBOX360 0x0200
#define PE_PLAT_BIT_PS3 0x0400
#define PE_PLAT_BIT_IOS 0x0800

#ifndef PE_API_IS_D3D9
#define PE_API_IS_D3D9 0
#endif

#ifndef PE_API_IS_D3D11
#define PE_API_IS_D3D11 0
#endif


#ifndef PE_API_IS_GL
#define PE_API_IS_GL 0
#endif


#ifndef PE_PLAT_IS_WIN32
#define PE_PLAT_IS_WIN32 0
#endif


#ifndef PE_PLAT_IS_PS3
#define PE_PLAT_IS_PS3 0
#endif

#ifndef PE_PLAT_IS_IOS
#define PE_PLAT_IS_IOS 0
#endif

#if PE_PLAT_IS_IOS
#define MAX_TEXTURE_LOAD_WIDTH 512
#elif PE_PLAT_IS_PS3
#define MAX_TEXTURE_LOAD_WIDTH 512
#else
#define MAX_TEXTURE_LOAD_WIDTH 2048
#endif

#define DEBUG_DRAW_CALLS 0


#if APIABSTRACTION_PSVITA || APIABSTRACTION_IOS || PE_PLAT_IS_PS4
#define PE_INLINE inline
#else
#define PE_INLINE __forceinline
#endif

#endif
