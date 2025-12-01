#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <map>

#include <reactphysics3d/reactphysics3d.h>
#include <glad/glad.h>

#include <rendering/shader.h>
#include <components/sprite.h>
#include <actor.h>
#include <core/maths.h>
#include <ui/base_widget.h>
#include <rendering/texture.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class LightComponent;
class AnimMap;
class Window;
class CameraComponent;

struct UIScreen {
    std::vector<widget::BaseWidget*> widgets_ptr;
};

class Render {
public:
    std::vector<SpriteComponent*> sprites;
    std::map<std::string, std::unique_ptr<Texture>> texturesCache;
    std::vector<LightComponent*> lights;

    Render();
    ~Render();
    void update(Window& window, glm::vec4 backgroundColor = glm::vec4(0.01f));

    //LIGHTS
    void addLight(LightComponent* newLight);
    void delLight(LightComponent* oldLight);

    void useCamera(CameraComponent* camera);

    Shader getShader();
    [[nodiscard]] GLuint getVBO() const;

    //LEVEL
    bool loadLevel(const std::string& levelPackedPath);
    const std::string& getCurrentLevelName();

    //REACTPHYSICS3D
    [[nodiscard]] reactphysics3d::PhysicsCommon* getPhysicsCommon();
    [[nodiscard]] reactphysics3d::PhysicsWorld* getPhysicsWorld() const;


    //GESTIONS ACTORS
    Actor* construct(const std::string& instanceID, const std::string& className, bool init = true);
    [[nodiscard]] Actor* getActorByID(const std::string& ID) const;
    Actor* spawnActorFromClass(const std::string& className);

    void destroyActorFromPointer(Actor* actorptr);
    void destroyActorFromID(const std::string& id);

    void removeSprite(SpriteComponent* spriteptr);

    //WIDGETS
    template <typename T>
    T* addWidgetFromClass(const std::string& id)
    {
        //on vérifie que le widget ajouté hérite bien de BaseWidget
        if (std::derived_from<T, widget::BaseWidget>)
        {
            auto widget = std::make_unique<T>();
            T* ptr = widget.get();
            widgets[id] = std::move(widget);
            return ptr;
        }
        return nullptr;
    }
    widget::BaseWidget* constructWidget(const std::string& instanceID, const std::string& className, bool init = true);
    bool loadUIScreen(const std::string& packedPath);

private:
    std::map<std::string, std::unique_ptr<Actor>> actors;

    //pour éviter de supprimer pendant qu'on itère sur les actors (dangereux)
    std::vector<std::string> actorsToRemove;

    //initialiser les variables qui vont etre utiles pour le rendu
    GLuint VAO, VBO, EBO;

    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> ui_shader;

    CameraComponent* cameraInUse = nullptr;

    //light
    int lightNum = 0;

    std::string currentLevelName;

    //UI
    std::map<std::string, std::unique_ptr<widget::BaseWidget>> widgets;
    std::vector<UIScreen> ui_screens;
};

NAMEPSACE_AH2D_END;