#pragma once
#include <memory>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

#include <components/base_component.h>
#include <core/reflection.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Render;
class Window;

class Actor : public Reflectable {
public:
    glm::vec3 actorLocation;
    float actorRotation;
    glm::vec2 actorScaleFactor;

    Actor();
    ~Actor() override;

    virtual void init(Render& render);

    virtual void update(Window& window);
    virtual void processInput(Window& window);

    //components
    template<typename T, typename... Args>
    bool addComponent(const std::string& id, Args &&... args)
    {
        if (components.contains(id))
            return false;

        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        comp->setParent(this);
        //init sert a appeler une fonction ou le parent est initialisé (le constructeur sert surtout à passer des arguments pour les utiliser dans le init)
        comp->init();
        components[id] = std::move(comp);
        return true;
    }

    [[nodiscard]] const std::map<std::string, std::unique_ptr<BaseComponent>>& getComponents() const;
    BaseComponent* getComponentByID(const std::string& id);

    //transformations
    void setActorLocation(glm::vec3 loc, bool resetVelocity = true);
    void setActorRotation(float radiansAngle);
    void setActorScale(glm::vec2 scale);
    glm::vec3 getActorLocation();
    [[nodiscard]] float getActorRotation() const;
    glm::vec2 getActorScale();

    void possess();
    void unpossess();

    float xVelocity = 0.0f;
    float yVelocity = 0.0f;

    Render* getRenderObject();
    reactphysics3d::RigidBody* getRigidBody();

    float getXAcceleration();
    float getYAcceleration();

protected:
    bool possessed = false;

private:
    std::map<std::string, std::unique_ptr<BaseComponent>> components;
    Render* renderObject = nullptr;

    reactphysics3d::RigidBody* rigid_body = nullptr;

    float xVelocityLastFrame = 0.0f;
    float yVelocityLastFrame = 0.0f;

    float xAcceleration = 0.0f;
    float yAcceleration = 0.0f;
};

NAMEPSACE_AH2D_END;