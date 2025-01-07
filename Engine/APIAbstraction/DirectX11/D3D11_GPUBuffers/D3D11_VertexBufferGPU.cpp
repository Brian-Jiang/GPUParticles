// This class is a helper for VertexBufferGPU
// IMPORTANT: In D3D vertex buffers store all vertex information: position, normals, tex coordinates
// Thus, in D3D there are no Normal buffers, or tex coord buffers per se; they are part of index buffers

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D11

// Outer-Engine includes

// Inter-Engine includes

// Sibling/Children includes
#include "D3D11_VertexBufferGPU.h"

ID3D11Buffer *D3D11_VertexBufferGPU::CreateVertexBufferInGPUFromVb(
		// Input
		ID3D11Device *pD3DDevice, void *pData, PrimitiveTypes::UInt32 size, 
		PrimitiveTypes::Bool constant,// DEF = true
		PrimitiveTypes::Bool streamOutput // DEF = false
		// Output
	)
	{
		// Create the buffer itself
		D3D11_BUFFER_DESC vbd;
			vbd.Usage = constant ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
			vbd.CPUAccessFlags = 0;
			vbd.ByteWidth = size;
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.MiscFlags = 0; //D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			//vbd.StructureByteStride = sizeof(float);

			if (!constant && streamOutput)
			{
				vbd.BindFlags = vbd.BindFlags | D3D11_BIND_STREAM_OUTPUT;
			}

		ID3D11Buffer *pBuffer;
		if (pData != 0)
		{
			D3D11_SUBRESOURCE_DATA vinitData;
				vinitData.pSysMem = pData;
				// vinitData.SysMemPitch = N/A not used for vertex buffers
				// vinitData.SysMemSlicePitch = N/A not used for vertex buffers
			pD3DDevice->CreateBuffer(&vbd, &vinitData, &pBuffer);
		}
		else
		{
			pD3DDevice->CreateBuffer(&vbd, 0, &pBuffer);
		}
		return pBuffer;
	}

	// TODO: implement these

void D3D11_VertexBufferGPU::setAsCurrent(ID3D11DeviceContext *pD3DDeviceContext, ID3D11Buffer *pBuf, PrimitiveTypes::UInt32 vertexSize, PrimitiveTypes::UInt32 startOffset)
{
	pD3DDeviceContext->IASetVertexBuffers(0, 1, &pBuf, &vertexSize, &startOffset);
}

void D3D11_VertexBufferGPU::setAsCurrent(ID3D11DeviceContext *pD3DDeviceContext, 
		Array<ID3D11Buffer *> &pBufs, 
		Array<PrimitiveTypes::UInt32> &vertexSizes, 
		Array<PrimitiveTypes::UInt32> &startOffsets)
{
	pD3DDeviceContext->IASetVertexBuffers(0, pBufs.m_size, &pBufs[0], &vertexSizes[0], &startOffsets[0]);
}
#endif // APIAbstraction
