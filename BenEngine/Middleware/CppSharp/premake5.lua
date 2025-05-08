project "CppSharp"
    kind "StaticLib"
    language "C++"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**/**.cs",  
        "src/**/**cpp",
        "src/**/**.h", 

        "Include/**.cpp", 
        "Include/**.h", 
        "Include/**.cs" 


    }

    -- Reference .NET packages (you may need to install CppSharp using NuGet in the .NET project)
    includedirs
    {
        "BenEngine/Middleware/CppSharp/Include",
        "Include"  -- Include CppSharp C# library
    }



    libdirs
    {
        "BenEngine/Middleware/CppSharp/lib/"
    }
    
    links
    {
        "CppSharp",
        "mono-2.0-sgen"
    }


    postbuildcommands 
    {
        -- could call CppSharp generated code after the project is built

        "dotnet build --project %{prj.location}/CppSharp.csproj",
        "dotnet run --project %{prj.location}/CppSharp.csproj"
    }