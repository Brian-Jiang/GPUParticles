// This class is a helper for VertexBufferGPU
// IMPORTANT: In D3D vertex buffers store all vertex information: position, normals, tex coordinates
// Thus, in D3D there are no Normal buffers, or tex coord buffers per se; they are part of index buffers


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Render/GLRenderer.h"
#if APIABSTRACTION_OGL

// Outer-Engine includes

// Inter-Engine includes

// Sibling/Children includes
#include "OGL_VertexBufferGPU.h"

GLuint OGL_VertexBufferGPU::CreateVertexBufferInGPUFromVbNbTb(
	PE::GameContext &context,
	PE::MemoryArena arena,
	PositionBufferCPU *pDataVb,
	ColorBufferCPU *pDataCb,
	TexCoordBufferCPU *pDataTCb,
	NormalBufferCPU *pDataNb,
	TangentBufferCPU *pDataTb,

	SkinWeightsCPU *pDataSkinWeights,

	PrimitiveTypes::Bool constant, // = true
	// Output
	GLuint *out_vbos /* = NULL */
)
{
	GLuint vbo[16];
	memset(vbo, 0, sizeof(vbo));
	
#if OGL_USE_VERTEX_BUFFER_ARRAYS
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);

	// Bind our Vertex Array Object as the current used object
	glBindVertexArray(vao);
