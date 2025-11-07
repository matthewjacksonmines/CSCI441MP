cmake_minimum_required(VERSION 3.14)
project(MP)
set(CMAKE_CXX_STANDARD 17)

# Automatically gather source files
file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/main.cpp"
    "${CMAKE_SOURCE_DIR}/MPEngine.cpp"
    "${CMAKE_SOURCE_DIR}/MPEngine.h"
    "${CMAKE_SOURCE_DIR}/Hero.h"
    "${CMAKE_SOURCE_DIR}/heroes/*.cpp"
    "${CMAKE_SOURCE_DIR}/heroes/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/heroes
)

# Platform-specific configuration
if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" AND MINGW )
    include_directories("C:/MinGW/include/CSCI441")
    target_link_directories(${PROJECT_NAME} PUBLIC "C:/MinGW/include/CSCI441")
    target_link_libraries(${PROJECT_NAME} opengl32 glfw3 glad gdi32)
elseif( APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
    include_directories("/usr/local/include")
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" glfw3 glad)
elseif( UNIX AND ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    include_directories("/usr/local/include")
    target_link_directories(${PROJECT_NAME} PUBLIC "/usr/local/lib")
    target_link_libraries(${PROJECT_NAME} GL glfw glad)
endif()
