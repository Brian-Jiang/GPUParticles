//
//  StandardIOSEvents.cpp
//

#include "StandardIOSEvents.h"
#include "EventDataCreators.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Lua/EventGlue/EventDataCreators.h"

namespace PE {
    namespace Events {
        PE_IMPLEMENT_CLASS1(Event_IOS_TOUCH_MOVED, Event);
    }
}