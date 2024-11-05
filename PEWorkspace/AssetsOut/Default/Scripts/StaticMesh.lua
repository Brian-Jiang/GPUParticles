-- this is game object script for a static model
-- this game object represents a basic static model that is added to the scene
outputDebugString("Executing StaticMesh.lua\n")
function runScript(args)
    handler = getGameObjectManagerHandle(l_getGameContext())
    local pos = args['base']['pos']
    local u = args['base']['u']
    local v = args['base']['v']
    local n = args['base']['n']
    
    evt = root.PE.Events.Event_CREATE_MESH.Construct(
    l_getGameContext(),
    args['meshName'], args['meshPackage'], 
    pos[1], pos[2], pos[3],
    u[1], u[2], u[3],
    v[1], v[2], v[3],
    n[1], n[2], n[3],
    args['peuuid'])
    root.PE.Components.Component.SendEventToHandle(handler, evt)
end
