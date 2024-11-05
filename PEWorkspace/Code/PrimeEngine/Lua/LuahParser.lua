-- this module has functionality to parse all *.luah files and create namespaces
-- for detected enums, peuuids, etc

-- manual modul loading since will need to oevrride _G metatable
-- if wre to use module() would not be able to override the metatable

local modname = 'LuahParser'
local M = {}
_G[modname] = M
package.loaded[modname] = M
local outputDebugString = outputDebugString

if (outputDebugString ~= nil) then
    outputDebugString("Dbg Lua: Initializing LuahParser\n")
end

require "peuuid"

function M.PEMACRO_COMMENT(s)
    outputDebugString("PEMACRO_COMMENT: "..s.."\n")
end

function M.PEMACRO_START_LUAH_NAMESPACE_TABLE(tableName)
    local strtableName = M.g_LastUnknownKeys[#M.g_LastUnknownKeys]
    --outputDebugString("Setting Target Table Name: "..strtableName.."\n")
    
    if (M.g_PETargetTable == nil) then
        -- error. root is top level.
        outputDebugString("Error: g_PETargetTableName is nil. It should at least be at PE level. Check PEMACRO_{START,END}_LUAH_NAMESPACE_TABLE nesting \n")
        M.g_PETargetTable = M.root
    end
    if M.g_PETargetTable[strtableName] == nil then
        --outputDebugString("Target Namespace Table : "..strtableName.." Does not exits in current scope. Creating one. \n")
        M.g_PETargetTable[strtableName] = {}
        M.g_PETargetTable[strtableName].PERESERVED_prevTable = M.g_PETargetTable -- store for scope backtracking
    end
    M.g_PETargetTable = M.g_PETargetTable[strtableName]
end

function M.PEMACRO_END_LUAH_NAMESPACE_TABLE()
    M.g_PETargetTable = M.g_PETargetTable.PERESERVED_prevTable
    --outputDebugString("Popping Target Table/Namespace Name.\n")
end

function M.PEMACRO_DECLARE_PESUUID(cname, v0, v1, v2, v3)
    local strname = tostring(M.g_LastUnknownKeys[#M.g_LastUnknownKeys])
    --outputDebugString("PEMACRO_DECLARE_PESUUID with "..strname.."\n")
    M.g_PETargetTable[strname] = peuuid.constructPEUUID(v0, v1, v2, v3)
    --outputDebugString(tostring(M.g_PETargetTable[strname]).."\n")
    return
end

function M.outputTableKeysRecursive(name, t, prefix, offset)
    outputDebugString(prefix..name..":\n")
    local hasTables = 0
    -- initial pass to see if has tables
    for i,v in pairs(t) do
        if i ~= 'PERESERVED_prevTable' then
            if type(v) == 'table' then
                hasTables = 1
            end
        end
    end
    --delete redundant reference
    t['PERESERVED_prevTable'] = nil
    if hasTables then
        for i,v in pairs(t) do
            if i ~= 'PERESERVED_prevTable' then
                if type(v) == 'table' then
                    M.outputTableKeysRecursive(i, v, prefix..name..'/', offset.."  ")
                end
            end
        end
    end
    for i,v in pairs(t) do
        if i ~= 'PERESERVED_prevTable' then
            if type(v) ~= 'table' then
                outputDebugString(offset..i.."\n")
            end
        end
    end
end
M.g_LastUnknownKeys = {}
M.g_PETargetTable = {}
function M.parseLuahFiles()
    
    local PEGlobalMetaTable = {}

    PEGlobalMetaTable.__index = function(table, key)
        -- outputDebugString("Trying to access _G['"..tostring(key).."']\n")
        if M[key] ~= nil then
            -- outputDebugString("LuahParser has it\n")
            return M[key]
        end
        M.g_LastUnknownKeys[#M.g_LastUnknownKeys+1] = key
        return nil
    end
    
    local root = {} -- table to put all namespaces in
    M.g_LastUnknownKeys = {}
    M.g_PETargetTable = root
    
    
    outputDebugString("Dbg Lua: Overriding _G metatable\n")
    
    setmetatable(_G, PEGlobalMetaTable)

    outputDebugString("PE: Progress: Lua: Looking for files...\n")
    
    local fileTab = l_findluaFiles(l_getGameContext(), '*.luah');
    for i,v in ipairs(fileTab) do 
        outputDebugString(tostring(i)..":"..tostring(v).."\n")
        dofile(v)
    end

    outputDebugString("Dbg Lua: Restoring _G metatable\n")
    
    setmetatable(_G, nil)

    -- at this point all namespaces have been declared. PE should be the only namespace in the root

    outputDebugString("Parsed namespaces and peuuids: \n")

    M.outputTableKeysRecursive('/', root, "", "")
    
    return root
end
