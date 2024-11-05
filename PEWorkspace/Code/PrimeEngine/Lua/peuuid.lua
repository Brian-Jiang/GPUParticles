-- peuuid.lua
-- functions for working with peuuid
module(..., package.seeall)

function constructPEUUID(v0, v1, v2, v3)
    return l_peuuid.l_constructPEUUID(v0, v1, v2, v3)
end

function equalPEUUID(peuuid0, peuuid1)
    return l_peuuid.l_equalPEUUID(peuuid0, peuuid1)
end

function findKeyInTableByPEUUIDValue(t, id)
    outputDebugString("peuuid.findKeyInTableByPEUUIDValue Entry\n")
    if (t == nil) then
        outputDebugString("ERROR: peuuid.findKeyInTableByPEUUIDValue: table provided is nil\n")
        return nil
    end
    if (type(id) ~= 'string') then
        outputDebugString("ERROR: peuuid.findKeyInTableByPEUUIDValue: id provided is not string (peuuid is string)\n")
        return nil
    end
    for k,v in pairs(t) do
        if type(v) == 'string' and equalPEUUID(v, id) then
            outputDebugString("peuuid.findKeyInTableByPEUUIDValue Exit\n")
            return k
        end
    end
    outputDebugString("peuuid.findKeyInTable Exit\n")
    return nil
end
