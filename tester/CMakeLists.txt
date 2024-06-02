set(EXE_NAME tester)

add_executable(${EXE_NAME} main.cpp)

target_include_directories(
    ${EXE_NAME}
    PRIVATE
)

target_include_directories(
    ${EXE_NAME}
    PRIVATE ${LIB8080_INCLUDE}
    PRIVATE ${EXTERNALS}/fmt/include
)

target_link_libraries(${EXE_NAME} ${LIB8080_TARGET})
