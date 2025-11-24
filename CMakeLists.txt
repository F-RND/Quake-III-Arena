cmake_minimum_required(VERSION 3.15)
project(Quake3Arena VERSION 1.0.0 LANGUAGES C CXX)

# Modern C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_CLIENT "Build client" ON)
option(BUILD_SERVER "Build dedicated server" OFF) # Temporarily disabled during modernization
option(BUILD_GAME_QVM "Build game QVM modules" OFF)
option(USE_OPENAL "Use OpenAL for sound" ON)
option(USE_OPENAL_DLOPEN "Dynamically load OpenAL" OFF)
option(USE_CURL "Use libcurl for downloads" ON)
option(USE_CURL_DLOPEN "Dynamically load libcurl" OFF)
option(USE_VOIP "Enable VoIP support" OFF)
option(USE_INTERNAL_JPEG "Use internal JPEG library" OFF)  # JPEG code is embedded in tr_image.c

# Platform detection
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wno-unused-function
        -Wno-unused-variable
        -Wno-unused-but-set-variable
        -Wno-misleading-indentation
        -Wno-format-overflow
        -Wno-format-truncation
        -pipe
    )

    # 64-bit compatibility fixes
    add_compile_options(
        -fno-strict-aliasing
        -fPIC
    )

    # Optimization for release
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        add_compile_options(-O2 -ffast-math)
    endif()
endif()

# Architecture detection
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    add_definitions(-DARCH_X64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86")
    add_definitions(-DARCH_X86)
endif()

# Platform-specific definitions
if(LINUX)
    add_definitions(-D__linux__)
    add_definitions(-DLINUX)
elseif(WIN32)
    add_definitions(-D_WIN32)
elseif(APPLE)
    add_definitions(-D__APPLE__)
    add_definitions(-DMACOS_X)
endif()

# Find required packages
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

if(LINUX)
    find_package(X11 REQUIRED)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "X11 not found")
    endif()
endif()

if(USE_OPENAL)
    find_package(OpenAL)
    if(OPENAL_FOUND)
        add_definitions(-DUSE_OPENAL)
    endif()
endif()

if(USE_CURL)
    find_package(CURL)
    if(CURL_FOUND)
        add_definitions(-DUSE_CURL)
    endif()
endif()

# Source files organization
set(CODE_DIR ${CMAKE_SOURCE_DIR}/code)

