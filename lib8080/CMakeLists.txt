set(LIBRARY_NAME lib8080)
set(LIBRARY_DIR ${CMAKE_SOURCE_DIR}/${LIBRARY_NAME})

set(SOURCES
    asm.cpp
    cpu.cpp
    bus.cpp
)

add_library(${LIBRARY_NAME} ${SOURCES})

target_include_directories(
    ${LIBRARY_NAME}
    PRIVATE ${LIBRARY_DIR}/include/i8080
    PRIVATE ${EXTERNALS}/fmt/include
) 

target_link_libraries(
    ${LIBRARY_NAME}
    PRIVATE fmt::fmt
)

set(LIB8080_INCLUDE ${LIBRARY_DIR}/include PARENT_SCOPE)
set(LIB8080_TARGET ${LIBRARY_NAME} PARENT_SCOPE)
