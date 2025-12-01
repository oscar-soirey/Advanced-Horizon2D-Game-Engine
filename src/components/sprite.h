#pragma once

#include <string>
#include <glm/glm.hpp>

#include <components/base_component.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Render;
class Window;
class Shader;
class CameraComponent;
class Texture;

struct gl_UV {
    float uMin = 0.0f;
    float uMax = 1.0f;
    float vMin = 0.0f;
    float vMax = 1.0f;
};

class SpriteComponent : public BaseComponent{
public:
    SpriteComponent(Render& renderObject, const std::string& texturePackedPath, float zOrder);
    ~SpriteComponent() override;

    void update(Window& window) override;  //virtual pour permettre le polymorphisme (override de la fonction)
    void draw(Shader& shader, Window& window);  //non executée si le sprite est n'est pas visible

    //camera visibility -> optimisation
    bool isVisibleY(CameraComponent& camera, Window& window);
    bool isVisibleX(CameraComponent& camera, Window& window);

    void setZOrder(float newZOrder);
    void setPivot(glm::vec2 newPivot);

    bool visible = true;
    glm::vec4 tintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    gl_UV uv;
    void bindGLUV();

protected:
    Render& render;
    glm::mat4 model;

    std::string texturePackedPath;
    Texture* activeTex;

    glm::vec2 defaultTextureScale;

    glm::vec2 pivot{0.0f};

    //gere les couches de sprites (plus la valeur est élevée, plus le sprite va s'afficher devant les autres)
    float zOrder;

    //update la variable model qui constitue toutes les données de transformation du sprite (sous forme de matrice 4x4)
    void updateModelMatrix();
};

NAMEPSACE_AH2D_END;