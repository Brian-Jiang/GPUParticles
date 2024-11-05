
if (outputDebugString ~= nil) then
    outputDebugString("Initializing Skeleton[Lua]\n")
end


-- Skeleton functions -------------------------------------------------
function root.PE.Components.Skeleton.SetSkeletonAnim(handle, val)
	handle = root.PE.Components.Component.CheckHandle(handle)
	root.PE.Components.Skeleton.l_SetSkeletonAnim(handle, val)
end

function root.PE.Components.Skeleton.GetSkeleton(handle)
	outputDebugString("PYENGINE: DBG: Lua: root.PE.Components.Skeleton.GetSkeleton() entry\n");
	handle = root.PE.Components.Component.CheckHandle(handle)
	outputDebugString("PYENGINE: DBG: Calling root.PE.Components.Skeleton.l_GetSkeleton\n");
	t = root.PE.Components.Skeleton.l_GetSkeleton(handle)
	
	res = "{"
		res = res.."'anims':["
		for i,v in ipairs(t["anims"]) do
			outputDebugString(tostring(v).."\n")
			res = res.."{".."'name':'"..tostring(v['name']).."', 'animSetIndex':"..tostring(v['animSetIndex'])..",'index':"..tostring(v['index']).."},"
		end
		res = res.."]," -- end anims
	res = res.."}\n"
	
	
	outputDebugString("root.PE.Components.Skeleton.GetSkeleton(). Sending\n")
	outputDebugString(res.."\n")
	
	CommandServer.sendToPyClient(res)
end
