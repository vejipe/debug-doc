add_subdirectory(comp/data)
add_subdirectory(comp/proc)

add_executable(main
    main.cpp
)

target_include_directories(
    main
    PUBLIC
        comp/data/include
        comp/proc/include
)

target_link_libraries(
    main
    PRIVATE
        data
        proc
    # For "-ldl"
    PUBLIC
        dl
)
