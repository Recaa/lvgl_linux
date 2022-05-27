cmake_minimum_required (VERSION 3.16)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-g")

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "-O3")

project(lvgl_linux)

set(LVGL_INCLUDE_PATH "lvgl-8.2.0" 
                      "lv_drivers-8.2.0/sdl"
                      "include"
)

find_package(SDL2 REQUIRED SDL2)

file(GLOB_RECURSE SRC_FILE_PATH "assets/*.c"
                                "src/*.c"
)

include_directories(${LVGL_INCLUDE_PATH}
                    ${SDL2_INCLUDE_DIRS}
)

add_subdirectory(lvgl-8.2.0)
add_subdirectory(lv_drivers-8.2.0)

# set target file
add_executable(lvgl_linux ${SRC_FILE_PATH}
)

target_link_libraries(lvgl_linux lvgl
                                 lv_drivers
                                 pthread
                                 ${SDL2_LIBRARIES}
)