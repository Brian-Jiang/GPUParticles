newoption {
	trigger     = "platformapi",
	value       = "PlatformAPI",
	description = "Choose a particular platform and api",
	allowed = {
		{ "win32d3d9",    "Win 32 D3D 9" },
		{ "win32d3d11",   "Win 32 D3D 11" },
		{ "win32gl",      "Win32 OpenGL" },
		{ "ios",          "iOS" },
	}
}
_OPTIONS["platformapi"] = tostring(_OPTIONS["platformapi"])
solution("PEWorkspace-".._OPTIONS["platformapi"])
	configurations { "Debug", "Release" }
	
	objdir "../build-intermediate"
	targetdir "../build"
	
	
	local _platforms = {}
	if _OPTIONS["platformapi"] == "win32d3d9"   then _platforms = { "x32" };       defines { "APIABSTRACTION_D3D9=1", "PE_PLAT_API=0x0101", "PE_PLAT_IS_WIN32=1", "PE_API_IS_D3D9=1" } end
	if _OPTIONS["platformapi"] == "win32d3d11"  then _platforms = { "x32" };       defines { "APIABSTRACTION_D3D11=1", "PE_PLAT_API=0x0102", "PE_PLAT_IS_WIN32=1", "PE_API_IS_D3D11=1" } end
	if _OPTIONS["platformapi"] == "win32gl"     then _platforms = { "x32" };       defines { "APIABSTRACTION_OGL=1", "APIABSTRACTION_GLPC=1", "PE_PLAT_API=0x0104", "PE_PLAT_IS_WIN32=1", "PE_API_IS_GL=1" } end
	
	platforms(_platforms)
	
	
	if _OPTIONS["platformapi"] == "ios" then
	
		xcodebuildsettings
		{
		"INFOPLIST_FILE = info.plist",
		'CODE_SIGN_IDENTITY = "iPhone Developer"',
		'OTHER_LDFLAGS = ("-framework",Foundation,"-framework",UIKit)',
		'SDKROOT = iphoneos6.0',
		"ONLY_ACTIVE_ARCH = NO", --setting to yes will build only one. setting to no build architectures defined by ARCHS and creates a fat binary
		'VALID_ARCHS = "$(ARCHS_STANDARD_32_BIT)"', -- sets valid architectures
		'ARCHS = "$(ARCHS_STANDARD_32_BIT)"',
		--'CURRENT_ARCH = "$(ARCHS_STANDARD_32_BIT)"',
		'TARGETED_DEVICE_FAMILY = 2',
		'IPHONEOS_DEPLOYMENT_TARGET = 8.1'
		}
		
		defines { "__USE_IOS_GLES__" }
	end
	
	flags { "EnableSSE", "EnableSSE2" }
	
	flags { "NoIncrementalLink" }
	--configuration "Debug"
	--defines { "MY_SYMBOL" }
	
	includedirs {"lua_dist/src", "."}
	
	--windows stuff
	if (_OPTIONS["platformapi"] == "win32gl" or _OPTIONS["platformapi"] == "win32d3d9" or _OPTIONS["platformapi"] == "win32d3d11") then
	
		gWinProgramFiles = "C:/Program Files (x86)"
		gWinDevKit = gWinProgramFiles.."/Windows Kits/8.0"
		gWinD3DIncludeDirs = { gWinDevKit.."/Include/um", gWinDevKit.."/Include/shared" } 
		--"$(ProgramFiles)\Windows Kits\8.0\Include\um;$(ProgramFiles)\Windows Kits\8.0\Include\shared;$(VCInstallDir)include;$(VCInstallDir)atlmfc\include;$(FrameworkSDKDir)\include;
	
	
		includedirs {gWinDevKit.."/Include/um", gWinDevKit.."/Include/shared"}
	
		if(_OPTIONS["platformapi"] == "win32gl") then
			gWinGLIncludeDirs = {"../External/DownloadedLibraries/glew-1.9.0/include/GL", "../External/DownloadedLibraries/", "../External/DownloadedLibraries/Cg/include" }
			includedirs { gWinGLIncludeDirs }
		
			gWinGLLibDirs = {"../External/DownloadedLibraries/glew-1.9.0/lib", "../External/DownloadedLibraries/Cg/lib/"}
			libdirs { gWinGLLibDirs }
		end
	
	
		gWinD3DLibDirs = { gWinDevKit.."/lib/win8/um/x86" } 
		libdirs { gWinD3DLibDirs }
		
		flags { "NoMinimalRebuild" } -- need to not compete with /MP
		buildoptions { "/MP" } -- add directly to cl.exe command line. note, in future premake releases, this is done with MultiProcessorCompile premake flag
		
		
	end
    
    
    
	debugdir "$(SolutionDir)../"

	configuration "Debug"
		defines { "DEBUG", "_DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }
	
	dofile("CharacterControl/premake4-charactercontrol.lua")

	dofile("lua_dist/premake4-lua_dist.lua")
	dofile("luasocket_dist/premake4-luasocket_dist.lua")
	dofile("PrimeEngine/premake4-primeengine.lua")
	if (_OPTIONS["platformapi"] == "ios") then
        dofile("glues/premake4-glues.lua")
    end

