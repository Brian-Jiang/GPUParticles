#ifndef HLSL_MINIMALMESH_STRUCTS
#define HLSL_MINIMALMESH_STRUCTS

#include "APIAbstraction.gpu"


// Vertex Shader  Input Structure /////////////////////////////////////////////////////////////////////////

struct COLORED_MINIMAL_MESH_VS_IN
{
    float3 iPosL        API_SEMANTIC(VSIN_POSITION);
    float3 iColor       API_SEMANTIC(VSIN_COLOR);
};

struct COLORED_MINIMAL_MESH_PS_IN
{
    float4 iPosH	API_SEMANTIC(PIPELINE_POSITION);
    float3 iPosW	API_SEMANTIC(PSIN_EXTRA_POSITION);
    float3 iColor   API_SEMANTIC(PSIN_COLOR);
};

// Vertex Shader Function Definition //////////////////////////////////////////////////////////////////////

#if APIABSTRACTION_IOS

    //vertex inputs for IOS. will compile only for vertex shaders
    #if APIABSTRACTION_IOS_VERTEX
        attribute vec3 vIn_iPosL;
        attribute vec3 vIn_iColor;

        #define COLORED_MINIMAL_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
            vIn.iPosL = vIn_iPosL; \
            vIn.iColor = vIn_iColor;

        //used to convert run-time vertex shader values to fragment input
    	#define VS_RETURN_COLORED_MINIMAL_MESH_PS_IN_FILL_SEPARATE_ATTRIBUTES_FROM_STRUCT(pIn) \
        	gl_Position = pIn.iPosH; \
        	pIn_iPosW = pIn.iPosW; \
        	pIn_iColor = pIn.iColor;

		#define VS_wrapper_COLORED_MINIMAL_MESH(func) \
		    void main() { \
                COLORED_MINIMAL_MESH_VS_IN vIn; \
                COLORED_MINIMAL_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn); \
				COLORED_MINIMAL_MESH_PS_IN pIn; \
                pIn = func(vIn); \
                VS_RETURN_COLORED_MINIMAL_MESH_PS_IN_FILL_SEPARATE_ATTRIBUTES_FROM_STRUCT(pIn); \
            }
            
    #endif
#else
	#define VS_wrapper_COLORED_MINIMAL_MESH(func) \
        COLORED_MINIMAL_MESH_PS_IN main(COLORED_MINIMAL_MESH_VS_IN API_NOSTRIP vIn) { \
            COLORED_MINIMAL_MESH_PS_IN pIn; \
            pIn = func(vIn); \
            return pIn; \
        }
#endif


// Pixel Shader Function Defintion ////////////////////////////////////////////////////////////////////////////
#if APIABSTRACTION_IOS
    
    //vertex output = fragment input - will compile for both vertex and fragment shaders
    //varying float4 pIn_iPosH; use gl_Position
    varying float3 pIn_iPosW;
    varying float3 pIn_iColor;

    #define COLORED_MINIMAL_MESH_PS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(pIn) \
        /*pIn.iPosH = gl_Position;*/ \
        pIn.iPosW = pIn_iPosW; \
        pIn.iColor = pIn_iColor;

    #define PS_wrapper_COLORED_MINIMAL_MESH(func) \
        void main() { \
        	COLORED_MINIMAL_MESH_PS_IN pIn; \
        	COLORED_MINIMAL_MESH_PS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(pIn); \
        	gl_FragColor = func(pIn); \
        }
    
#else
    #define PS_wrapper_COLORED_MINIMAL_MESH(func) \
        float4 main(COLORED_MINIMAL_MESH_PS_IN pIn) : PSOUT_COLOR \
        { \
            return func(pIn); \
        }

#endif

#endif // file guard
