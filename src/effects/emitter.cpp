#include "emitter.h"
#include <random>

#include <reactphysics3d/reactphysics3d.h>

#include <components/sprite.h>
#include <core/window.h>
#include <utils/log.h>
#include <utils/conversions.h>
#include <core/maths.h>
#include <actor.h>

using namespace reactphysics3d;

NAMEPSACE_AH2D_START;

//<----- EmitterParticle component ----->//
//constructeur, initialiser les valeurs
EmitterParticle::EmitterParticle(Render& render, const std::string& sprite_packed_path, const std::string& collision_shape, float gravity_scale, int zOrder, Vector3 box_extent, float sphere_radius, collision_channel particle_channel, collision_channel collide_with_channel)
: render(render), sprite_packed_path(sprite_packed_path), collision_shape(collision_shape), gravity_scale(gravity_scale), zOrder(zOrder), box_extent(box_extent), sphere_radius(sphere_radius), particle_channel(particle_channel), collide_with_channel(collide_with_channel) {}

//destructeur : on destroy le rigid body
EmitterParticle::~EmitterParticle()
{
    //détruire les shapes si elles existent puis détruire le rigid body
    if (box_shape)
        render.getPhysicsCommon()->destroyBoxShape(box_shape);
    if (sphere_shape)
        render.getPhysicsCommon()->destroySphereShape(sphere_shape);

    if (rigid_body)
        render.getPhysicsWorld()->destroyRigidBody(rigid_body);
}

void EmitterParticle::init()
{
    //créer et instancier le sprite component
    sprite_component = std::make_unique<SpriteComponent>(render, sprite_packed_path, zOrder);
    //debug collision
    if (collision_shape == "box")
    {
        debug_sprite_component = std::make_unique<SpriteComponent>(render, "engine/box_collision.png", 1000);
        debug_sprite_component->setRelativeScale(glm::vec2(box_extent.x/10, box_extent.y/10));
        debug_sprite_component->visible = false;
    }
    if (collision_shape == "sphere")
    {
        debug_sprite_component = std::make_unique<SpriteComponent>(render, "engine/sphere_collision.png", 1000);
        debug_sprite_component->setRelativeScale(glm::vec2(sphere_radius/10, sphere_radius/10));
        debug_sprite_component->visible = false;
    }

    //créer le rigid body
    Vector3 loc = glmvec3_to_vector3(getRelativeLocation());
    rigid_body = render.getPhysicsWorld()->createRigidBody(Transform(loc, Quaternion::identity()));
    rigid_body->setLinearLockAxisFactor(Vector3(1,1,0));

    //si le collision shape est cube ou sphere
    if (collision_shape == "box" || collision_shape == "sphere")
    {
        rigid_body->setType(BodyType::DYNAMIC);
    }

    Log::Debug("add shape collision");
    //ajouter la shape de collision
    if (collision_shape == "box")
    {
        box_shape = render.getPhysicsCommon()->createBoxShape(box_extent);
        collider = rigid_body->addCollider(box_shape, Transform::identity());
        collider->setCollisionCategoryBits(particle_channel);
        collider->setCollideWithMaskBits(collide_with_channel);
    }
    if (collision_shape == "sphere")
    {
        Log::Debug("sphere collider");
        sphere_shape = render.getPhysicsCommon()->createSphereShape(sphere_radius);
        collider = rigid_body->addCollider(sphere_shape, Transform::identity());
        collider->setCollisionCategoryBits(particle_channel);
        collider->setCollideWithMaskBits(collide_with_channel);
    }
    Log::Debug("add shape collision completed");

}

void EmitterParticle::update(Window &window)
{
    //on update la location en fonction de la location du rigid body
    glm::vec3 loc = vector3_to_glmvec3(rigid_body->getTransform().getPosition());
    relativeLoc = loc;
    //transform du sprite component
    sprite_component->setRelativeLocation(loc + parent->getActorLocation());
    if (collision_shape == "box" || collision_shape == "sphere")
        debug_sprite_component->setRelativeLocation(loc + parent->getActorLocation());
    //rotation----->
}

RigidBody* EmitterParticle::getRigidBody()
{
    return rigid_body;
}

void EmitterParticle::setRelativeLocation(glm::vec3 newLoc)
{
    rigid_body->setLinearVelocity(Vector3(0,0,0));  //on stoppe le mouvement avant de déplacer le rigid body
    //ici on passe un world location car le rigid body attend une loc absolue
    rigid_body->setTransform(Transform(Vector3(glmvec3_to_vector3(newLoc + parent->getActorLocation())), rigid_body->getTransform().getOrientation()));

    BaseComponent::setRelativeLocation(newLoc);
}

