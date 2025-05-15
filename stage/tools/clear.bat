cls
@echo off

IF EXIST ..\build\.vs rmdir /s /q
IF EXIST ..\bin\debug rmdir /s /q ..\bin\debug
IF EXIST ..\bin\release rmdir /s /q ..\bin\release
IF EXIST ..\build rmdir /s /q ..\build
IF EXIST ..\bin rmdir /s /q ..\bin
