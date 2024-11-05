// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

// Sibling/Children includes
#include "Profiling.h"

#include "PrimeEngine/Scene/TextMesh.h"
#include "PrimeEngine/Scene/LineMesh.h"
#include "PrimeEngine/Scene/TextSceneNode.h"
#include "PrimeEngine/Scene/RootSceneNode.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

using namespace PE::Events;
using namespace PE::Components;
namespace PE {
#if PE_ENABLE_GPU_PROFILING

namespace Profiling {
Handle Profiler::s_hProfiler;
Profiler *Profiler::s_pProfiler = 0;

void Profiler::Construct()
{
	s_hProfiler = Handle("Profiler", sizeof(Profiler));
	s_pProfiler = new (s_hProfiler) Profiler();

	
}

void Profiler::Destroy()
{
	s_pProfiler->~Profiler();
	s_hProfiler.release();
}

void Profiler::startEventQuery(Group group, LPDIRECT3DDEVICE9 pD3D9Device, Timer::TimeType time, const char *dbgName)
{
	GroupQueries &queries = m_queries[group];
	int index = queries.m_availableQueries[--queries.m_numAvailableQueries];
	queries.m_activeQueries[queries.m_numActiveQueries++] = index;
	queries.m_queries[index].start(group, pD3D9Device, time, dbgName);
	
	if (!queries.m_recordStarted)
	{
		queries.m_recordStarted = true;
		queries.m_startTime = time;
	}
	queries.m_numQueriesScheduled++;
}

Profiler::Profiler()
{
	for (int group = 0; group < Group_NumGroups; ++group)
	{
		GroupQueries &queries = m_queries[group];

		for (int i = 0; i < GroupQueries::NUM_QUERIES_IN_GROUP; ++i)
			queries.m_availableQueries[i] = (GroupQueries::NUM_QUERIES_IN_GROUP - 1) - i;
		queries.m_numAvailableQueries = GroupQueries::NUM_QUERIES_IN_GROUP;
	}
}

void Profiler::update(Group group, bool flushCommandBuffer, bool waitForCompletion, Timer::TimeType time, Timer &timer)
{
	GroupQueries &queries = m_queries[group];

	int newNumActiveQueries = 0;
	
	for (int i = 0; i < queries.m_numActiveQueries; ++i)
	{
		int queryIndex = queries.m_activeQueries[i];
		EventQuery &query = queries.m_queries[queryIndex];
		bool done = query.checkFinished(flushCommandBuffer, waitForCompletion, time, timer);
		//flushCommandBuffer = false; // we only need to flush once, even if have more than one query
		if (!done)
		{
			if (waitForCompletion)
				assert(!"when waitForCompletion is set, there should be no queries left");

			queries.m_activeQueries[newNumActiveQueries++] = queryIndex;
		}
		else
		{
			queries.m_numQueriesExecuted++;
			queries.m_endTime = time;

			if (group == Group_DrawCalls)
			{
				// print out each draw call
				double timeDelta = Timer::GetTimeDeltaInSeconds(query.m_startTime, query.m_endTime);
				static int print = false;
				if (print)
				{
					//causes race condition
					//PEINFO("Draw Thread: Group_DrawCalls Call [%s] finished in %f seconds index %d\n", query.m_dbgName, timeDelta, queries.m_numQueriesExecuted);
				}
			}
			queries.m_availableQueries[queries.m_numAvailableQueries++] = queryIndex;
		}
	}
	//PEINFO("Profiler::update() group updated from %d -> %d queries\n", queries.m_numActiveQueries, newNumActiveQueries);
	
	queries.m_numActiveQueries = newNumActiveQueries;
}
void Profiler::finalize(Group group)
{
	GroupQueries &queries = m_queries[group];

	float timeDelta = Timer::GetTimeDeltaInSeconds(queries.m_startTime, queries.m_endTime);
 
	if (group == Group_DrawCalls)
	{
		PEINFO("Draw Thread: Group_DrawCalls [%d/%d] finished in %f sec\n", queries.m_numQueriesExecuted, queries.m_numQueriesScheduled, timeDelta);
		Event_SET_MESH_TEXT fpsEvt(PEString::s_buf, true, false, false, false, 0, Vector3(-1.0f, .75f, 0), 0.45f);
		DebugRenderer::Instance()->createTextMesh(&fpsEvt);
	}
	else if (group == Group_DrawThread)
	{
		//PEINFO("Draw Thread: Group_DrawThread finished in %f sec\n", timeDelta);
		Event_SET_MESH_TEXT fpsEvt(PEString::s_buf, true, false, false, false, 0, Vector3(-1.0f, .7f, 0), 0.45f);
		DebugRenderer::Instance()->createTextMesh(&fpsEvt);
	}

	queries.finalize();
}

void GroupQueries::finalize()
{
	m_recordStarted = false;
	m_numQueriesScheduled = m_numQueriesExecuted =0;
	m_startTime = 0;
	m_endTime = 0;
}

EventQuery::EventQuery()
: m_pEventQuery(NULL)
{
}

void EventQuery::start(Group group, LPDIRECT3DDEVICE9 pD3D9Device, Timer::TimeType time, const char *dbgName)
{
	if (!m_pEventQuery)
		pD3D9Device->CreateQuery(D3DQUERYTYPE_EVENT, &m_pEventQuery);
/*
	// 2. Add an end marker to the command buffer queue. This tells driver to track when GPU finishes whatever commands were issued before end marker
	m_pEventQuery->Issue(D3DISSUE_END); // this end is needed here so that test below can be performed on this query, otherwise there would be no way to check if command buffer is full

	
	// 7. Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	while(S_FALSE == m_pEventQuery->GetData( NULL, 0, D3DGETDATA_FLUSH ))
		; // GetData returns false until the query is in signaled state
	*/
	m_pEventQuery->Issue(D3DISSUE_END);
	m_startTime = time;
	m_dbgName = dbgName;
}

bool EventQuery::checkFinished(bool flushCommandBuffer, bool waitForCompletion, Timer::TimeType &time, Timer &timer)
{
	if (waitForCompletion)
	{
		// note if we are not flushing yet waiting for completion, neeed to make sure that a flushing command has been called before like Present()
		while(S_FALSE == m_pEventQuery->GetData( NULL, 0, flushCommandBuffer ? D3DGETDATA_FLUSH : 0))
		;
		time = timer.TickAndGetCurrentTime(); // need to get new time since loop above could take time
		m_endTime = time;
		return true;
	}

	m_endTime = time;
	return S_OK == m_pEventQuery->GetData( NULL, 0, 0 );
}

EventQuery::~EventQuery()
{
	m_pEventQuery->Release();
}
} // profiling
#endif
}; //namespace PE
