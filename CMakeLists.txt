cmake_minimum_required(VERSION 3.16)
project(shiny LANGUAGES C VERSION 0.0.1)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED)

add_subdirectory(src)

include(deps/clay.cmake)
include(deps/sdl.cmake)
include(deps/stb.cmake)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