void EmitterParticle::setRelativeRotation(float newRotation)
{
    BaseComponent::setRelativeRotation(newRotation);
}

void EmitterParticle::setRelativeScale(glm::vec2 scale)
{
    BaseComponent::setRelativeScale(scale);
}

void EmitterParticle::drawDebugCollisions(bool draw)
{
    debug_sprite_component->visible = draw;
}

//<----- Emitter actor ----->//
Emitter::Emitter()
{
    //exposer toutes les valeurs de l'emitter
    HPROPERTY(sprite_packed_path, Access::Exposed);
    HPROPERTY(lifespan, Access::Exposed);
    HPROPERTY(particles_num, Access::Exposed);
    HPROPERTY(zOrder, Access::Exposed);

    HPROPERTY(collision_shape, Access::Exposed);
    HPROPERTY(box_collision_extent, Access::Exposed);
    HPROPERTY(sphere_collision_radius, Access::Exposed);
    HPROPERTY(particle_channel, Access::Exposed);
    HPROPERTY(collide_with_channel, Access::Exposed);

    HPROPERTY(relative_default_loc, Access::Exposed);
    HPROPERTY(relative_min_rot, Access::Exposed);
    HPROPERTY(relative_max_rot, Access::Exposed);
    HPROPERTY(relative_min_scale, Access::Exposed);
    HPROPERTY(relative_max_scale, Access::Exposed);

    HPROPERTY(min_velocity, Access::Exposed);
    HPROPERTY(max_velocity, Access::Exposed);
    HPROPERTY(min_velocity_direction, Access::Exposed);
    HPROPERTY(max_velocity_direction, Access::Exposed);
    HPROPERTY(gravity_scale, Access::Exposed);
}

void destroyEmitter(Render* render, Actor* actorptr)
{
    render->destroyActorFromPointer(actorptr);
}

void Emitter::init(Render &render)
{
    Actor::init(render);
    for (int i = 0; i < particles_num; i++)
    {
        //pour le nombre de particules, on ajoute un component EmitterParticle et on lui donne comme id, un random number + son index (pour éviter les copies d'id)
        //on pourrait generer un  random id pour toute la boucle et rajouter la valeur de i au nom pour chaque itération -> moins couteux et moins de risques de collisions de noms
        std::string id = maths::generateRandomID("e_particle_") + std::to_string(i);
        if (addComponent<EmitterParticle>(id, render, sprite_packed_path, collision_shape, gravity_scale, zOrder, box_collision_extent, sphere_collision_radius, particle_channel, collide_with_channel))
        {
            auto* particle = static_cast<EmitterParticle*>(getComponentByID(id));
            particles.push_back(particle);

            //location
            particle->setRelativeLocation(relative_default_loc);

            //random rotation in range
            particle->setRelativeRotation(maths::randomFloatInRange(relative_min_rot, relative_max_rot));

            //random scale in range
            float xScale = maths::randomFloatInRange(relative_min_scale.x, relative_max_scale.x);
            float yScale = maths::randomFloatInRange(relative_min_scale.y, relative_max_scale.y);
            particle->setRelativeScale(glm::vec2(xScale, yScale));

            //appliquer la velocity
            float speed = maths::randomFloatInRange(min_velocity, max_velocity);
            glm::vec3 direction{
                maths::randomFloatInRange(min_velocity_direction.x, max_velocity_direction.x),
                maths::randomFloatInRange(min_velocity_direction.y, max_velocity_direction.y),
                0.0f
            };
            direction = glm::normalize(direction);
            particle->getRigidBody()->setLinearVelocity(Vector3(direction.x * speed, direction.y * speed, 0.0f));
        }
        else
        {
            Log::Error("add component failed !");
        }
        //lancer un timer de la durée du lifespan, quand il est terminé, on supprime l'emitter
        life_timer = Timer(lifespan, [this]()
        {
            destroyEmitter(getRenderObject(), this);
        });
    }
}

void Emitter::update(Window &window)
{
    Actor::update(window);
    life_timer.update(window.getDeltaTime());
}

void Emitter::drawDebugCollisions(bool draw)
{
    if (collision_shape == "box" || collision_shape == "sphere")
    {
        for (auto particle : particles)
        {
            if (particle)
            {
                particle->drawDebugCollisions(draw);
            }
        }
    }
    else
        Log::Warning("Cannot draw debug collision if particle doesn't have collision type");
}

NAMEPSACE_AH2D_END;