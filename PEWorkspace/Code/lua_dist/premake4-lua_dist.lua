-- A project defines one build target
project("lua_dist-".._OPTIONS["platformapi"])
	kind "StaticLib"
	language "C"
	files { "**.h", "**.c" }
	excludes { "src/luac.c", "src/lua.c" }
