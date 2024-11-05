--
-- vs2010_vcxproj.lua
-- Generate a Visual Studio 2010 C/C++ project.
-- Copyright (c) 2009-2011 Jason Perkins and the Premake project
--

	premake.vstudio.vc2010 = { }
	local vc2010 = premake.vstudio.vc2010
	local vstudio = premake.vstudio


	local function vs2010_config(prj)
		_p(1,'<ItemGroup Label="ProjectConfigurations">')
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
				_p(2,'<ProjectConfiguration Include="%s">', premake.esc(cfginfo.name))
					_p(3,'<Configuration>%s</Configuration>',cfginfo.buildcfg)
					_p(3,'<Platform>%s</Platform>',cfginfo.platform)
				_p(2,'</ProjectConfiguration>')
		end
		_p(1,'</ItemGroup>')
	end

	local function vs2010_globals(prj)
		_p(1,'<PropertyGroup Label="Globals">')
			_p(2,'<ProjectGuid>{%s}</ProjectGuid>',prj.uuid)
			_p(2,'<RootNamespace>%s</RootNamespace>',prj.name)
		--if prj.flags is required as it is not set at project level for tests???
		--vs200x generator seems to swap a config for the prj in test setup
		if prj.flags and prj.flags.Managed then
			_p(2,'<TargetFrameworkVersion>v4.0</TargetFrameworkVersion>')
			_p(2,'<Keyword>ManagedCProj</Keyword>')
		else
			_p(2,'<Keyword>Win32Proj</Keyword>')
		end
		_p(1,'</PropertyGroup>')
	end

	function vc2010.config_type(config)
		local t =
		{
			SharedLib = "DynamicLibrary",
			StaticLib = "StaticLibrary",
			ConsoleApp = "Application",
			WindowedApp = "Application"
		}
		return t[config.kind]
	end



	local function if_config_and_platform()
		return 'Condition="\'$(Configuration)|$(Platform)\'==\'%s\'"'
	end

	local function optimisation(cfg)
		local result = "Disabled"
		for _, value in ipairs(cfg.flags) do
			if (value == "Optimize") then
				result = "Full"
			elseif (value == "OptimizeSize") then
				result = "MinSpace"
			elseif (value == "OptimizeSpeed") then
				result = "MaxSpeed"
			end
		end
		return result
	end


