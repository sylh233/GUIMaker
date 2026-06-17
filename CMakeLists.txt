cmake_minimum_required(VERSION 4.1)

project(GUIMaker)

set(HOME ${CMAKE_CURRENT_SOURCE_DIR}/home)
set(EXECUTABLE_OUTPUT_PATH ${HOME}/bin)
set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty)
file(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${THIRD_PARTY}/inc)

link_directories(${THIRD_PARTY}/lib)

set(SDL_SHAERD OFF CACHE BOOL "" FORCE)
add_subdirectory(${THIRD_PARTY}/SDL)

set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE) #使用自身的freetype等
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) #静态链接
set(SDLTTF_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(${THIRD_PARTY}/SDL_ttf)

add_executable(guiMaker ${SRC_LIST})

target_link_options(guiMaker PRIVATE
			      -static 
			      -static-libstdc++
			      -static-libgcc
			      -pthread
			      -Wl,--as-needed
			      )

target_link_libraries(guiMaker PRIVATE SDL3::SDL3-static SDL3_ttf::SDL3_ttf-static
)
