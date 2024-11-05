#ifndef __Render_Job_H__
#define __Render_Job_H__

#include "PrimeEngine/Render/IRenderer.h"

#include "PrimeEngineIncludes.h"

namespace PE {

	void drawThreadFunctionJob(void *params);

	void runDrawThreadSingleFrame(PE::GameContext &ctx);

	void runDrawThreadSingleFrameThreaded(PE::GameContext &ctx);


}; // namespace PE

#endif