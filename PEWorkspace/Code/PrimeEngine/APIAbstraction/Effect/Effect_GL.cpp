#define NOMINMAX
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "Effect.h"

#include "EffectManager.h"

#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/Game/Common/GameContext.h"


namespace PE {

namespace Components {

#if APIABSTRACTION_OGL

void Effect::loadTechnique_GL(const char *vsFilename, const char *vsName, 
    const char *psFilename, const char *psName,
    const char *techniqueName,
    bool reload)
{
	lock();

	m_isReady = false;

	StringOps::writeToString(techniqueName, m_techName, 256);

	StringOps::writeToString(vsName, m_vsName, 256);
	StringOps::writeToString(vsFilename, m_vsFilename, 256);

	StringOps::writeToString(psName, m_psName, 256);
	StringOps::writeToString(psFilename, m_psFilename, 256);

	StringOps::writeToString("", m_gsName, 256);
	StringOps::writeToString("", m_gsFilename, 256);

	StringOps::writeToString("", m_csName, 256);
	StringOps::writeToString("", m_csFilename, 256);

	StringOps::writeToString("", m_spesFilename, 256);
	StringOps::writeToString("", m_spesName, 256);

	unlock();

	loadTechniqueAsync(); // calls loadTechniqueSync wither i=within function or from different thread
}

void Effect::loadTechniqueSync_GL()
{
	lock();
    
    #if !PE_API_IS_IOS
        GLRenderer *pGLRenderer = static_cast<GLRenderer *>(m_pContext->getGPUScreen());
    #endif
    
	if (StringOps::length(m_spesFilename)){return;}
	if (StringOps::length(m_gsFilename)){return;}
	if (StringOps::length(m_csFilename)){return;}

	//D3DXMACRO Shader_Macros[3] = { {"HLSL_SEPARATE_LOAD", "1"} , {"APIABSTRACTION_D3D9", "1"} , {0, 0}};

	if (StringOps::length(m_psFilename))
	{
		/* todo: don't compile pixel shaders
        int existingIndex = EffectManager::Instance()->m_pixelShaders.findIndex(m_psName);
         
        if (existingIndex != -1)
		{
			pPixelShader = EffectManager::Instance()->m_pixelShaders.m_pairs[existingIndex];
		}
		else
		*/
		{
			if (!glLoadShader(m_psFilename, m_psName,
				// Shader_Macros, 
#               if APIABSTRACTION_IOS
                    GL_FRAGMENT_SHADER,
                #else
                   pGLRenderer->m_cgFragmentProfile, // "ps_3_0",
#               endif
				m_cgFragmentProgram
				)) 
			{
				assert(!"Could not compile fragment program");
				unlock();
				return;
			}
#           if !APIABSTRACTION_IOS
                cgGLLoadProgram(m_cgFragmentProgram);
               pGLRenderer->checkForErrors("loading fragment program");
#           endif

		}
	}

	if (StringOps::length(m_vsFilename))
	{
		if (!glLoadShader(m_vsFilename, m_vsName,
			//Shader_Macros,
            #if APIABSTRACTION_IOS
                GL_VERTEX_SHADER,
            #else
				pGLRenderer->m_cgVertexProfile,
			#endif
			m_cgVertexProgram))
		{
			assert(!"Could not compile vertex program");
			unlock();
			return;
		}
        #if !APIABSTRACTION_IOS
            cgGLLoadProgram(m_cgVertexProgram);
			pGLRenderer->checkForErrors("loading vertex program");
        #endif

	}

#if APIABSTRACTION_IOS
	//m_pInputLayout = pVertexDeclaration;
	m_glslProgram = glCreateProgram();
    
    // Attach vertex shader to program.
    glAttachShader(m_glslProgram, m_cgVertexProgram);
    
    // Attach fragment shader to program.
    glAttachShader(m_glslProgram, m_cgFragmentProgram);
    
    
    glLinkProgram(m_glslProgram);
    
    #if defined(DEBUG)
        GLint logLength;
        glGetProgramiv(m_glslProgram, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(m_glslProgram, logLength, &logLength, log);
            NSLog(@"Program link log:\n%s", log);
            free(log);
        }
    #endif
    GLint status;
    glGetProgramiv(m_glslProgram, GL_LINK_STATUS, &status);
    assert(status);
    
    // Link program.
    if (status == 0)
    {
        NSLog(@"Failed to link program: %d", m_glslProgram);
        
        if (m_cgVertexProgram)
        {
            glDeleteShader(m_cgVertexProgram);
            m_cgVertexProgram = 0;
        }
        if (m_cgFragmentProgram)
        {
            glDeleteShader(m_cgFragmentProgram);
            m_cgFragmentProgram = 0;
        }
        if (m_glslProgram)
        {
            glDeleteProgram(m_glslProgram);
            m_glslProgram = 0;
        }
        
        return;
    }
#endif
	m_externalPerTechniqueData.initVertexAttributeBindings(this);
	m_externalPerTechniqueData.init(this);

	m_isReady = true;
	
	unlock();
}
    
int Effect::processIncludes(const char *source, const char*curFolderPath, const char *curFile, PrimitiveTypes::UInt32 curPos, Array<PEString> &allIncludes, Array<PEString> &filenames, Array<int> &lineCounts)
{
#if APIABSTRACTION_IOS
    NSLog(@"Processing:\n %s", curFile);
    int totalLen = strlen(source);
    GLchar * newStr = (char *)malloc(totalLen);
    GLchar *curDest = newStr;
    const GLchar *cur = source;
    int numIncludesAdded = 0;
    int numLines = 0;
    while(1)
    {
        const GLchar *wendl = strstr(cur, "\n\r");
        if (!wendl) wendl = (const GLchar *)(0xFFFFFFFF);
        
        const GLchar *uendl = strstr(cur, "\n");
        if (!uendl) uendl = (const GLchar *)(0xFFFFFFFF);
        
        const GLchar *endl = min(wendl, uendl);
        
        bool last = false;
        if (endl == (const GLchar *)(0xFFFFFFFF))
        {
            endl = source;
            endl += totalLen+1; //include '/0' at the end
            last = true;
        }
        else
        {
            if (endl == wendl)
                endl += 2;
            else
                endl += 1;
        }
        numLines++;
        //endl point to character after end of string
        
        GLchar *includePtr = strstr(cur, "#include");
        if (includePtr && includePtr < endl)
        {
            printf("Looking at string: ");
            for (const char *p = cur; p < endl; p++)
            {
                printf("%c", *p);
            }
            
            const GLchar *qstart = strstr(cur, "\"");
            if (!qstart) qstart = (const GLchar *)(0xFFFFFFFF);
            const GLchar *bstart = strstr(cur, "<");
            if (!bstart) bstart = (const GLchar *)(0xFFFFFFFF);
            const GLchar *start = min(qstart, bstart);
            if (start != (const GLchar *)(0xFFFFFFFF) && start < endl)
            {
                const GLchar *end = 0;
                if (start == qstart)
                    end = strstr(start+1, "\"");
                else
                    end = strstr(start+1, ">");
                printf("found: ");
                for (const char *p = start+1; p < end; p++)
                {
                    printf("%c", *p);
                }
                printf("\n");
                
                PEString s(*m_pContext, m_arena);
                s.set(start+1, end - start - 1);
                
                printf("String: %s", s.getCStrPtr());
                PEString::generatePathname(*m_pContext, s.getCStrPtr(), "Default", "GPUPrograms", PEString::s_buf, PEString::BUF_SIZE);
                
                
                const GLchar *source;
                
                NSString *string = [NSString stringWithUTF8String: PEString::s_buf];
                
                source = (GLchar *)[[NSString stringWithContentsOfFile:string encoding:NSWindowsCP1252StringEncoding error:nil] UTF8String];
                if (!source)
                {
                    NSLog(@"Failed to load shader source");
                    return 0;
                }
                
                PEString::generatePathname(*m_pContext, "", "Default", "GPUPrograms", PEString::s_buf, PEString::BUF_SIZE);
                
                int subIncludesAdded = processIncludes(source, PEString::s_buf, s.getCStrPtr(), curPos, allIncludes, filenames, lineCounts);
                s.release();
                
                curPos += subIncludesAdded;
                numIncludesAdded += subIncludesAdded;
            }
            //this line has include, remove it
            //NSLog(@"Found: %s", includePtr);
            cur = endl;
        }
        else
        {
            for (; cur < endl; cur++)
            {
                //NSLog(@"adding %c", *cur);
                *curDest = *cur;
                curDest++;
            }
        }
            
        if (last)
            break;
        
    }
    PEString s(*m_pContext, m_arena), name(*m_pContext, m_arena);
    s.set(newStr);
    allIncludes.insert(s, curPos);
    name.set(curFile);
    filenames.insert(name, curPos);
    lineCounts.insert(numLines, curPos);
    
    //NSLog(@"End result: \n%s", s.getCStrPtr());
    
    curPos++;
    numIncludesAdded++;
    return numIncludesAdded;
#else
	return 0;
#endif
}

bool Effect::glLoadShader(const char *filename, const char *name,
		/*D3DXMACRO *pMacros,*/
#       if APIABSTRACTION_IOS //ios uses original opengl API, not cg
            GLenum shaderType, GLuint &out_program //todo:anything else here for ios?
#       else
            CGprofile cgProfile, CGprogram &out_program
#       endif
		)
{
	PEString::generatePathname(*m_pContext, filename, "Default", "GPUPrograms", PEString::s_buf, PEString::BUF_SIZE);

	PEINFO("PE: PROGRESS: loading shader: %s from %s\n", name, PEString::s_buf);

	if (strcmp("main", name))
	{
		assert(!"Only shaders programs with name \"main()\" are supported");
	}

    #if !PE_API_IS_IOS
        GLRenderer *pGLRenderer = static_cast<GLRenderer *>(m_pContext->getGPUScreen());
    #endif

    out_program = 0;
    #if !APIABSTRACTION_IOS
        out_program =
            cgCreateProgramFromFile(
                pGLRenderer->m_cgContext,    /* Cg runtime context */ 
                    //CG_BINARY,                             /* Program in human-readable form */
                    CG_SOURCE,
                PEString::s_buf,                           /* Name of file containing program */
                cgProfile,                                 /* Profile: OpenGL ARB vertex or fragment program */
				//	NULL,                                  /* Entry function name. artem: didnt work for me. will stick with "main" as name of shaders and levae NULL here */
					name,                                  /* Entry function name */
				NULL);                                     /* No extra compiler options */
		return pGLRenderer->checkForErrors("compiling gpu program");
    #else
     
        const GLchar *source;
    
        NSString *string = [NSString stringWithUTF8String: PEString::s_buf];
    
        source = (GLchar *)[[NSString stringWithContentsOfFile:string encoding:NSWindowsCP1252StringEncoding error:nil] UTF8String];
        if (!source)
        {
            NSLog(@"Failed to load shader source");
            return false;
        }
    
        PEString::generatePathname(*m_pContext, "", "Default", "GPUPrograms", PEString::s_buf, PEString::BUF_SIZE);

        Array<PEString> includes(*m_pContext, m_arena), filenames(*m_pContext, m_arena);
        Array<int> lineCounts(*m_pContext, m_arena);
        includes.reset(32);
        filenames.reset(32);
        lineCounts.reset(32);
        /*int numSources = */processIncludes(source, PEString::s_buf, filename, 0, includes, filenames, lineCounts);
    
        const char *sources[32];
    
        assert(includes.m_size < 31);
        int size = includes.m_size;
        if (size > 31)
            size = 31;
        for (int is = 0; is < size; is++)
            sources[is+1] = includes[is].getCStrPtr();
    
        if (shaderType == GL_FRAGMENT_SHADER)
            sources[0] = "#define APIABSTRACTION_IOS 1\n#define APIABSTRACTION_IOS_VERTEX 0\n#define APIABSTRACTION_IOS_FRAGMENT 1\n";
        else
            sources[0] = "#define APIABSTRACTION_IOS 1\n#define APIABSTRACTION_IOS_VERTEX 1\n#define APIABSTRACTION_IOS_FRAGMENT 0\n";
        size++;
        out_program = glCreateShader(shaderType);
        glShaderSource(out_program, size, sources, NULL);
        glCompileShader(out_program);
    
        #if defined(DEBUG)
            bool errFound = false;
            GLint logLength;
            glGetShaderiv(out_program, GL_INFO_LOG_LENGTH, &logLength);
            if (logLength > 0)
            {
                GLchar *log = (GLchar *)malloc(logLength);
                glGetShaderInfoLog(out_program, logLength, &logLength, log);
                NSLog(@"Shader compile log:\n%s", log);
                
                PEINFO("Shader Compilation Failed. Will compile all sources in increasing order to see which source failed on\n");
                errFound = true;
                int iProg = 2;
                int linesToSkip = 3;
                while (iProg < size)
                {
                    glShaderSource(out_program, iProg, sources, NULL);
                    glCompileShader(out_program);
                    
                    GLint logLength;
                    glGetShaderiv(out_program, GL_INFO_LOG_LENGTH, &logLength);
                    if (logLength > 0)
                    {
                        GLchar *sublog = (GLchar *)malloc(logLength);
                        glGetShaderInfoLog(out_program, logLength, &logLength, sublog);
                        if (!strcmp(log, sublog))
                        {
                            printf("PE:Error: Shader compilation of sub source %s failed:\n", filenames[iProg - 2].getCStrPtr());
                            printf("PE:Error: Subtract %d lines from error line numbers\n", linesToSkip);
                            NSLog(@"Shader compile log:\n%s", log);
                            free(sublog);
                            break;
                        }
                    }
                    linesToSkip += lineCounts[iProg-2];
                    iProg++;
                }
                free(log);
                
            }
        #endif
    
        assert(!errFound);
        //free allocated memory
        for (int is = 0; is < includes.m_size; is++)
        {
            includes[is].release();
            filenames[is].release();
        }
        includes.reset(0);
        filenames.reset(0);
        lineCounts.reset(0);
    
    
        GLint status;
        glGetShaderiv(out_program, GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            glDeleteShader(out_program);
            return false;
        }
        return true;
    #endif
    
	return false;
}



#endif

}; // namespace Components
}; // namespace PE
