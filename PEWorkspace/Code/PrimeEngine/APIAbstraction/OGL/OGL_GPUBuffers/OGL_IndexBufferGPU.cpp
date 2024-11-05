// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_OGL

// Inter-Engine includes

// Outer-Engine includes
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif

// Sibling/Children includes
#include "OGL_IndexBufferGPU.h"

GLuint OGL_IndexBufferGPU::CreateIndexBufferInGPU(void *indices, PrimitiveTypes::UInt32 size, PrimitiveTypes::Bool constant/*DEF = true*/)
{
	/*
	IDirect3DIndexBuffer9 *mIB;
	pD3DDevice->CreateIndexBuffer(size, 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &mIB, NULL);
	if (indices != 0)
	{
		VOID* pVoid;
		// lock v_buffer and load the vertices into it
		mIB->Lock(0, 0, (void**)&pVoid, 0);
		memcpy(pVoid, indices, size);
		mIB->Unlock();
	}
	return mIB;
	*/

	GLuint ibo;
	/* Allocate and assign two Vertex Buffer Objects to our handle */
	glGenBuffers(1, &ibo);

	/* Bind our first VBO as being the active buffer and storing vertex attributes (coordinates) */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	/* 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values */
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return ibo;
}

void OGL_IndexBufferGPU::releaseGPUBuffer(GLuint bufId)
{
	glDeleteBuffers(1, &bufId);
}

#endif // APIAbstraction
