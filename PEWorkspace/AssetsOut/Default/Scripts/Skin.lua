-- this is game object script for a skin (skinned mesh with animations)
-- this game object represents a basic static model that is added to the scene
outputDebugString("Executing Skin.lua")
function runScript(args)
    handler = getGameObjectManagerHandle(l_getGameContext())
    
    local pos = args['base']['pos']
    local u = args['base']['u']
    local v = args['base']['v']
    local n = args['base']['n']

    evt = root.PE.Events.Event_CREATE_SKELETON.Construct(l_getGameContext(), 
    args['skelName'], args['skelPackage'],
    pos[1], pos[2], pos[3],
    u[1], u[2], u[3],
    v[1], v[2], v[3],
    n[1], n[2], n[3],
    args['peuuid'])
    root.PE.Components.Component.SendEventToHandle(handler, evt)
    
    evt = root.PE.Events.Event_CREATE_MESH.Construct(
    l_getGameContext(),
    args['meshName'], args['meshPackage'], 
    pos[1], pos[2], pos[3],
    u[1], u[2], u[3],
    v[1], v[2], v[3],
    n[1], n[2], n[3],
    args['peuuid'])
    
    root.PE.Components.Component.SendEventToHandle(handler, evt)
    
    evt = root.PE.Events.Event_CREATE_ANIM_SET.Construct(l_getGameContext(),
    args['animsetName'], args['animsetPackage'],
    args['peuuid'])
    root.PE.Components.Component.SendEventToHandle(handler, evt)
    
    
    if args['defaultAnimSet'] ~= nil and args['defaultAnim'] ~= nil then
        hSkin = l_getLastAddedGameObjectHandle(l_getGameContext())
        evtH = root.PE.Events.Event_PLAY_ANIMATION.Construct(args['defaultAnimSet'], args['defaultAnim'])
        root.PE.Components.Component.SendEventToHandle(hSkin, evtH)
    end
end
