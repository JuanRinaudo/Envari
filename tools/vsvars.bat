@echo off

set "VSVARSALLPATH_2017=\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
set "VSVARSALLPATH_2019=\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"

echo %VSVARSALLPATH%

set ARCHITECTURE=%1
if [%1] == [] set ARCHITECTURE=x86

if exist "%ProgramFiles%%VSVARSALLPATH_2017%" (
    "%ProgramFiles%%VSVARSALLPATH_2017%" %ARCHITECTURE%
) else (
    if exist "%ProgramFiles(X86)%%VSVARSALLPATH_2017%" (
        "%ProgramFiles(X86)%%VSVARSALLPATH_2017%" %ARCHITECTURE%
    ) else (
        if exist "%ProgramFiles(X86)%%VSVARSALLPATH_2019%" (
            "%ProgramFiles(X86)%%VSVARSALLPATH_2019%" %ARCHITECTURE%
        ) else (
            if exist "%ProgramFiles%%VSVARSALLPATH_2019%" (
                "%ProgramFiles%%VSVARSALLPATH_2019%" %ARCHITECTURE%
            ) else (
                echo "Visual studio was not found, check vsvars inside Envari/tools/vsvars"
            )
        )
    )    
)