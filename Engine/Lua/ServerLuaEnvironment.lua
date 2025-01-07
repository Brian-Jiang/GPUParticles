-- LuaEnvironment is responsible for maintaining lua environment
-- do initialization here
if (outputDebugString ~= nil) then
    outputDebugString("Initializing Server LuaEnvironment[Lua]\n")
end

local projRoot = l_getGameProjRoot(l_getGameContext())
local delim = l_getPathDelimeter()
local PERootDelim = projRoot..'Code'..delim..'PrimeEngine'..delim
package.path = PERootDelim..'Lua'..delim..'?.lua;'..PERootDelim..'Lua'..delim..'Generated'..delim..'?.lua;'
package.cpath = '' --only have static libs, no dynamic dll loading

outputDebugString('package.path:'..package.path..'\n')

require "LuahParser"
root = LuahParser.parseLuahFiles() -- need to do this call as early as possible since there is a LUA hack to track uninitialized names
-- thus need _G global environment to be as clean as possible

require "socket"
require "peuuid"
require "int32"
require "HierarchyBuilder"
require "CommandServer"
require "LevelLoader"
require "os"

PyEngine = {}

PE = root.PE

g_poll_counter = 0;

local portNum =  l_getLuaCommandServerPort(l_getGameContext())
local intetaddr = '0.0.0.0'

outputDebugString("PE: Server: creating Lua command server at "..intetaddr..":"..tostring(portNum).."\n")

portNum = CommandServer.createTCPServer( intetaddr, portNum, 1 )

l_setCreatedLuaCommandServerPort(l_getGameContext(), portNum)

g_tcp_co = coroutine.create( CommandServer.runTCPServer );

function handleEvent(evt)
	--LuahParser.outputTableKeysRecursive('/', root, "", "")
        
    --outputDebugString("function handleEvent(evt)\n")

    if int32.equalInt32(evt.evtClassId, root.PE.Events.Event_UPDATE.ClassId) then
		--outputDebugString("PE: PROGRESS: [LuaComponent] Handling UPDATE Event..\n")
        --run console server
        g_poll_counter = g_poll_counter + 1;
        --if (g_poll_counter > 60) then
            coroutine.resume( g_tcp_co )
            g_poll_counter = 0
            --outputDebugString("PE Lua: LuaEnvironment[Lua]: ServerTick\n")
        --end
    end
end
