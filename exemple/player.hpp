#pragma once

#include "test_emitter.hpp"
#include <ah2d/character.h>
#include <ah2d/core/reflection.h>
#include <ah2d/components/light.h>
#include <ah2d/components/skeletal_sprite.h>
#include <ah2d/components/colliders/capsule_collision.h>

#include <ah2d/utils/log.h>
#include <ah2d/core/h2d_namespace.h>

USING_NAMESPACE_AH2D;

inline auto movementValues = Movement(
    1.0f,150.0f, 150.0f, 2.0f, 15.0f, 0.6f, 1
    );

class Player : public Character {
public:
    Player()
    {
        mainSpritePath = "dragon.png";
        zOrder = 0;
        capsule_radius = 10;
        capsule_height = 15;
        capsule_collision_channel = 0xFFFF;
    }

    void init(Render &renderObject) override
    {
        /*----Character basics-----*/
        Character::init(renderObject);
        movement_component->setMovementValues(movementValues);

        addComponent<SkeletalSpriteComponent>("sprite_component", renderObject, "anim/spine-test.atlas", "anim/spine-test.png", "anim/spine-test.skel", zOrder);
        //addComponent<SkeletalSpriteComponent>("sprite_component", renderObject, "anim/speedy-ess.atlas", "anim/speedy-ess.png", "anim/speedy-ess.skel", zOrder);
        //addComponent<SkeletalSpriteComponent>("sprite_component", renderObject, "spineboy-pro.atlas", "spineboy-pro.png", "spineboy-pro.skel", zOrder);
        sprite_component = static_cast<SkeletalSpriteComponent*>(getComponentByID("sprite_component"));

        addComponent<LightComponent>("light0", renderObject, glm::vec3(0), glm::vec3(1.0), 10000);

        capsule_component->getCollider()->setCollisionCategoryBits(PLAYER);
        capsule_component->getCollider()->setCollideWithMaskBits(ALL);
    }
    void processInput(Window &window) override
    {
        float dt = window.getDeltaTime();

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_D) == GLFW_PRESS)
        {
            sprite_component->setRelativeScale(glm::vec2(-1.0, 1.0));
            movement_component->moveX(window.getDeltaTime(), D_RIGHT);
        }
        else if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_A) == GLFW_PRESS)
        {
            sprite_component->setRelativeScale(glm::vec2(1.0));
            movement_component->moveX(window.getDeltaTime(), D_LEFT);
        }
        else
            movement_component->stopMoveX();


        //jump comme Hollow Knight, on peut rester appuyer et des qu'on relache, la vélocity est reinitialisée
        bool isSpacePressed = glfwGetKey(window.getGLFWwindow(), GLFW_KEY_SPACE) == GLFW_PRESS;
        if (isSpacePressed)
        {
            //touche appuyée
            jumping = true;
            movement_component->jump();
        }

        if (!isSpacePressed && spacePressed)
        {
            //touche vient juste d'être relâchée
            yVelocity = 0;
        }

        //mettre à jour l'état pour le prochain frame
        spacePressed = isSpacePressed;

        if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_G) == GLFW_PRESS)
        {
            if (!emitter_spawned)
            {
                auto emitter = static_cast<Emitter*>(getRenderObject()->spawnActorFromClass("TestEmitter"));
                emitter->setActorLocation(getActorLocation());
                emitter->drawDebugCollisions();
                //count++;
                //Log::Debug(std::to_string(count));
                emitter_spawned = false;
            }
        }
        else
            emitter_spawned = false;
    }


private:
    SkeletalSpriteComponent* sprite_component = nullptr;

    bool spacePressed = false;
    bool emitter_spawned = false;

    int count = 0;
};
HCLASS(Player);