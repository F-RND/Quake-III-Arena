# UI module (user interface)
file(GLOB UI_SOURCES
    *.c
)

# Add shared game files
list(APPEND UI_SOURCES
    ../game/bg_misc.c
    ../game/bg_lib.c
    ../game/q_math.c
    ../game/q_shared.c
)

add_library(ui SHARED ${UI_SOURCES})

target_compile_definitions(ui PRIVATE
    UI
    Q3_VM
)

target_include_directories(ui PRIVATE
    ${CMAKE_SOURCE_DIR}/code
    ${CMAKE_SOURCE_DIR}/code/ui
    ${CMAKE_SOURCE_DIR}/code/game
    ${CMAKE_SOURCE_DIR}/code/qcommon
)

set_target_properties(ui PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/baseq3"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/baseq3"
)

if(WIN32)
    set_target_properties(ui PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(ui PROPERTIES SUFFIX ".dylib")
else()
    # Linux - use architecture-specific naming
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        set_target_properties(ui PROPERTIES SUFFIX "x86_64.so")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86")
        set_target_properties(ui PROPERTIES SUFFIX "i386.so")
    else()
        set_target_properties(ui PROPERTIES SUFFIX ".so")
    endif()
endif()

install(TARGETS ui
    LIBRARY DESTINATION baseq3
    RUNTIME DESTINATION baseq3
)
