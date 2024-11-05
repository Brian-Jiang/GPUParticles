
-- this script is in lua format
-- this is a meta script that fills in data that is passed to 'myScript' that in turn calls C++ function
-- some of the data can be set by default, some of the data might be required to be edited from maya
function fillMetaInfoTable(args) -- the script fromat requires existance of this function

-- user modifed data
args['myScript']="LightSource.lua"
args['myScriptPackage']="Default"

args['lightType'] = "directional"
args['meshPackage'] = "Default"

args['diffuse'] = {0.5, 0.5, 0.5, 1.0}
args['spec'] = {0, 0, 0, 1.0}
args['ambient'] = {0.2, 0.2, 0.25, 1.0}
args['attenuation'] = {0, 0, 0} -- does not matter fordirectional light
args['spotPower'] = 0 -- does not matter fordirectional light
args['range'] = 0 -- does not matter fordirectional light

end -- required
