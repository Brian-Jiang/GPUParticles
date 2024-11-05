#ifndef __PYENGINE_2_0_SKELETON_CPU__
#define __PYENGINE_2_0_SKELETON_CPU__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "../../Utils/Array/Array.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "../PositionBufferCPU/PositionBufferCPU.h"

// Sibling/Children includes
#include "SkinWeightsCPU.h"
#include "AnimationCPU.h"
#include "AnimationSetCPU.h"

namespace PE {
struct BoneTQ
{
	Quaternion m_quat;
	Vector4 m_translation;
};

struct JointParentData
{
	int m_parentJointIndex;
};

struct JointCPU
{
	JointCPU(PE::GameContext &context, PE::MemoryArena arena) : m_subJoints(context, arena)
	{}

	Array<JointCPU> m_subJoints;
	PrimitiveTypes::UInt32 m_index;
	PrimitiveTypes::Float32 m_matrix[16];
	char m_name[256];
};

struct FastJoint
{
	FastJoint() : m_parent(0), m_sibling(0), m_child(0)
	{
	}

	PrimitiveTypes::UInt32 m_index;
	FastJoint *m_parent;
	FastJoint *m_sibling;
	FastJoint *m_child;
	JointCPU *m_pJointCPU;
};

struct SkeletonCPU : PE::PEAllocatableAndDefragmentable
{
	Array<FastJoint> m_fastJoints;
	Array<JointParentData> m_jointParents;
	char m_name[256];

	SkeletonCPU(PE::GameContext &context, PE::MemoryArena arena):m_bindInverses(context, arena), m_root(context, arena)
	, m_fastJoints(context, arena)
	, m_jointParents(context, arena)
	{
		m_arena = arena; m_pContext = &context;
	}

	void readJoint(JointCPU &dest, JointCPU *pParentJoint, FastJoint *fastJointParent, FastJoint *&fastJointSibling, FileReader &f, PrimitiveTypes::Int32 &jointsLeft, float positionFactor, bool isInWorldSpace);

	// Reads the specified buffer from file
	void ReadSkeleton(const char *filename, const char *package = NULL);

	
	PrimitiveTypes::UInt32 countJoints(JointCPU &jnt)
	{
		PrimitiveTypes::UInt32 res = 1;
		for (PrimitiveTypes::UInt32 ij = 0; ij < jnt.m_subJoints.m_size; ij++)
		{
			res += countJoints(jnt.m_subJoints[ij]);
		}
		return res;
	}
	
	void convertPaletteFromLocalToModelSpaceAndMultiplyByInverse(Matrix4x4 *res, BoneTQ *pTQ)
	{
/*
		Matrix4x4 *pNewParent = res;
		const Matrix4x4 *pInv = m_bindInverses.getFirstPtr();
		const Matrix4x4 *pCurInv = pInv;
	
		JointCPU *pJoint = m_root.m_subJoints.getFirstPtr();
		
		for (PrimitiveTypes::UInt32 ij = 0, e = m_root.m_subJoints.m_size; ij < e; ++ij)
		{
			++res;
			++pInv;
			internalConvertPaletteFromLocalToModelSpace(*pJoint++, res, pInv, *pNewParent);
		}

		// apply inverse
		*pNewParent = *pNewParent * *pCurInv;
*/

		Matrix4x4 *pInv = m_bindInverses.getFirstPtr();
		
		Matrix4x4 parentTransform;
		parentTransform.loadIdentity();
		FastJoint *fj = &m_fastJoints[0];
		Matrix4x4 *start = res;
		int numSteps = 0;
		int numInv = 0;
		bool skipChildren = false;

		res->setFromQuatAndPos(pTQ->m_quat, pTQ->m_translation.asVector3Ref());
		while (fj)
		{
			if (skipChildren)
				numSteps--;
			else	
				//(*res) = parentTransform * (*res);
				res->getTransformedByOther(parentTransform);

			numSteps++;
			PEASSERT(numSteps <= (int)(m_bindInverses.m_size), "Invlaid number of steps!");
			if (fj->m_child && !skipChildren)
			{
				parentTransform = (*res);
				fj = fj->m_child;
				++res;
				++pTQ;
				res->setFromQuatAndPos(pTQ->m_quat, pTQ->m_translation.asVector3Ref());
			}
			else if (fj->m_sibling)
			{
				start[fj->m_index].transformOtherAndStore(pInv[fj->m_index]);
				numInv++;
			
				fj = fj->m_sibling;
				skipChildren = false;
				++res;
				++pTQ;
				res->setFromQuatAndPos(pTQ->m_quat, pTQ->m_translation.asVector3Ref());
			}
			else if (fj->m_parent)
			{
				start[fj->m_index].transformOtherAndStore(pInv[fj->m_index]);
				numInv++;
				fj = fj->m_parent;
				if (fj->m_parent)
					parentTransform = start[fj->m_parent->m_index];
				skipChildren = true;
			}
			else
			{
				start[0].transformOtherAndStore(pInv[0]);
				numInv++;
				break;
			}
			
		}
		PEASSERT(numSteps == m_bindInverses.m_size, "Invlaid number of steps!");
		PEASSERT(numInv == m_bindInverses.m_size, "Invlaid number of steps!");
	}

