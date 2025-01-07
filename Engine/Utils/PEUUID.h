#ifndef __PYENGINE_2_PEUUID_H__
#define __PYENGINE_2_PEUUID_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
// Sibling/Children includes


struct PEUUID
{
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning (disable: 4201)
#endif
	union
	{
		struct
		{
			PrimitiveTypes::UInt32 Data1;
			PrimitiveTypes::UInt16 Data2;
			PrimitiveTypes::UInt16 Data3;
			PrimitiveTypes::Byte Data4[8];
		};

		struct 
		{
			PrimitiveTypes::Byte m_bytes[16];
		};

		struct 
		{
			PrimitiveTypes::UInt32 m_uints32[4];
		};
	};
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning(default : 4201)
#endif
	PEUUID()
	{
		set(0,0,0,0);
	}

	PEUUID(const PEUUID &cpy)
	{
		memcpy(m_bytes, cpy.m_bytes, sizeof(PEUUID));
	}

	void set(PrimitiveTypes::UInt32 v0,
		PrimitiveTypes::UInt32 v1,
		PrimitiveTypes::UInt32 v2,
		PrimitiveTypes::UInt32 v3)
	{
		m_uints32[0] = v0;
		m_uints32[1] = v1;
		m_uints32[2] = v2;
		m_uints32[3] = v3;

	}

	bool isEqualTo(const PEUUID &uuid) const
	{
		return 
			m_uints32[0] == uuid.m_uints32[0] &&
			m_uints32[1] == uuid.m_uints32[1] &&
			m_uints32[2] == uuid.m_uints32[2] &&
			m_uints32[3] == uuid.m_uints32[3];
	}

	bool isEqualTo(
		const PrimitiveTypes::UInt32 &v0,
		const PrimitiveTypes::UInt32 &v1,
		const PrimitiveTypes::UInt32 &v2,
		const PrimitiveTypes::UInt32 &v3) const
	{
		return
			m_uints32[0] == v0 &&
			m_uints32[1] == v1 &&
			m_uints32[2] == v2 &&
			m_uints32[3] == v3;
	}

	bool isZero() { return m_uints32[0] == 0 && m_uints32[1] == 0 && m_uints32[2] == 0 && m_uints32[3] == 0; }
};

template <PrimitiveTypes::UInt32 v0, PrimitiveTypes::UInt32 v1, PrimitiveTypes::UInt32 v2, PrimitiveTypes::UInt32 v3>
struct PESUUID_t
{
	static const PrimitiveTypes::UInt32 s_v0 = v0;
	static const PrimitiveTypes::UInt32 s_v1 = v1;
	static const PrimitiveTypes::UInt32 s_v2 = v2;
	static const PrimitiveTypes::UInt32 s_v3 = v3;

	static PEUUID peuuid()
	{
		PEUUID res;
		res.set(s_v0, s_v1, s_v2, s_v3);
		return res;
	}
};

template <typename PESUUID_T>
struct PESLookupBySUUID_t
{
	static const bool s_defined = false;
	typedef void Type;
};

#define PEMACRO_DECLARE_PESUUID(name, v0, v1, v2, v3) typedef PESUUID_t<v0, v1, v2, v3> name;

#define PEMACRO_REGISTER_LOOKUP_BY_SUUID(suuid, cls) \
template<> \
struct PESLookupBySUUID_t<suuid> { \
static const bool s_defined = true; \
	typedef cls Type; \
};

#define PEMACRO_START_LUAH_NAMESPACE_TABLE(name) namespace name {
#define PEMACRO_END_LUAH_NAMESPACE_TABLE() }
#define PEMACRO_COMMENT(s)


#endif
