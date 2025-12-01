//----- fonctionnement des widgets -----//
//widget -> seulement 1 objet (genre un bouton, un texte, et autres) (ecris en cpp)
//widget group -> plusieurs widgets(ecris en xml .wg)

#pragma once

#include <core/reflection.h>
#include <glm/glm.hpp>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

typedef int fill_method_t;
#define FILL_NONE  0
#define FILL_LEFT  1
#define FILL_RIGHT 2
#define FILL_FULL  3

typedef int pivot_t;
#define PIVOT_CENTER       0
#define PIVOT_TOP_LEFT     1
#define PIVOT_TOP_RIGHT    2
#define PIVOT_BOTTOM_LEFT  3
#define PIVOT_BOTTOM_RIGHT 4

class Window;
class Render;
class Shader;

namespace widget
{
    class BaseWidget : public Reflectable {
    public:
        glm::vec2 position;
        float rotation;
        glm::vec2 scale;
        bool visible = true;

        pivot_t pivot_method = PIVOT_CENTER;

        fill_method_t fill_method = FILL_NONE;
        //garder que les glm vec2
        glm::vec2 padding_x{0.0f};
        glm::vec2 padding_y{0.0f};
        float x_padding_left;
        float x_padding_right;
        float y_padding_top;
        float y_padding_bottom;

        BaseWidget();

        virtual void init(Render& render);

        virtual void update(Window* window);
        virtual void draw(Shader& shader, Window& window);

        virtual void setPosition(glm::vec2 pos);
        virtual void setRotation(float radians_angle);
        virtual void setScale(glm::vec2 scale_);

    protected:
        Render* render_object;

        glm::vec2 default_texture_scale{1.0f};

    private:
        glm::mat4 model;
        void updateModelMatrix(Window& window);
    };
}

NAMEPSACE_AH2D_END;