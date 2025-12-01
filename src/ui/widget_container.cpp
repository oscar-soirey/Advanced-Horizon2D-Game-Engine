#include "widget_container.h"

NAMEPSACE_AH2D_START;

//----- appliquer les transformation a soi meme et a tous les widgets -----//
void widget::WidgetContainer::setPosition(glm::vec2 pos)
{
    BaseWidget::setPosition(pos);
    for (const auto& [id, widget] : widgets)
    {
        widget->setPosition(pos);
    }
}
void widget::WidgetContainer::setRotation(float radians_angle)
{
    BaseWidget::setRotation(radians_angle);
    for (const auto& [id, widget] : widgets)
    {
        widget->setRotation(radians_angle);
    }
}
void widget::WidgetContainer::setScale(glm::vec2 scale)
{
    BaseWidget::setScale(scale);
    for (const auto& [id, widget] : widgets)
    {
        widget->setScale(scale);
    }
}


NAMEPSACE_AH2D_END;