project "Enet"
    kind "StaticLib"
    language "C"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files { 
        "include/**.h",
        "src/**.c",
    }
    
    includedirs { 
        "include/"
    }
    
    -- Link Winsock and WinMM
    links { "ws2_32", "winmm" }

    filter "system:windows"
        buildoptions { "-std=c11" }
        systemversion "latest"
        staticruntime "On"
        
        files
        {
            "src/win32.c",
        }
        
        defines
        {
            "ws2_32",
        }
        
        filter {"system:windows", "configurations:Release"}
