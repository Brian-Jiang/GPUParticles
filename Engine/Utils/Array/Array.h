#ifndef __PYENGINE_2_0_ARRAY_H__
#define __PYENGINE_2_0_ARRAY_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Utils/PEClassDecl.h"
#include "PrimeEngine/Game/Common/GameContext.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
// Sibling/Children includes

template <typename stored_t, int resizable = 0>
struct Array : PE::PEAllocatableAndDefragmentable
{
    PE::Handle m_dataHandle;
	PrimitiveTypes::UInt32 m_capacity; // how much can store
	PrimitiveTypes::UInt32 m_size; // how much is stored at the moment
	PrimitiveTypes::UInt32 m_dataSize;
	PE::MemoryArena m_memoryArena;
	PE::GameContext *m_pContext;

	Array(PE::GameContext &context, PE::MemoryArena arena)
	{
		init(context, arena);
	}

	Array()
	{
		m_memoryArena = PE::MemoryArena_Invalid;
		m_pContext = NULL;
	}

	void init(PE::GameContext &context, PE::MemoryArena arena)
	{
		m_memoryArena = arena;
		m_pContext = &context;
		m_size = 0;
		m_capacity = 0;
		m_dataSize = 0;
		//m_dataHandle is initialized as INVALID
	}

	Array(PE::GameContext &context, PE::MemoryArena arena, PrimitiveTypes::UInt32 capacity)
	{
		m_memoryArena = arena;
		m_pContext = &context;
		constructFromCapacity(capacity);
	}

	Array(PE::GameContext &context, PE::MemoryArena arena, PrimitiveTypes::UInt32 capacity, const stored_t &val)
	{
		m_memoryArena = arena;
		m_pContext = &context;

		constructFromCapacity(capacity);
		for (PrimitiveTypes::UInt32 i = 0; i < capacity; i++)
			add(val);
	}

	~Array()
	{
		//m_dataHandle.release();
	}

	void constructFromCapacity(PrimitiveTypes::UInt32 capacity)
	{
		m_size = 0;

		// how much will be stored max
		m_capacity = capacity;

		m_dataSize = sizeof(stored_t) * capacity;

		// Allocate data block
		if (m_dataSize > 0)
		{
			if (m_memoryArena != PE::MemoryArena_Invalid)
			{
				m_dataHandle.m_cachedPtr = PE::pemallocAlligned(m_memoryArena, m_dataSize, ALLIGNMENT, m_dataHandle.m_memoryOffset);
			}
			else
				m_dataHandle = PE::Handle("RAW_DATA", m_dataSize);
			memset(m_dataHandle.getObject(), 0, m_dataSize);
		}
		else
		{
			m_dataHandle = PE::Handle(); // INVALID
		}
	}

	void reset(PrimitiveTypes::UInt32 capacity, bool copyOld = false)
	{
		PrimitiveTypes::UInt32 oldSize = m_size;
		PE::Handle oldHandle = m_dataHandle;
		if (!copyOld && m_dataHandle.isValid())
		{
			if (m_memoryArena != PE::MemoryArena_Invalid)
				PE::pefreeAlligned(m_memoryArena, m_dataHandle.m_cachedPtr, m_dataHandle.m_memoryOffset);
			else
				m_dataHandle.release();
		}

		constructFromCapacity(capacity);

		if (copyOld && oldHandle.isValid())
		{
			assert(oldSize <= capacity);
			memcpy(m_dataHandle.getObject(), oldHandle.getObject(), oldSize * sizeof(stored_t));
			m_size = oldSize;
			if (m_memoryArena != PE::MemoryArena_Invalid)
				PE::pefreeAlligned(m_memoryArena, oldHandle.m_cachedPtr, oldHandle.m_memoryOffset);
			else
				oldHandle.release();
		}
	}

	void clear()
	{
		m_size = 0;
	}

	stored_t &operator[](PrimitiveTypes::UInt32 index)
	{
		// TODO: OPTIMIZE

		if (index >= m_size)
		{
			assert(index < m_size);
		}

		return m_dataHandle.getObject<stored_t>()[index];
	}

	stored_t &getByIndexUnchecked(PrimitiveTypes::UInt32 index)
	{
		return m_dataHandle.getObject<stored_t>()[index];
	}

	stored_t *getFirstPtr()
	{
		return m_dataHandle.getObject<stored_t>();
	}

	void operator=(const Array<stored_t> &rhs)
	{
		this->m_capacity = rhs.m_capacity;
		this->m_size = rhs.m_size;
		this->m_dataSize = rhs.m_dataSize;
		this->m_dataHandle = rhs.m_dataHandle;
	}

	PrimitiveTypes::Bool add(const stored_t &val)
	{
		if (m_size < m_capacity)
		{
			//memcpy(&(this->operator [](m_size)), &val, sizeof(stored_t));
			getByIndexUnchecked(m_size) = val;
			m_size++;
			return true; // success
		}
		else
		{
			// need to resize
			if (resizable)
			{
				reset(m_capacity * 2, true);
				return add(val);
			}
			else
			{
				assert(false);
				return false; // failure
			}
		}
	}

	PrimitiveTypes::Bool add(const stored_t &val0, const stored_t &val1)
	{
		if (m_size+1 < m_capacity)
		{
			//memcpy(&(this->operator [](m_size)), &val, sizeof(stored_t));
			getByIndexUnchecked(m_size) = val0;
			getByIndexUnchecked(m_size + 1) = val1;
			m_size+=2;
			return true; // success
		}
		else
		{
			assert(false);
			return false; // failure
		}
	}

