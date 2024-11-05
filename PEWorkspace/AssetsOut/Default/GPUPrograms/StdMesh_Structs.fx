#ifndef HLSL_STDMESH_STRUCTS
#define HLSL_STDMESH_STRUCTS

#include "APIAbstraction.gpu"

// Vertex Shader  Input Structure /////////////////////////////////////////////////////////////////////////

struct STD_MESH_VS_IN
{
    float3 iPosL            API_SEMANTIC(VSIN_POSITION);
    float2 iTexCoord        API_SEMANTIC(TEXCOORD);
    float3 iNormal          API_SEMANTIC(VSIN_NORMAL); // CGGL: tc2
};

struct STD_MESH_PS_IN
{
    float4 iPosH            API_SEMANTIC(PIPELINE_POSITION); //cggl: position
    float3 iPosW            API_SEMANTIC(PSIN_EXTRA_POSITION); // cggl: tc3
    float2 iTexCoord        API_SEMANTIC(TEXCOORD); // cggl tc0
    float3 iNormalW         API_SEMANTIC(PIPELINE_NORMAL); // cggl: tc2
};

struct STD_MESH_SHADOWED_PS_IN
{
    float4 iPosH            API_SEMANTIC(PIPELINE_POSITION); //cggl: position
    float3 iPosW            API_SEMANTIC(PSIN_EXTRA_POSITION); // cggl: tc3
    float2 iTexCoord        API_SEMANTIC(TEXCOORD); // cggl tc0
    float3 iNormalW         API_SEMANTIC(PIPELINE_NORMAL); // cggl: tc2
    float4 iProjTexCoord    API_SEMANTIC(TEXCOORD1);// cggl: tc4
};

// Vertex Shader Function Definition //////////////////////////////////////////////////////////////////////

#if APIABSTRACTION_IOS

    //vertex inputs for IOS. will compile only for vertex shaders
    #if APIABSTRACTION_IOS_VERTEX
        attribute vec3 vIn_iPosL;
        attribute vec2 vIn_iTexCoord;
        attribute vec3 vIn_iNormal;
		
        #define STD_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
            vIn.iPosL = vIn_iPosL; \
            vIn.iTexCoord = vIn_iTexCoord; \
            vIn.iNormal = vIn_iNormal;

        //used to convert run-time vertex shader values to fragment input
    	#define VS_RETURN_STD_MESH_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn) \
        	gl_Position = pIn.iPosH; \
        	pIn_iPosW = pIn.iPosW; \
        	pIn_iTexCoord = pIn.iTexCoord; \
        	pIn_iNormalW = pIn.iNormalW;

		#define VS_RETURN_STD_MESH_SHADOWED_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn) \
        	gl_Position = pIn.iPosH; \
        	pIn_iPosW = pIn.iPosW; \
        	pIn_iTexCoord = pIn.iTexCoord; \
        	pIn_iNormalW = pIn.iNormalW; \
			pIn_iProjTexCoord = pIn.iProjTexCoord;

		#define VS_wrapper_STD_MESH(func) \
            void main() { \
                STD_MESH_VS_IN vIn; \
                STD_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn); \
                STD_MESH_PS_IN pIn; \
                pIn = func(vIn); \
                VS_RETURN_STD_MESH_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn); \
            }
		#define VS_wrapper_STD_MESH_SHADOWED(func) \
            void main() { \
                STD_MESH_VS_IN vIn; \
                STD_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn); \
                STD_MESH_SHADOWED_PS_IN pIn; \
                pIn = func(vIn); \
                VS_RETURN_STD_MESH_SHADOWED_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn); \
            }
    #endif
#else
	#define VS_wrapper_STD_MESH(func) \
        STD_MESH_PS_IN main(STD_MESH_VS_IN API_NOSTRIP vIn) { \
            STD_MESH_PS_IN pIn; \
            pIn = func(vIn); \
            return pIn; \
        }
	#define VS_wrapper_STD_MESH_SHADOWED(func) \
        STD_MESH_SHADOWED_PS_IN main(STD_MESH_VS_IN API_NOSTRIP vIn) { \
            STD_MESH_SHADOWED_PS_IN pIn; \
            pIn = func(vIn); \
            return pIn; \
        }
#endif


// Pixel Shader Function Defintion ////////////////////////////////////////////////////////////////////////////
#if APIABSTRACTION_IOS
    
    //vertex output = fragment input - will compile for both vertex and fragment shaders
    //varying float4 pIn_iPosH; use gl_Position
    varying float3 pIn_iPosW;
    varying float2 pIn_iTexCoord;
    varying float3 pIn_iNormalW;
	varying float4 pIn_iProjTexCoord;

    #define STD_MESH_PS_IN_FILL_FROM_VARYINGS(pIn) \
        /*pIn.iPosH = gl_Position;*/ \
        pIn.iPosW = pIn_iPosW; \
        pIn.iTexCoord = pIn_iTexCoord; \
        pIn.iNormalW = pIn_iNormalW;

	#define STD_MESH_SHADOWED_PS_IN_FILL_FROM_VARYINGS(pIn) \
        /*pIn.iPosH = gl_Position;*/ \
        pIn.iPosW = pIn_iPosW; \
        pIn.iTexCoord = pIn_iTexCoord; \
        pIn.iNormalW = pIn_iNormalW; \
		pIn.iProjTexCoord = pIn_iProjTexCoord;

    #define PS_wrapper_STD_MESH(func) \
        void main() { \
        	STD_MESH_PS_IN pIn; \
        	STD_MESH_PS_IN_FILL_FROM_VARYINGS(pIn); \
        	gl_FragColor = func(pIn); \
        }
	#define PS_wrapper_STD_MESH_SHADOWED(func) \
        void main() { \
        	STD_MESH_SHADOWED_PS_IN pIn; \
        	STD_MESH_SHADOWED_PS_IN_FILL_FROM_VARYINGS(pIn); \
        	gl_FragColor = func(pIn); \
        }
#else
    #define PS_wrapper_STD_MESH(func) \
        float4 main(STD_MESH_PS_IN pIn) : PSOUT_COLOR \
        { \
            return func(pIn); \
        }
	#define PS_wrapper_STD_MESH_SHADOWED(func) \
        float4 main(STD_MESH_SHADOWED_PS_IN pIn) : PSOUT_COLOR \
        { \
            return func(pIn); \
        }
#endif

#endif // file guard
