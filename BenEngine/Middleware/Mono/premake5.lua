project "Mono"
    kind "SharedLib"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Assuming Mono runtime is pre-downloaded
    files
    {
        "BenEngine/Middleware/Mono/include/mono/*.h",
        "BenEngine/Middleware/Mono/Data/",
    }
    
    includedirs
    {
        -- Point to Mono's include directory
        "BenEngine/Middleware/Mono/include",
        "BenEngine/Middleware/Mono/Data/",
    }
    
    libdirs
    {
        -- Point to Mono's lib directory (if linking statically)
        "BenEngine/Middleware/Mono/lib",
        "BenEngine/Middleware/Mono/Data/",

    }
    
    links
    {
        "mono-2.0-sgen"
    }
