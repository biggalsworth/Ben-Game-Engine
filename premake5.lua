workspace "BenEngine"
    architecture "x64"
    startproject "BenEngine"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to solution directory
IncludeDir = {}
IncludeDir["GLFW"] = "BenEngine/Middleware/GLFW/include"
IncludeDir["Glad"] = "BenEngine/Middleware/Glad/include"
IncludeDir["spdlog"] = "BenEngine/Middleware/spdlog/include"
IncludeDir["glm"] = "BenEngine/Middleware/glm"
IncludeDir["IMGUI"] = "BenEngine/Middleware/IMGUI"
IncludeDir["enTT"] = "BenEngine/Middleware/enTT/single_include/entt"
IncludeDir["IMGUIZMO"] = "BenEngine/Middleware/ImGuizmo"
IncludeDir["YAMLCPP"] = "BenEngine/Middleware/YAML-CPP/include"
IncludeDir["Box2D"] = "BenEngine/Middleware/Box2D/include"
IncludeDir["FMOD"] = "BenEngine/Middleware/FMOD"
IncludeDir["Mono"] = "BenEngine/Middleware/Mono"
--IncludeDir["CppSharp"] = "BenEngine/Middleware/CppSharp"
IncludeDir["Enet"] = "BenEngine/Middleware/Enet"
IncludeDir["Filewatch"] = "BenEngine/Middleware/Filewatch"
IncludeDir["SFML"] = "BenEngine/Middleware/SFML"
IncludeDir["ImPlot"] = "BenEngine/Middleware/ImPlot"
IncludeDir["Recast"] = "BenEngine/Middleware/Recast"

--Includes the premake5.lua file for the middleware, attaching it to this file
include "BenEngine/Middleware/GLFW"
include "BenEngine/Middleware/Glad"
include "BenEngine/Middleware/IMGUI"
include "BenEngine/Middleware/YAML-CPP"
include "BenEngine/Middleware/Box2D"
include "BenEngine/Middleware/Mono"
--include "BenEngine/Middleware/CppSharp"
include "BenEngine/Middleware/Enet"
include "ScriptCore"

project "BenEngine"
    location "BenEngine"
    kind "ConsoleApp"
    language "C++"
    staticruntime "off"
    cppdialect "C++17"
    --clr "Unsafe"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "Engine_PCH.h"
    pchsource "%{prj.name}/src/Engine_PCH.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{IncludeDir.IMGUIZMO}/**.h",
        "%{IncludeDir.IMGUIZMO}/**.cpp",
        "%{IncludeDir.ImPlot}/**.h",
        "%{IncludeDir.ImPlot}/**.cpp",
        
        "%{IncludeDir.Recast}/Include/**.cpp",
        "%{IncludeDir.Recast}/Include/**.h",
        --"%{IncludeDir.CppSharp}/CppSharpBindings/**.cs",  -- Include C# bindings files
        --"%{IncludeDir.CppSharp}/CppSharpBindings/**/*.cs",  -- Include all .cs files in the CppSharpBindings folder
        
         -- add the generated C# bindings to the C++ project
        "BenEngine/Resource/Scripts/**/*.cs"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "YAML_CPP_STATIC_DEFINE"
    }

    includedirs
    {
        "%{prj.name}/src/",
        "%{prj.name}/Middleware",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.IMGUI}",
        "%{IncludeDir.enTT}",
        "%{IncludeDir.IMGUIZMO}",
        "%{IncludeDir.YAMLCPP}",
        "%{IncludeDir.Box2D}",
        "%{IncludeDir.FMOD}/inc",
        "%{IncludeDir.Mono}/include",
        "%{IncludeDir.Enet}/include",
        "%{IncludeDir.Filewatch}",
        "%{IncludeDir.SFML}/include",
        "%{IncludeDir.ImPlot}",
        "%{IncludeDir.Recast}/Recast/Include",
        "%{IncludeDir.Recast}/Recast/Source",
        "%{IncludeDir.Recast}/Detour/Include",
        "%{IncludeDir.Recast}/Detour/Source",

        --"%{IncludeDir.CppSharp}/Include",
        --"%{IncludeDir.CppSharp}/src"
    }   
    

    libdirs
    {
        "%{IncludeDir.FMOD}/lib/",
        "%{IncludeDir.Mono}/lib/",
        "%{IncludeDir.SFML}/lib/",
        "%{IncludeDir.Recast}/lib/",
        --"bin/" .. outputdir .. "/ScriptAPI",
        --"%{IncludeDir.CppSharp}/lib/"
    }


    links
    {
        "GLFW",
        "Glad",
        "IMGUI",
        "YAML-CPP",
        "Box2D",
        "fmod_vc",
        "mono-2.0-sgen",
        --"CppSharp"
        "Enet",
        "Pdh",
        "Recast",
        "Detour"

    }

    dependson
    {
        "ScriptCore",
        "Pdh"
    }

    filter "files:BenEngine/Middleware/ImGuizmo/**.cpp"
        flags { "NoPCH" }

    filter "files:BenEngine/Middleware/ImPlot/**.cpp"
        flags { "NoPCH" }

    filter "files:BenEngine/Middleware/ImPlot/**.cpp"
        flags { "NoPCH" }

    filter "files:BenEngine/Middleware/Recast/**.cpp"
        flags { "NoPCH" }


    filter "system:windows"
        buildoptions { "/utf-8" }
        systemversion "latest"
        defines
        {
            "GLFW_INCLUDE_NONE"
        }

        postbuildcommands {
         '{COPY} "../BenEngine/Middleware/FMOD/lib/fmod.dll" "%{cfg.targetdir}"',
         --'{COPY} "../BenEngine/Middleware/Mono/bin/mono-2.0-sgen.dll" "%{cfg.targetdir}"',
         --'{COPY} "../BenEngine/Middleware/Mono/etc" "%{cfg.targetdir}/etc"',
         --"\"C:\\Program Files\\dotnet\\sdk\\8.0.100\\Roslyn\\csc.exe\" -out:../BenEngine/Resource/Scripts/ScriptCore.dll /BenEngine/Resource/Scripts/**/*.cs"
         '{COPY} "../BenEngine/Resource/Scripts/ScriptCore.dll" "%{cfg.targetdir}"',

        }

 

    filter "configurations:Debug"
        staticruntime "off"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        staticruntime "off"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
