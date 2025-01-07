#include "SkeletonCPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"
namespace PE {

void SkeletonCPU::readJoint(JointCPU &dest, JointCPU *pParentJoint, FastJoint *fastJointParent, FastJoint *&fastJointSibling, FileReader &f, PrimitiveTypes::Int32 &jointsLeft, float positionFactor, bool isInWorldSpace)
{
	if (jointsLeft == 0)
		return;

	jointsLeft--;

	f.nextNonEmptyLine(dest.m_name, 255); // reading joint name, don't really need it

	PrimitiveTypes::Int32 jointIndex;
	f.nextInt32(jointIndex);
	dest.m_index = (PrimitiveTypes::UInt32)(jointIndex);

	FastJoint &fj = m_fastJoints[jointIndex];
	fj.m_parent = fastJointParent;
	fj.m_index = dest.m_index;
	fj.m_pJointCPU = &dest;

	m_jointParents[fj.m_index].m_parentJointIndex = fastJointParent ? fastJointParent->m_index : -1;

	if (fastJointSibling)
		fastJointSibling->m_sibling = &fj;
	fastJointSibling = &fj; // next sibling will set this joint as referencing sibling

	for (PrimitiveTypes::UInt32 iM = 0; iM < 16; iM++)
	{
		f.nextFloat32(dest.m_matrix[iM]);
	}

	dest.m_matrix[12] *= positionFactor;
	dest.m_matrix[13] *= positionFactor;
	dest.m_matrix[14] *= positionFactor;

	PrimitiveTypes::Int32 n;
	f.nextInt32(n); // this joint has n sub-joints
	dest.m_subJoints.reset(n);

	FastJoint *prevSibling = fastJointSibling;
	fastJointSibling = NULL; // firt child doesnt have referencing sibling
	for (PrimitiveTypes::Int32 iJoint = 0; iJoint < n; iJoint++)
	{
		dest.m_subJoints.add(JointCPU(*m_pContext, m_arena));
		readJoint(dest.m_subJoints[dest.m_subJoints.m_size- 1], &dest, &fj, fastJointSibling, f, jointsLeft, positionFactor, isInWorldSpace);
		if (iJoint == 0)
		{
			// this is the first child
			fj.m_child = fastJointSibling;
		}
	}
	fastJointSibling = prevSibling;
	/*
	// need to multiply this joint by parent's inverse to get local xform
	if (pParentJoint && isInWorldSpace)
	{
		Matrix4x4 parent(pParentJoint->m_matrix);
		Matrix4x4 thisInWorldSpace(dest.m_matrix);
		Matrix4x4 thisInLocal = thisInWorldSpace * parent.inverse();
		thisInLocal.toFloat16(dest.m_matrix);
	}
	*/

	/*
	{
		Matrix4x4 thisInWorldSpace(dest.m_matrix);
		Vector3 u = thisInWorldSpace.getU();
		Vector3 v = thisInWorldSpace.getV();
		Vector3 n = thisInWorldSpace.getN();
		u.normalize();
		v.normalize();
		n.normalize();
		thisInWorldSpace.setU(u);
		thisInWorldSpace.setV(v);
		thisInWorldSpace.setN(n);

		thisInWorldSpace.toFloat16(dest.m_matrix);
	}
	*/
}

// Reads the specified buffer from file
void SkeletonCPU::ReadSkeleton(const char *filename, const char *package)
{
    PEString::generatePathname(*m_pContext, filename, package, "Skeletons", PEString::s_buf, PEString::BUF_SIZE);
	
	StringOps::writeToString(filename, m_name, 256);

    // Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	bool inWorldSpace = false;

	if (0 == strcmp(line, "SKELETON_WORLD"))
		inWorldSpace = true;

	// TODO : make sure it is "SKELETON" and set version to 0 or other version otherwise
	int version = 0;

	float positionFactor = version == 0 ? (1.0f / 100.0f) : 1.0f;

	if (inWorldSpace)
		positionFactor = 1.0f; // new version of exporter uses meters

	PrimitiveTypes::Int32 n;
	f.nextInt32(n);
	// there are n joints in the skeleton
	m_fastJoints.reset(n);
	m_fastJoints.m_size = n;

	m_jointParents.reset(n);
	m_jointParents.m_size = n;

	FastJoint *sibling = NULL;
	readJoint(m_root, NULL, NULL, sibling, f, n, positionFactor, inWorldSpace);
	m_numJoints = countJoints(m_root);
	PEASSERT(m_numJoints == m_fastJoints.m_size, "Unexpected number of joints");
	calculateAndStoreBindInverses(inWorldSpace);
}

void SkeletonCPU::calculateAndStoreBindInverses(bool alreadyInWorldSpace)
{
	m_bindInverses.reset(m_numJoints);
	m_bindInverses.m_size = m_numJoints;
	prepareBindPoseMatrixPalette(m_bindInverses, alreadyInWorldSpace);
	for (PrimitiveTypes::UInt32 ij = 0; ij < m_numJoints; ij++)
	{
		m_bindInverses[ij] = m_bindInverses[ij].inverse();
	}
}
void SkeletonCPU::prepareBindPoseMatrixPalette(Array<Matrix4x4> &res, bool alreadyInWorldSpace)
{
	PEASSERT(res.m_size == m_numJoints, "palette must match num joints");
	PrimitiveTypes::UInt32 curIndex = 0;
	Matrix4x4 m;
	m.loadIdentity();
	internalPrepareBindPoseMatrixPalette(m_root, res, curIndex, m, alreadyInWorldSpace);
}
void SkeletonCPU::internalPrepareBindPoseMatrixPalette(JointCPU &jnt, Array<Matrix4x4> &res, PrimitiveTypes::UInt32 &curIndex, const Matrix4x4 &parentTransform, bool alreadyInWorldSpace)
{
	assert(curIndex == jnt.m_index);

	Matrix4x4 local(jnt.m_matrix);

	Matrix4x4 curTransform = alreadyInWorldSpace ? local : parentTransform * local;

	memcpy(res[curIndex].m, curTransform.m, sizeof(curTransform.m));
	
	for (PrimitiveTypes::UInt32 ij = 0; ij < jnt.m_subJoints.m_size; ij++)
	{
		curIndex += 1;
		internalPrepareBindPoseMatrixPalette(jnt.m_subJoints[ij], res, curIndex, curTransform, alreadyInWorldSpace);
	}
}

void SkeletonCPU::copyBindInverses(Array<Matrix4x4> &res)
{
	PEASSERT(res.m_size == m_numJoints && m_bindInverses.m_size == m_numJoints, "palette must match num joints");
	PrimitiveTypes::UInt32 curIndex = 0;
	memcpy(res.getFirstPtr(), m_bindInverses.getFirstPtr(), m_numJoints * sizeof(Matrix4x4));
}

void SkeletonCPU::prepareMatrixPalette(JointCPU &jnt,
	Array<Handle> &hAnimSetGPUs, Array<AnimationSlot> &slots,
	Array<Matrix4x4> &additionalLocalTransforms, Array<PrimitiveTypes::Bool> &additionalLocalTransformFlags,
	Array<Matrix4x4> &res, PrimitiveTypes::UInt32 &curIndex, const Matrix4x4 &parentTransform)
{
	assert(curIndex == jnt.m_index);

	PrimitiveTypes::UInt32 toBlend[32]; // indices of slots needed to blend

	PrimitiveTypes::UInt32 nBlend = 0;

	bool hasPartial = false;
	for (PrimitiveTypes::UInt32 iSlot = 0; iSlot < slots.m_size; iSlot++)
	{
		AnimationSlot &curSlot = slots[iSlot];
		//dbg
        //int active = curSlot.m_flags & ACTIVE;
		if ((curSlot.m_flags & ACTIVE) == 0)
			continue;
		if (curSlot.m_flags & PARTIAL_BODY_ANIMATION)
		{
			if (curSlot.m_startJoint <= curIndex && curIndex <= curSlot.m_endJoint)
			{
				// Partial body animation overrides any other animation
				hasPartial = true;
				break;
			}
		}
	}

	for (PrimitiveTypes::UInt32 iSlot = 0; iSlot < slots.m_size; iSlot++)
	{
		AnimationSlot &curSlot = slots[iSlot];
		if (!(curSlot.m_flags & ACTIVE))
			continue;

		if (hasPartial)
		{
			if (curSlot.m_flags & PARTIAL_BODY_ANIMATION)
			{
				if (curSlot.m_startJoint <= curIndex && curIndex <= curSlot.m_endJoint)
				{
					toBlend[nBlend++] = iSlot;
				}
			}
		}
		else
		{
			toBlend[nBlend++] = iSlot;
		}
	}

	Matrix4x4 local; // will store local joint transformation

	if (nBlend >= 1)
	{
		AnimationSlot &slot0 = slots[toBlend[0]];
		// create matrix from stored TSQ
		AnimSetBufferGPU *pAnimSetGPU = hAnimSetGPUs[slot0.m_animationSetIndex].getObject<AnimSetBufferGPU>();
		AnimationSetCPU *pAnimSetCPU = pAnimSetGPU->m_hAnimationSetCPU.getObject<AnimationSetCPU>();

		AnimationCPU &anim0 = pAnimSetCPU->m_animations[slot0.m_animationIndex];
		TSQ a0 = anim0.m_frames[slot0.m_iFrameIndex0][jnt.m_index];
		TSQ b0 = anim0.m_frames[slot0.m_iFrameIndex1][jnt.m_index];
		TSQ avg0 = TSQ::SLERP(a0, b0, slot0.m_blendFactor);
		
		if (nBlend < 2)
		{
			local = avg0.createMatrix();
		}
		else
		{
			
			AnimationSlot &slot1 = slots[toBlend[1]];
			AnimSetBufferGPU *pAnimSetGPU1 = hAnimSetGPUs[slot1.m_animationSetIndex].getObject<AnimSetBufferGPU>();
			AnimationSetCPU *pAnimSetCPU1 = pAnimSetGPU1->m_hAnimationSetCPU.getObject<AnimationSetCPU>();
			AnimationCPU &anim1 = pAnimSetCPU1->m_animations[slot1.m_animationIndex];
			TSQ a1 = anim1.m_frames[slot1.m_iFrameIndex0][jnt.m_index];
			TSQ b1 = anim1.m_frames[slot1.m_iFrameIndex1][jnt.m_index];
			TSQ avg1 = TSQ::SLERP(a1, b1, slot1.m_blendFactor);


			if (nBlend < 3)
			{
				PrimitiveTypes::Float32 blendFactor = slot1.m_weight / (slot0.m_weight + slot1.m_weight);

				TSQ avg = TSQ::SLERP(avg0, avg1, blendFactor);

				local = avg.createMatrix();
			}
			else
			{
				PrimitiveTypes::Float32 blendFactor01 = slot1.m_weight / (slot0.m_weight + slot1.m_weight);

				TSQ avg01 = TSQ::SLERP(avg0, avg1, blendFactor01);

				AnimationSlot &slot2 = slots[toBlend[2]];
				AnimSetBufferGPU *pAnimSetGPU2 = hAnimSetGPUs[slot1.m_animationSetIndex].getObject<AnimSetBufferGPU>();
				AnimationSetCPU *pAnimSetCPU2 = pAnimSetGPU2->m_hAnimationSetCPU.getObject<AnimationSetCPU>();

				AnimationCPU &anim2 = pAnimSetCPU2->m_animations[slot2.m_animationIndex];
				TSQ a2 = anim2.m_frames[slot2.m_iFrameIndex0][jnt.m_index];
				TSQ b2 = anim2.m_frames[slot2.m_iFrameIndex1][jnt.m_index];
				TSQ avg2 = TSQ::SLERP(a2, b2, slot2.m_blendFactor);

				if (nBlend < 4)
				{
					PrimitiveTypes::Float32 blendFactor = slot2.m_weight / (slot0.m_weight + slot1.m_weight + slot2.m_weight);

					TSQ avg = TSQ::SLERP(avg01, avg2, blendFactor);

					local = avg.createMatrix();

				}
				else
				{
					assert(!"Can't blend more than three animations!");
				}
			}
		}

		
	}
	else
	{
		//assert(!"Have no animation for this joint!");
	}

	if (additionalLocalTransformFlags.m_size > jnt.m_index && additionalLocalTransformFlags[jnt.m_index])
	{
		local = additionalLocalTransforms[jnt.m_index] * local;
	}

	Matrix4x4 curTransform = parentTransform * local;

	res[curIndex] = curTransform;

	for (PrimitiveTypes::UInt32 ij = 0; ij < jnt.m_subJoints.m_size; ij++)
	{
		curIndex += 1;
		prepareMatrixPalette(jnt.m_subJoints[ij],
			hAnimSetGPUs, slots, 
			additionalLocalTransforms, additionalLocalTransformFlags,
			res, curIndex, curTransform);
	}
}



void SkeletonCPU::testApplyBindPose(PositionBufferCPU &vb, SkinWeightsCPU &weights, bool isInWorldSpace)
{
	Array<PrimitiveTypes::Float32> diffs(*m_pContext, m_arena, vb.m_values.m_size / 3);
	PrimitiveTypes::Float32 maxDiff = 0.0f;
	Array<Matrix4x4> pal(*m_pContext, m_arena);
	pal.reset(m_bindInverses.m_size);
	pal.m_size = m_bindInverses.m_size;
	prepareBindPoseMatrixPalette(pal, isInWorldSpace);
	assert(vb.m_values.m_size / 3 == weights.m_weightsPerVertex.m_size);
	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		Vector3 v0(vb.m_values[iv *3], vb.m_values[iv *3 + 1], vb.m_values[iv *3 + 2]);
		Vector3 v(0, 0, 0);
		// this vertex is diformed by these joints:
			
		for (PrimitiveTypes::UInt32 ij = 0; ij < weights.m_weightsPerVertex[iv].m_size; ij++)
		{
			WeightPair &wp = weights.m_weightsPerVertex[iv][ij];
			PrimitiveTypes::UInt32 jointIndex = wp.m_jointIndex;
			Matrix4x4 &inv = m_bindInverses[jointIndex];
			Matrix4x4 &m = pal[jointIndex];
			Matrix4x4 m_inv = m * inv;
			v += m_inv * v0 * wp.m_weight;
		}

		PrimitiveTypes::Float32 diff = (v0 - v).length();
		diffs.add(diff);
		if (diff > maxDiff)
		{
			maxDiff = diff;
		}
	}
	
