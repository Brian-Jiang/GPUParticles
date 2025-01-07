//
//  StandardIOSEvents.h
//
#if APIABSTRACTION_IOS
// Sibling/Children includes
#include "Event.h"
#import <Foundation/Foundation.h>

#ifndef SampleWorkspace_StandardIOSEvents_h
#define SampleWorkspace_StandardIOSEvents_h

namespace PE {
    namespace Events {
        
        struct Event_IOS_TOUCH_MOVED : public Event {
            PE_DECLARE_CLASS(Event_IOS_TOUCH_MOVED);
            
            Event_IOS_TOUCH_MOVED() : m_dx(0), m_dy(0), m_normalized_dx(0), m_normalized_dy(0) {}
            PrimitiveTypes::Float32 m_dx;
            PrimitiveTypes::Float32 m_dy;
            PrimitiveTypes::Float32 m_normalized_dx;
            PrimitiveTypes::Float32 m_normalized_dy;
            
            NSSet* touches;//an NSSet of UITouch objects that are received by this event from ios
            int touchesCount;
        };
    }
}


#endif
#endif //#if APIABSTRACTION_IOS
