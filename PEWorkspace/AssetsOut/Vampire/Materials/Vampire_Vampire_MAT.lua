function fillMaterialTable(args) -- the script format requires existence of this function
args['version']=2
args['diffuse']={0.000000, 0.000000, 0.000000, 1.000000}
args['shininess']=2.000000
args['reflectivity']={0.000000, 0.000000, 0.000000}
args['specular']={0.500000, 0.500000, 0.500000}
args['emissive']={0.500000, 0.500000, 0.500000}
args['textures']={
  {'NORMAL', 'Vampire_normal.png'},
  {'SPECULAR', 'Vampire_specular.png'},
  {'GLOW', 'Vampire_emission.png'},
} -- end textures
--notes overrides
args['textures']={
{'COLOR', 'Vampire_diffuse.dds'},
{'SPECULAR', 'Vampire_specular.dds'},
{'NORMAL', 'Vampire_normal.dds'},
{'GLOW', 'Vampire_emission.dds'},
}--end textures
--notes end

end
