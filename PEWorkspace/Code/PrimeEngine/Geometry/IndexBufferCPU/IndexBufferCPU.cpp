// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "IndexBufferCPU.h"

#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

IndexRange::IndexRange(PE::GameContext &context, PE::MemoryArena arena)
: m_boneSegments(context, arena)
{

}
// Reads the specified buffer from file
void IndexBufferCPU::ReadIndexBuffer(const char *filename, const char *package)
{
	m_minVertexIndex = 0x7FFFFFFF;
	m_maxVertexIndex = 0x80000000;
    strcpy(&m_dbgName[0], filename);

    PEString::generatePathname(*m_pContext, filename, package, "IndexBuffers", PEString::s_buf, PEString::BUF_SIZE);
	
	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);
	
	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "INDEX_BUFFER"

	f.nextInt32(m_verticesPerPolygon);

	PEASSERT(m_verticesPerPolygon == 3, "Non triangle index buffers not supported when reading from disk!");

	m_verticesPerPolygon = 3;
	m_primitiveTopology = PEPrimitveTopology_TRIANGLES;

	PrimitiveTypes::Int32 totalPolygonCount;
	f.nextInt32(totalPolygonCount);

	PrimitiveTypes::Int32 numberOfSets;
	f.nextInt32(numberOfSets);
	m_indexRanges.reset(numberOfSets);
	m_vertsPerFacePerRange.reset(numberOfSets); // stores number of vertices in face for every range

	m_values.reset(totalPolygonCount * m_verticesPerPolygon); // verticesPerPolygon UInt16 per vertex
	//dbg
    //PrimitiveTypes::Int32 memoryAddress = (PrimitiveTypes::Int32)m_values.m_dataHandle.getObject();

	PrimitiveTypes::Int32 curVertexIndexIndex = 0; // tracks number of vertices processed so far
	// Read all values
	for (int i = 0; i < numberOfSets; i++)// TODO : Right now, no distinction is really made once the different sets are loaded in
	{
		m_vertsPerFacePerRange.add(m_verticesPerPolygon);
		PrimitiveTypes::Int32 boneSegmentCount;
		f.nextInt32(boneSegmentCount);

		IndexRange range(*m_pContext, m_arena); // stores range of indices for material index range. it also stores separation bone segments
		range.m_start = curVertexIndexIndex;
		range.m_boneSegments.reset(boneSegmentCount);
		int minIndexInRange = 0x7FFFFFFF;
		int maxIndexInRange = 0x80000000;


		PrimitiveTypes::UInt32 rangePolygonCount = 0;
		for (int i = 0; i < boneSegmentCount; i++)
		{
			range.m_boneSegments.add(IndexRange::BoneSegment(*m_pContext, m_arena));
			IndexRange::BoneSegment &boneSegment = range.m_boneSegments[range.m_boneSegments.m_size-1];

			PrimitiveTypes::Int32 boneCountInSegment;
			f.nextInt32(boneCountInSegment); // number of bones used in the segment

			boneSegment.m_boneSegmentBones.reset(boneCountInSegment);
			boneSegment.m_start = curVertexIndexIndex;

			for (PrimitiveTypes::UInt32 ib = 0; ib < (PrimitiveTypes::UInt32)(boneCountInSegment); ib++)
			{
				PrimitiveTypes::Int32 boneId;
				f.nextInt32(boneId); // number of bones used in the segment
				boneSegment.m_boneSegmentBones.add(boneId);
			}

			PrimitiveTypes::Int32 boneSegmentPolygonCount;
			f.nextInt32(boneSegmentPolygonCount);

			if (boneSegmentPolygonCount > 0)
			{
				boneSegment.m_end = boneSegment.m_start + boneSegmentPolygonCount * m_verticesPerPolygon - 1;

				rangePolygonCount += boneSegmentPolygonCount;
				curVertexIndexIndex += boneSegmentPolygonCount * m_verticesPerPolygon;
				
				PrimitiveTypes::Int32 val;
				for (int j = 0; j < boneSegmentPolygonCount * m_verticesPerPolygon; j++)
				{
					f.nextInt32(val);
					m_values.add(val);
					if (val > maxIndexInRange)
						maxIndexInRange = val;
					if (val < minIndexInRange)
						minIndexInRange = val;
				}
			}
			else
			{
				// this bone segment has no polygons in it, so we need to skip it
				boneSegment.m_boneSegmentBones.reset(0); // make sure we release this memory, but there still is a spot in the array that will not be used
				i--;
				boneSegmentCount--;
				range.m_boneSegments.remove(range.m_boneSegments.m_size-1);
				PEINFO("PE: Warning: Empty bone segment was found in index buffer. This will lead to extra memory allocated for the segment that never is added");
			}
		}

		range.m_end = range.m_start + rangePolygonCount * m_verticesPerPolygon - 1;
		range.m_minVertIndex = minIndexInRange;
		range.m_maxVertIndex = maxIndexInRange;
		if (maxIndexInRange > m_maxVertexIndex)
			m_maxVertexIndex = maxIndexInRange;
		if (minIndexInRange < m_minVertexIndex)
			m_minVertexIndex = minIndexInRange;

		if (range.m_boneSegments.m_size == 1 && range.m_boneSegments[0].m_boneSegmentBones.m_size == 0)
		{
			// we have only one bone segment in range and the are 0 bones - this is a static mesh case
			// we dont need to store list of bone segments then. the range will be drawn

			range.m_boneSegments.reset(0);
		}

		#if APIABSTRACTION_D3D11
			// we are going to combine all bone segments into 1
			if (range.m_boneSegments.m_size > 1)
			{
				IndexRange::BoneSegment &boneSegment = range.m_boneSegments[0];

				boneSegment.m_start = range.m_start;
				boneSegment.m_end = range.m_end;	

				range.m_boneSegments.m_size = 1;

				//release other bone segments
				for (int is = 1; is < range.m_boneSegments.m_size; ++is)
				{
					IndexRange::BoneSegment &boneSegment = range.m_boneSegments[is];
					boneSegment.m_boneSegmentBones.reset(0);
				}
			}
		#endif
		

		if (range.m_end >= range.m_start)
		{
			m_indexRanges.add(range);
		}
		else
		{
			PEINFO("PE: Warning: Empty Index range (material) found. This will lead to extra memory allocated for the material taht is never used.\n");
			range.m_boneSegments.reset(0); // we release this memory but there is still one spot in the array for unused index range
		}
	}
}


