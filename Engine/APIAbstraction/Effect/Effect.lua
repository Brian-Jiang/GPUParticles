
if (outputDebugString ~= nil) then
    outputDebugString("Initializing Effect[Lua]\n")
end

-- Effect/Shader functions

function root.PE.Components.Effect.GetTechnique(handle)
	handle = root.PE.Components.Component.CheckHandle(handle)
	t = root.PE.Components.Effect.l_GetTechnique(l_getGameContext(), handle)
	res = "{'name':'"..t.name.."',"
	if t.vs then
		res = res.."'vs':{".."'vsname':'"..t.vs.vsname.."','filename':'"..t.vs.filename.."','text':'"..t.vs.text.."'},"
	end
	
	if t.gs then
		res = res.."'gs':{".."'gsname':'"..t.gs.gsname.."','filename':'"..t.gs.filename.."','text':'"..t.gs.text.."'},"
	end
	
	if t.ps then
		res = res.."'ps':{".."'psname':'"..t.ps.psname.."','filename':'"..t.ps.filename.."','text':'"..t.ps.text.."'},"
	end
	
	if t.cs then
		res = res.."'cs':{".."'csname':'"..t.cs.csname.."','filename':'"..t.cs.filename.."','text':'"..t.cs.text.."'},"
	end
	
	if t.spes then
		res = res.."'spes':{".."'spesname':'"..t.spes.spesname.."','filename':'"..t.spes.filename.."','text':'"..t.spes.text.."'},"
	end
	
	res = res.."}\n"
	
	
	outputDebugString("root.PE.Components.Effect.GetTechnique(). Sending\n")
	
	CommandServer.sendToPyClient(res)
end

function root.PE.Components.Effect.SetTechnique(handle, val)
	handle = root.PE.Components.Component.CheckHandle(handle)
	t = root.PE.Components.Effect.l_SetTechnique(l_getGameContext(), handle, val)
end

function root.PE.Components.Effect.SetSpeShaderData(val)
	outputDebugString("Calling l_SetSpeShaderData\n")
	root.PE.Components.Effect.l_SetSpeShaderData(l_getGameContext(), val)
end

