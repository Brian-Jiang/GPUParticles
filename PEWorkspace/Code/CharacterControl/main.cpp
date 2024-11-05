
#include "ClientCharacterControlGame.h"
#include "ServerCharacterControlGame.h"

//this is main fucntion body
// the beginning and end are defined in MainFunctionBegin.h / MainFunctionEnd.h
#include "PrimeEngine/MainFunction/MainFunctionBegin.h"
{
	// need to set global game construction to this game
	PE::Components::ClientGlobalGameCallbacks::s_constructFunction = CharacterControl::Components::ClientCharacterControlGame::ConstructCallback;

	PE::Components::ServerGlobalGameCallbacks::s_constructFunction = CharacterControl::Components::ServerCharacterControlGame::ConstructCallback;
	//in case a different engine initialization needed, set PE::Components::ClientGlobalGameCallbacks::s_initEngineFunction to a custom initializer function

	//set engine params if needed
}
#include "PrimeEngine/MainFunction/MainFunctionPreEnd_iOS.h" // with iOS we will never get past this include. the code inside will end up including MainFunctionEnd.h

#if !PE_PLAT_IS_IOS
#include "PrimeEngine/MainFunction/MainFunctionEnd.h"
#endif
