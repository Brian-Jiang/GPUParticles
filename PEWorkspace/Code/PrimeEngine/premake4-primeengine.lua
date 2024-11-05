-- A project defines one build target
project("PrimeEngine-".._OPTIONS["platformapi"])
	kind "StaticLib"
	language "C++"
	files { "**.h", "**.cpp", "**.lua" }
	files { "../../AssetsOut/**.*"}
	excludes { "../../AssetsOut/Default/GPUPrograms/*.sdb", "../../AssetsOut/Default/GPUPrograms/*.cg", "../../AssetsOut/Default/GPUPrograms/*.gxp", "../../AssetsOut/Default/GPUPrograms/*.sb" }
	excludes { "**_Master0.cpp" }

		
		excludes { "APIAbstraction/PS3/PS3_PadInput/*.*" }

    
	
	
	if _OPTIONS["platformapi"] ~= "ios" then
		-- MSFT based platfroms: PC DX9, PC DX11, PC GL
		
		--excludes { "src/usocket.c", "src/unix.c"}
	else
		-- Non-MSFT based platforms 
	
		excludes { "APIAbstraction/DirectX*/**.*", "Application/WinApplication.*"}
	end
	
	if _OPTIONS["platformapi"] == "win32d3d9" or _OPTIONS["platformapi"] == "win32d3d11" then
		-- DX based platforms: PC DX9, PC DX11
		
	else
		-- Non-DX based platforms: PC GL, PS3, iOS GL, PSVita
		excludes { "APIAbstraction/Texture/Texture_DDS_Loader_D3D.cpp" }
		excludes { "Render/D3D*.*" }
	end
	
	if _OPTIONS["platformapi"] == "win32d3d9" then
		-- DX9 based platforms
	else
		-- Non-DX9 platfroms
		
		excludes { "Render/D3D9*.*" }
	end
	
	
	if _OPTIONS["platformapi"] == "ps3" or _OPTIONS["platformapi"] == "ios" or _OPTIONS["platformapi"] == "win32gl" then
		-- GL based platforms
	else
		-- Non-GL based platforms
		
		excludes { "APIAbstraction/Texture/Texture_DDS_Loader_GL.cpp" }
		
		excludes { "Render/GL*.*" }
	end
	
	
	if _OPTIONS["platformapi"] == "win32d3d11" then
		-- DX11
	else
		-- Non-DX11
		excludes { "Render/D3D11*.*" }
	end
	
	
	if _OPTIONS["platformapi"] == "ios" then
		-- iOS
		files { "GameIOSInput.mm"}
    else
		-- Non-iOS
        excludes { "Application/IOS*.*", "Render/IOS*.*", "Game/Client/IOS*.*", "Events/StandardIOS*.*" }
	end
	
	
	
	

    
function TableConcat(t1,t2)
    for i=1,#t2 do
        t1[#t1+1] = t2[i]
    end
    return t1
end

	--includedirs(TableConcat({"../", "."}, gWinD3DIncludeDirs))
	configuration "Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "Release"
		defines { "NDEBUG" }
		flags { "Optimize" }

		