
module(..., package.seeall)

if (outputDebugString ~= nil) then
    outputDebugString("Dbg Lua: Initializing HierarchyBuilder\n")
end

require "peuuid"
require "CommandServer"

function handleAsPyList(handle)
    res = "['"..tostring(handle[0]).."',"..tostring(handle[1])..","..tostring(handle[2])..","..tostring(l_convertUserdataPointerToNumber(handle[3])).."]"
    return res
end

-- create Python Dictionary representation of component and its children
function buildHierarchy(handle, level)
    -- outputDebugString("Building hierarchy..\n")
    -- header of the object
    outputDebugString("buildHierarchy(): Entry \n")
            
    local componentTypeOverride = "UNKNOWN"
    local componentInfo = root.PE.Components.Component.GetComponentInfo(handle)
    local nativeComponentType = componentInfo.className
    
    outputDebugString("buildHierarchy(): Looked up types \n")
    
    outputDebugString("buildHierarchy(): Looked up component types \n")
    
    -- name
    res = "{'name':'"..tostring(nativeComponentType).."',"
    -- handle
    res = res.."'handle':"..handleAsPyList(handle)..","
    -- native component type
    res = res.."'compType':'"..tostring(nativeComponentType).."',"
    -- level
    res = res.."'level':"..level..",'selected':None,"
    if level < 6 then
     -- fill in handler queues
        res = res.." 'handlerQueues':[" -- list of queues(dictionaries)
        q = root.PE.Components.Component.GetHandlerQueueTable(handle)
        if (q ~= nil) then
            outputDebugString("Retrieved handlerQueues: "..tostring(q).." \n")
            for i,v in pairs(q) do 
                outputDebugString(tostring(i)..":"..tostring(v).."\n")
                res = res.."{" -- queue dictionary
                    res = res.."'evtPEUUID':'"..tostring(l_getClassNameByClassId(i)).."'," -- id of the event of the queue
                    res = res.."'handlers':[" -- list of handlers, v is the table of handlers
                        for hi,hv in ipairs(v) do
                            if type(hv) == 'table' then
                                -- handler is a table (component)
                                outputDebugString("buildHierarchy(): handler is a table (component)\n")
                                res = res..handleAsPyList(hv)..","
                            else
                                -- handler is a pointer to a method
                                outputDebugString("buildHierarchy(): handler is a pointer to a method\n")
                                res = res..'"method",'
                            end
                        end
                    res = res.."]," -- end of list of handlers
                res = res.."}," -- end of queue dictionary
            end
        else
            outputDebugString("ERROR Retrieving handlerQueues: got nil")
        end
        
        res = res.."],"
        -- fill in data components
        -- note, data components have no children components
        res = res.." 'dataComponents':["
        res = res.."],"
        -- fill in components
        res = res.." 'components':["
        local handles = root.PE.Components.Component.GetComponentHandles(handle)
        --outputDebugString("Retrieved "..#handles.."handles\n")
        for i,v in ipairs(handles) do 
            --outputDebugString(tostring(i)..":"..tostring(v).."\n")
            res = res..buildHierarchy(v, level + 1)
        end
        res = res.."],"
    else
        -- outputDebugString("Maximum Depth Reached\n")
    end
    res = res.."},"
    return res
end

function synchComponentsWithPyClient()
    outputDebugString("Synching components with pyclient..\n")
    -- name
    res = "{'name':'ROOT:ROOT','handle':[0,0,0,0],'compType':'ROOT','level':0,'selected':None,"
    res = res.." 'dataComponents':[],"
    res = res.." 'components':["
    res = res..buildHierarchy(getGameObjectManagerHandle(l_getGameContext()), 1)
    res = res..buildHierarchy(l_getRootSceneNodeHandle(), 1)
    res = res.."],},\n"
    outputDebugString("Built hierarchy. Sending\n")
    
    CommandServer.sendToPyClient(res)
end