void IndexBufferCPU::createEmptyCPUBuffer()
{
	m_indexRanges.reset(1);
	m_vertsPerFacePerRange.reset(1);
	IndexRange range(*m_pContext, m_arena);
	range.m_start = 0;
	range.m_end = -1;
	range.m_minVertIndex = 0;
	range.m_maxVertIndex = 0;
	m_indexRanges.add(range);
	m_vertsPerFacePerRange.add(3);
	m_values.reset(0);
	
	m_minVertexIndex = range.m_minVertIndex;
	m_maxVertexIndex = range.m_maxVertIndex;

	m_primitiveTopology = PEPrimitveTopology_TRIANGLES;
	m_verticesPerPolygon = 3;
}
void IndexBufferCPU::createBillboardCPUBuffer()
{
	m_indexRanges.reset(1);
	m_vertsPerFacePerRange.reset(1);
	IndexRange range(*m_pContext, m_arena);
	range.m_start = 0;
	range.m_end = 5;
	range.m_minVertIndex = 0;
	range.m_maxVertIndex = 3;
	m_indexRanges.add(range);
	m_vertsPerFacePerRange.add(3);
	m_values.reset(6);
	m_values.add(0); m_values.add(2); m_values.add(1);
	m_values.add(2); m_values.add(0); m_values.add(3);

	m_minVertexIndex = range.m_minVertIndex;
	m_maxVertexIndex = range.m_maxVertIndex;

	m_primitiveTopology = PEPrimitveTopology_TRIANGLES;
	m_verticesPerPolygon = 3;
}
