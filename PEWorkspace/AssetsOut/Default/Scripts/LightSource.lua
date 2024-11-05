-- this is game object script for a static model
-- this game object represents a basic static model that is added to the scene
outputDebugString("Executing LightSource.lua\n")
function runScript(args)
    handler = getGameObjectManagerHandle(l_getGameContext())
    local pos = args['base']['pos']
    local u = args['base']['u']
    local v = args['base']['v']
    local n = args['base']['n']
    local diffuse = args['diffuse']
    local spec = args['spec']
    local ambient = args['ambient']
    local att = args['attenuation']
    evt = root.PE.Events.Event_CREATE_LIGHT.Construct( 
    pos[1], pos[2], pos[3],
    u[1], u[2], u[3],
    v[1], v[2], v[3],
    n[1], n[2], n[3],
    args['lightType'],
    diffuse[1], diffuse[2], diffuse[3], diffuse[4],
    spec[1], spec[2], spec[3], spec[4],
    ambient[1], ambient[2], ambient[3], ambient[4],
    att[1], att[2], att[3],
    args['spotPower'],
    args['range'],
    args['isShadowCaster'],
    args['peuuid'])
    root.PE.Components.Component.SendEventToHandle(handler, evt)
end
