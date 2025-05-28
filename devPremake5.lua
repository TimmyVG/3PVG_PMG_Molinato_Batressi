project "Motor"
    kind "StaticLib"
    targetname "mew"
    targetdir "build/%{cfg.buildcfg}"
    
    includedirs { "include", "build/deps/bindings_include" }
    conan_config_lib()
    pchheader "stdafx.hpp"
    pchsource "src/stdafx.cpp"
    forceincludes { "stdafx.hpp" }

    files {
        "premake5.lua",
        "src/build/conanfile.txt",
        "src/stdafx.cpp", "src/stdafx.hpp",
        "src/*.cpp", "include/mew/*.hpp",
        "build/deps/bindings/*.cpp","build/deps/bindings_include/*.h"
        }
           project"Window"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/window.cpp"

    project "Motor"
    kind "StaticLib"
    targetname "mew"
    targetdir "build/%{cfg.buildcfg}"
    
    includedirs { "include", "build/deps/bindings_include" }
    conan_config_lib()
    pchheader "stdafx.hpp"
    pchsource "src/stdafx.cpp"
    forceincludes { "stdafx.hpp" }

    files {
        "premake5.lua",
        "src/build/conanfile.txt",
        "src/build/conan.lua",
        "src/*.cpp", "include/mew/*.hpp",
        "build/deps/bindings/*.cpp","build/deps/bindings_include/*.h"
        }


    project"Triangle"
        kind "WindowedApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/triangle.cpp"

    project"TriangleMove"
        kind "WindowedApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/trianglemove.cpp"

    project"MeshTextureLoader"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/meshtextureloader.cpp"

    project"JobSystem"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/jobsystem.cpp"

     project"Scripting"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/scriptinglua.cpp"

    project"ECS"
        kind "consoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/ecs.cpp"

    project"Lights Forward"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/lightsforward.cpp"

        
    project"Deferred"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/mapdeferred.cpp"

    project"Physics"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/physics.cpp"

    project"SSAO"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/mapSSAO.cpp"
    
    project"DisplacementMap"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/waterDisplacementExample.cpp"

     project"Map"
        kind "ConsoleApp"
        language "C++"
        targetdir "build/%{prj.name}/%{cfg.buildcfg}"
        includedirs "include"
        libdirs { "build/%{cfg.buildcfg}" }
        links {"mew"}
        conan_config_exec("Debug")
        conan_config_exec("Release")
        conan_config_exec("RelWithDebInfo")
        debugargs { _MAIN_SCRIPT_DIR .. "/examples/data" }
        files "examples/map.cpp"