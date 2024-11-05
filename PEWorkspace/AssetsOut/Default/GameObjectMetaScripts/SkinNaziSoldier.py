t = {}
t["mayaRep"] = "Maya/Soldier/soldier_rep.mb"

t["callerScript"] = '''
-- this script is in lua format
-- this is a meta script that fills in data that is passed to 'myScript' that in turn calls C++ function
-- some of the data can be set by default, some of the data might be required to be edited from maya
function fillMetaInfoTable(args) -- the script fromat requires existance of this function

-- user modifed data
args['myScript']="Skin.lua"
args['myScriptPackage']="Default"

args['skelName'] = "soldier_Soldier_Skeleton.skela"
args['skelPackage'] = "Soldier"

args['meshName'] = "SoldierTransform.mesha"
args['meshPackage'] = "Soldier"

args['animsetName'] = "soldier_Soldier_Skeleton.animseta"
args['animsetPackage'] = "Soldier"

args['defaultAnimSet'] = 0  
args['defaultAnim'] = 20

end -- required
'''
