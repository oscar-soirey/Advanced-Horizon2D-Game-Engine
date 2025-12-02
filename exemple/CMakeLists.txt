cmake_minimum_required(VERSION 3.24)
project(PlatformerGame)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(include/ah2d)

#trouver la lib opengl systeme
find_package(OpenGL REQUIRED)

add_executable(PlatformerGame main.cpp)

#inclure les headers
target_include_directories(PlatformerGame PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}
)


#lier les libs
target_link_libraries(PlatformerGame
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/libHorizon2D.a
        ${CMAKE_SOURCE_DIR}/libs/libglfw3.a
        ${CMAKE_SOURCE_DIR}/libs/libglad.a
        ${OPENGL_gl_LIBRARY}
        ${CMAKE_SOURCE_DIR}/libs/libOpenAL32.dll.a
        ${CMAKE_SOURCE_DIR}/libs/libphysfs.dll.a
        ${CMAKE_SOURCE_DIR}/libs/libreactphysics3d.a
	${CMAKE_SOURCE_DIR}/libs/libspine-cpp.a
)