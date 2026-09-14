cmake_minimum_required(VERSION 3.22...4.1)

set(APP_NAME DailyDungeon)
project(${APP_NAME})

# Keep the Axmol checkout external to the game project.  Cache the path so an
# IDE-triggered reconfigure keeps working even when AX_ROOT is not inherited.
if(DEFINED ENV{AX_ROOT} AND NOT "$ENV{AX_ROOT}" STREQUAL "")
    set(DAILYDUNGEON_AX_ROOT "$ENV{AX_ROOT}" CACHE PATH "Axmol engine root" FORCE)
elseif(DAILYDUNGEON_AX_ROOT)
    set(ENV{AX_ROOT} "${DAILYDUNGEON_AX_ROOT}")
else()
    set(DAILYDUNGEON_AX_ROOT "D:/axmolProjects/axmol-for-pixel" CACHE PATH "Axmol engine root" FORCE)
    set(ENV{AX_ROOT} "${DAILYDUNGEON_AX_ROOT}")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(AXGameEngineOptions)
include(AXGameEngineSetup)

file(GLOB_RECURSE GAME_HEADER CONFIGURE_DEPENDS Source/*.h Source/*.hpp)
file(GLOB_RECURSE GAME_SOURCE CONFIGURE_DEPENDS Source/*.cpp Source/*.c)

# Steam is keyboard/mouse/controller only.  Keep the old mobile joystick files
# as migration reference, but do not compile or link them into this target.
list(REMOVE_ITEM GAME_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/SneakyJoystick.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/SneakyJoystickSkinnedBase.cpp")

set(GAME_INC_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
    "${_AX_ROOT}/axmol"
    "${_AX_ROOT}/axmol/audio"
    "${_AX_ROOT}/3rdparty")
set(content_folder "${CMAKE_CURRENT_SOURCE_DIR}/Content")

if(APPLE)
    ax_mark_multi_resources(common_content_files RES_TO "Resources" FOLDERS ${content_folder})
elseif(WINDOWS)
    ax_mark_multi_resources(common_content_files RES_TO "Content" FOLDERS ${content_folder})
endif()

set(APP_SOURCES ${GAME_HEADER} ${GAME_SOURCE})
include(AXGameTargetSetup)
target_compile_definitions(${APP_NAME} PRIVATE DAILYDUNGEON_STEAM=1)
ax_setup_app_config(${APP_NAME})
include(AXGamePlatformSetup)
include(AXGameFinalSetup)
