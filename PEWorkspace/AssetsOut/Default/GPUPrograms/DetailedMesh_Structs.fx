#ifndef HLSL_DETAILEDMESH_STRUCTS
#define HLSL_DETAILEDMESH_STRUCTS

#include "StdMesh_Structs.fx"

//------------------------------------------------------------------------------
// structures
//------------------------------------------------------------------------------

struct DETAILED_MESH_VS_IN
{
    float3 iPosL      API_SEMANTIC(VSIN_POSITION);
    float2 iTexCoord  API_SEMANTIC(TEXCOORD0);
    float3 iNormal    API_SEMANTIC(VSIN_NORMAL);
    float3 iTangent   API_SEMANTIC(VSIN_TANGENT);
};

//struct DETAILED_MESH_INSTANCED_VS_IN
//{
//    float3 iPosL      API_SEMANTIC(VSIN_POSITION);
//    float2 iTexCoord  API_SEMANTIC(TEXCOORD0);
//    float3 iNormal    API_SEMANTIC(VSIN_NORMAL);
//    float3 iTangent   API_SEMANTIC(VSIN_TANGENT);
//    uint iInstanceId   API_SEMANTIC(SV_INSTANCEID);
//};

#if APIABSTRACTION_D3D11
//todo: port to all platforms
struct DETAILED_MESH_INSTANCED_VS_IN
{
    float3 iPosL      API_SEMANTIC(VSIN_POSITION);
    float2 iTexCoord  API_SEMANTIC(TEXCOORD0);
    float3 iNormal    API_SEMANTIC(VSIN_NORMAL);
    float3 iTangent   API_SEMANTIC(VSIN_TANGENT);
	unsigned int iInstanceId  API_SEMANTIC(SV_InstanceID);
};
#endif

#if APIABSTRACTION_IOS

    //vertex inputs for IOS. will compile only for vertex shaders
    #if APIABSTRACTION_IOS_VERTEX
        //attributes already in STD_MESH_VS_IN:
        //attribute vec3 vIn_iPosL;
        //attribute vec2 vIn_iTexCoord;
        //attribute vec3 vIn_iNormal;

        //additional attributes:
        attribute vec3 vIn_iTangent;

        #define DETAILED_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
            STD_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
            vIn.iTangent = vIn_iTangent;

        //used to convert run-time vertex shader values to fragment input
        #define VS_RETURN_DETAILED_MESH_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn) \
            VS_RETURN_STD_MESH_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn) \
            pIn_iTangentW = pIn.iTangentW;
       #define VS_RETURN_DETAILED_MESH_SHADOWED_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn) \
            VS_RETURN_STD_MESH_SHADOWED_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn) \
            pIn_iTangentW = pIn.iTangentW;


        // shader function entry and exit decalration /////////////////////////////////////////

        #define VS_wrapper_DETAILED_MESH(func) \
            void main() { \
                DETAILED_MESH_VS_IN vIn; \
                DETAILED_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn); \
                DETAILED_MESH_PS_IN pIn; \
                pIn = func(vIn); \
                VS_RETURN_DETAILED_MESH_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn); \
            }
            
        #define VS_wrapper_DETAILED_MESH_SHADOWED(func) \
            void main() { \
                DETAILED_MESH_VS_IN vIn; \
                DETAILED_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn); \
                DETAILED_MESH_SHADOWED_PS_IN pIn; \
                pIn = func(vIn); \
                VS_RETURN_DETAILED_MESH_SHADOWED_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn); \
            }

    #endif    
