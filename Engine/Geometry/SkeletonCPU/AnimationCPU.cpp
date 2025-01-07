#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "AnimationCPU.h"

#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/FileSystem/FileWriter.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "SkeletonCPU.h"

namespace PE {
	// Reads the animation from file
	void AnimationCPU::ReadAnimation(FileReader &f, SkeletonCPU &skel, float positionFactor, int version)
	{
		
		// Read animation
		f.nextNonEmptyLine(m_name, 128);

		// Partial body animation
		PrimitiveTypes::Int32 numJoints;
		
		f.nextInt32(numJoints);
		
		m_numJoints = numJoints;

		m_startJoint = 0; // todo: partial body animation shouldnt be controlled by range, but rather by retargetting array that we have below
		m_endJoint = m_numJoints-1;

		int targetSkelNumJoints = skel.m_numJoints;

		int animJointToSkelJoint[512];
		PEASSERT(m_endJoint < 512, "not enough buffer to retarget, increase 512");

		int jointUsage[512];
		memset(&jointUsage[0], 0, sizeof(jointUsage));
		PEASSERT(skel.m_numJoints < 512, "need to increase buffer");
		if (version < 3) 
		{
			// no retargetting
			for (int i = 0; i < 512; ++i)
				animJointToSkelJoint[i] = i;
		}
		else
		{
			for (int i = 0; i < 512; ++i)
				animJointToSkelJoint[i] = -1;
			// fill in based on names of joints
			for (int iSrcJoint = 0; iSrcJoint < m_numJoints; ++iSrcJoint)
			{
				char fullJointName[256];
				f.nextNonEmptyLine(fullJointName, 256);
				// need find same joint in skel
				// need to strip out maya napesapce in case the rig was referenced in, in which case joint name will be something like mixamorig:Spine
				int pos = StringOps::lfind(fullJointName, ':');
				const char *jointName = fullJointName;
				if (pos != -1)
				{
					jointName = &fullJointName[pos+1];
				}
				for (int iSkelJoint = 0; iSkelJoint < targetSkelNumJoints; ++iSkelJoint)
				{
					FastJoint &fj = skel.m_fastJoints[iSkelJoint];
					if (StringOps::strcmp(fj.m_pJointCPU->m_name, jointName) == 0 || StringOps::strcmp(fj.m_pJointCPU->m_name, fullJointName) == 0)
					{
						animJointToSkelJoint[iSrcJoint] = iSkelJoint;
						break;
					}
				}
			}
		}

		PrimitiveTypes::Int32 numFrames;
		f.nextInt32(numFrames);
		m_frames.reset(numFrames);

		for (PrimitiveTypes::UInt32 iFrame = 0; iFrame < (PrimitiveTypes::UInt32)(numFrames); iFrame++)
		{
			m_frames.add(Array<TSQ>(*m_pContext, m_arena));
			Array<TSQ> &curFrame = m_frames[iFrame];
			curFrame.reset(targetSkelNumJoints);

			// fill in with default values
			for (int iJoint = 0; iJoint < targetSkelNumJoints; ++iJoint)
			{
				FastJoint &fj = skel.m_fastJoints[iJoint];
				Matrix4x4 parentMatrix;
				parentMatrix.loadIdentity();
				
				if (fj.m_parent)
				{
					float *mf = fj.m_parent->m_pJointCPU->m_matrix;
					int iFloat = 0;

					for (PrimitiveTypes::UInt32 row = 0; row < 4; row++)
					{
						for (PrimitiveTypes::UInt32 col = 0; col < 4; col++)
						{
							parentMatrix.m[col][row] = mf[iFloat++];

							// i dont think we need to multiply since we already have a full matrix from bind pose
							//m.setPos(m.getPos() * positionFactor);
						}
					}
				}
				Matrix4x4 m;
				float *mf = fj.m_pJointCPU->m_matrix;
				int iFloat = 0;

				for (PrimitiveTypes::UInt32 row = 0; row < 4; row++)
				{
					for (PrimitiveTypes::UInt32 col = 0; col < 4; col++)
					{
						m.m[col][row] = mf[iFloat++];

						// i dont think we need to multiply since we already have a full matrix from bind pose
						//m.setPos(m.getPos() * positionFactor);
					}
				}
				
				m = parentMatrix.inverse() * m;
				TSQ tsq(m); // this is experimental, not 100 sure it works properly
				curFrame.add(tsq);
			}

			// read the data in
			for (PrimitiveTypes::UInt32 iJoint = 0; iJoint < m_numJoints; iJoint++)
			{
				// read each frame == numJoints matrices
				int targetJoint = animJointToSkelJoint[iJoint]; // might be retargetting or skipping completely

				Matrix4x4 m;
				Vector3 scale(1.0f, 1.0f, 1.0f);
				if (version == 0)
				{
					for (PrimitiveTypes::UInt32 row = 0; row < 4; row++)
					{
						for (PrimitiveTypes::UInt32 col = 0; col < 4; col++)
						{
							PrimitiveTypes::Float32 val;
							f.nextFloat32(val);
							m.m[col][row] = val;
						}
					}
					scale = Vector3(1.f, 1.f, 1.f);
				}
				else if (version == 1 || version == 2 || version == 3)
				{
					// read translation, rotation, scale
					Vector3 pos;
					f.nextFloat32(pos.m_x); f.nextFloat32(pos.m_y); f.nextFloat32(pos.m_z);
					Matrix4x4 tm(pos);


					Matrix3x3 rm;

					if (version == 1)
					{
						Vector3 rot;
						f.nextFloat32(rot.m_x); f.nextFloat32(rot.m_y); f.nextFloat32(rot.m_z);
						rot *= -1.0f;
						rot *= (PrimitiveTypes::Constants::c_Pi_F32 / 180.0f);

						rm = Matrix3x3(Rotate, rot, RotateOrder_ZYX);
					}
					else
					{
						Quaternion q;
						f.nextFloat32(q.m_w); f.nextFloat32(q.m_x); f.nextFloat32(q.m_y); f.nextFloat32(q.m_z);
						rm = Matrix3x3(q);
					}

					if (iJoint == 0)
					{
						f.nextFloat32(scale.m_x); f.nextFloat32(scale.m_y); f.nextFloat32(scale.m_z);
					}
					else
					{
						Vector3 temp;
						f.nextFloat32(temp.m_x); f.nextFloat32(temp.m_y); f.nextFloat32(temp.m_z);
					}
					Matrix3x3 res3x3 = rm;
					Matrix4x4 res(res3x3, pos);
					m = res;
				}

				m.setPos(m.getPos() * positionFactor);

				TSQ tsq(m, scale);
				if (targetJoint != -1)
				{
					curFrame[targetJoint] = tsq;
					jointUsage[targetJoint]++;
				}
				else
					PEINFO("Skipping joint %d\n", iJoint);
			}
		}
	}


}; // namespace PE
