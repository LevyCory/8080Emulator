set(PROJECT_NAME i8080)

set(INCLUDE_DIR ${INCLUDE_DIR}/${PROJECT_NAME})
set(SOURCE_DIR ${SOURCE_DIR}/${PROJECT_NAME})

set(SOURCE_FILES
    ${SOURCE_DIR}/asm.cpp
    ${SOURCE_DIR}/cpu.cpp
    ${SOURCE_DIR}/bus.cpp)

add_library(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