--
-- This property group describes a particular configuration: what
-- kind of binary it produces, and some global settings.
--

	function vc2010.configurationPropertyGroup(cfg, cfginfo)
		_p(1,'<PropertyGroup '..if_config_and_platform() ..' Label="Configuration">'
				, premake.esc(cfginfo.name))
		_p(2,'<ConfigurationType>%s</ConfigurationType>',vc2010.config_type(cfg))
		_p(2,'<UseDebugLibraries>%s</UseDebugLibraries>', iif(optimisation(cfg) == "Disabled","true","false"))
		_p(2,'<CharacterSet>%s</CharacterSet>',iif(cfg.flags.Unicode,"Unicode","MultiByte"))

		if _ACTION > "vs2010" then
			_p(2,'<PlatformToolset>%s</PlatformToolset>', "v110")
		end

		if cfg.flags.MFC then
			_p(2,'<UseOfMfc>%s</UseOfMfc>', iif(cfg.flags.StaticRuntime, "Static", "Dynamic"))
		end

		if cfg.flags.ATL or cfg.flags.StaticATL then
			_p(2,'<UseOfAtl>%s</UseOfAtl>', iif(cfg.flags.StaticATL, "Static", "Dynamic"))
		end

		if cfg.flags.Managed then
			_p(2,'<CLRSupport>true</CLRSupport>')
		end
		
		--akovalovs: PS3 specific exception property
		if cfg.platform == 'PS3' then
			if cfg.flags.SEH then
				_p(2,'<ExceptionsAndRtti>WithExceptsWithRtti</ExceptionsAndRtti>')
			else
				_p(2,'<ExceptionsAndRtti>NoExceptsWithRtti</ExceptionsAndRtti>')
			end
		end
		
		
		_p(1,'</PropertyGroup>')
	end


	local function import_props(prj)
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			_p(1,'<ImportGroup '..if_config_and_platform() ..' Label="PropertySheets">'
					,premake.esc(cfginfo.name))
				_p(2,'<Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')" Label="LocalAppDataPlatform" />')
			_p(1,'</ImportGroup>')
		end
	end

	function vc2010.outputProperties(prj)
			for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
				local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
				local target = cfg.buildtarget

				_p(1,'<PropertyGroup '..if_config_and_platform() ..'>', premake.esc(cfginfo.name))

				_p(2,'<OutDir>%s\\</OutDir>', premake.esc(target.directory))

				if cfg.platform == "Xbox360" then
					_p(2,'<OutputFile>$(OutDir)%s</OutputFile>', premake.esc(target.name))
                    _p(2,'<RemoteRoot>%s</RemoteRoot>', "devkit:\\PEW\\$(ProjectName)") -- todo: use deploymentoptions or other deploy commands
					_p(2,'<ImageXexOutput>%s\\$(ProjectName).xex</ImageXexOutput>', premake.esc(target.directory))
					
					
                end

				_p(2,'<IntDir>%s\\</IntDir>', premake.esc(cfg.objectsdir))
				_p(2,'<TargetName>%s</TargetName>', premake.esc(path.getbasename(target.name)))
				_p(2,'<TargetExt>%s</TargetExt>', premake.esc(path.getextension(target.name)))

				if cfg.kind == "SharedLib" then
					local ignore = (cfg.flags.NoImportLib ~= nil)
					 _p(2,'<IgnoreImportLibrary>%s</IgnoreImportLibrary>', tostring(ignore))
				end

				if cfg.kind ~= "StaticLib" then
					_p(2,'<LinkIncremental>%s</LinkIncremental>', tostring(premake.config.isincrementallink(cfg)))
				end

				if cfg.flags.NoManifest then
					_p(2,'<GenerateManifest>false</GenerateManifest>')
				end
                

				_p(1,'</PropertyGroup>')
			end

	end

	local function runtime(cfg)
		local runtime
		local flags = cfg.flags
		if premake.config.isdebugbuild(cfg) then
			runtime = iif(flags.StaticRuntime and not flags.Managed, "MultiThreadedDebug", "MultiThreadedDebugDLL")
		else
			runtime = iif(flags.StaticRuntime and not flags.Managed, "MultiThreaded", "MultiThreadedDLL")
		end
		return runtime
	end

	local function precompiled_header(cfg)
      	if not cfg.flags.NoPCH and cfg.pchheader then
			_p(3,'<PrecompiledHeader>Use</PrecompiledHeader>')
			_p(3,'<PrecompiledHeaderFile>%s</PrecompiledHeaderFile>', path.getname(cfg.pchheader))
		else
			_p(3,'<PrecompiledHeader></PrecompiledHeader>')
		end
	end

	local function preprocessor(indent,cfg)
		if #cfg.defines > 0 then
			_p(indent,'<PreprocessorDefinitions>%s;%%(PreprocessorDefinitions)</PreprocessorDefinitions>'
				,premake.esc(table.concat(cfg.defines, ";")))
		else
			_p(indent,'<PreprocessorDefinitions></PreprocessorDefinitions>')
		end
	end

	local function include_dirs(indent,cfg)
		if #cfg.includedirs > 0 then
			_p(indent,'<AdditionalIncludeDirectories>%s;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>'
					,premake.esc(path.translate(table.concat(cfg.includedirs, ";"), '\\')))
		end
	end

	local function resource_compile(cfg)
		_p(2,'<ResourceCompile>')
			preprocessor(3,cfg)
			include_dirs(3,cfg)
		_p(2,'</ResourceCompile>')

	end

	local function exceptions(cfg)
		if cfg.flags.NoExceptions then
			_p(2,'<ExceptionHandling>false</ExceptionHandling>')
		elseif cfg.flags.SEH then
			_p(2,'<ExceptionHandling>Async</ExceptionHandling>')
		--SEH is not required for Managed and is implied
		end
	end

	local function rtti(cfg)
		if cfg.flags.NoRTTI and not cfg.flags.Managed then
			_p(3,'<RuntimeTypeInfo>false</RuntimeTypeInfo>')
		end
	end

	local function wchar_t_buildin(cfg)
		if cfg.flags.NativeWChar then
			_p(3,'<TreatWChar_tAsBuiltInType>true</TreatWChar_tAsBuiltInType>')
		elseif cfg.flags.NoNativeWChar then
			_p(3,'<TreatWChar_tAsBuiltInType>false</TreatWChar_tAsBuiltInType>')
		end
	end

	local function sse(cfg)
		if cfg.flags.EnableSSE then
			_p(3,'<EnableEnhancedInstructionSet>StreamingSIMDExtensions</EnableEnhancedInstructionSet>')
		elseif cfg.flags.EnableSSE2 then
			_p(3,'<EnableEnhancedInstructionSet>StreamingSIMDExtensions2</EnableEnhancedInstructionSet>')
		end
	end

	local function floating_point(cfg)
	     if cfg.flags.FloatFast then
			_p(3,'<FloatingPointModel>Fast</FloatingPointModel>')
		elseif cfg.flags.FloatStrict and not cfg.flags.Managed then
			_p(3,'<FloatingPointModel>Strict</FloatingPointModel>')
		end
	end


	local function debug_info(cfg)
	--
	--	EditAndContinue /ZI
	--	ProgramDatabase /Zi
	--	OldStyle C7 Compatable /Z7
	--
		local debug_info = ''
		if cfg.flags.Symbols then
			if cfg.platform == "x64"
				or cfg.flags.Managed
				or premake.config.isoptimizedbuild(cfg.flags)
				or cfg.flags.NoEditAndContinue
			then
					debug_info = "ProgramDatabase"
			else
				debug_info = "EditAndContinue"
			end
		end

		_p(3,'<DebugInformationFormat>%s</DebugInformationFormat>',debug_info)
		
		--akovalovs: for ps3, we need to set generation of debug info in C++ properties (not just linker)
		if cfg.platform == 'PS3' or cfg.platform == 'PSVita' then
			_p(3,'<GenerateDebugInformation>%s</GenerateDebugInformation>', tostring(cfg.flags.Symbols ~= nil))
		end
		
	end

	local function minimal_build(cfg)
		if premake.config.isdebugbuild(cfg) and not cfg.flags.NoMinimalRebuild then
			_p(3,'<MinimalRebuild>true</MinimalRebuild>')
		else
			_p(3,'<MinimalRebuild>false</MinimalRebuild>')
		end
	end

	local function compile_language(cfg)
		if cfg.language == "C" then
			_p(3,'<CompileAs>CompileAsC</CompileAs>')
		end
	end

	local function vs10_clcompile(cfg)
		print "Entering vs10_clcompile(cfg).. create per project compilation properties, like include dirs, preprocessor, etc"
		
		_p(2,'<ClCompile>')

		if #cfg.buildoptions > 0 then
			_p(3,'<AdditionalOptions>%s %%(AdditionalOptions)</AdditionalOptions>',
					table.concat(premake.esc(cfg.buildoptions), " "))
		end

			_p(3,'<Optimization>%s</Optimization>',optimisation(cfg))

			include_dirs(3,cfg)
			preprocessor(3,cfg)
			minimal_build(cfg)

		if  not premake.config.isoptimizedbuild(cfg.flags) then
			if not cfg.flags.Managed then
				_p(3,'<BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>')
			end

			if cfg.flags.ExtraWarnings then
				_p(3,'<SmallerTypeCheck>true</SmallerTypeCheck>')
			end
		else
			_p(3,'<StringPooling>true</StringPooling>')
		end

			_p(3,'<RuntimeLibrary>%s</RuntimeLibrary>', runtime(cfg))

			_p(3,'<FunctionLevelLinking>true</FunctionLevelLinking>')

			precompiled_header(cfg)

		if cfg.flags.ExtraWarnings then
			_p(3,'<WarningLevel>Level4</WarningLevel>')
		else
			_p(3,'<WarningLevel>Level3</WarningLevel>')
		end

		if cfg.flags.FatalWarnings then
			_p(3,'<TreatWarningAsError>true</TreatWarningAsError>')
		end

			exceptions(cfg)
			rtti(cfg)
			wchar_t_buildin(cfg)
			sse(cfg)
			floating_point(cfg)
			debug_info(cfg)

		if cfg.flags.Symbols then
			_p(3,'<ProgramDataBaseFileName>$(OutDir)%s.pdb</ProgramDataBaseFileName>'
				, path.getbasename(cfg.buildtarget.name))
		end

		if cfg.flags.NoFramePointer then
			_p(3,'<OmitFramePointers>true</OmitFramePointers>')
		end

			compile_language(cfg)

		_p(2,'</ClCompile>')
	end

	local function event_hooks(cfg)
		if #cfg.postbuildcommands> 0 then
		    _p(2,'<PostBuildEvent>')
				_p(3,'<Command>%s</Command>',premake.esc(table.implode(cfg.postbuildcommands, "", "", "\r\n")))
			_p(2,'</PostBuildEvent>')
		end

		if #cfg.prebuildcommands> 0 then
		    _p(2,'<PreBuildEvent>')
				_p(3,'<Command>%s</Command>',premake.esc(table.implode(cfg.prebuildcommands, "", "", "\r\n")))
			_p(2,'</PreBuildEvent>')
		end

		if #cfg.prelinkcommands> 0 then
		    _p(2,'<PreLinkEvent>')
				_p(3,'<Command>%s</Command>',premake.esc(table.implode(cfg.prelinkcommands, "", "", "\r\n")))
			_p(2,'</PreLinkEvent>')
		end
	end

	local function additional_options(indent,cfg)
		if #cfg.linkoptions > 0 then
				_p(indent,'<AdditionalOptions>%s %%(AdditionalOptions)</AdditionalOptions>',
					table.concat(premake.esc(cfg.linkoptions), " "))
		end
	end

	local function link_target_machine(index,cfg)
		local platforms = {x32 = 'MachineX86', x64 = 'MachineX64'}
		if platforms[cfg.platform] then
			_p(index,'<TargetMachine>%s</TargetMachine>', platforms[cfg.platform])
		end
	end

	local function item_def_lib(cfg)
       -- The Xbox360 project files are stored in another place in the project file.
		if cfg.kind == 'StaticLib' and cfg.platform ~= "Xbox360" then
			_p(1,'<Lib>')
				_p(2,'<OutputFile>$(OutDir)%s</OutputFile>',cfg.buildtarget.name)
				additional_options(2,cfg)
				link_target_machine(2,cfg)
			_p(1,'</Lib>')
		end
	end



	local function import_lib(cfg)
		--Prevent the generation of an import library for a Windows DLL.
		if cfg.kind == "SharedLib" then
			local implibname = cfg.linktarget.fullpath
			_p(3,'<ImportLibrary>%s</ImportLibrary>',iif(cfg.flags.NoImportLib, cfg.objectsdir .. "\\" .. path.getname(implibname), implibname))
		end
	end