	// find average error
	PrimitiveTypes::Float32 avg = 0;
	for (PrimitiveTypes::UInt32 i = 0; i < diffs.m_size; i++)
	{
		avg += diffs[i];
	}
	avg /= (PrimitiveTypes::Float32)(diffs.m_size);
	return;
}


void SkeletonCPU::testCreatePositionBufferCPU(PositionBufferCPU &res, Array<Matrix4x4> &pal, Array<Matrix4x4> &bindPal, PositionBufferCPU &vb, SkinWeightsCPU &weights)
{
	assert(m_numJoints == pal.m_size);
	assert(vb.m_values.m_size / 3 == weights.m_weightsPerVertex.m_size);
	res.m_values.reset(vb.m_values.m_size);

	for (PrimitiveTypes::UInt32 iv = 0; iv < vb.m_values.m_size / 3; iv++)
	{
		Vector3 v0(vb.m_values[iv *3], vb.m_values[iv *3 + 1], vb.m_values[iv *3 + 2]);
		Vector3 v(0, 0, 0);
		Vector3 vt(0, 0, 0);

		// this vertex is deformed by these joints:
			
		for (PrimitiveTypes::UInt32 ij = 0; ij < weights.m_weightsPerVertex[iv].m_size; ij++)
		{
			WeightPair &wp = weights.m_weightsPerVertex[iv][ij];
			PrimitiveTypes::UInt32 jointIndex = wp.m_jointIndex;
			Matrix4x4 &inv = m_bindInverses[jointIndex];
			Matrix4x4 &m = pal[jointIndex];
			//Matrix4x4 bindm = bindPal[jointIndex];
				

			/*
			Quaternion rotation = m.createQuat();
			Vector3 translation(m.m[0][3], m.m[1][3], m.m[2][3]);
			Vector3 vAfterInv = (inv * v0);
			Vector3 vAfterRot = rotateCW(rotation, vAfterInv);
			Vector3 vAfterTrans = vAfterRot + translation;
			v += vAfterTrans * wp.m_weight;
			*/

			//Vector3 com = (inv * v0) * wp.m_weight;
			//vt += ((bindm * inv) * v0) * wp.m_weight;
			v += (m * (inv * v0)) * wp.m_weight;
		}
		res.m_values.add(v.m_x);
		res.m_values.add(v.m_y);
		res.m_values.add(v.m_z);
	}
}
}; // namespace PE
