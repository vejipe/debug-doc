# make a static lib
add_library(
    data
    STATIC
        src/data/Data.cpp
        src/data/DataImpl.cpp
)

target_include_directories(
    data
    PUBLIC
        include/
)

set_property(TARGET data PROPERTY POSITION_INDEPENDENT_CODE ON)