	PrimitiveTypes::Bool add(const stored_t &val0, const stored_t &val1, const stored_t &val2)
	{
		if (m_size+2 < m_capacity)
		{
			//memcpy(&(this->operator [](m_size)), &val, sizeof(stored_t));
			getByIndexUnchecked(m_size) = val0;
			getByIndexUnchecked(m_size + 1) = val1;
			getByIndexUnchecked(m_size + 2) = val2;
			m_size+=3;
			return true; // success
		}
		else
		{
			assert(false);
			return false; // failure
		}
	}

	PrimitiveTypes::Bool add(const stored_t &val0, const stored_t &val1, const stored_t &val2, const stored_t &val3)
	{
		if (m_size+3 < m_capacity)
		{
			//memcpy(&(this->operator [](m_size)), &val, sizeof(stored_t));
			getByIndexUnchecked(m_size) = val0;
			getByIndexUnchecked(m_size + 1) = val1;
			getByIndexUnchecked(m_size + 2) = val2;
			getByIndexUnchecked(m_size + 3) = val3;
			m_size+=4;
			return true; // success
		}
		else
		{
			assert(false);
			return false; // failure
		}
	}

	PrimitiveTypes::Bool insert(const stored_t &val, PrimitiveTypes::UInt32 pos)
	{
		if (m_size < m_capacity)
		{
			
			if (pos < m_size)
				memmove(&(*this)[pos+1], &(*this)[pos], (m_size + 1 - pos) * sizeof(stored_t));
		
			m_size++;
			
			this->operator [](pos) = val;
			return true; // success
		}
		else
		{
			assert(false);
			return false; // failure
		}
	}


	PrimitiveTypes::UInt32 indexOf(const stored_t &v)
	{
		PrimitiveTypes::UInt32 res = PrimitiveTypes::Constants::c_MaxUInt32;
		for (PrimitiveTypes::UInt32 i = 0; i < m_size; i++)
		{
			if ( (*this)[i] == v)
			{
				return i;
			}
		}
		return res;
	}

	void remove(PrimitiveTypes::UInt32 index)
	{
		assert(index < m_size);
		if (index < m_size-1)
		{
			memmove(&(*this)[index], &(*this)[index+1], (m_size - 1 - index) * sizeof(stored_t));
		}
		else
		{
			// last element
		}
		m_size -= 1;

	}

   
};

template <typename T0, typename T1, int ID> struct TypeChooser { typedef T0 Type; };

template <typename T0, typename T1> struct TypeChooser<T0, T1, 1> { typedef T1 Type; };

template <typename stored_t, int SIZE, int AllowDefaultContructor = 1>
struct PEStaticArray
{
	stored_t &operator[](PrimitiveTypes::UInt32 index)
	{
		return *(reinterpret_cast<stored_t *>(&m_data[index]));
	}

	stored_t *getFirstPtr()
	{
		return reinterpret_cast<stored_t*>(&m_data[0]);
	}

	struct _StoredMemoryStruct
	{
		char m_data[sizeof(stored_t)];
	};

	typedef typename TypeChooser<_StoredMemoryStruct, stored_t, AllowDefaultContructor>::Type StoredMemoryStruct;
	StoredMemoryStruct m_data[SIZE];
};

template <typename stored_t, int SIZE, int AllowDefaultContructor = 1>
struct PEStaticVector
{
	struct _StoredMemoryStruct
	{
		char m_data[sizeof(stored_t)];
	};

	typedef typename TypeChooser<_StoredMemoryStruct, stored_t, AllowDefaultContructor>::Type StoredMemoryStruct;

	PEStaticVector()
	{
		m_size = 0;
	}

	~PEStaticVector()
	{
	}

	void clear()
	{
		m_size = 0;
	}

	stored_t &operator[](PrimitiveTypes::UInt32 index)
	{
		// TODO: OPTIMIZE

		if (index >= m_size)
		{
			assert(index < m_size);
		}

		return *(reinterpret_cast<stored_t *>(&m_data[index]));
	}

	PrimitiveTypes::Bool add(const stored_t &val)
	{
		if (m_size < SIZE)
		{
			m_size++;
			this->operator [](m_size - 1) = val;
			return true; // success
		}
		else
		{
			assert(false);
			return false; // failure
		}
	}

	PrimitiveTypes::Bool insert(const stored_t &val, PrimitiveTypes::UInt32 pos)
	{
		if (m_size < SIZE)
		{
			m_size++;

			if (pos < m_size-1)
				memmove(&(*this)[pos+1], &(*this)[pos], (m_size - pos) * sizeof(stored_t));

			this->operator [](pos) = val;
			return true; // success
		}
		else
		{
			assert(false);
			return false; // failure
		}
	}

	stored_t *getFirstPtr()
	{
		return reinterpret_cast<stored_t*>(&m_data[0]);
	}

	PrimitiveTypes::UInt32 indexOf(const stored_t &v)
	{
		PrimitiveTypes::UInt32 res = PrimitiveTypes::Constants::c_MaxUInt32;
		for (PrimitiveTypes::UInt32 i = 0; i < m_size; i++)
		{
			if ( (*this)[i] == v)
			{
				return i;
			}
		}
		return res;
	}

	void remove(PrimitiveTypes::UInt32 index)
	{
		assert(index < m_size);
		if (index < m_size - 1)
		{
			memmove(&(*this)[index], &(*this)[index+1], (m_size - index - 1) * sizeof(stored_t));
		}
		else
		{
			// last element
		}
		m_size -= 1;
	}

	StoredMemoryStruct m_data[SIZE];
	PrimitiveTypes::UInt32 m_size; // how much is stored at the moment
};
#endif
