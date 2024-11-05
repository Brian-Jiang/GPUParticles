-- A project defines one build target
project("glues-".._OPTIONS["platformapi"])
	kind "StaticLib"
	language "C++"
	files { "**.h", "**.m", "**.c" }
	defines { "__USE_IOS_GLES__=1" }
	