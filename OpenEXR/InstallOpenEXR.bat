@echo off
cd ..\..\openexr
if not exist %cd%\_build (
    mkdir _build
    cd _build
    mkdir install
    cmake %cd% --install-prefix %cd%\_build\install
    cmake --build %cd%\_build --target install --config Debug
    cmake --build %cd%\_build --target install --config Release
    
    cd install
    mkdir Debug
    mkdir Release
    move /y "bin\*_d.dll" "Debug\"
    move /y "lib\*_d.lib" "Debug\"
    move /y "bin\*.dll" "Release\"
    move /y "lib\*.lib" "Release\"

    for /f %%N in ('dir /b Debug\*lib') do (
        for /f "tokens=1-3 delims=_" %%A in ("%%N") do (
            ren ""Debug\%%N"" "%%A_%%B.lib"
        )
    )
)