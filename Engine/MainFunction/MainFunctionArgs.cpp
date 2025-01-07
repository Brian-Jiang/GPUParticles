// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "MainFunctionArgs.h"

#include "PrimeEngine/Utils/PEClassDecl.h"

#if APIABSTRACTION_IOS
	#import <Foundation/NSBundle.h>
    #import <Foundation/NSString.h>
#endif

#if PE_PLAT_IS_WIN32
MainFunctionArgs::MainFunctionArgs(PE::GameContext &context, PE::MemoryArena arena, const char *lpCmdLine, HINSTANCE hInstance)
#else
MainFunctionArgs::MainFunctionArgs(PE::GameContext &context, PE::MemoryArena arena, const char *lpCmdLine)
#endif
: m_args(context, arena, 4)
{

	#if PE_PLAT_IS_WIN32
		m_hInstance = hInstance;
	#endif

	// need to create platform specific object
    #if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11

		// Path provided is path to GameProject folder
		// it will be used to locate files
		StringOps::writeToString(".\\", m_gameProjRoot, 256);
    #endif

	#if PE_PLAT_IS_PSVITA
		StringOps::writeToString("app0:", m_gameProjRoot, 256);
		//StringOps::writeToString("host0:", m_gameProjRoot, 256); // host0: maps to file serving directory. // host:C:/path can be used for absolute paths
	#endif

    #if APIABSTRACTION_OGL

        #if APIABSTRACTION_PS3
            StringOps::writeToString("/app_home/", m_gameProjRoot, 256);
        #elif APIABSTRACTION_IOS
            NSString *vertShaderPathname;
            vertShaderPathname = [[NSBundle mainBundle] resourcePath];
            char * cStr       = (char *)[vertShaderPathname UTF8String];
            char buf[256];
            sprintf(buf, "%s/", cStr);
            StringOps::writeToString(buf, m_gameProjRoot, 256);
        #else
            StringOps::writeToString(".\\", m_gameProjRoot, 256);
        #endif

    #endif

	if (false) // debug output
	{
		PEINFO("%s\n", lpCmdLine);
	}

	char const * args[1] = {lpCmdLine};
	setArgs(1, &args[0]);
}

const char *MainFunctionArgs::gameProjRoot()
{
    return m_gameProjRoot;
}

PrimitiveTypes::Bool MainFunctionArgs::hasArgs()
{
    return m_args.m_size > 0;
}

const char *MainFunctionArgs::getArg(PrimitiveTypes::UInt32 i)
{
    return m_args[i].getCStrPtr();
}

void MainFunctionArgs::setArgs(int argc, char const **argv)
{
	for (PrimitiveTypes::Int32 i = 0; i < argc; i++)
	{
		m_args.add(PEString(*m_pContext, m_arena));
		m_args[i].set(argv[i]);
	}
}