	void convertPaletteFromLocalToModelSpace(Matrix4x4 *res, Matrix4x4 *pTQ)
	{
/*
		Matrix4x4 *pNewParent = res;
		const Matrix4x4 *pInv = m_bindInverses.getFirstPtr();
		const Matrix4x4 *pCurInv = pInv;
	
		JointCPU *pJoint = m_root.m_subJoints.getFirstPtr();
		
		for (PrimitiveTypes::UInt32 ij = 0, e = m_root.m_subJoints.m_size; ij < e; ++ij)
		{
			++res;
			++pInv;
			internalConvertPaletteFromLocalToModelSpace(*pJoint++, res, pInv, *pNewParent);
		}

		// apply inverse
		*pNewParent = *pNewParent * *pCurInv;
*/

		Matrix4x4 *pInv = m_bindInverses.getFirstPtr();
		
		Matrix4x4 parentTransform;
		parentTransform.loadIdentity();
		FastJoint *fj = &m_fastJoints[0];
		Matrix4x4 *start = res;
		int numSteps = 0;
		int numInv = 0;
		bool skipChildren = false;

		memcpy(res, pTQ, sizeof(Matrix4x4));
		while (fj)
		{
			if (skipChildren)
				numSteps--;
			else	
				//(*res) = parentTransform * (*res);
				res->getTransformedByOther(parentTransform);

			numSteps++;
			PEASSERT(numSteps <= (int)(m_bindInverses.m_size), "Invlaid number of steps!");
			if (fj->m_child && !skipChildren)
			{
				parentTransform = (*res);
				fj = fj->m_child;
				++res;
				++pTQ;
				memcpy(res, pTQ, sizeof(Matrix4x4));
			}
			else if (fj->m_sibling)
			{
				start[fj->m_index].transformOtherAndStore(pInv[fj->m_index]);
				numInv++;
			
				fj = fj->m_sibling;
				skipChildren = false;
				++res;
				++pTQ;
				memcpy(res, pTQ, sizeof(Matrix4x4));
			}
			else if (fj->m_parent)
			{
				start[fj->m_index].transformOtherAndStore(pInv[fj->m_index]);
				numInv++;
				fj = fj->m_parent;
				if (fj->m_parent)
					parentTransform = start[fj->m_parent->m_index];
				skipChildren = true;
			}
			else
			{
				start[0].transformOtherAndStore(pInv[0]);
				numInv++;
				break;
			}
			
		}
		PEASSERT(numSteps == m_bindInverses.m_size, "Invlaid number of steps!");
		PEASSERT(numInv == m_bindInverses.m_size, "Invlaid number of steps!");
	}

	void internalConvertPaletteFromLocalToModelSpace(JointCPU &jnt, Matrix4x4 *&res, Matrix4x4 const *&pInv, const Matrix4x4 &parentTransform)
	{
		(*res) = parentTransform * (*res);

		Matrix4x4 *pNewParent = res;
		const Matrix4x4 *pCurInv = pInv;
	
		JointCPU *pJoint = jnt.m_subJoints.getFirstPtr();

		for (PrimitiveTypes::UInt32 ij = 0, e = jnt.m_subJoints.m_size; ij < e; ++ij)
		{
			++res;
			++pInv;
			internalConvertPaletteFromLocalToModelSpace(*pJoint++, res, pInv, *pNewParent);
			
		}

		// apply inverse
		*pNewParent = *pNewParent * *pCurInv;

/*
		JointCPU *pCurJoint = &m_root;
		int curChild = -1;

		for (int i = 0; i < m_numJoints; ++i)
		{
			(*res) = parentTransform * (*res);
			if (curChild >= pCurJoint->m_subJoints.size 
	
			for (PrimitiveTypes::UInt32 ij = 0, e = jnt.m_subJoints.m_size; ij < e; ++ij)
			{
				++res;
				++pInv;
				internalConvertPaletteFromLocalToModelSpace(*pJoint++, res, pInv, *pNewParent);
			
			}
		}
*/
	}


	void applyInverses(Matrix4x4 *res)
	{
		Matrix4x4 *pInv = m_bindInverses.getFirstPtr();
		for (unsigned int i = 0; i < m_numJoints; ++i)
		{
			//*res = *res * *pInv
			res->transformOtherAndStore(*pInv);
			++res;
			++pInv;
		}
	}

	void applyInverses(Matrix4x4 *res, const Matrix4x4 *src)
	{
		Matrix4x4 *pInv = m_bindInverses.getFirstPtr();
		for (unsigned int i = 0; i < m_numJoints; ++i)
		{
			*res = (*src) * (*pInv);
			++src;
			++res;
			++pInv;
		}
	}

	
	void prepareMatrixPalette(AnimationCPU &anim, PrimitiveTypes::UInt32 curFrame, Array<Matrix4x4> &res)
	{
		PEASSERT(res.m_size == m_numJoints, "palette must match num joints");
		PrimitiveTypes::UInt32 curIndex = 0;
		Matrix4x4 m;
		m.loadIdentity();
		prepareMatrixPalette(m_root, anim, curFrame, res, curIndex, m);
	}

