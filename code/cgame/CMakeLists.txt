# CGame module (client-side game logic)
file(GLOB CGAME_SOURCES
    *.c
)
# Exclude Team Arena code
list(FILTER CGAME_SOURCES EXCLUDE REGEX ".*cg_newdraw\\.c$")
list(FILTER CGAME_SOURCES EXCLUDE REGEX ".*cg_syscalls\\.c$")
list(FILTER CGAME_SOURCES EXCLUDE REGEX ".*cg_particles\\.c$")  # Conflicts with cg_marks.c

# Add shared game files
list(APPEND CGAME_SOURCES
    ../game/bg_misc.c
    ../game/bg_pmove.c
    ../game/bg_slidemove.c
    ../game/bg_lib.c
)

add_library(cgame SHARED ${CGAME_SOURCES})

target_compile_definitions(cgame PRIVATE
    CGAME
    Q3_VM
)

target_include_directories(cgame PRIVATE
    ${CMAKE_SOURCE_DIR}/code
    ${CMAKE_SOURCE_DIR}/code/cgame
    ${CMAKE_SOURCE_DIR}/code/game
)

set_target_properties(cgame PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/baseq3"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/baseq3"
)

if(WIN32)
    set_target_properties(cgame PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(cgame PROPERTIES SUFFIX ".dylib")
else()
    # Linux - use architecture-specific naming
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        set_target_properties(cgame PROPERTIES SUFFIX "x86_64.so")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86")
        set_target_properties(cgame PROPERTIES SUFFIX "i386.so")
    else()
        set_target_properties(cgame PROPERTIES SUFFIX ".so")
    endif()
endif()

install(TARGETS cgame
    LIBRARY DESTINATION baseq3
    RUNTIME DESTINATION baseq3
)
