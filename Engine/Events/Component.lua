
if (outputDebugString ~= nil) then
    outputDebugString("Initializing Component[Lua]\n")
end

function root.PE.Components.Component.CheckHandle(h)
	if type(h) == 'string' then
		return root.PE.Components.Component.StringAsHandle(h)
	else
		return h
	end
end

function root.PE.Components.Component.StringAsHandle(hstring)
	--outputDebugString("stringAsHandle: starting")
	handle = {}
	counter = 0
	_, _, q0, dbgName, pool, block = string.find(hstring, "([\"'])(.-)%1%s+(0x[0-9a-f]+)%s+(0x[0-9a-f]+)")
	outputDebugString("root.PE.Components.Component.StringAsHandle(hstring): "..tostring(dbgName).." : "..tostring(pool).." : "..tostring(block))
	handle = {[0] = dbgName, tonumber(pool), tonumber(block), 0}
	
	return handle
end

function root.PE.Components.Component.SendEventToHandle(handle, evt)
	handle = root.PE.Components.Component.CheckHandle(handle)
	t = root.PE.Components.Component.l_SendEventToHandle(handle, evt)
end
