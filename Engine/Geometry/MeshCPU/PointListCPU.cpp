#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// immediate include 
#include "PointListCPU.h"

#include "../MaterialCPU/MaterialSetCPU.h"

namespace PE {

void PointListCPU::createPS3PS(PrimitiveTypes::Float32 x0, PrimitiveTypes::Float32 x1, PrimitiveTypes::Float32 y0, PrimitiveTypes::Float32 y1, PrimitiveTypes::UInt32 nsprites)
{
	StringOps::writeToString( "SimpleCS_Tech", m_csTechName, 64);
	PrimitiveTypes::Float32 xrange, yrange;

	xrange = x1 - x0;
	yrange = y1 - y0;
	
	m_hPositionBufferCPU = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvbcpu = new(m_hPositionBufferCPU) PositionBufferCPU(*m_pContext, m_arena);
	pvbcpu->m_values.reset(nsprites * 3);

	m_hTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *ptcbcpu = new(m_hTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	ptcbcpu->m_values.reset(nsprites * 2);

	m_hOrientationTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *porienttcbcpu = new(m_hOrientationTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	porienttcbcpu->m_values.reset(nsprites * 2);

	for (PrimitiveTypes::UInt32 ip = 0; ip < nsprites; ip++)
	{
		PrimitiveTypes::Float32 alpha = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;
		PrimitiveTypes::Float32 beta = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;

		pvbcpu->m_values.add(x0 + alpha * xrange);
		pvbcpu->m_values.add(100.0f);
		pvbcpu->m_values.add(y0 + beta * yrange);
		
		alpha = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;
		beta = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;

		ptcbcpu->m_values.add(50.0f + 70.0f * alpha);
		ptcbcpu->m_values.add(50.0f + 50.0f * beta);

		alpha = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;
		alpha *= PrimitiveTypes::Constants::c_Pi_F32;
		porienttcbcpu->m_values.add(cos(alpha));
		porienttcbcpu->m_values.add(sin(alpha));
	}
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
	pmscpu->createSetWithOneTexturedMaterial("baloon.dds", "Default");
}


void PointListCPU::create(PrimitiveTypes::Float32 x0, PrimitiveTypes::Float32 x1, PrimitiveTypes::Float32 y0, PrimitiveTypes::Float32 y1, PrimitiveTypes::UInt32 nsprites)
{
	StringOps::writeToString( "SimpleCS_Tech", m_csTechName, 64);
	PrimitiveTypes::Float32 xrange, yrange;

	xrange = x1 - x0;
	yrange = y1 - y0;
	
	m_hPositionBufferCPU = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvbcpu = new(m_hPositionBufferCPU) PositionBufferCPU(*m_pContext, m_arena);
	pvbcpu->m_values.reset(nsprites * 3);

	m_hTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *ptcbcpu = new(m_hTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	ptcbcpu->m_values.reset(nsprites * 2);

	m_hOrientationTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *porienttcbcpu = new(m_hOrientationTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	porienttcbcpu->m_values.reset(nsprites * 2);

	for (PrimitiveTypes::UInt32 ip = 0; ip < nsprites; ip++)
	{
		PrimitiveTypes::Float32 alpha = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;
		PrimitiveTypes::Float32 beta = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;

		pvbcpu->m_values.add(x0 + alpha * xrange);
		pvbcpu->m_values.add(100.0f);
		pvbcpu->m_values.add(y0 + beta * yrange);
		
		alpha = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;
		beta = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;

		ptcbcpu->m_values.add(50.0f + 70.0f * alpha);
		ptcbcpu->m_values.add(50.0f + 50.0f * beta);

		alpha = (PrimitiveTypes::Float32)(rand() % 100) / 100.0f;
		alpha *= PrimitiveTypes::Constants::c_Pi_F32;
		porienttcbcpu->m_values.add(cos(alpha));
		porienttcbcpu->m_values.add(sin(alpha));
	}
	
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
	pmscpu->createSetWithOneTexturedMaterial("baloon.dds", "Default");
}

void PointListCPU::createVoxelSpace(PrimitiveTypes::Float32 x0, PrimitiveTypes::Float32 x1,
	PrimitiveTypes::Float32 y0, PrimitiveTypes::Float32 y1,
	PrimitiveTypes::Float32 z0, PrimitiveTypes::Float32 z1,
	PrimitiveTypes::UInt32 numVoxelsPerDim,
	char *pTechName)
{
	StringOps::writeToString(pTechName, m_csTechName, 64);
	PrimitiveTypes::Float32 xrange, yrange, zrange;

	xrange = x1 - x0;
	yrange = y1 - y0;
	zrange = z1 - z0;

	PrimitiveTypes::Float32 dx, dy, dz;

	dx = xrange / (PrimitiveTypes::Float32)(numVoxelsPerDim);
	dy = yrange / (PrimitiveTypes::Float32)(numVoxelsPerDim);
	dz = zrange / (PrimitiveTypes::Float32)(numVoxelsPerDim);

	PrimitiveTypes::UInt32 totalVoxels = numVoxelsPerDim * numVoxelsPerDim * 1; //numVoxelsPerDim;

	m_hPositionBufferCPU = Handle("VERTEX_BUFFER_CPU", sizeof(PositionBufferCPU));
	PositionBufferCPU *pvbcpu = new(m_hPositionBufferCPU) PositionBufferCPU(*m_pContext, m_arena);
	pvbcpu->m_values.reset(totalVoxels * 3);

	m_hTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *ptcbcpu = new(m_hTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	ptcbcpu->m_values.reset(totalVoxels * 2);

	m_hOrientationTexCoordBufferCPU = Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
	TexCoordBufferCPU *porienttcbcpu = new(m_hOrientationTexCoordBufferCPU) TexCoordBufferCPU(*m_pContext, m_arena);
	porienttcbcpu->m_values.reset(totalVoxels * 2);

	m_colorArr.reset(1280*720);

	m_colorArr.m_size = 1280*720;
	for (PrimitiveTypes::UInt32 iy = 0; iy < 720; iy++)
	{
		for (PrimitiveTypes::UInt32 ix = 0; ix < 1280; ix++)
		{
			if (ix < 500 || ix > 780)
				m_colorArr[iy * 1280 + ix] = Vector4();
			else
				m_colorArr[iy * 1280 + ix] = Vector4((float)ix / 1280, (float)iy / 720, 0, 1.0f);
		}
	}
	//memset(m_colorArr.getFirstPtr(), 128, m_colorArr.m_dataSize);
	


	pvbcpu->m_values.m_size = totalVoxels * 3;
	ptcbcpu->m_values.m_size = totalVoxels * 2;

	memset(&pvbcpu->m_values[0], 0, pvbcpu->m_values.m_dataSize);
	memset(&ptcbcpu->m_values[0], 0, ptcbcpu->m_values.m_dataSize);
	/*

	PrimitiveTypes::Float32 y = y0;
	for (PrimitiveTypes::UInt32 iy = 0; iy < numVoxelsPerDim; iy++)
	{
		PrimitiveTypes::Float32 z = z0;
		for (PrimitiveTypes::UInt32 iz = 0; iz < numVoxelsPerDim; iz++)
		{
		
			PrimitiveTypes::Float32 x = x0;
			for (PrimitiveTypes::UInt32 ix = 0; ix < numVoxelsPerDim; ix++)
			{
				pvbcpu->m_values.add(x);
				pvbcpu->m_values.add(y);
				pvbcpu->m_values.add(z);
				
				ptcbcpu->m_values.add(0);
				ptcbcpu->m_values.add(0);
				x += dx;
			}
			z += dz;
		}
		y += dy;
	}
*/
	m_hMaterialSetCPU = Handle("MATERIAL_SET_CPU", sizeof(MaterialSetCPU));
	MaterialSetCPU *pmscpu = new(m_hMaterialSetCPU) MaterialSetCPU(*m_pContext, m_arena);
	pmscpu->createSetWithOneTexturedMaterial("grass.dds", "Default");
}
}; // namespace PE {
