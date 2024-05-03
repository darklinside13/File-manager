cmake_minimum_required(VERSION 3.12)

project(FileManager)

include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/imgui/)
include_directories(${PROJECT_SOURCE_DIR}/gui/)

find_package(SFML COMPONENTS system graphics audio network REQUIRED)
find_package(OpenGL REQUIRED)

FILE(GLOB 
    ImGui
    ${PROJECT_SOURCE_DIR}/imgui/*.h
    ${PROJECT_SOURCE_DIR}/imgui/*.cpp
)

FILE(
    GLOB
    GUI
    ${PROJECT_SOURCE_DIR}/gui/*
)

add_executable(${PROJECT_NAME} ${ImGui} ${GUI} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-window sfml-network sfml-graphics sfml-system OpenGL::GL)

install(FILES ${PROJECT_SOURCE_DIR}/deps/fileExample.txt DESTINATION ${PROJECT_BINARY_DIR}/)