#else
    
    // shader function entry and exit decalration /////////////////////////////////////////////

    #define VS_wrapper_DETAILED_MESH(func) \
        DETAILED_MESH_PS_IN main(DETAILED_MESH_VS_IN API_NOSTRIP vIn) { \
            DETAILED_MESH_PS_IN pIn; \
            pIn = func(vIn); \
            return pIn; \
        }
	#define VS_wrapper_DETAILED_MESH_SHADOWED(func) \
        DETAILED_MESH_SHADOWED_PS_IN main(DETAILED_MESH_VS_IN API_NOSTRIP vIn) { \
            DETAILED_MESH_SHADOWED_PS_IN pIn; \
            pIn = func(vIn); \
            return pIn; \
        }

    #define VS_wrapper_DETAILED_MESH_PARTICLE(func) \
        DETAILED_MESH_SHADOWED_PS_IN main(DETAILED_MESH_INSTANCED_VS_IN API_NOSTRIP vIn) { \
            DETAILED_MESH_SHADOWED_PS_IN pIn; \
            pIn = func(vIn); \
            return pIn; \
        }


#endif

struct DETAILED_MESH_PS_IN
{
    float4 iPosH     API_SEMANTIC(PIPELINE_POSITION);
    float3 iPosW     API_SEMANTIC(PSIN_EXTRA_POSITION);
    float2 iTexCoord API_SEMANTIC(TEXCOORD0);
    float3 iNormalW  API_SEMANTIC(PIPELINE_NORMAL);
    float3 iTangentW API_SEMANTIC(PSIN_TANGENT);
};

struct DETAILED_MESH_SHADOWED_PS_IN
{
    float4 iPosH     API_SEMANTIC(PIPELINE_POSITION);
    float3 iPosW     API_SEMANTIC(PSIN_EXTRA_POSITION);
    float2 iTexCoord API_SEMANTIC(TEXCOORD0);
    float3 iNormalW  API_SEMANTIC(PIPELINE_NORMAL);
    float4 iProjTexCoord API_SEMANTIC(PSIN_PROJECTION);
    float3 iTangentW API_SEMANTIC(PSIN_TANGENT);
};

#if APIABSTRACTION_IOS
    //vertex output = fragment input - will compile for both vertex and fragment shaders
    //varyings already in STD_MESH_SHADOWED_PS_IN:
    //varying highp vec3 pIn_iPosH;
    //varying highp vec3 pIn_iPosW;
    //varying mediump vec2 pIn_iTexCoord;
    //varying highp vec3 pIn_iNormalW;
    //varying vec4 pIn_iProjTexCoord

    //additioonal varyings:
    varying highp vec3 pIn_iTangentW;

    #define DETAILED_MESH_PS_IN_FILL_FROM_VARYINGS(pIn) \
        STD_MESH_PS_IN_FILL_FROM_VARYINGS(pIn) \
        pIn.iTangentW = pIn_iTangentW;
    #define DETAILED_MESH_SHADOWED_PS_IN_FILL_FROM_VARYINGS(pIn) \
        STD_MESH_SHADOWED_PS_IN_FILL_FROM_VARYINGS(pIn) \
        pIn.iTangentW = pIn_iTangentW;
		        
    #define PS_wrapper_DETAILED_MESH(func) \
    	void main() { \
			DETAILED_MESH_PS_IN pIn; \
        	DETAILED_MESH_PS_IN_FILL_FROM_VARYINGS(pIn); \
        	gl_FragColor = func(pIn); \
        }

    #define PS_wrapper_DETAILED_MESH_SHADOWED(func) \
    	void main() { \
			DETAILED_MESH_SHADOWED_PS_IN pIn; \
        	DETAILED_MESH_SHADOWED_PS_IN_FILL_FROM_VARYINGS(pIn); \
        	gl_FragColor = func(pIn); \
        }
#else
    #define PS_wrapper_DETAILED_MESH(func) \
        float4 main(DETAILED_MESH_PS_IN pIn) : PSOUT_COLOR \
        { \
            return func(pIn); \
        }
    #define PS_wrapper_DETAILED_MESH_SHADOWED(func) \
        float4 main(DETAILED_MESH_SHADOWED_PS_IN pIn) : PSOUT_COLOR \
        { \
            return func(pIn); \
        }
#endif

#endif














































