#endif
	/* Allocate and assign two Vertex Buffer Objects to our handle */
	int bufsNeeded = 0;
	if (pDataVb) bufsNeeded++;
	if (pDataCb) bufsNeeded++;
	if (pDataTCb) bufsNeeded++;
	if (pDataNb) bufsNeeded++;
	if (pDataTb) bufsNeeded++;
	if (pDataSkinWeights) bufsNeeded+=4;

    PE::IRenderer::checkForErrors("");

	glGenBuffers(bufsNeeded, vbo);
	int curBuf = 0;

	glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
	glBufferData(GL_ARRAY_BUFFER, pDataVb->m_values.m_size * sizeof(GLfloat), pDataVb->m_values.getFirstPtr(), GL_STATIC_DRAW);

	out_vbos[curBuf] = vbo[curBuf];
	curBuf++;

	#if !OGL_USE_VERTEX_BUFFER_ARRAYS
		// for PS3 we use VBOs only
	#else
		// for win 32 opengl we use VertexArray objects
		/* Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		/* Enable attribute index 0 as being used */
		glEnableVertexAttribArray(0);
	#endif
    PE::IRenderer::checkForErrors("");

	if (pDataCb)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
		glBufferData(GL_ARRAY_BUFFER, pDataCb->m_values.m_size * sizeof(GLfloat), pDataCb->m_values.getFirstPtr(), GL_STATIC_DRAW);

		out_vbos[curBuf] = vbo[curBuf];
		curBuf++;
		
		#if !OGL_USE_VERTEX_BUFFER_ARRAYS
		#else
			/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			/* Enable attribute index 1 as being used */
			glEnableVertexAttribArray(1);
		#endif
        PE::IRenderer::checkForErrors("");
	}

	if (pDataTCb)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
		glBufferData(GL_ARRAY_BUFFER, pDataTCb->m_values.m_size * sizeof(GLfloat), pDataTCb->m_values.getFirstPtr(), GL_STATIC_DRAW);
		
		out_vbos[curBuf] = vbo[curBuf];
		curBuf++;
	
		#if !OGL_USE_VERTEX_BUFFER_ARRAYS
		#else
			/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			/* Enable attribute index 1 as being used */
			glEnableVertexAttribArray(1);
		#endif
        PE::IRenderer::checkForErrors("");
	}

	if (pDataNb)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
		glBufferData(GL_ARRAY_BUFFER, pDataNb->m_values.m_size * sizeof(GLfloat), pDataNb->m_values.getFirstPtr(), GL_STATIC_DRAW);
		out_vbos[curBuf] = vbo[curBuf];
		curBuf++;
		#if !OGL_USE_VERTEX_BUFFER_ARRAYS
		#else
				/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
				/* Enable attribute index 1 as being used */
				glEnableVertexAttribArray(2);
		#endif
        PE::IRenderer::checkForErrors("");
    }

	if (pDataTb)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
		glBufferData(GL_ARRAY_BUFFER, pDataTb->m_values.m_size * sizeof(GLfloat), pDataTb->m_values.getFirstPtr(), GL_STATIC_DRAW);

		out_vbos[curBuf] = vbo[curBuf];
		curBuf++;
		#if !OGL_USE_VERTEX_BUFFER_ARRAYS
		#else
				/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
				/* Enable attribute index 1 as being used */
				glEnableVertexAttribArray(3);
		#endif
        PE::IRenderer::checkForErrors("");
    }

	if (pDataSkinWeights)
	{
		// process skin weight information
		PositionBufferCPU jointWeights(context, arena),
			jointWeights1(context, arena),
			jointIndices(context, arena),
			jointIndices1(context, arena);

		unsigned int numVertices = (pDataVb->m_values.m_size / 3);
		jointWeights.m_values.reset(numVertices * 4);
		jointWeights1.m_values.reset(numVertices * 4);
		jointIndices.m_values.reset(numVertices * 4);
		jointIndices1.m_values.reset(numVertices * 4);

		for (PrimitiveTypes::UInt32 iv = 0; iv < numVertices; iv++)
		{
			PrimitiveTypes::Float32 vweights[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
			PrimitiveTypes::Float32 jointIndices8[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
			assert(pDataSkinWeights->m_weightsPerVertex[iv].m_size <= 5);
			PrimitiveTypes::UInt32 nWeights = pDataSkinWeights->m_weightsPerVertex[iv].m_size;

			for (PrimitiveTypes::UInt32 ivw = 0; ivw < nWeights; ivw++)
			{
				vweights[ivw] = (PrimitiveTypes::Float32)(pDataSkinWeights->m_weightsPerVertex[iv][ivw].m_weight);
				jointIndices8[ivw] = (PrimitiveTypes::Float32)(pDataSkinWeights->m_weightsPerVertex[iv][ivw].m_localJointIndex);
			}

			jointWeights.m_values.add(vweights[0]); jointWeights.m_values.add(vweights[1]); jointWeights.m_values.add(vweights[2]); jointWeights.m_values.add(vweights[3]);
			jointWeights1.m_values.add(vweights[4]); jointWeights1.m_values.add(vweights[5]); jointWeights1.m_values.add(vweights[6]); jointWeights1.m_values.add(vweights[7]);
			jointIndices.m_values.add(jointIndices8[0]); jointIndices.m_values.add(jointIndices8[1]); jointIndices.m_values.add(jointIndices8[2]); jointIndices.m_values.add(jointIndices8[3]);
			jointIndices1.m_values.add(jointIndices8[4]); jointIndices1.m_values.add(jointIndices8[5]); jointIndices1.m_values.add(jointIndices8[6]); jointIndices1.m_values.add(jointIndices8[7]);
            PE::IRenderer::checkForErrors("");
        }

		// jointWeights
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
			glBufferData(GL_ARRAY_BUFFER, jointWeights.m_values.m_size * sizeof(GLfloat), jointWeights.m_values.getFirstPtr(), GL_STATIC_DRAW);
			out_vbos[curBuf] = vbo[curBuf];
			curBuf++;
			#if OGL_USE_VERTEX_BUFFER_ARRAYS
				/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(4);/* Enable attribute index as being used */
			#endif
            PE::IRenderer::checkForErrors("");
        }

		// jointWeights1
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
			glBufferData(GL_ARRAY_BUFFER, jointWeights1.m_values.m_size * sizeof(GLfloat), jointWeights1.m_values.getFirstPtr(), GL_STATIC_DRAW);
			out_vbos[curBuf] = vbo[curBuf];
			curBuf++;
			#if OGL_USE_VERTEX_BUFFER_ARRAYS
				/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(5);/* Enable attribute index as being used */
			#endif
            PE::IRenderer::checkForErrors("");
        }

		// jointIndices
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
			glBufferData(GL_ARRAY_BUFFER, jointIndices.m_values.m_size * sizeof(GLfloat), jointIndices.m_values.getFirstPtr(), GL_STATIC_DRAW);
			out_vbos[curBuf] = vbo[curBuf];
			curBuf++;
			#if OGL_USE_VERTEX_BUFFER_ARRAYS
				/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(6);/* Enable attribute index as being used */
			#endif
            PE::IRenderer::checkForErrors("");
		}

		// jointIndices
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo[curBuf]);
			glBufferData(GL_ARRAY_BUFFER, jointIndices1.m_values.m_size * sizeof(GLfloat), jointIndices1.m_values.getFirstPtr(), GL_STATIC_DRAW);
			out_vbos[curBuf] = vbo[curBuf];
			curBuf++;
			#if OGL_USE_VERTEX_BUFFER_ARRAYS
						/* Specify that our color data is going into attribute index 1, and contains three floats per vertex */
						glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 0, 0);
						glEnableVertexAttribArray(7);/* Enable attribute index as being used */
			#endif
            PE::IRenderer::checkForErrors("");
		}
		
		jointWeights.m_values.reset(0);
		jointWeights1.m_values.reset(0);
		jointIndices.m_values.reset(0);
		jointIndices1.m_values.reset(0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	PE::IRenderer::checkForErrors("");

#if !OGL_USE_VERTEX_BUFFER_ARRAYS
	return 0;
#else
	return vao;
#endif
}
	

void OGL_VertexBufferGPU::releaseGPUBuffers(GLuint vao, GLuint *vbos, int numVBOs)
{
	#if OGL_USE_VERTEX_BUFFER_ARRAYS
		assert(!"Not Iplemented!");
	#else
		for (int ivbo = 0; ivbo < numVBOs; ivbo++)
		{
			if (vbos[ivbo])
			{
				glDeleteBuffers(1, &vbos[ivbo]);
				vbos[ivbo] = 0;
			}
		}
	#endif
}

void OGL_VertexBufferGPU::setAsCurrent(
	//IDirect3DDevice9 *pD3DDevice, 
	GLuint vao, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset)
{
	// Bind our Vertex Array Object as the current used object
	// from docs, we dont need to set any other states
	// the vao knows whic attruibutes are enabled
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS
	glBindVertexArray(vao);
#endif
}
#endif // APIAbstraction
