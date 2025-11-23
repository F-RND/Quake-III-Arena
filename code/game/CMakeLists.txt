# Game module (server-side game logic)
file(GLOB GAME_SOURCES
    *.c
)

# Remove files that shouldn't be in the game module
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*bg_lib\\.c$")
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*g_syscalls\\.c$")  # Don't use in shared library build
list(FILTER GAME_SOURCES EXCLUDE REGEX ".*g_rankings\\.c$")  # Old GameSpy rankings

add_library(qagame SHARED ${GAME_SOURCES})

target_compile_definitions(qagame PRIVATE
    QAGAME
    Q3_VM
)

target_include_directories(qagame PRIVATE
    ${CMAKE_SOURCE_DIR}/code
    ${CMAKE_SOURCE_DIR}/code/game
)

# Game module doesn't link to anything (uses trap interface)
set_target_properties(qagame PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/baseq3"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/baseq3"
)

# Architecture-specific naming
if(WIN32)
    set_target_properties(qagame PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(qagame PROPERTIES SUFFIX ".dylib")
else()
    # Linux - use architecture-specific naming
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        set_target_properties(qagame PROPERTIES SUFFIX "x86_64.so")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86")
        set_target_properties(qagame PROPERTIES SUFFIX "i386.so")
    else()
        set_target_properties(qagame PROPERTIES SUFFIX ".so")
    endif()
endif()

install(TARGETS qagame
    LIBRARY DESTINATION baseq3
    RUNTIME DESTINATION baseq3
)

list(FILTER GAME_SOURCES EXCLUDE REGEX ".*g_rankings\\.c$")  # Old GameSpy rankings - deprecated
