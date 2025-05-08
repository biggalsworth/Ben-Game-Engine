@echo off
setlocal
set script_DIR=%~dp0

:: Recursively find all premake5.lua files in the current directory and subdirectories
for /r "%script_DIR%" %%f in (premake5.lua) do (
    echo Running premake5 for %%f

    :: Run Premake
    call ..\premake\premake5.exe --file="%%f" vs2022

    :: Check if Premake succeeded
    if errorlevel 1 (
        echo Premake failed for %%f.
    ) else (
        echo Premake succeeded for %%f.

        :: Extract the directory path and create folders
        mkdir "%%~dpfAssets/Scripts"
        mkdir "%%~dpfAssets/Textures"

        :: Skip to the next file
        goto :nextdir
    )
)
:nextdir

endlocal
exit