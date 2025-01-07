#ifndef __PYENGINE_2_0_PROFILING_H__
#define __PYENGINE_2_0_PROFILING_H__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/APIAbstraction/Timer/Timer.h"

// Sibling/Children includes
namespace PE {
	namespace Profiling {

#if PE_ENABLE_GPU_PROFILING
enum Group
{
	Group_DrawCalls,
	
	Group_DrawThread,


	Group_NumGroups
};

struct EventQuery
{
	EventQuery();
	void start(Group group, LPDIRECT3DDEVICE9 pD3D9Device, Timer::TimeType time, const char *dbgName);

	bool checkFinished(bool flushCommandBuffer, bool waitForCompletion, Timer::TimeType &time, Timer &timer);
	~EventQuery();

	IDirect3DQuery9* m_pEventQuery;
	Timer::TimeType m_startTime;
	Timer::TimeType m_endTime;
	const char *m_dbgName;
};

struct GroupQueries
{
	GroupQueries():m_numQueries(0)
	{
		finalize();
	}

	void finalize();

	static const int NUM_QUERIES_IN_GROUP = 1024;
	EventQuery m_queries[NUM_QUERIES_IN_GROUP];
	unsigned int m_numQueries;
	unsigned int m_activeQueries[NUM_QUERIES_IN_GROUP];
	unsigned int m_availableQueries[NUM_QUERIES_IN_GROUP];
	int m_numActiveQueries;
	int m_numAvailableQueries;

	bool m_recordStarted;
	int m_numQueriesExecuted;
	int m_numQueriesScheduled;
	Timer::TimeType m_startTime;
	Timer::TimeType m_endTime;

	float m_time;
};


struct Profiler : PE::PEAllocatableAndDefragmentable
{
	Profiler();

	void startEventQuery(Group group, LPDIRECT3DDEVICE9 pD3D9Device, Timer::TimeType time, const char *dbgName);
	void update(Group group, bool flushCommandBuffer, bool waitForCompletion, Timer::TimeType time, Timer &timer);
	void finalize(Group group);


	static void Construct();
	static void Destroy();
	static Profiler* Instance(){return s_pProfiler;}

	GroupQueries m_queries[Group_NumGroups];

	static Handle s_hProfiler;
	static Profiler *s_pProfiler;
};
#endif
}
}; // namespace PE
#endif
