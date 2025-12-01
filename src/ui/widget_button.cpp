#include "widget_button.h"

#include <cmath>

#include <core/window.h>
#include <utils/log.h>
#include <core/maths.h>

NAMEPSACE_AH2D_START;

void widget::Button::update(Window *window)
{
    Image::update(window);

    //on calcule le hover et le clique, que si la rotation est 0° ou 180°
    if (rotation == 0 || rotation == M_PI)
    {
        float aspect = (float)window->width / (float)window->height;
        //on divise par l'aspect sur les x car on veut le scale visible
        glm::vec2 full_scale{(scale.x * default_texture_scale.x)/aspect, scale.y * default_texture_scale.y};
        glm::vec2 half_scale{(scale.x * default_texture_scale.x * 0.5)/aspect, scale.y * default_texture_scale.y * 0.5};

        //half scale + la position du widget dans le plan
        glm::vec2 min_scale_coordinates = -full_scale + position;
        glm::vec2 max_scale_coordinates = full_scale + position;

        glm::vec2 half_min_scale_coordinates = -half_scale + position;
        glm::vec2 half_max_scale_coordinates = half_scale + position;

        //calculer la collision entre le curseur de souris et le bouton
        bool x_match = false;
        bool y_match = false;
        if (pivot_method == PIVOT_CENTER)
        {
            x_match = maths::floatInRange(window->getNormalizedCursorPos().x, half_min_scale_coordinates.x, half_max_scale_coordinates.x);
            y_match = maths::floatInRange(window->getNormalizedCursorPos().y, half_min_scale_coordinates.y, half_max_scale_coordinates.y);
        }
        else if (pivot_method == PIVOT_TOP_LEFT)
        {
            x_match = maths::floatInRange(window->getNormalizedCursorPos().x, position.x, max_scale_coordinates.x);
            y_match = maths::floatInRange(window->getNormalizedCursorPos().y, position.y, max_scale_coordinates.y);
        }
        else if (pivot_method == PIVOT_TOP_RIGHT)
        {
            x_match = maths::floatInRange(window->getNormalizedCursorPos().x, min_scale_coordinates.x, position.x);
            y_match = maths::floatInRange(window->getNormalizedCursorPos().y, position.y, max_scale_coordinates.y);
        }
        else if (pivot_method == PIVOT_BOTTOM_LEFT)
        {
            x_match = maths::floatInRange(window->getNormalizedCursorPos().x, position.x, max_scale_coordinates.x);
            y_match = maths::floatInRange(window->getNormalizedCursorPos().y, min_scale_coordinates.y, position.y);
        }
        else if (pivot_method == PIVOT_BOTTOM_RIGHT)
        {
            x_match = maths::floatInRange(window->getNormalizedCursorPos().x, min_scale_coordinates.x, position.x);
            y_match = maths::floatInRange(window->getNormalizedCursorPos().y, min_scale_coordinates.y, position.y);
        }

        if (x_match && y_match)
        {
            //on appelle une seule fois le callback au moment ou la souris arrive sur le bouton
            if (is_hovering == false)
            {
                is_hovering = true;
                tint_color = glm::vec4(0.6, 0.6, 0.6, 1.0f);
                onHovered();
            }
        }
        else
        {
            if (is_hovering == true)
            {
                is_hovering = false;
                tint_color = glm::vec4(1);
                onUnhovered();
            }
        }

        //détecter le clique
        if (glfwGetMouseButton(window->getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (!is_mouse_left_pressed)
            {
                is_mouse_left_pressed = true;
                if (is_hovering)
                {
                    onClicked();
                }
            }
        }
        else
            is_mouse_left_pressed = false;
    }
}

bool widget::Button::isHovering() const
{
    return is_hovering;
}


void widget::Button::onClicked() {}
void widget::Button::onHovered() {}
void widget::Button::onUnhovered() {}

NAMEPSACE_AH2D_END;