cmake_minimum_required(VERSION 3.5)

project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find OpenCV dependencies
find_package(OpenCV REQUIRED)



add_executable(main main.cpp ${ALL_SRCS})


target_link_libraries(main ${OpenCV_LIBS})
