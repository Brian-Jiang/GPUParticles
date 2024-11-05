//
//  main.m
//  SampleApp
//
//  Created by artem on 9/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "../PrimeEngine/PrimeEngineIncludes.h"

#if APIABSTRACTION_IOS
int main(int argc, char *argv[])
{
    // need to set global game contruction to this game
    //PE::Components::GlobalGame::s_constructFunction = Basic::Components::BasicGame::ConstructCallback;
    //SampleApp uses default game
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}

#else

#define RETURN_VALUE 0

#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_OGL
#if APIABSTRACTION_PS3
int main()
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int showCmd)
#endif
#else
int main(int argc, char *argv[])
#endif
{
    PE::Components::Game::EngineInitParams params;
    
    params.m_windowRes = Vector2i(854, 480);
    params.m_windowCaption = "Demo DX 9"; // give pointer to static initialized data. be careful with mem address if it is dynamic string
    
    //initialize engine
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
	
    if(!PE::Components::Game::initEngine(hInstance, hPrevInstance, lpCmdLine, showCmd, params)) return RETURN_VALUE;

#elif APIABSTRACTION_OGL
    if(!PE::Components::Game::initEngine("", 0,params)) return RETURN_VALUE;
#endif
    
    // need to set global game contruction to this game
    PE::Components::GlobalGame::s_constructFunction = Basic::Components::BasicGame::ConstructCallback;
    
    //Create game singleton. Has to be done after initializing PrimeEngine in order to use MemoryManager
    PE::Components::GlobalGame::Construct();
	
    //Initialize game
    if(!PE::Components::GlobalGame::getGameInstance()->initGame())
        return RETURN_VALUE;
    
    // initialize game
    
    //Run the game
    if(!PE::Components::GlobalGame::getGameInstance()->runGame()) 
        return RETURN_VALUE;
    return RETURN_VALUE;
}
#endif // non IOS main