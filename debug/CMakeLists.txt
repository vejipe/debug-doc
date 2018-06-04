add_library(
    debug
    SHARED
        src/debug.cpp
)

set_target_properties(debug PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(debug PROPERTIES SOVERSION 1)
set_target_properties(debug PROPERTIES PUBLIC_HEADER include/main.h)

target_include_directories(
    debug
    PUBLIC
        include/
)

target_link_libraries(
    debug
    PRIVATE
        data
        proc
)

