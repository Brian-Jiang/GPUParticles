// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Events/StandardControllerEvents.h"
#include "PrimeEngine/Events/EventQueueManager.h"

// Sibling/Children includes
#include "DX9_XInput.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"

using namespace PE::Events;
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(DX9_XInput, Component);

DX9_XInput::DX9_XInput(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself): Component(context, arena, hMyself)
{
	ZeroMemory( &lastState, sizeof(XINPUT_STATE) );
	// Simply get the state of the controller from XInput.
	XInputGetState( 0, &lastState );
}
// Component ------------------------------------------------------------

void DX9_XInput::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, DX9_XInput::do_UPDATE);
}

// Individual events -------------------------------------------------------
void DX9_XInput::do_UPDATE(Events::Event *pEvt)
{
	//poll for input

	ZeroMemory( &state, sizeof(XINPUT_STATE) );

	// Simply get the state of the controller from XInput.
	DWORD dwResult = XInputGetState( 0, &state );
	if (dwResult == ERROR_SUCCESS)
	{
		m_pQueueManager = Events::EventQueueManager::Instance();

		generateButtonEvents();
		
		generateAxisEvents();//Regardless if they are the same or not
		lastState = state;
	}
}

void DX9_XInput::generateButtonEvents()
{ 
	//TODO : Pass state.Gamepad.wButtons as data in event? in case a function needs to know about multiple buttons at once. If we have a single m_pQueueManager for control events this isnt necessary as it could be searched through.
	
	//Check for Button Down events
	WORD newlyDownButtons = (state.Gamepad.wButtons ^ lastState.Gamepad.wButtons) & state.Gamepad.wButtons;
	if(newlyDownButtons & XINPUT_GAMEPAD_Y)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_Y_DOWN));
		new (h) Event_BUTTON_Y_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_A)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_A_DOWN));
		new (h) Event_BUTTON_A_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_B)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_B_DOWN));
		new (h) Event_BUTTON_B_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_X)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_X_DOWN));
		new (h) Event_BUTTON_X_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_L_THUMB_DOWN));
		new (h) Event_BUTTON_L_THUMB_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_R_THUMB_DOWN));
		new (h) Event_BUTTON_R_THUMB_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_L_SHOULDER_DOWN));
		new (h) Event_BUTTON_L_SHOULDER_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_R_SHOULDER_DOWN));
		new (h) Event_BUTTON_R_SHOULDER_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_START)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_START_DOWN));
		new (h) Event_BUTTON_START_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_BACK)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_BACK_DOWN));
		new (h) Event_BUTTON_BACK_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		Handle h("EVENT", sizeof(Event_PAD_N_DOWN));
		new (h) Event_PAD_N_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		Handle h("EVENT", sizeof(Event_PAD_S_DOWN));
		new (h) Event_PAD_S_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		Handle h("EVENT", sizeof(Event_PAD_W_DOWN));
		new (h) Event_PAD_W_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyDownButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		Handle h("EVENT", sizeof(Event_PAD_E_DOWN));
		new (h) Event_PAD_E_DOWN;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}

	//Check for Button Up events
	WORD newlyUpButtons = (state.Gamepad.wButtons ^ lastState.Gamepad.wButtons) & lastState.Gamepad.wButtons;
	if(newlyUpButtons & XINPUT_GAMEPAD_Y)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_Y_UP));
		new (h) Event_BUTTON_Y_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_A)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_A_UP));
		new (h) Event_BUTTON_A_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_B)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_B_UP));
		new (h) Event_BUTTON_B_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_X)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_X_UP));
		new (h) Event_BUTTON_X_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_L_THUMB_UP));
		new (h) Event_BUTTON_L_THUMB_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_R_THUMB_UP));
		new (h) Event_BUTTON_R_THUMB_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_L_SHOULDER_UP));
		new (h) Event_BUTTON_L_SHOULDER_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_R_SHOULDER_UP));
		new (h) Event_BUTTON_R_SHOULDER_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_START)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_START_UP));
		new (h) Event_BUTTON_START_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_BACK)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_BACK_UP));
		new (h) Event_BUTTON_BACK_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		Handle h("EVENT", sizeof(Event_PAD_N_UP));
		new (h) Event_PAD_N_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		Handle h("EVENT", sizeof(Event_PAD_S_UP));
		new (h) Event_PAD_S_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		Handle h("EVENT", sizeof(Event_PAD_W_UP));
		new (h) Event_PAD_W_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(newlyUpButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		Handle h("EVENT", sizeof(Event_PAD_E_UP));
		new (h) Event_PAD_E_UP;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}

	//Check for Button Held events
	WORD heldButtons = (state.Gamepad.wButtons & lastState.Gamepad.wButtons);
	if(heldButtons & XINPUT_GAMEPAD_Y)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_Y_HELD));
		new (h) Event_BUTTON_Y_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_A)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_A_HELD));
		new (h) Event_BUTTON_A_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_B)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_B_HELD));
		new (h) Event_BUTTON_B_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_X)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_X_HELD));
		new (h) Event_BUTTON_X_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_LEFT_THUMB)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_L_THUMB_HELD));
		new (h) Event_BUTTON_L_THUMB_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_R_THUMB_HELD));
		new (h) Event_BUTTON_R_THUMB_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_L_SHOULDER_HELD));
		new (h) Event_BUTTON_L_SHOULDER_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_R_SHOULDER_HELD));
		new (h) Event_BUTTON_R_SHOULDER_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_START)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_START_HELD));
		new (h) Event_BUTTON_START_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_BACK)
	{
		Handle h("EVENT", sizeof(Event_BUTTON_BACK_HELD));
		new (h) Event_BUTTON_BACK_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		Handle h("EVENT", sizeof(Event_PAD_N_HELD));
		new (h) Event_PAD_N_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		Handle h("EVENT", sizeof(Event_PAD_S_HELD));
		new (h) Event_PAD_S_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_DPAD_LEFT)
	{
		Handle h("EVENT", sizeof(Event_PAD_W_HELD));
		new (h) Event_PAD_W_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}
	if(heldButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		Handle h("EVENT", sizeof(Event_PAD_E_HELD));
		new (h) Event_PAD_E_HELD;
		m_pQueueManager->add(h, Events::QT_INPUT);
	}

}

