@echo off
rem Usage: run.bat [Debug|Release|MinSizeRel|RelWithDebInfo]
set myDir=%~dp0
cd /d %myDir%
set APP_NAME=DailyDungeon
set BUILD_DIR=build
set BUILD_CFG=%1
if not defined BUILD_CFG set BUILD_CFG=Debug
start /D %myDir%Content %BUILD_DIR%/bin/%APP_NAME%/%BUILD_CFG%/%APP_NAME%.exe
