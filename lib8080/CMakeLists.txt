set(SOURCES
    asm.cpp
    cpu.cpp
    bus.cpp
)

add_library(${LIBRARY_NAME} ${SOURCES})

target_include_directories(
    ${LIBRARY_NAME}
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include/i8080
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include
) 

target_link_libraries(
    ${LIBRARY_NAME} PUBLIC fmt::fmt
)
