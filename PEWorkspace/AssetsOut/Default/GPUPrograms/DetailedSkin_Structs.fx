#ifndef HLSL_DETAILED_SKIN_STRUCTS
#define HLSL_DETAILED_SKIN_STRUCTS

#include "DetailedMesh_Structs.fx"

//------------------------------------------------------------------------------
// structures
//------------------------------------------------------------------------------


struct DETAILED_SKIN_VS_IN
{
	float3 iPosL        API_SEMANTIC(VSIN_POSITION);
	float4 jointWeights  API_SEMANTIC(VSIN_JOINTWEIGHTS0);
	#if DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8
		float4 jointWeights1 API_SEMANTIC(VSIN_JOINTWEIGHTS1);
	#endif

	float4 jointIndices  API_SEMANTIC(VSIN_BONEINDICES0);
	#if DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8
		float4 jointIndices1 API_SEMANTIC(VSIN_BONEINDICES1);
	#endif

	float2 iTexCoord    API_SEMANTIC(TEXCOORD0);
	float3 iNormal      API_SEMANTIC(VSIN_NORMAL);
	float3 iTangent     API_SEMANTIC(VSIN_TANGENT);
};


#if APIABSTRACTION_D3D11
struct DETAILED_SKIN_INSTANCED_VS_IN
{
	float3 iPosL        API_SEMANTIC(VSIN_POSITION);
	float4 jointWeights  API_SEMANTIC(VSIN_JOINTWEIGHTS0);
	#if DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8
		float4 jointWeights1 API_SEMANTIC(VSIN_JOINTWEIGHTS1);
	#endif

	float4 jointIndices  API_SEMANTIC(VSIN_BONEINDICES0);
	#if DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8
		float4 jointIndices1 API_SEMANTIC(VSIN_BONEINDICES1);
	#endif
	float2 iTexCoord    API_SEMANTIC(TEXCOORD0);
	float3 iNormal      API_SEMANTIC(PIPELINE_NORMAL);
	float3 iTangent     API_SEMANTIC(VSIN_TANGENT);
	unsigned int iInstanceId  API_SEMANTIC(SV_InstanceID);
};
#endif

#if APIABSTRACTION_IOS

    //vertex inputs for IOS. will compile only for vertex shaders
    #if APIABSTRACTION_IOS_VERTEX
        //attributes already in DETAILED_MESH_VS_IN:
        //attribute vec3 vIn_iPosL;
        //attribute vec2 vIn_iTexCoord;
        //attribute vec3 vIn_iNormal;
        //attribute vec3 vIn_iTangent;

        //additional attributes

        attribute vec4 vIn_jointWeights;
		#if DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8
	        attribute vec4 vIn_jointWeights1;
		#endif
        attribute vec4 vIn_jointIndices;
        
		#if DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8
			attribute vec4 vIn_jointIndices1;
		#endif

		#if DEFAULT_SKIN_WEIGHTS_PER_VERTEX == 8
			#define DETAILED_SKIN_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
				DETAILED_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
				vIn.jointWeights = vIn_jointWeights; \
				vIn.jointWeights1 = vIn_jointWeights1; \
				vIn.jointIndices = vIn_jointIndices; \
				vIn.jointIndices1 = vIn_jointIndices1;
		#else
			#define DETAILED_SKIN_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
				DETAILED_MESH_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn) \
				vIn.jointWeights = vIn_jointWeights; \
				vIn.jointIndices = vIn_jointIndices; 
		#endif

//since DetailedSkin_Shadowed_VS has the same VS output as DetailedMesh_Shadowed_VS, we reuse the same macro defined in DetailedMesh_Structs.fx
//#define VS_RETURN_DETAILED_MESH_PS_IN_FILL_SEPARATE_ATTRIBUTES_FROM_STRUCT(pIn) \

// shader function entry and exit decalration /////////////////////////////////////////

    #define VS_wrapper_DETAILED_SKIN(func) \
        void main() { \
            DETAILED_SKIN_VS_IN vIn; \
            DETAILED_SKIN_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn); \
            DETAILED_MESH_PS_IN pIn; \
            pIn = func(vIn); \
            VS_RETURN_DETAILED_MESH_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn); \
        }
    #define VS_wrapper_DETAILED_SKIN_SHADOWED(func) \
        void main() { \
            DETAILED_SKIN_VS_IN vIn; \
            DETAILED_SKIN_VS_IN_FILL_FROM_SEPARATE_ATTRIBUTES(vIn); \
            DETAILED_MESH_SHADOWED_PS_IN pIn; \
            pIn = func(vIn); \
            VS_RETURN_DETAILED_MESH_SHADOWED_PS_IN_FILL_VARYINGS_FROM_STRUCT(pIn); \
        }

    #endif    
#else

// shader function entry and exit decalration /////////////////////////////////////////////

    #define VS_wrapper_DETAILED_SKIN(func) \
        DETAILED_MESH_PS_IN main(DETAILED_SKIN_VS_IN API_NOSTRIP vIn) { \
        DETAILED_MESH_PS_IN pIn; \
        pIn = func(vIn); \
        return pIn; \
    }

	
    #define VS_wrapper_DETAILED_SKIN_SHADOWED(func) \
        DETAILED_MESH_SHADOWED_PS_IN main(DETAILED_SKIN_VS_IN API_NOSTRIP vIn) { \
        DETAILED_MESH_SHADOWED_PS_IN pIn; \
        pIn = func(vIn); \
        return pIn; \
    }

	#define VS_wrapper_DETAILED_SKIN_INSTANCED(func) \
        DETAILED_MESH_PS_IN main(DETAILED_SKIN_INSTANCED_VS_IN vIn) { \
        DETAILED_MESH_PS_IN pIn; \
        pIn = func(vIn); \
        return pIn; \
    }
	#define VS_wrapper_DETAILED_SKIN_SHADOWED_INSTANCED(func) \
        DETAILED_MESH_SHADOWED_PS_IN main(DETAILED_SKIN_INSTANCED_VS_IN vIn) { \
        DETAILED_MESH_SHADOWED_PS_IN pIn; \
        pIn = func(vIn); \
        return pIn; \
    }
#endif

// we dont have DetailedSkin_PS since it is teh same as DetailedMesh_PS

#endif














































































