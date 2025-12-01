cmake_minimum_required(VERSION 3.31)
project(Horizon2D)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(src)

#trouver la lib opengl systeme
find_package(OpenGL REQUIRED)


#creer le .a final
add_library(Horizon2D STATIC
        #EXTERNAL LIBS
        #Tinyxml
        src/external/tinyxml2/tinyxml2.cpp

        #3D ENGINE
        #components
        src/components/base_component.cpp
        src/components/camera.cpp
        src/components/light.cpp
        src/components/sprite.cpp
        src/components/skeletal_sprite.cpp
        src/components/character_movement.cpp
        src/components/colliders/box_collision.cpp
        src/components/colliders/capsule_collision.cpp
        src/components/line_trace.cpp

        #basic classes
        src/actor.cpp
        src/character.cpp
        src/controller.cpp

        #core classes
        src/core/window.cpp
        src/core/reflection.cpp
        src/core/assets_manager.cpp

        #rendering
        src/rendering/renderer.cpp
        src/rendering/shader.cpp
        src/rendering/texture.cpp
        src/rendering/texture_atlas.cpp

        #widgets
        src/ui/base_widget.cpp
        src/ui/widget_container.cpp
        src/ui/widget_image.cpp
        src/ui/widget_button.cpp

        #utility
        src/utils/line_trace.cpp

        #effects
        src/effects/emitter.cpp

        #AUDIO ENGINE
        src/audio/al_audio_utils.cpp
        src/audio/al_audio_buffer.cpp
        src/audio/al_audio_source.cpp
)

#ajouter les dépendances
target_include_directories(Horizon2D PRIVATE ${CMAKE_SOURCE_DIR}/include)

#lier les libs
target_link_libraries(Horizon2D
        PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/libglfw3.a
        ${CMAKE_SOURCE_DIR}/libs/libglad.a
        ${OPENGL_gl_LIBRARY}
        ${CMAKE_SOURCE_DIR}/libs/libOpenAL32.dll.a
        ${CMAKE_SOURCE_DIR}/libs/libphysfs.dll.a
        ${CMAKE_SOURCE_DIR}/libs/libreactphysics3d.a
        ${CMAKE_SOURCE_DIR}/libs/libspine-cpp.a
)