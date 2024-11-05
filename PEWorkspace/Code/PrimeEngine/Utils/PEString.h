#ifndef __PYENGINE_2_0_STRING_H__
#define __PYENGINE_2_0_STRING_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Utils/PEClassDecl.h"
#include "PrimeEngine/Game/Common/GameContext.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

// Sibling/Children includes
#include "PrimeEngine/Utils/Array/Array.h"

struct PEString : PE::PEAllocatable
{
    static const int BUF_SIZE = 1024;
    static char s_buf[BUF_SIZE];
    
	PEString(PE::GameContext &context, PE::MemoryArena arena) : m_data(context, arena) { set(""); }

	~PEString() {}
	
    static void generatePathname(PE::GameContext &context, const char *filename, const char *package, const char *assetType, char *out_path, int len);
	static const char* generateScriptPathname(PE::GameContext &context, const char *filename, const char *module, const char *folder, char *out_path, int len);

	void set(const char *zstr);
    void set(const char *str, int sizeNoZero);

	void append(PrimitiveTypes::Int32 val);

	void append(PrimitiveTypes::Float32 val);

	void append(const char *val);

	void append(PEString &val);
	char *getCStrPtr() { return m_data.getFirstPtr(); }

	void decreaseSize(PrimitiveTypes::UInt32 v)
	{
		m_data.m_size -= v;
		m_data[m_data.m_size-1] = '\0';
	}

	void release() { m_data.reset(0);}

	Array<char> m_data;
};
#endif
