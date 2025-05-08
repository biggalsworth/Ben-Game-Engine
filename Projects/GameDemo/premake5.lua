workspace "GameDemo"
    architecture "x64"
    startproject "GameDemo"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }



project "GameDemo"
    kind "SharedLib"
    language "C#"
    clr "Unsafe"
    dotnetframework "4.8"


    targetdir ("../../BenEngine/Resource/Scripts")
    objdir ("../../BenEngine/Resource/Scripts/Intermediates")

    pchheader "Engine_PCH.h"
    pchsource "%{prj.name}/src/Engine_PCH.cpp"
    
    files{
        "Assets/**",
        "Assets/Scripts/**.cs",
        --"%{wks.location}/BenEngine/Resource/Scripts/",
        --"../../BenEngine/assets/Scripts/**.cs",
        --"../../ScriptCore/Source/**.cs",
    }
    includedirs{
        "Assets/Textures/",
        "Assets/Scripts/",
        --"../../BenEngine/assets/Scripts/**.cs",
        --"../../ScriptCore/Source/**.cs",

    }
    libdirs{
        "%{wks.location}/BenEngine/Resource/Scripts/",
        "../../BenEngine/Middleware/CppSharp/CppSharpBindings/bin/Debug/net8.0",
    }

    links{
        "ScriptCore",
        "CppSharp",
    }
        
    postbuildcommands {

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