# Common source files
file(GLOB QCOMMON_SOURCES
    ${CODE_DIR}/qcommon/*.c
)

# Remove vm_*.c files from qcommon for separate handling
list(FILTER QCOMMON_SOURCES EXCLUDE REGEX ".*vm_.*\\.c$")
list(FILTER QCOMMON_SOURCES EXCLUDE REGEX ".*q3_laffer.*\\.c$")

# Add shared game code needed by engine
list(APPEND QCOMMON_SOURCES
    ${CODE_DIR}/game/q_math.c
    ${CODE_DIR}/game/q_shared.c
    ${CODE_DIR}/qcommon/common_compat.c
)

# Server source files
file(GLOB SERVER_SOURCES
    ${CODE_DIR}/server/*.c
)

# Remove old GameSpy rankings (deprecated)
list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*sv_rankings\\.c$")

# Client source files
file(GLOB CLIENT_SOURCES
    ${CODE_DIR}/client/*.c
)

# Renderer source files
file(GLOB RENDERER_SOURCES
    ${CODE_DIR}/renderer/*.c
)

# BotLib source files
file(GLOB BOTLIB_SOURCES
    ${CODE_DIR}/botlib/*.c
)

# Set BOTLIB define for botlib sources
set_source_files_properties(${BOTLIB_SOURCES} PROPERTIES
    COMPILE_DEFINITIONS "BOTLIB"
)

# Platform-specific source files
if(LINUX)
    # Common platform code for both client and dedicated server
    set(PLATFORM_COMMON
        ${CODE_DIR}/unix/unix_main.c
        ${CODE_DIR}/unix/unix_shared.c
        ${CODE_DIR}/unix/unix_net.c
        ${CODE_DIR}/unix/linux_signals.c
        ${CODE_DIR}/unix/linux_common.c
    )

    # Client-only platform code
    set(PLATFORM_CLIENT
        ${CODE_DIR}/unix/linux_glimp.c
        ${CODE_DIR}/unix/linux_qgl.c
        ${CODE_DIR}/unix/linux_snd.c
        ${CODE_DIR}/unix/linux_joystick.c
    )

    # VM source files (x86/x64 JIT)
    file(GLOB VM_SOURCES
        ${CODE_DIR}/qcommon/vm.c
        ${CODE_DIR}/qcommon/vm_interpreted.c
        ${CODE_DIR}/unix/vm_x86.c
    )
elseif(WIN32)
    set(PLATFORM_COMMON
        ${CODE_DIR}/win32/win_main.c
        ${CODE_DIR}/win32/win_shared.c
        ${CODE_DIR}/win32/win_net.c
        ${CODE_DIR}/win32/win_syscon.c
    )

    set(PLATFORM_CLIENT
        ${CODE_DIR}/win32/win_gamma.c
        ${CODE_DIR}/win32/win_glimp.c
        ${CODE_DIR}/win32/win_input.c
        ${CODE_DIR}/win32/win_qgl.c
        ${CODE_DIR}/win32/win_snd.c
        ${CODE_DIR}/win32/win_wndproc.c
    )

    file(GLOB VM_SOURCES
        ${CODE_DIR}/qcommon/vm.c
        ${CODE_DIR}/qcommon/vm_interpreted.c
        ${CODE_DIR}/win32/vm_x86.c
    )
endif()

# JPEG library
if(USE_INTERNAL_JPEG OR TRUE)  # Always use for now
    file(GLOB JPEG_SOURCES
        ${CODE_DIR}/jpeg-6/j*.c
    )
    # Remove example/test programs, utilities, and platform-specific memory managers
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*example\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*ansi2knr\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*jmemdos\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*jmemmac\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*jmemname\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*jpegtran\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*cjpeg\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*djpeg\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*rdjpgcom\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*wrjpgcom\\.c$")
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*jmemnobs\\.c$")
    # Exclude jcapistd.c - tr_image.c has custom implementations of those functions
    list(FILTER JPEG_SOURCES EXCLUDE REGEX ".*jcapistd\\.c$")
else()
    set(JPEG_SOURCES "")
endif()

# Splines library (exclude q_shared.cpp - we use game/q_shared.c instead)
file(GLOB SPLINES_SOURCES
    ${CODE_DIR}/splines/*.cpp
)
list(FILTER SPLINES_SOURCES EXCLUDE REGEX ".*q_shared\\.cpp$")

# Include directories
set(INCLUDE_DIRS
    ${CODE_DIR}
    ${CODE_DIR}/qcommon
    ${CODE_DIR}/server
    ${CODE_DIR}/client
    ${CODE_DIR}/renderer
    ${CODE_DIR}/botlib
    ${CODE_DIR}/game
    ${CODE_DIR}/cgame
    ${CODE_DIR}/ui
    ${CODE_DIR}/jpeg-6
    ${CODE_DIR}/splines
    ${OPENGL_INCLUDE_DIR}
)

if(LINUX)
    list(APPEND INCLUDE_DIRS ${X11_INCLUDE_DIR})
endif()

# Main client executable
if(BUILD_CLIENT)
    add_executable(quake3
        ${QCOMMON_SOURCES}
        ${SERVER_SOURCES}
        ${CLIENT_SOURCES}
        ${RENDERER_SOURCES}
        ${BOTLIB_SOURCES}
        ${PLATFORM_COMMON}
        ${PLATFORM_CLIENT}
        ${VM_SOURCES}
        ${JPEG_SOURCES}
        ${SPLINES_SOURCES}
    )

    target_include_directories(quake3 PRIVATE ${INCLUDE_DIRS})

    target_link_libraries(quake3
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS}
        Threads::Threads
        m
    )

    if(LINUX)
        target_link_libraries(quake3
            ${X11_LIBRARIES}
            Xxf86dga
            Xxf86vm
        )
    endif()

    if(USE_OPENAL AND OPENAL_FOUND)
        target_link_libraries(quake3 ${OPENAL_LIBRARY})
    endif()

    if(USE_CURL AND CURL_FOUND)
        target_link_libraries(quake3 ${CURL_LIBRARIES})
    endif()

    # Install target
    install(TARGETS quake3 RUNTIME DESTINATION bin)
endif()

# Dedicated server executable
if(BUILD_SERVER)
    add_executable(quake3ded
        ${QCOMMON_SOURCES}
        ${SERVER_SOURCES}
        ${BOTLIB_SOURCES}
        ${PLATFORM_COMMON}
        ${VM_SOURCES}
        ${CODE_DIR}/null/null_client.c
        ${CODE_DIR}/null/null_input.c
        ${CODE_DIR}/null/null_snddma.c
    )

    target_compile_definitions(quake3ded PRIVATE DEDICATED)
    target_include_directories(quake3ded PRIVATE ${INCLUDE_DIRS})

    target_link_libraries(quake3ded
        ${CMAKE_DL_LIBS}
        Threads::Threads
        m
    )

    install(TARGETS quake3ded RUNTIME DESTINATION bin)
endif()

# Game modules (will be expanded for open-world features)
add_subdirectory(code/game)
add_subdirectory(code/cgame)
add_subdirectory(code/ui)

# Status message
message(STATUS "===========================================")
message(STATUS "Quake III Arena - Modernized Build System")
message(STATUS "===========================================")
message(STATUS "Build client: ${BUILD_CLIENT}")
message(STATUS "Build server: ${BUILD_SERVER}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "OpenAL support: ${USE_OPENAL}")
message(STATUS "libcurl support: ${USE_CURL}")
message(STATUS "===========================================")
