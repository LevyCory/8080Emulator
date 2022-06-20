set(PROJECT_NAME i8080tests)

set(SOURCE_DIR ${SOURCE_DIR}/${PROJECT_NAME})

set(SOURCE_FILES
    ${SOURCE_DIR}/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} i8080)