	JointCPU jointByIndex(PrimitiveTypes::UInt32 jointIndex)
	{
		JointCPU res(*m_pContext, m_arena);
		_jointByIndex(jointIndex, m_root, res);
		return res;
	}

	bool _jointByIndex(PrimitiveTypes::UInt32 jointIndex, JointCPU &curJoint, JointCPU &res)
	{
		if (curJoint.m_index == jointIndex)
		{
			res = curJoint;
			return true;
		}

		for (PrimitiveTypes::UInt32 ij = 0; ij < curJoint.m_subJoints.m_size; ij++)
		{
			if (_jointByIndex(jointIndex, curJoint.m_subJoints[ij], res))
			{
				return true;
			}
		}
		return false;
	}

	void prepareMatrixPalette(
		PrimitiveTypes::UInt32 startJoint, const Matrix4x4 &startTransform,
		Array<Handle> &hAnimSetGPUs, Array<AnimationSlot> &slots, 
		Array<Matrix4x4> &additionalLocalTransforms, Array<PrimitiveTypes::Bool> &additionalLocalTransformFlags,
		Array<Matrix4x4> &res)
	{
		assert(res.m_capacity >= m_numJoints);

		res.m_size = startJoint; // skip first joints
		PrimitiveTypes::UInt32 curIndex = startJoint;
		Matrix4x4 m = startTransform;
		
		JointCPU joint = jointByIndex(startJoint);

		prepareMatrixPalette(
			joint,
			hAnimSetGPUs, slots,
			additionalLocalTransforms, additionalLocalTransformFlags,
			res, curIndex, m);
	}
	
	void prepareMatrixPalette(
		Array<Handle> &hAnimSetsGPUs, Array<AnimationSlot> &slots, 
		Array<Matrix4x4> &additionalLocalTransforms, Array<PrimitiveTypes::Bool> &additionalLocalTransformFlags,
		Array<Matrix4x4> &res)
	{
		PEASSERT(res.m_size == m_numJoints, "palette must match num joints");
		PrimitiveTypes::UInt32 curIndex = 0;
		Matrix4x4 m;
		m.loadIdentity();
		
		prepareMatrixPalette(
			m_root,
			hAnimSetsGPUs, slots,
			additionalLocalTransforms, additionalLocalTransformFlags,
			res, curIndex, m);
	}
	
	

	void prepareMatrixPalette(JointCPU &jnt, AnimationCPU &anim, PrimitiveTypes::UInt32 curFrame, Array<Matrix4x4> &res, PrimitiveTypes::UInt32 &curIndex, const Matrix4x4 &parentTransform)
	{
		assert(curIndex == jnt.m_index);

		res[curIndex] = Matrix4x4();

#		ifdef STORE_AS_TSQ
			// create matrix from stores TSQ
			Matrix4x4 local = anim.m_frames[curFrame][jnt.m_index].createMatrix();
#		else
			Matrix4x4 local(anim.m_frames[curFrame][jnt.m_index]);
#		endif

		Matrix4x4 curTransform = parentTransform * local;

		res[curIndex] = curTransform;
	
		for (PrimitiveTypes::UInt32 ij = 0; ij < jnt.m_subJoints.m_size; ij++)
		{
			curIndex += 1;
			prepareMatrixPalette(jnt.m_subJoints[ij], anim, curFrame, res, curIndex, curTransform);
		}
	}

	void prepareMatrixPalette(JointCPU &jnt,
		Array<Handle> &hAnimSetGPUs, Array<AnimationSlot> &slots,
		Array<Matrix4x4> &additionalLocalTransforms, Array<PrimitiveTypes::Bool> &additionalLocalTransformFlags,
		Array<Matrix4x4> &res, PrimitiveTypes::UInt32 &curIndex, const Matrix4x4 &curTransform);

	void copyBindInverses(Array<Matrix4x4> &res);
	Matrix4x4 *getBindInversesPtr(){return m_bindInverses.getFirstPtr();}

private:
	void calculateAndStoreBindInverses(bool alreadyInWorldSpace);
	void internalPrepareBindPoseMatrixPalette(JointCPU &jnt, Array<Matrix4x4> &res, PrimitiveTypes::UInt32 &curIndex, const Matrix4x4 &parentTransform, bool alreadyInWorldSpace);

public:
	void testApplyBindPose(PositionBufferCPU &vb, SkinWeightsCPU &weights, bool isInWorldSpace);
	void testCreatePositionBufferCPU(PositionBufferCPU &res, Array<Matrix4x4> &pal, Array<Matrix4x4> &bindPal, PositionBufferCPU &vb, SkinWeightsCPU &weights);
	void prepareBindPoseMatrixPalette(Array<Matrix4x4> &res, bool alreadyInWorldSpace);

	
	PrimitiveTypes::UInt32 m_numJoints;
private:
	JointCPU m_root;
	Array<Matrix4x4> m_bindInverses;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

}; // namespace PE

#endif
