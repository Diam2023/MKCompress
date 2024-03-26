cmake_minimum_required(VERSION 3.5)

project(MKCompress VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adding Source Code
aux_source_directory(src SRCS)

add_executable(
        ${PROJECT_NAME}
        ${SRCS}
)

find_package(Qt5 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS

include_directories(libs/bit7z/include)
add_subdirectory(libs/bit7z)

# Qt and bit7z
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets bit7z)
