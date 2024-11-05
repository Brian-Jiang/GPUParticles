function fillMaterialTable(args) -- the script fromat requires existance of this function

outputDebugString("fillMaterialTable entry\n")
args['version'] = 2
args['diffuse']={0.8, 0.8, 0.8, 1.0 }
args['shininess']=0.0
args['reflectivity']={0,0,0}
args['specular'] = {0,0,0}
args['emissive'] = {0,0,0}
args['textures'] = {
    {'COLOR', 'Imrod_Diffuse.dds'},
    {'NORMAL', 'Imrod_norm.dds'},
    {'GLOW', 'Imrod_Glow.dds'},
    {'SPECULAR', 'Imrod_spec.dds'},
}
end -- required
