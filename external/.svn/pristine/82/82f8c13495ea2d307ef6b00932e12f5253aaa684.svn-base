@echo off

REM Converts multiple png files to a texture array dds
REM Usage: dirassembler.bat [EXE] SOURCE_DIRECTORY DESTINATION_FILE [ADDITIONAL_PARAMS]
REM Author: F. Reichl

if [%1] == [] (
	echo ERROR: Executable not defined. Usage: dirassembler.bat [exe_path] [input_path] [output_file.dds]
)

if [%2] == [] (
	echo ERROR: Input path not defined. Usage: dirassembler.bat [exe_path] [input_path] [output_file.dds]
	exit /b
)

if [%3] == [] (
	echo ERROR: Output file not defined. Usage: dirassembler.bat [exe_path] [input_path] [output_file.dds]
	exit /b
)

REM pushd %1

echo Converting textures from "%~2" to "%~3"

set params=-array -o "%~3"
setlocal enabledelayedexpansion
echo Params: !params!

REM Parse all parameters starting from the 3rd
REM This is done using an explicit counter due to possible quotation marks in the first parameters...
set /a count=0
for %%x in (%*) do (
	set /a count=count+1
	if !count! GTR 3 (
		set addParams=!addParams! %%x
	)
)

REM Code below should be the more elegant way, but quotation marks mess this up...
REM for /f "tokens=1-2*" %%a in ("%*") do (
	REM set addParams=%%c
REM )

for %%x in (!addParams!) do (
	set params=!params! %%x
)

REM Add all files of the folder to the parameters
for %%i in (%2\*.png) do (
	set params=!params! "%%i"
)

REM popd
echo Command line: %1 !params!

%1 !params!