--
-- Generate the <Link> element and its children.
--

	function vc2010.link(cfg)
		_p(2,'<Link>')
		--akovalovs: dont set subsystem for xbox360. it doesnt need that field
		if cfg.platform ~= "Xbox360" then	
			_p(3,'<SubSystem>%s</SubSystem>', iif(cfg.kind == "ConsoleApp", "Console", "Windows"))
		end
		_p(3,'<GenerateDebugInformation>%s</GenerateDebugInformation>', tostring(cfg.flags.Symbols ~= nil))

		if premake.config.isoptimizedbuild(cfg.flags) then
			_p(3,'<EnableCOMDATFolding>true</EnableCOMDATFolding>')
			_p(3,'<OptimizeReferences>true</OptimizeReferences>')
		end

		if cfg.kind ~= 'StaticLib' then
			vc2010.additionalDependencies(cfg)
			_p(3,'<OutputFile>$(OutDir)%s</OutputFile>', cfg.buildtarget.name)

			if #cfg.libdirs > 0 then
				_p(3,'<AdditionalLibraryDirectories>%s;%%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>',
						premake.esc(path.translate(table.concat(cfg.libdirs, ';'), '\\')))
			end

			if vc2010.config_type(cfg) == 'Application' and not cfg.flags.WinMain and not cfg.flags.Managed then
				_p(3,'<EntryPointSymbol>mainCRTStartup</EntryPointSymbol>')
			end

			import_lib(cfg)

			local deffile = premake.findfile(cfg, ".def")
			if deffile then
				_p(3,'<ModuleDefinitionFile>%s</ModuleDefinitionFile>', deffile)
			end

			link_target_machine(3,cfg)
			additional_options(3,cfg)
		end

		_p(2,'</Link>')
	end


    
