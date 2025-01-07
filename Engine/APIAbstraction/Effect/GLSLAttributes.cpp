
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"


#include "GLSLAttributes.h"
#include "Effect.h"

namespace PE {

void GLSLAttributeLocations::findAttrBinding(Components::Effect *pEffect, ApiBindingType &resBinding, const char *name)
{
#if APIABSTRACTION_IOS
	resBinding = glGetAttribLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s attribute location: %d", name, vIn_iPosL_Attrib);
#elif PE_PLAT_IS_PSVITA
#endif
}

void GLSLAttributeLocations::initialize(Components::Effect *pEffect)
{
#if PE_PLAT_IS_IOS
	findAttrBinding(pEffect, vIn_iPosL_Attrib, "vIn_iPosL");
	findAttrBinding(pEffect, vIn_iTexCoord_Attrib, "vIn_iTexCoord");
	findAttrBinding(pEffect, vIn_iNormal_Attrib, "vIn_iNormal");
	findAttrBinding(pEffect, vIn_iTangent_Attrib, "vIn_iTangent");
	findAttrBinding(pEffect, vIn_iColor_Attrib, "vIn_iColor");
	findAttrBinding(pEffect, vIn_jointWeights_Attrib, "vIn_jointWeights");
	findAttrBinding(pEffect, vIn_jointWeights1_Attrib, "vIn_jointWeights1");
	findAttrBinding(pEffect, vIn_jointIndices_Attrib, "vIn_jointIndices");
	findAttrBinding(pEffect, vIn_jointIndices1_Attrib, "vIn_jointIndices1");
#endif
}

}; //namespace PE

