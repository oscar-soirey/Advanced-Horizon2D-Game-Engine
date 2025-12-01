#pragma once

#include <core/h2d_namespace.h>
#include <components/base_component.h>
#include <string>
#include <map>
#include <spine/spine.h>
#include <spine/Extension.h>

//NE pas mettre inline a la compilation, et le mettre inline a l'include (je sais pas pourquoi mais ca marche)
namespace spine
{
    //dire a spine qu'on utilise l'extention* par défaut
    SpineExtension *getDefaultExtension()
    {
        return new DefaultSpineExtension();
    }
}

NAMEPSACE_AH2D_START;

class Render;
class Window;
class SpriteComponent;

typedef struct {
    SpriteComponent* sprite;
    //la rotation de base dans l'atlas
    int degrees;
}sprite_part_t;

class SkeletalSpriteComponent : public BaseComponent {
public:
    SkeletalSpriteComponent(Render &renderObject, const std::string& atlasPath, const std::string& imagePath, const std::string& skeletonPath, float zOrder);
    ~SkeletalSpriteComponent() override;

    void update(Window& window) override;

    spine::AnimationState* getState() const;

private:
    float animTimeAccumulator = 0.0f;

    std::string randomUniqueID;
    std::map<std::string, sprite_part_t> subsprites;

    spine::Atlas* atlas_ = nullptr;
    spine::Skeleton* skeleton_ = nullptr;
    spine::AnimationState* state_ = nullptr;
};

NAMEPSACE_AH2D_END;