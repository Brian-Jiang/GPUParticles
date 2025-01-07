#ifndef __PYENGINE_2_0_QUEUE_H__
#define __PYENGINE_2_0_QUEUE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Utils/PEClassDecl.h"
// Sibling/Children includes

template <typename stored_t>
struct Queue : PE::PEAllocatableAndDefragmentable
{
	struct Node : PE::PEAllocatableAndDefragmentable
	{
		stored_t info;
        PE::Handle next;
	};

	Queue()
	{
		m_size = 0;
	}

	~Queue()
	{
	}
	
	PrimitiveTypes::Bool add(const stored_t &val)
	{
		//Create new node and allocate data block
        PE::Handle nh = PE::Handle("QUEUE_NODE", sizeof(Node));

		Node *pNewNode = new(nh) Node();
		
		assert(pNewNode != 0);

		nh.getObject<Node >()->info = val;

		//Add to queue, if first node, becomes head of queue
		if (isEmpty())
		{
			head = nh;
			tail = nh;

			m_size++;
			return true; // success
		}
		else
		{
			tail.getObject<Node>()->next = nh;
			tail = nh;
			m_size++;
			return true; // success
		}
	}

	PrimitiveTypes::Bool deleteFront()
	{
        PE::Handle temp;

		if(!isEmpty())
		{
			temp = head;
			head = head.getObject<Node>()->next;
			temp.release(); 

			if(isEmpty())
			{
				tail = PE::Handle(); //default handle is invalid
			}
			m_size--;
			return true;
		}
		
		assert(false);
		return false; // failure
	}

	void destroy()
	{
        PE::Handle temp;

		while(!isEmpty())
		{
			temp = head;
			head = head.getObject<Node>()->next;
			temp.release(); 
		}
		
		tail = PE::Handle();
	}

	stored_t *getFront() //returns first element (data)
	{
		assert(!isEmpty());
		return &head.getObject<Node>()->info;
	}

	stored_t *getBack() const //returns last element (data)
	{
		assert(!isEmpty());
		return &tail.getObject<Node>()->info;
	}

	PrimitiveTypes::Bool isEmpty() const
	{
		return (!head.isValid()); 
	}

	PrimitiveTypes::UInt32 m_size; // how much is stored at the moment

private:
    PE::Handle head; //handle to front of queue
    PE::Handle tail; //handle to end of queue

};
#endif
