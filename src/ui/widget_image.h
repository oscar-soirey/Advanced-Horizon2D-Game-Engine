#pragma once

#include "base_widget.h"
#include <core/reflection.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Texture;

namespace widget
{
    class Image : public BaseWidget {
    public:
        std::string texture_path = "engine/widget_box.png";
        glm::vec4 tint_color{1.0f};

        Image();

        void init(Render &render) override;
        void draw(Shader &shader, Window& window) override;

    private:
        Texture* texture_object;
    };
    HCLASS(Image);
}

NAMEPSACE_AH2D_END;