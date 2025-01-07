#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "SkinWeightsCPU.h"

#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/FileSystem/FileWriter.h"
#include "PrimeEngine/Utils/StringOps.h"
#include <float.h>

SkinWeightsCPU::SkinWeightsCPU(PE::GameContext &context, PE::MemoryArena arena) : m_weightsPerVertex(context, arena)
{
	m_arena = arena; m_pContext = &context;
}

// Reads the specified buffer from file
void SkinWeightsCPU::ReadSkinWeights(const char *filename, const char *package)
{
    PEString::generatePathname(*m_pContext, filename, package, "SkinWeights", PEString::s_buf, PEString::BUF_SIZE);
		
	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "SKIN_WEIGHTS"
	PrimitiveTypes::Int32 n;
	f.nextInt32(n);
	// there are n vertices in these skin weights
		
	m_weightsPerVertex.reset((PrimitiveTypes::UInt32)(n));

	for (PrimitiveTypes::Int32 iv = 0; iv < n; iv++)
	{
		m_weightsPerVertex.add(Array<WeightPair>(*m_pContext, m_arena));
		// read how many joints this vertex depends on
		PrimitiveTypes::Int32 nj;
		f.nextInt32(nj);

		m_weightsPerVertex[iv].reset(nj);
		for (PrimitiveTypes::Int32 ij = 0; ij < nj; ij++)
		{
			m_weightsPerVertex[iv].add(WeightPair());
			PrimitiveTypes::Int32 jointIndex;
			f.nextInt32(jointIndex);

			PrimitiveTypes::Float32 weight;
			f.nextFloat32(weight);

			PrimitiveTypes::Int32 localJointIndex;
			f.nextInt32(localJointIndex);

			m_weightsPerVertex[iv][ij].m_jointIndex = jointIndex;
			m_weightsPerVertex[iv][ij].m_weight = weight;
			#if APIABSTRACTION_D3D11
				m_weightsPerVertex[iv][ij].m_localJointIndex = jointIndex;
			#else
				m_weightsPerVertex[iv][ij].m_localJointIndex = localJointIndex;
			#endif
		}
	}

	reduceWeights(DEFAULT_SKIN_WEIGHTS_PER_VERTEX);
		
}

	
void SkinWeightsCPU::reduceWeights(int num)
{
	for (int iv = 0; iv < m_weightsPerVertex.m_size; ++iv)
	{
		Array<WeightPair> &vertWeights = m_weightsPerVertex[iv];
		while (vertWeights.m_size > num)
		{
			float minContribution = FLT_MAX ;
			int minContributionIndex = -1;
			for (int iw = 0; iw < vertWeights.m_size; ++iw)
			{
				if (vertWeights[iw].m_weight < minContribution)
				{
					minContribution = vertWeights[iw].m_weight;
					minContributionIndex = iw;
				}
			}
			if (minContributionIndex != -1)
			{
				vertWeights.remove((PrimitiveTypes::UInt32)(minContributionIndex));
			}
		}
	}
}

