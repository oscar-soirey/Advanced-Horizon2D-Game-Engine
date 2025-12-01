#pragma once

#include "widget_image.h"
#include <core/reflection.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

namespace widget
{
    class Button : public Image {
    public:
        void update(Window *window) override;

        [[nodiscard]] bool isHovering() const;

        //callbacks
        virtual void onClicked();
        virtual void onHovered();
        virtual void onUnhovered();

    private:
        bool is_hovering = false;
        bool is_mouse_left_pressed = false;
    };
    HCLASS(Button);
}

NAMEPSACE_AH2D_END;