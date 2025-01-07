module(..., package.seeall)

if (outputDebugString ~= nil) then
    outputDebugString("Dbg Lua: Initializing MaterialLoader\n")
end

function loadMaterial(materialPath, package, resHandle)
	outputDebugString('Loading Provided Material:'..tostring(materialPath)..' by executing it\n')
	
	outputDebugString('executing file: '..materialPath..'\n')
		
    dofile(materialPath)  -- will define the fillMaterialTable function

	
	outputDebugString("file executed. fillMaterialTable() should be defined now. Calling it.\n")

	args = {}
    
    fillMaterialTable(args)

	outputDebugString("Calling MaterialCPU.l_CreateMaterial()("..tostring(MaterialCPU.l_CreateMaterial)..") with filled in args.\n")

	MaterialCPU.l_CreateMaterial(args, package, resHandle)
end
