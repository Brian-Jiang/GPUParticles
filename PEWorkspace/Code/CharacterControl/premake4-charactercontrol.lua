-- A project defines one build target
project("CharacterControl-".._OPTIONS["platformapi"])
	
	configurations {"Debug", "Release"}

	-- KIND
	if _OPTIONS["platformapi"] == "ios" then
		kind "WindowedApp"
	else
		--on windows lets have a console too
		kind "ConsoleApp"
	end


	language "C++"

	
	files { "**.h", "**.cpp" }
	if (_OPTIONS["platformapi"] == "win32d3d11" or _OPTIONS["platformapi"] == "win32d3d9" or _OPTIONS["platformapi"] == "win32gl") then
		files { "**.asm" }
	end
	
	--excludes { "**/Win32Specific/**" }
	
	links { "lua_dist-".._OPTIONS["platformapi"], "luasocket_dist-".._OPTIONS["platformapi"], "PrimeEngine-".._OPTIONS["platformapi"] }
	
	
	packagesUsed = { "Default", "Basic", "City", "CharacterControl", "Soldier" } -- will use the list to generate deployment script
	
	if _OPTIONS["platformapi"] == "ios" then
		links { "glues-".._OPTIONS["platformapi"], "GLKit.framework", "libstdc++.dylib", "OpenGLES.framework", "QuartzCore.framework" }
		
		postbuildcommands { 'python' }
		
		postbuildcommands { 'echo "XCODE: PYENGINE_WORKSPACE_DIR = PYENGINE_WORKSPACE_DIR"' }
		postbuildcommands { 'cd $SOURCE_ROOT/../../Tools' }
		postbuildcommands { 'echo "XCODE: Deploying PrimeEngine"' }
		postbuildcommands { 'python Deployment/DeployPyengine2.py Default IOS $CONFIGURATION_BUILD_DIR/$EXECUTABLE_FOLDER_PATH/'}
		for _,package in pairs(packagesUsed) do
			postbuildcommands { 'echo "XCODE: Deploying '..tostring(package)..' package"'}
			postbuildcommands { 'python Deployment/DeployPackage.py '..tostring(package)..' IOS $CONFIGURATION_BUILD_DIR/$EXECUTABLE_FOLDER_PATH/'}
		end
	end
	
	if (_OPTIONS["platformapi"] == "win32d3d11" or _OPTIONS["platformapi"] == "win32d3d9" or _OPTIONS["platformapi"] == "win32gl") then
		-- pc
		links { "ws2_32" }
		
		-- prebuildcommands { 'ml.exe /c /nologo /Zi /Fo "$(IntDir)test.obj" /Fl"" /W3 /errorReport:prompt  /Ta test.asm' }
		linkoptions { ' /NODEFAULTLIB:LIBC /SAFESEH:NO' }
		
		-- need to copy over some dlls to be able to launch program
		
		postbuildcommands { 'copy /Y ..\\External\\DownloadedLibraries\\glew-1.9.0\\bin\\*.dll "$(OutDir)" ' }
		postbuildcommands { 'copy /Y ..\\External\\DownloadedLibraries\\Cg\\bin\\cg.dll "$(OutDir)" ' }
		postbuildcommands { 'copy /Y ..\\External\\DownloadedLibraries\\Cg\\bin\\cgGL.dll "$(OutDir)" ' }
		postbuildcommands { 'copy /Y ..\\External\\DownloadedLibraries\\Cg\\bin\\cgGL.dll "$(OutDir)" ' }
		postbuildcommands { 'copy /Y "'..gWinDevKit..'\\bin\\x86\\d3dcompiler_46.dll" "$(OutDir)" ' }
	end
	
	if (_OPTIONS["platformapi"] == "win32d3d11") then
		links { "DXGI", "d3d11", "Xinput9_1_0", "d3dcompiler" }
	elseif (_OPTIONS["platformapi"] == "win32d3d9") then
		links { "d3d9", "Xinput9_1_0", "d3dcompiler" }
	elseif(_OPTIONS["platformapi"] == "win32gl") then
		links { "glew32", "opengl32", "glu32", "cg", "cgGL" }
	end
	
	if _OPTIONS["platformapi"] == "ios" then
		files { "info.plist" }
	end
	
	--configuration { "*.asm" }
	--buildoptions { "`wx-config --cxxflags`", "-ansi", "-pedantic" }
	
	--configuration "windows"
	
	--links { "png", "zlib" }
	--links { "Cocoa.framework" }
	
	--libdirs { "libs", "../mylibs" }
	--libdirs { os.findlib("X11") }
	
	--ps3 have per configuration link libs. we could do the same for dx on pc
	configuration "Debug"
		
        if _OPTIONS["platformapi"] == "ps3" then
			links { 'PSGL\\RSX\\debug\\libPSGLFX.a', 'PSGL\\RSX\\debug\\libPSGL.a', 'PSGL\\RSX\\debug\\libPSGLU.a'}
		end
	configuration "Release"
	
		if _OPTIONS["platformapi"] == "ps3" then
			links { 'PSGL\\RSX\\ultra-opt\\libPSGLFX.a', 'PSGL\\RSX\\ultra-opt\\libPSGL.a', 'PSGL\\RSX\\ultra-opt\\libPSGLU.a'}
		end
