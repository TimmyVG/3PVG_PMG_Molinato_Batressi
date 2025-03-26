cls
@ECHO off

call "gendeps.bat"
cd /d "%~dp0"
call "compile_vs2022.bat"
cd /d "%~dp0"

msbuild ..\build\Motor.vcxproj /p:Configuration=Debug /p:Platform=x64
msbuild ..\build\Motor.vcxproj /p:Configuration=RelWithDebInfo /p:Platform=x64
msbuild ..\build\Motor.vcxproj /p:Configuration=Release /p:Platform=x64

xcopy "..\tools\compile_vs2022.bat" "..\stage\tools\" /Y
xcopy "..\tools\clear.bat" "..\stage\tools\" /Y
xcopy "..\tools\compile_vs2019.bat" "..\stage\tools\" /Y
xcopy "..\tools\gendeps.bat" "..\stage\tools\" /Y
xcopy "..\tools\Premake5.exe" "..\stage\tools\" /Y
xcopy "..\premake5.lua" "..\stage\" /Y
xcopy "..\src\build\conanfile.txt" "..\stage\src\build\" /Y

xcopy "..\include\mew\*" "..\stage\include\mew\*" /Y
xcopy "..\include\*" "..\stage\include\*" /Y
xcopy "..\examples\*" "..\stage\examples\*" /Y
xcopy "..\deps\imgui\*" "..\stage\deps\imgui\*" /Y
xcopy "..\data\*" "..\stage\data\*" /Y /E
xcopy "..\build\Debug\mew.lib" "..\stage\lib\Debug\" /Y
xcopy "..\build\RelWithDebInfo\mew.lib" "..\stage\lib\RelWithDebInfo\" /Y
xcopy "..\build\Release\mew.lib" "..\stage\lib\Release\" /Y

cd /d "%~dp0"
powershell Compress-Archive ..\stage release.zip
pause