--
-- Generate the <Link/AdditionalDependencies> element, which links in system
-- libraries required by the project (but not sibling projects; that's handled
-- by an <ItemGroup/ProjectReference>).
--

	function vc2010.additionalDependencies(cfg)
		local links = premake.getlinks(cfg, "system", "fullpath")
		if #links > 0 then
			_p(3,'<AdditionalDependencies>%s;%%(AdditionalDependencies)</AdditionalDependencies>',
						table.concat(links, ";"))
		end
	end


    function vc2010.deploy(cfg)
		if cfg.platform == "Xbox360" then	
            _p(2,'<Deploy>')
                _p(3,'<DeploymentType>%s</DeploymentType>', "CopyToHardDrive") -- todo: use deployment options
            _p(2,'</Deploy>')
        end
		
		
	end
    
	local function item_definitions(prj)
		print "Entering item_definitions(prj).."
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			_p(1,'<ItemDefinitionGroup ' ..if_config_and_platform() ..'>'
					,premake.esc(cfginfo.name))
				vs10_clcompile(cfg)
				resource_compile(cfg)
				item_def_lib(cfg)
				vc2010.link(cfg)
				event_hooks(cfg)
                vc2010.deploy(cfg)
			_p(1,'</ItemDefinitionGroup>')


		end
	end