void DX9_XInput::generateAxisEvents(){
	//here are the if statements for every axis
	float LX = state.Gamepad.sThumbLX;
	float LY = state.Gamepad.sThumbLY;
	//determine how far the controller is pushed
	float LMagnitude = sqrtf(LX*LX + LY*LY);
	//determine the direction the controller is pushed
	float normalizedLX = LX / LMagnitude;
	float normalizedLY = LY / LMagnitude;
	float LNormalizedMagnitude = 0;
	//check if the controller is outside a circular dead zone
	if (LMagnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		//clip the LMagnitude at its expected maximum value
		if (LMagnitude > 32767) LMagnitude = 32767;
		//adjust LMagnitude relative to the end of the dead zone
		LMagnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		//optionally normalize the LMagnitude with respect to its expected range
		//giving a LMagnitude value of 0.0 to 1.0
		LNormalizedMagnitude = LMagnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}
	else //if the controller is in the deadzone zero out the LMagnitude
	{
		LMagnitude = 0.0;
		LNormalizedMagnitude = 0.0;
	}
	//repeat for right thumb stick
	float RX = state.Gamepad.sThumbRX;
	float RY = state.Gamepad.sThumbRY;
	//determine how far the controller is pushed
	float RMagnitude = sqrtf(RX*RX + RY*RY);
	//determine the direction the controller is pushed
	float normalizedRX = RX / RMagnitude;
	float normalizedRY = RY / RMagnitude;
	float RNormalizedMagnitude = 0;
	//check if the controller is outside a circular dead zone
	if (RMagnitude > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		//clip the RMagnitude at its expected maximum value
		if (RMagnitude > 32767) RMagnitude = 32767;
		//adjust RMagnitude relative to the end of the dead zone
		RMagnitude -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		//optionally normalize the RMagnitude with respect to its expected range
		//giving a RMagnitude value of 0.0 to 1.0
		RNormalizedMagnitude = RMagnitude / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	}
	else //if the controller is in the deadzone zero out the RMagnitude
	{
		RMagnitude = 0.0;
		RNormalizedMagnitude = 0.0;
	}

	//Calculating normalized triggers
	PrimitiveTypes::Float32 triggerLNormalized = state.Gamepad.bLeftTrigger / 255.0f;
	PrimitiveTypes::Float32 triggerRNormalized = state.Gamepad.bRightTrigger / 255.0f;

	//Vibration for testing
	XINPUT_VIBRATION *vibState = new XINPUT_VIBRATION();
	//vibState->wLeftMotorSpeed = (WORD)(LMagnitude);
	//vibState->wRightMotorSpeed = (WORD)(RMagnitude);
	XInputSetState( 0, vibState );

	//Creating the thubmstick events
	Vector3 thumbLVector(normalizedLX,normalizedLY,0);
	thumbLVector *= LNormalizedMagnitude;
	Vector3 thumbRVector(normalizedRX,normalizedRY,0);
	thumbRVector *= RNormalizedMagnitude;
	

	Handle hThumbLEvt("EVENT", sizeof(Event_ANALOG_L_THUMB_MOVE));
	Event_ANALOG_L_THUMB_MOVE *thumbLEvt = new (hThumbLEvt) Event_ANALOG_L_THUMB_MOVE;
	
	thumbLEvt->m_absPosition = thumbLVector;
	
	Handle hThumbREvt("EVENT", sizeof(Event_ANALOG_R_THUMB_MOVE));
	Event_ANALOG_R_THUMB_MOVE *thumbREvt = new (hThumbREvt) Event_ANALOG_R_THUMB_MOVE;
		
	thumbREvt->m_absPosition = thumbRVector;
	
	m_pQueueManager->add(hThumbLEvt, Events::QT_INPUT);
	m_pQueueManager->add(hThumbREvt, Events::QT_INPUT);

	//Creating the trigger events
	Handle hTriggerLEvt("EVENT", sizeof(Event_ANALOG_L_TRIGGER_MOVE));
	Event_ANALOG_L_TRIGGER_MOVE *triggerLEvt = new (hTriggerLEvt) Event_ANALOG_L_TRIGGER_MOVE;
	
	triggerLEvt->m_absPosition = triggerLNormalized;

	Handle hTriggerREvt("EVENT", sizeof(Event_ANALOG_R_TRIGGER_MOVE));
	Event_ANALOG_R_TRIGGER_MOVE *triggerREvt = new (hTriggerREvt) Event_ANALOG_R_TRIGGER_MOVE;
	
	triggerREvt->m_absPosition = triggerRNormalized;

	m_pQueueManager->add(hTriggerLEvt, Events::QT_INPUT);
	m_pQueueManager->add(hTriggerREvt, Events::QT_INPUT);
}
}; // namespace Components
}; // namespace PE

#endif // API Abstraction
