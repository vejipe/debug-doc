# make a static lib
add_library(
    proc
    STATIC
        src/proc/Proc.cpp
)

target_include_directories(
    proc
    PUBLIC
        include/
        ../data/include/
)

set_property(TARGET proc PROPERTY POSITION_INDEPENDENT_CODE ON)
