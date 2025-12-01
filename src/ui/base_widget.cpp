#include "base_widget.h"

#include <core/window.h>
#include <core/reflection.h>
#include <glm/ext/matrix_transform.hpp>
#include <rendering/shader.h>
#include <rendering/renderer.h>
#include <utils/log.h>
#include <core/maths.h>

NAMEPSACE_AH2D_START;

float vert_center[] = {
    // Positions        // Coordonnées de texture
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f,  // Haut gauche
     0.5f, -0.5f, 0.0f,  1.0f, 1.0f,  // Haut droit
     0.5f,  0.5f, 0.0f,  1.0f, 0.0f,  // Bas droit
    -0.5f,  0.5f, 0.0f,  0.0f, 0.0f   // Bas gauche
};

float vert_top_right[] = {
    // Positions        // Coordonnées de texture
    -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,  // Haut gauche
     0.0f, -1.0f, 0.0f,  1.0f, 1.0f,  // Haut droit
     0.0f,  0.0f, 0.0f,  1.0f, 0.0f,  // Bas droit
    -1.0f,  0.0f, 0.0f,  0.0f, 0.0f   // Bas gauche
};

float vert_top_left[] = {
    // Positions        // Coordonnées de texture
    0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,  // Haut gauche
    1.0f,  -1.0f, 0.0f,  1.0f, 1.0f,  // Haut droit
    1.0f,   0.0f, 0.0f,  1.0f, 0.0f,  // Bas droit
    0.0f,   0.0f, 0.0f,  0.0f, 0.0f   // Bas gauche
};

float vert_bottom_left[] = {
    // Positions        // Coordonnées de texture
     0.0f, 0.0f, 0.0f,  0.0f, 1.0f,  // Haut gauche
     1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // Haut droit
     1.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // Bas droit
     0.0f, 1.0f, 0.0f,  0.0f, 0.0f   // Bas gauche
};

float vert_bottom_right[] = {
    // Positions        // Coordonnées de texture
    -1.0f,  0.0f, 0.0f,  0.0f, 1.0f,  // Haut gauche
     0.0f,  0.0f, 0.0f,  1.0f, 1.0f,  // Haut droit
     0.0f,  1.0f, 0.0f,  1.0f, 0.0f,  // Bas droit
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f   // Bas gauche
};

widget::BaseWidget::BaseWidget()
{
    HPROPERTY(position, Access::Exposed);
    HPROPERTY(rotation, Access::Exposed);
    HPROPERTY(scale, Access::Exposed);
    HPROPERTY(visible, Access::Exposed);
    HPROPERTY(pivot_method, Access::Exposed);
    HPROPERTY(fill_method, Access::Exposed);
}

void widget::BaseWidget::draw(Shader& shader, Window& window)
{
    //on reinitialise aussi les uv qui ont été changée dans les sprites
    if (pivot_method == PIVOT_CENTER)
    {
        glBindBuffer(GL_ARRAY_BUFFER, render_object->getVBO());
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert_center), vert_center);
    }
    else if (pivot_method == PIVOT_TOP_LEFT)
    {
        glBindBuffer(GL_ARRAY_BUFFER, render_object->getVBO());
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert_top_left), vert_top_left);
    }
    else if (pivot_method == PIVOT_TOP_RIGHT)
    {
        glBindBuffer(GL_ARRAY_BUFFER, render_object->getVBO());
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert_top_right), vert_top_right);
    }
    else if (pivot_method == PIVOT_BOTTOM_LEFT)
    {
        glBindBuffer(GL_ARRAY_BUFFER, render_object->getVBO());
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert_bottom_left), vert_bottom_left);
    }
    else if (pivot_method == PIVOT_BOTTOM_RIGHT)
    {
        glBindBuffer(GL_ARRAY_BUFFER, render_object->getVBO());
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert_bottom_right), vert_bottom_right);
    }

    updateModelMatrix(window);
    shader.setMat4("model", model);
}

void widget::BaseWidget::update(Window *window) {}

void widget::BaseWidget::init(Render &render)
{
    render_object = &render;
}

void widget::BaseWidget::setPosition(glm::vec2 pos)
{
    position = pos;
}

void widget::BaseWidget::setRotation(float radians_angle)
{
    rotation = radians_angle;
}

void widget::BaseWidget::setScale(glm::vec2 scale_)
{
    scale = scale_;
}

void widget::BaseWidget::updateModelMatrix(Window& window)
{
    glm::vec2 projected_loc;
    float aspect = (float)window.width / (float)window.height;
    float half_aspect = aspect * 0.5f;

    //position x projetée sur l'intervalle [-half_width ; half_width], par exemple en 16/9 : [-16/18 ; 16/18]
    projected_loc = glm::vec2(maths::mapRange(position.x, 0.0f, 1.0f, -half_aspect, half_aspect), position.y);
    glm::vec3 pos3d{projected_loc, 0.0f};

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos3d);
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1));
    //calculer le scale en fonction du fill method
    glm::mat4 sclFactor;
    if (fill_method == FILL_NONE)
    {
        sclFactor = glm::scale(glm::mat4(1.0f), glm::vec3(scale * default_texture_scale, 1.0f));
    }
    else if (fill_method == FILL_FULL)
    {
        // normaliser les paddings (en pixels → [0,1])
        float x_left = x_padding_left / window.width;
        float x_right = x_padding_right / window.width;
        float y_top = y_padding_top / window.height;
        float y_bottom = y_padding_bottom / window.height;

        // appliquer offset si paddings asymétriques
        float x_offset = (x_right - x_left) * half_aspect;
        float y_offset = (y_bottom - y_top) * 0.5f;
        pos3d.x += x_offset;
        pos3d.y += y_offset;
        translation = glm::translate(glm::mat4(1.0f), pos3d);

        // calcul du scale
        float width_scale  = (1.0f - x_left - x_right) * aspect;
        float height_scale = (1.0f - y_top - y_bottom);

        sclFactor = glm::scale(glm::mat4(1.0f),
            glm::vec3((scale * default_texture_scale).x * width_scale,
                      (scale * default_texture_scale).y * height_scale,
                      1.0f));
    }

    model = translation * rot * sclFactor;
}

NAMEPSACE_AH2D_END;