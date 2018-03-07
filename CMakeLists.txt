cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(PROJECT_NAME Chimera)
project(${PROJECT_NAME} CXX C)

## Folders files
#set(CMAKE_CXX_STANDARD 17)
# Define Release by default.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified: Using Release by default.")
endif (NOT CMAKE_BUILD_TYPE)

add_definitions(-D_WINDOWS
        -D_USRDLL
        -DUNICODE
        -D_UNICODE
        )

#check for msvc -> not currently supported.
if (MSVC)
    message(AUTHOR_WARNING "Building Chimera with ${CMAKE_CXX_COMPILER_ID} is untested. Use at your own risk!")

    set(COMPILER_REL_FLDR msvc)
    #Microsoft's c++17 implementation isn't complete, so we need to ensure using latest
    set(CMAKE_CXX_FLAGS /std:c++latest)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /Od /Zi /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Od /Oi /Gy /Zi /EHsc")
elseif (MINGW)
    message(STATUS "Mingw detected. ")

    set(COMPILER_REL_FLDR mingw)
    set(CMAKE_CXX_FLAGS -std=c++14)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -m32 -masm=intel")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -g -m32 -masm=intel")
else (MSVC)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ /permissive")
        message(FATAL_ERROR "Chimera does not yet support clang, if it ever will.")
    endif ()
    message(FATAL_ERROR "Unrecognized compiler/suite: ${CMAKE_CXX_COMPILER_ID}")
endif (MSVC)

# Output Variables
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(GLOB DBG_CLIENT_G ./client/debug/*.cpp)
    set(OUTPUT_DIR ./bin/${COMPILER_REL_FLDR}-debug)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OUTPUT_DIR ./bin/${COMPILER_REL_FLDR}-Release)
endif ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/ar")
set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/exe")

#set(LUA_TMP_OUT "./client/lua/lua/lua")
if (true)#(NOT EXISTS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lua.a")
    # Build lua

    file(GLOB LUA_CLIENT_G ./client/lua/*.cpp ./client/lua/*.h)
    file(GLOB LUA_LUA_G ./client/lua/lua/*.c ./client/lua/lua*.h)

    add_library(LUA_LUA STATIC ${LUA_LUA_G} ${LUA_LUA_G})
    set_target_properties(LUA_LUA PROPERTIES LINKER_LANGUAGE C)
endif ()

file(GLOB DTA_CLIENT_G ./client/halo_data/chat.cpp
        ./client/halo_data/resolution.cpp
        ./client/halo_data/global.cpp
        ./client/halo_data/keyboard.cpp
        ./client/halo_data/map.cpp
        ./client/halo_data/server.cpp
        ./client/halo_data/spawn_object.cpp
        ./client/halo_data/script.cpp
        ./client/halo_data/table.cpp
        ./client/halo_data/tag_data.cpp)

file(GLOB TIA_CLIENT_G ./client/halo_data/tiarace/*.cpp)
file(GLOB HAC_INJECT_G ./code_injection/hacclient/*.cpp)
file(GLOB POL_CLIENT_G ./client/interpolation/*.cpp)
file(GLOB ENH_CLIENT_G ./client/enhancements/*.cpp)
file(GLOB MSG_CLIENT_G ./client/messaging/*.cpp)
file(GLOB CMD_CLIENT_G ./client/command/*.cpp)
file(GLOB HUD_CLIENT_G ./client/hud_mod/*.cpp)
file(GLOB STT_CLIENT_G ./client/startup/fast_startup.cpp ./client/startup/crc32.c)
file(GLOB VIS_CLIENT_G ./client/visuals/*.cpp)
file(GLOB HKS_CLIENT_G ./client/hooks/*.cpp)
file(GLOB XBX_CLIENT_G ./client/xbox/*.cpp)
file(GLOB FIX_CLIENT_G ./client/fix/*.cpp)
file(GLOB INJECT_G ./code_injection/*.cpp)
file(GLOB CLIENT_G ./client/lua/*.h ./client/*.cpp main.cpp)
file(GLOB MATH_G ./math/*.cpp)

#add_library(MAIN STATIC main.cpp)

#add_library(DTA_CLIENT STATIC ${DTA_CLIENT_G})

#add_library(CLIENT STATIC ${CLIENT_G})
if (MSVC)
    message(STATUS "Using mvc, so we're ignoring the resolution.cpp file.")
    add_library(DTA_CLIENT STATIC ${DTA_CLIENT_G})
elseif (NOT MSVC)
    add_library(DTA_CLIENT STATIC ${DTA_CLIENT_G})
    #set(CMAKE_CXX_FLAGS -llua -shared -lws2_32 -static-libgcc -static-libstdc++ -static -luserenv -static -lpthread -static -ladvapi32)
endif ()

link_libraries()
#some weirdness surrounding version.h and version.rc, so I'm gonna be leaving them out until further notice.
add_library(${PROJECT_NAME} SHARED ${CLIENT_G} ${INJECT_G} ${MATH_G} ${DTA_CLIENT_G} ${DBG_CLIENT_G}
        ${CMD_CLIENT_G}
        ${ENH_CLIENT_G}
        ${FIX_CLIENT_G}
        ${TIA_CLIENT_G}
        ${HKS_CLIENT_G}
        ${HUD_CLIENT_G}
        ${POL_CLIENT_G}
        ${MSG_CLIENT_G}
        ${STT_CLIENT_G}
        ${VIS_CLIENT_G}
        ${XBX_CLIENT_G}
        ${HAC_INJECT_G}
        ${HKS_CLIENT}
        ${HUD_CLIENT}
        ${POL_CLIENT}
        ${MSG_CLIENT}
        ${STT_CLIENT}
        ${VIS_CLIENT}
        ${XBX_CLIENT})
#-L client/lua/lua/bin -llua -shared -lws2_32 -static-libgcc -static-libstdc++ -static -luserenv -static -lpthread -static -ladvapi32
#target_link_libraries(LUA_LUA -llua -shared -lws2_32 -static-libgcc -static-libstdc++ -static -luserenv -static -lpthread -static -ladvapi32)
target_link_libraries(${LUA_LUA} ${PROJECT_NAME})