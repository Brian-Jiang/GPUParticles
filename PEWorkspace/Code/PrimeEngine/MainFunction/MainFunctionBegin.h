
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

#if APIABSTRACTION_IOS
#include "PrimeEngine/Application/IOSApplication.h"

	int main(int argc, char *argv[])
	{
		PE::Components::ClientGame::EngineInitParams &params = PE::Components::ClientGame::EngineInitParams::s_params;
		assert(PE::Components::ClientGame::EngineInitParams::MAX_ARGS >= argc);

		for (int iarg = 0; iarg < argc; ++iarg)
			params.args[iarg] = argv[iarg];
		params.argc = argc;

		//set parameters after this file's #include
#else

	#if APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
		int main()
		{
	#else
		int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int showCmd)
		{
			{
				PE::Components::ClientGame::EngineInitParams &params = PE::Components::ClientGame::EngineInitParams::s_params;
				params.hInstance = hInstance;
				params.hPrevInstance = hPrevInstance;
				params.lpCmdLine = lpCmdLine;
				params.showCmd = showCmd;
				params.m_windowCaption = "Windows PrimeEngine";
			}
	#endif
		
			PE::Components::ClientGame::EngineInitParams &params = PE::Components::ClientGame::EngineInitParams::s_params;
			#if PE_PLAT_IS_PS3
				params.m_windowRes = Vector2i(854, 480);
			#else
				params.m_windowRes = Vector2i(1280, 720);
			#endif
			//set parameters after this file's #include

#endif // non IOS main
