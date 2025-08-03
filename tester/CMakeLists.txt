set(EXE_NAME tester)

add_executable(${EXE_NAME} main.cpp)

target_link_libraries(
    ${EXE_NAME} 
    PRIVATE ${LIBRARY_NAME}
)
