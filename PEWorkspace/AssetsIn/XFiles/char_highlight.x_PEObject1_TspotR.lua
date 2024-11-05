
-- this script is in lua format
-- this is a meta script that fills in data that is passed to 'myScript' that in turn calls C++ function
-- some of the data can be set by default, some of the data might be required to be edited from maya
function fillMetaInfoTable(args) -- the script fromat requires existance of this function

-- user modifed data
args['myScript']="LightSource.lua"
args['myScriptPackage']="Default"

args['lightType'] = "spot"
args['meshPackage'] = "Default"

args['diffuse'] = {2.5, 2.0, 2.0, 1.0}
args['spec'] = {10, 10, 10, 1.0}
args['ambient'] = {0, 0, 0, 1.0}
args['attenuation'] = {0.05, 0.05, 0.05}
args['spotPower'] = 20.0
args['range'] = 10.0
args['isShadowCaster'] = 0


end -- required
