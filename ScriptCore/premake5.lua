--local scripts_dir = "%{wks.location}/BenEngine/assets/Scripts"
--local files = {
--    "Source/**.cs",
--    --"%{wks.location}/BenEngine/Resource/Scripts/**.cs",
--    --"%{wks.location}/BenEngine/assets/Scripts/**.cs",
--    "Properties/**.cs"
--}
--
---- Dynamically include all .cs files in the directory
--for _, file in ipairs(os.matchfiles(scripts_dir .. "/*.cs")) do
--    table.insert(files, file)
--end


project "ScriptCore"
    kind "SharedLib"
    language "C#"
    staticruntime "off"
    dotnetframework "4.8"
    clr "Unsafe"
    buildoptions {
      "/p:LangVersion=10.0"  -- Set to your desired C# language version (e.g., 9.0, latest, etc.)
    }
    targetdir ("%{wks.location}/BenEngine/Resource/Scripts")
    objdir ("%{wks.location}/BenEngine/Resource/Scripts/Intermediates")
    --objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "Engine_PCH.h"
    pchsource "%{prj.name}/src/Engine_PCH.cpp"

    --files(files)
    files{
        "Source/**.cs",
        --"%{wks.location}/BenEngine/Resource/Scripts/**.cs",
        --"%{wks.location}/BenEngine/assets/Scripts/**.cs",
        --"%{wks.location}/BenEngine/assets/Scripts/**/*.cs",
        "Properties/**.cs"
    }
    -- Optional: If you need to explicitly match certain files or patterns
    --files {
    --   os.matchfiles("%{wks.location}/BenEngine/assets/Scripts/**.cs")  -- Dynamically match all .cs files
    --}
     
    links{
        --"BenEngine",
    }

    filter { "files:**.cs" }
        buildoptions { "/unsafe" } -- Adds the unsafe flag

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
