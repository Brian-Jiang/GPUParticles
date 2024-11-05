function fillMaterialTable(args) -- the script format requires existence of this function
args['version']=2
args['diffuse']={1.000000, 1.000000, 1.000000, 1.000000}
args['shininess']=2.000000
args['reflectivity']={0.000000, 0.000000, 0.000000}
args['specular']={0.000000, 0.000000, 0.000000}
args['emissive']={0.000000, 0.000000, 0.000000}
args['textures']={
  {'COLOR', 'space_frigate_6_color.dds'},
} -- end textures
--notes overrides
args['textures']={
{'COLOR', 'space_frigate_6_color.dds'},
{'SPECULAR', 'space_frigate_6_specular.dds'},
{'NORMAL', 'cobble2_normal.dds'},
{'GLOW', 'space_frigate_6_illumination.dds'},
}--end textures
args['technique']='Shadowed_A_Glow'



--notes end

end
