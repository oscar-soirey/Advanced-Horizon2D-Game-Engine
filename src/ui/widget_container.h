#pragma once

#include "base_widget.h"
#include <rendering/renderer.h>
#include <utils/log.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

namespace widget
{
    class WidgetContainer : public BaseWidget {
    public:
        template <typename T>
        T* addWidget(const std::string& id)
        {
            BaseWidget* w = render_object->addWidgetFromClass<T>(id);
            if (w != nullptr)
            {
                widgets[id] = w;
                return w;
            }
            //erreur si la classe n'est pas dérivée de BaseWidget
            LOG_ERROR("Trying to add widget to widget container but widget is not derived from BaseWidget");
            return nullptr;
        }

        template <typename T>
        T* getWidget(const std::string& id)
        {
            return widgets[id];
        }

        void setPosition(glm::vec2 pos) override;
        void setRotation(float radians_angle) override;
        void setScale(glm::vec2 scale) override;

    private:
        std::map<std::string, BaseWidget*> widgets;
    };
}

NAMEPSACE_AH2D_END;