@ECHO off
cls
cd %~dp0..
.\tools\Premake5.exe --file=premake5.lua vs2022
pause