--
-- Retrieve a list of files for a particular build group, one of
-- "ClInclude", "ClCompile", "ResourceCompile", and "None". or "MASM"
--

	function vc2010.getfilegroup(prj, group)
		local sortedfiles = prj.vc2010sortedfiles
		if not sortedfiles then
			sortedfiles = {
				ClCompile = {},
				ClInclude = {},
				None = {},
				ResourceCompile = {},
				MASM = {},
				cgbuild_ps3_vs = {}, -- tracks cgvs
				cgbuild_ps3_ps = {}, -- tracks cgps
				CustomBuild = {}, -- tracks both cgvs, cgps
			}

			for file in premake.project.eachfile(prj) do
				if path.iscppfile(file.name) then
					table.insert(sortedfiles.ClCompile, file)
				--akovalovs: custom file types
				elseif path.ismasmfile(file.name) then
					table.insert(sortedfiles.MASM, file)
				elseif path.iscgvsfile(file.name) then
					table.insert(sortedfiles.cgbuild_ps3_vs, file)
					table.insert(sortedfiles.CustomBuild, file)
				elseif path.iscgpsfile(file.name) then
					table.insert(sortedfiles.cgbuild_ps3_ps, file)
					table.insert(sortedfiles.CustomBuild, file)
				elseif path.iscppheader(file.name) then
					table.insert(sortedfiles.ClInclude, file)
				elseif path.isresourcefile(file.name) then
					table.insert(sortedfiles.ResourceCompile, file)
				else
					table.insert(sortedfiles.None, file)
				end
			end

			-- Cache the sorted files; they are used several places
			prj.vc2010sortedfiles = sortedfiles
		end

		return sortedfiles[group]
	end


