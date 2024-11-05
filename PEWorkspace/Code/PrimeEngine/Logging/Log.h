#ifndef __PYENGINE_2_0__LOG_H__
#define __PYENGINE_2_0__LOG_H__

#define NOMINMAX
// PrimeEngine includes
#include "../Utils/Array/Array.h"

// sibling includes

#include "../Events/Component.h"

//System includes
#include <time.h>

namespace PE {
namespace Components {

class Log : public Component
{
public:

	PE_DECLARE_CLASS(Log);

	Log(PE::GameContext &context, PE::MemoryArena arena, PE::Handle m_hMyself, const char * tagName = " EVENT CALLED BY "); //Tag name is for use when filtering through in DebugView. Has default value.

	virtual void addDefaultComponents() {} // no components to avoid log recusively added to log
	virtual void handleEvent(Events::Event *pEvt);
	void printDebugInt(PrimitiveTypes::Int32 n);

	//---Member variables---//

	const char* m_tagName;
	PrimitiveTypes::Bool m_isActivated;
	PrimitiveTypes::Bool m_activateOnNextFrame;
	
	private:
};

}; // namespace Components
}; // namespace PE
#endif
