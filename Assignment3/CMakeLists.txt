cmake_minimum_required(VERSION 3.5)

project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find OpenCV dependencies
find_package(OpenCV REQUIRED)

add_subdirectory(src)

include_directories(${CMAKE_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_SOURCE_DIR}/src ALL_SRCS)

add_executable(main main.cpp ${ALL_SRCS})

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(main ${OpenCV_LIBS})