--
-- Write the files section of the project file.
--

	function vc2010.files(prj)
		print "Entering vc2010.files(prj): will add groups (<ItemGroup>) for each file kind (compiled cpp, masm, resource, excluded)." 
		vc2010.simplefilesgroup(prj, "ClInclude")
		vc2010.compilerfilesgroup(prj)
		vc2010.cgvsfilesgroup(prj) --akovalovs: custom file types
		vc2010.cgpsfilesgroup(prj)
		vc2010.masmfilesgroup(prj)
		vc2010.simplefilesgroup(prj, "None")
		vc2010.simplefilesgroup(prj, "ResourceCompile")
	end


	function vc2010.simplefilesgroup(prj, section)
		local files = vc2010.getfilegroup(prj, section)
		if #files > 0  then
			_p(1,'<ItemGroup>')
			for _, file in ipairs(files) do
				_p(2,'<%s Include=\"%s\" />', section, path.translate(file.name, "\\"))
			end
			_p(1,'</ItemGroup>')
		end
	end


	function vc2010.compilerfilesgroup(prj)
		local configs = prj.solution.vstudio_configs
		local files = vc2010.getfilegroup(prj, "ClCompile")
		if #files > 0  then
			local config_mappings = {}
			for _, cfginfo in ipairs(configs) do
				local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
				if cfg.pchheader and cfg.pchsource and not cfg.flags.NoPCH then
					config_mappings[cfginfo] = path.translate(cfg.pchsource, "\\")
				end
			end

			_p(1,'<ItemGroup>')
			for _, file in ipairs(files) do
				local translatedpath = path.translate(file.name, "\\")
				_p(2,'<ClCompile Include=\"%s\">', translatedpath)
				for _, cfginfo in ipairs(configs) do
					if config_mappings[cfginfo] and translatedpath == config_mappings[cfginfo] then
						_p(3,'<PrecompiledHeader '.. if_config_and_platform() .. '>Create</PrecompiledHeader>', premake.esc(cfginfo.name))
						config_mappings[cfginfo] = nil  --only one source file per pch
					end
				end
				_p(2,'</ClCompile>')
			end
			_p(1,'</ItemGroup>')
		end
	end

	--akovalovs
	function vc2010.masmfilesgroup(prj)
		local configs = prj.solution.vstudio_configs
		local files = vc2010.getfilegroup(prj, "MASM")
		if #files > 0  then
			--local config_mappings = {}
			--for _, cfginfo in ipairs(configs) do
			--	local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			--	if cfg.pchheader and cfg.pchsource and not cfg.flags.NoPCH then
			--		config_mappings[cfginfo] = path.translate(cfg.pchsource, "\\")
			--	end
			--end

			_p(1,'<ItemGroup>')
			for _, file in ipairs(files) do
				local translatedpath = path.translate(file.name, "\\")
				_p(2,'<MASM Include=\"%s\">', translatedpath)
				--for _, cfginfo in ipairs(configs) do
				--	if config_mappings[cfginfo] and translatedpath == config_mappings[cfginfo] then
				--		_p(3,'<PrecompiledHeader '.. if_config_and_platform() .. '>Create</PrecompiledHeader>', premake.esc(cfginfo.name))
				--		config_mappings[cfginfo] = nil  --only one source file per pch
				--	end
				--end
				_p(2,'</MASM>')
			end
			_p(1,'</ItemGroup>')
		end
	end

	--akovalovs: this is called whenever we inject the files into vcxproj file
	function vc2010.cgvsfilesgroup(prj)
		return vc2010.cgXsfilesgroup(prj, 'cgvs', 'cgbuild_ps3_vs')
	end
	function vc2010.cgpsfilesgroup(prj)
		return vc2010.cgXsfilesgroup(prj, 'cgps', 'cgbuild_ps3_ps')
	end
	function vc2010.cgXsfilesgroup(prj, extension, tool)
		local configs = prj.solution.vstudio_configs
		local files = vc2010.getfilegroup(prj, tool)
		
		--for _, v in pairs(prj) do
		--	print(_ .. ":" .. tostring(v) .. "\n")
		--end
			
		if #files > 0  then
			--local config_mappings = {}
			-- for _, cfginfo in ipairs(configs) do
			--	local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			--	if cfg.pchheader and cfg.pchsource and not cfg.flags.NoPCH then
			--		config_mappings[cfginfo] = path.translate(cfg.pchsource, "\\")
			--	end
			--end
			
			_p(1,'<ItemGroup>')
			if (_OPTIONS["platformapi"] == 'psvita') or (_OPTIONS["platformapi"] == 'ps3') then
				local executable = ""
				local converter = ""
				local vertexFlags = ""
				local fragmentFlags = ""
				local commonFlags = ""
				local platform = ""
				if (_OPTIONS["platformapi"] == 'psvita') then
					print("Writing PSVita CustomBuild files")
					platform = "PSVita"
					executable = "&quot;$(SCE_PSP2_SDK_DIR)\\host_tools\\bin\\psp2cgc.exe&quot;"
					
					vertexFlags = "--profile sce_vp_psp2"
					fragmentFlags = "--profile sce_fp_psp2"
					
					commonFlags = "--cache -DAPIABSTRACTION_PSVITA=1"
				elseif (_OPTIONS["platformapi"] == 'ps3') then
					print("Writing PS3 CustomBuild files")
					platform = "PS3"
					executable = "&quot;$(SCE_PS3_ROOT)\\host-win32\\Cg\\bin\\sce-cgc&quot;"
					converter = "&quot;$(SCE_PS3_ROOT)\\host-win32\\Cg\\bin\\cgnv2elf.exe&quot;"
					
					vertexFlags = "-p sce_vp_rsx"
					fragmentFlags = "-p sce_fp_rsx"
					
					commonFlags = "-DAPIABSTRACTION_PS3=1"
				end
				local debugCondition = string.format('Condition="\'$(Configuration)|$(Platform)\'==\'Debug|%s\'"', platform)
				local releaseCondition = string.format('Condition="\'$(Configuration)|$(Platform)\'==\'Release|%s\'"', platform)
					
				for _, file in ipairs(files) do
					local translatedpath = path.translate(file.name, "\\")
					--note we could do this below, use custom build tool, but it doesnt work well for vita
					-- so we are going to use custom build tools and specify the params directly
					--_p(2,'<%s Include=\"%s\">', tool, translatedpath)
					--_p(2,'</%s>', tool)
					
					
					-- use CustomBuild generic tool and provide command line manually
					_p(2,'<CustomBuild Include=\"%s\">', translatedpath)
					_p(3,'<FileType>Document</FileType>')
					
					local vertexShader = extension == 'cgvs'
					
					local debugFlags = ""
					local releaseFlags = ""
					
					local programFlags = vertexFlags
					local shaderType = "Vertex"
					if not vertexShader then
						shaderType = "Fragment"
						programFlags = fragmentFlags
					end
					local inputFile = "&quot;%(FullPath)&quot;"
					local outputFileNoQuotes = ""
					local additionalDependencies = ""
					local commandLineDebug = ""
					local commandLineRelease = ""
					
					if platform == 'PSVita' then
						outputFileNoQuotes = "%(RootDir)%(Directory)%(Filename).gxp"
						local outputFile = string.format('&quot;%s&quot;', outputFileNoQuotes)
						commandLineDebug =   string.format('%s %s %s %s %s -o %s', executable, commonFlags, debugFlags, programFlags, inputFile, outputFile)
						commandLineRelease = string.format('%s %s %s %s %s -o %s', executable, commonFlags, releaseFlags, programFlags, inputFile, outputFile)
                        additionalDependencies = 'force_rebuild.txt'
					elseif platform == 'PS3' then
						outputFileNoQuotes = "%(RootDir)%(Directory)%(Filename).self"
						local outputFile = string.format('&quot;%s&quot;', outputFileNoQuotes)
						local intermediateFile = "&quot;%(RootDir)%(Directory)%(Filename).fpo&quot;"
						commandLineDebug =   string.format('%s %s %s %s %s -o %s; %s %s %s', executable, commonFlags, debugFlags, programFlags, inputFile, intermediateFile,  converter, intermediateFile, outputFile)
						commandLineRelease = string.format('%s %s %s %s %s -o %s; %s %s %s', executable, commonFlags, releaseFlags, programFlags, inputFile, intermediateFile, converter, intermediateFile, outputFile)
					end
					_p(3,'<Outputs>%s</Outputs>', outputFileNoQuotes)
					
					--debug version
					_p(3,'<Message %s>Compiling %s (%s) Shader For Debug For PSVita</Message>', debugCondition, extension, shaderType)
					_p(3,'<Command %s>%s</Command>', debugCondition, commandLineDebug)
					if platform == 'PSVita' then
						_p(3,'<AdditionalInputs %s>%s</AdditionalInputs>', debugCondition, additionalDependencies)
					end
					
					--release version
					_p(3,'<Message %s>Compiling %s (%s) Shader For Release For PSVita</Message>', releaseCondition, extension, shaderType)
					_p(3,'<Command %s>%s</Command>', releaseCondition, commandLineDebug)
					
					if platform == 'PSVita' then
						_p(3,'<AdditionalInputs %s>%s</AdditionalInputs>', releaseCondition, additionalDependencies)
					end
					_p(2,'</CustomBuild>')
					
					
				end -- for all files
				-- end of custom build rule platforms
			else
				-- for invalid case, meaning we actually don't need custom compile code, just add the files with CustomBuild build rule, but no command
				-- we could also add it as None
				
				for _, file in ipairs(files) do
					local translatedpath = path.translate(file.name, "\\")
					_p(2,'<CustomBuild Include=\"%s\" />', translatedpath)
				end
			end -- of non custom configs
			_p(1,'</ItemGroup>')
		end -- if #files > 0
	end



