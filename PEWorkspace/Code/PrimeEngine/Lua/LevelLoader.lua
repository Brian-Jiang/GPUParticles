module(..., package.seeall)

if (outputDebugString ~= nil) then
    outputDebugString("Dbg Lua: Initializing LevelLoader\n")
end

require "peuuid"

function loadLevel(level, package)
	local projRoot = l_getGameProjRoot(l_getGameContext())
    delim = l_getPathDelimeter()

	if level ~= nil and level ~= '' then
		outputDebugString('Loading Provided Level:'..level..'\n')
	else
		-- no level was proided, try looking up environment variable
		v = os.getenv('PYENGINE_STARTUP_LEVEL')
		if v ~= nil then
			outputDebugString('Loading Level Using PYENGINE_STARTUP_LEVEL Environment Variable:'..v..'\n')
			level = v
		end
	end
	
	if level ~= nil and level ~= '' then
		outputDebugString('Level is a lua script. Will just run it.\n')
		
		local filePath = ''
		
        filePath = l_getGameProjRoot(l_getGameContext())..'AssetsOut'..delim..package..delim..'Levels'..delim..level

        outputDebugString('Dbg: file: '..filePath..'\n')
		
        dofile(filePath)
	end
end

function CreateGameObject(name, m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m33, m03, m13, m23, m33, metaScriptFilename, metaScriptPackage, peuuid0, peuuid1, peuuid2, peuuid3)
    outputDebugString('Dbg: CreateGameObject("'..name..'")\n')
    delim = l_getPathDelimeter()

    if metaScriptPackage == 'Raw' then
        f = loadstring(metaScriptFilename);
        if (f ~= nil) then
            outputDebugString('Executing Raw MetaScript \n')
            
            f()
        else
            outputDebugString('Error while compiling Raw MetaScript\n')
            outputDebugString('Sent meta script had syntax errors. Aborting object loading. \n')
            return 0;
            
        end
    else
        local filePath =''
        
        local projRoot = l_getGameProjRoot(l_getGameContext())
	    
        filePath = l_getGameProjRoot(l_getGameContext())..'AssetsOut'..delim..metaScriptPackage..delim..'Levels'..delim..metaScriptFilename

        outputDebugString('Dbg: file: '..filePath..'\n')

        dofile(filePath) -- will define the fillMetaInfoTable function
    end
    args = {}
    
    -- add matrix to args
    args['base'] = {}
    args['base']['pos'] = {m03, m13, m23}
    args['base']['u'] =   {m00, m10, m20}
    args['base']['v'] =   {m01, m11, m21}
    args['base']['n'] =   {m02, m12, m22}
    
    if peuuid0 == nil or peuuid1 == nil or peuuid2 == nil or peuuid3 == nil then
        args['peuuid'] = peuuid.constructPEUUID(0, 0, 0, 0)
    else
        args['peuuid'] = peuuid.constructPEUUID(peuuid0, peuuid1, peuuid2, peuuid3)
    end
    fillMetaInfoTable(args)
    outputDebugString('PE: Progress: LevelLoader.lua: Filled in meta info table. running the script\n')
	
    local filePath = ''
	
    local projRoot = l_getGameProjRoot(l_getGameContext())
    
    filePath = l_getGameProjRoot(l_getGameContext())..'AssetsOut'..delim..tostring(args["myScriptPackage"])..delim..'Scripts'..delim..tostring(args["myScript"])

    outputDebugString('PE: Progress: LevelLoader.lua: compiling file: '..filePath..'...\n')
	dofile(filePath) -- will define a runScript(args) function
    
    outputDebugString('PE: Progress: LevelLoader.lua: runing runScript(args) from compiled script with arguments from metascript...\n')
	
    runScript(args) -- pass in arguments filled by fillMetaInfoTable(args). -- this call will envoke C++ functions
    
end

