#include "ClientBasicGame.h"
#include "GlobalRegistry.h"

//this is main fucntion body
// the beginning and end are defined in MainFunctionBegin.h / MainFunctionEnd.h
#include "PrimeEngine/APIAbstraction/MainFunction/MainFunctionBegin.h"
{
	// need to set global game construction to this game
	PE::Components::ClientGlobalGameCallbacks::s_constructFunction = Basic::Components::ClientBasicGame::ConstructCallback;

	//in case a different engine initialization needed, set PE::Components::ClientGlobalGameCallbacks::s_initEngineFunction to a custom initializer function

	//set engine params if needed
}
#include "PrimeEngine/APIAbstraction/MainFunction/MainFunctionEnd.h"