--
-- Output the VC2010 project file header
--

	function vc2010.header(targets)
		io.eol = "\r\n"
		_p('<?xml version="1.0" encoding="utf-8"?>')

		local t = ""
		if targets then
			t = ' DefaultTargets="' .. targets .. '"'
		end

		_p('<Project%s ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">', t)
	end


--
-- Output the VC2010 C/C++ project file
--

	function premake.vs2010_vcxproj(prj)
		print "Entering premake.vs2010_vcxproj: will generate the whole project file.."
		io.indent = "  "
		vc2010.header("Build")

			vs2010_config(prj)
			vs2010_globals(prj)

			_p(1,'<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />')

			for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
				local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
				vc2010.configurationPropertyGroup(cfg, cfginfo)
			end

			_p(1,'<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />')

			--check what this section is doing
			_p(1,'<ImportGroup Label="ExtensionSettings">')
				--akovalovs : adding support for masm and shader files
				_p(2, '<Import Project="$(VCTargetsPath)\\BuildCustomizations\\masm.props" />')
				if prj.name == 'PrimeEngine-ps3' then
					_p(2, '<Import Project="fxcompile.props" />')
				end
				
			_p(1,'</ImportGroup>')


			import_props(prj)

			--what type of macros are these?
			_p(1,'<PropertyGroup Label="UserMacros" />')

			vc2010.outputProperties(prj)

			item_definitions(prj)

			vc2010.files(prj)
			vc2010.projectReferences(prj)

			_p(1,'<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />')
			_p(1,'<ImportGroup Label="ExtensionTargets">')
				--akovalovs
				_p(2, '<Import Project="$(VCTargetsPath)\\BuildCustomizations\\masm.targets" />')
				if prj.name == 'PrimeEngine-ps3' or prj.name == 'PrimeEngine-psvita' then
					_p(2, '<Import Project="fxcompile.targets" />')
				end
			_p(1,'</ImportGroup>')

		_p('</Project>')
	end


