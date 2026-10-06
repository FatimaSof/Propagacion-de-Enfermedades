cmake_minimum_required(VERSION 3.16)

project(PropagacionEnfermedades)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML está en la raíz del proyecto
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window Audio Network System REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES "scr/*.cpp")
file(GLOB HEADERS "scr/*.h" "include/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
    SFML::Graphics
    SFML::Window
    SFML::Audio
    SFML::Network
    SFML::System
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/SFML/bin"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)