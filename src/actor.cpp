#include "actor.h"
#include <core/reflection.h>
#include <rendering/renderer.h>
#include <utils/log.h>
#include <core/window.h>
#include <utils/conversions.h>

NAMEPSACE_AH2D_START;

using namespace reactphysics3d;

Actor::Actor()
{
    HPROPERTY(actorLocation, Access::Exposed);
    HPROPERTY(actorRotation, Access::Exposed);
    HPROPERTY(actorScaleFactor, Access::Exposed);
}

Actor::~Actor()
{
#ifdef LOG_INFOS_FULL
    Log::Debug("Actor destroy");
#endif
    components.clear();
    renderObject->getPhysicsWorld()->destroyRigidBody(rigid_body);
}

//(le init est appellé apres l'initialisation du transform (on peut donc utiliser le transform dans cette methode))
void Actor::init(Render &render)
{
    renderObject = &render;

    Vector3 loc = glmvec3_to_vector3(getActorLocation());
    Quaternion rpRot(0, 0, std::sin(getActorRotation()/2.0f), std::cos(getActorRotation()/2.0f));
    rigid_body = render.getPhysicsWorld()->createRigidBody(Transform(loc, rpRot));
    rigid_body->setLinearLockAxisFactor(Vector3(1,1,0));
    rigid_body->setAngularLockAxisFactor(Vector3(0,0,1));
}

void Actor::update(Window& window)
{
    bool transform_valid = getRigidBody()->getTransform().isValid();
    if (transform_valid)
    {
        //on update la location en fonction de la location du rigid body
        glm::vec3 loc = vector3_to_glmvec3(rigid_body->getTransform().getPosition());
        actorLocation = loc;

        //on update la rotation en fonction de la rotation
        actorRotation = quaternion_to_float(getRigidBody()->getTransform().getOrientation()); //angle en radians, prevents from invalid value
    }
    else
        Log::Error("invalid transform");

    for (const auto& [id, unique_ptr] : components)
    {
        //call update pour chaque components
        unique_ptr->update(window);
    }

    //on calcule l'accélération et on la normalise (pour que droite et gauche soit la meme, accellere: a positive, ralentit: a négative)
    xAcceleration = (xVelocity - xVelocityLastFrame)/window.getDeltaTime();
    if (xVelocity < 0)
        xAcceleration = -xAcceleration;
    yAcceleration = (yVelocity - yVelocityLastFrame)/window.getDeltaTime();
    if (yVelocity > 0)
        yAcceleration = -yAcceleration;

    xVelocityLastFrame = xVelocity;
    yVelocityLastFrame = yVelocity;
}

void Actor::processInput(Window &window) {}


const std::map<std::string, std::unique_ptr<BaseComponent>>& Actor::getComponents() const
{
    return components;
}

BaseComponent *Actor::getComponentByID(const std::string& id)
{
    return components[id].get();
}

//-----transformations-----
glm::vec3 Actor::getActorLocation()
{
    return actorLocation;
}

float Actor::getActorRotation() const
{
    return actorRotation;
}

glm::vec2 Actor::getActorScale()
{
    return actorScaleFactor;
}

void Actor::setActorLocation(glm::vec3 loc, bool resetVelocity)
{
    if (resetVelocity)
        rigid_body->setLinearVelocity(Vector3(0,0,0));  //on stoppe le mouvement avant de déplacer le rigid body
    getRigidBody()->setTransform(Transform(Vector3(glmvec3_to_vector3(loc)), getRigidBody()->getTransform().getOrientation()));
    actorLocation = loc;
}

void Actor::setActorRotation(float radiansAngle)
{
    Vector3 pos = getRigidBody()->getTransform().getPosition();

    getRigidBody()->setTransform(Transform(pos, float_to_quaternion(radiansAngle)));
    actorRotation = radiansAngle;
}

void Actor::setActorScale(glm::vec2 scale)
{
    actorScaleFactor = scale;
}

void Actor::possess()
{
    possessed = true;
}

void Actor::unpossess()
{
    possessed = false;
}

Render *Actor::getRenderObject()
{
    return renderObject;
}

RigidBody *Actor::getRigidBody()
{
    return rigid_body;
}

float Actor::getXAcceleration()
{
    return xAcceleration;
}

float Actor::getYAcceleration()
{
    return yAcceleration;
}

NAMEPSACE_AH2D_END;