--
-- Generate the list of project dependencies.
--

	function vc2010.projectReferences(prj)
		local deps = premake.getdependencies(prj)
		if #deps > 0 then
			_p(1,'<ItemGroup>')
			for _, dep in ipairs(deps) do
				local deppath = path.getrelative(prj.location, vstudio.projectfile(dep))
				_p(2,'<ProjectReference Include=\"%s\">', path.translate(deppath, "\\"))
				_p(3,'<Project>{%s}</Project>', dep.uuid)
				_p(2,'</ProjectReference>')
			end
			_p(1,'</ItemGroup>')
		end
	end


--
-- Generate the .vcxproj.user file
--

	function vc2010.debugdir(cfg)
		if cfg.debugdir then
			_p('    <LocalDebuggerWorkingDirectory>%s</LocalDebuggerWorkingDirectory>', path.translate(cfg.debugdir, '\\'))
			_p('    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>')
		end
		if cfg.debugargs then
			_p('    <LocalDebuggerCommandArguments>%s</LocalDebuggerCommandArguments>', table.concat(cfg.debugargs, " "))
		end
		
		--akovalovs: set file serving and home directories
		if cfg.platform == 'PS3' then
			_p(3,'<LocalDebuggerFileServingDirectory>$(ProjectDir)/../../</LocalDebuggerFileServingDirectory>')
			_p(3,'<LocalDebuggerHomeDirectory>$(ProjectDir)/../../</LocalDebuggerHomeDirectory>')
		end
	end

	function vc2010.debugenvs(cfg)
		if cfg.debugenvs and #cfg.debugenvs > 0 then
			_p(2,'<LocalDebuggerEnvironment>%s%s</LocalDebuggerEnvironment>',table.concat(cfg.debugenvs, "\n")
					,iif(cfg.flags.DebugEnvsInherit,'\n$(LocalDebuggerEnvironment)','')
				)
			if cfg.flags.DebugEnvsDontMerge then
				_p(2,'<LocalDebuggerMergeEnvironment>false</LocalDebuggerMergeEnvironment>')
			end
		end
	end

	function premake.vs2010_vcxproj_user(prj)
		io.indent = "  "
		vc2010.header()
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			_p('  <PropertyGroup '.. if_config_and_platform() ..'>', premake.esc(cfginfo.name))
			vc2010.debugdir(cfg)
			vc2010.debugenvs(cfg)
			
			_p('  </PropertyGroup>')
		end
		_p('</Project>')
	end



