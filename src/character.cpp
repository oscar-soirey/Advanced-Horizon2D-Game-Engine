#include "character.h"

#include <minwindef.h>

#include <core/window.h>
#include <components/colliders/capsule_collision.h>
#include <components/line_trace.h>
#include <rendering/renderer.h>
#include <utils/conversions.h>
#include <utils/log.h>
#include <utils/line_trace.h>

using namespace reactphysics3d;

NAMEPSACE_AH2D_START;


//fonction qui sert de macro pour savoir si au moins un des traces d'un coté a hit
bool hasSideHit(const std::map<std::string, LineTraceComponent*>& map)
{
    for (const auto& [height, traceptr] : map)
    {
        if (traceptr->get_hit())
            return true;
    }
    return false;
}

void Character::init(Render& render)
{
    Actor::init(render);

    //on ajoute une structure Movement arbitraire qui pourra être changée dans la classe dérivée de Character
    Movement movement_default(1,100, 100, 6, 15, 0.05, 1);
    addComponent<CharacterMovementComponent>("character_movement", movement_default);
    movement_component = static_cast<CharacterMovementComponent*>(getComponentByID("character_movement"));

    //capsule component
    addComponent<CapsuleCollisionComponent>("capsule_component", capsule_radius, capsule_height);
    capsule_component = static_cast<CapsuleCollisionComponent*>(getComponentByID("capsule_component"));

    //on initialise les valeurs pour une collision de character
    getRigidBody()->setType(BodyType::KINEMATIC);
    capsule_component->getCollider()->getMaterial().setBounciness(0.0f);

    addLineTracesMacro();
}

CharacterMovementComponent* Character::getMovementComponent() const
{
    return movement_component;
}

void Character::update(Window &window)
{
    float dt = window.getDeltaTime();
    if (possessed)
        processInput(window);

    //check de collisions
    //bottom
    //on ne met pas de conditions de verification car peut importe la velocity, on veut verifier le is_falling
    for (const auto& [height, traceptr] : traces_components["bottom"])
        traceptr->cast_ray();

    if (yVelocity > 0)
    {
        //top
        for (const auto& [height, traceptr] : traces_components["top"])
            traceptr->cast_ray();
    }

    if (xVelocity > 0)
    {
        //right
        for (const auto& [height, traceptr] : traces_components["right"])
            traceptr->cast_ray();
    }
    if (xVelocity < 0)
    {
        //left
        for (const auto& [height, traceptr] : traces_components["left"])
            traceptr->cast_ray();
    }

    //on set la variable isFalling en fonction des ray du bottom
    is_falling = (!hasSideHit(traces_components["bottom"]));  //aucun rayon n'a touché

    //appliquer la gravité
    if (isFalling())
    {
        yVelocity -= 200.0f * getMovementComponent()->getMovementValues().gravity_multiplier * dt;
    }
    else  //on touche le sol
    {
        if (!jumping)   //seulement si on n'est pas en train de sauter
            yVelocity = 0;
    }

    //si on tape un mur en hauteur
    //on peut utiliser la valeur de yVelocity car on ne l'a pas encore changée a ce moment la
    if (hasSideHit(traces_components["top"]) && yVelocity > 0)  //un ou plus des rayons ont touchés
    {
        //on la change maintenant
        yVelocity = 0;
    }

    glm::vec3 newLoc = getActorLocation();
    if (hasSideHit(traces_components["bottom"]) && yVelocity < 0)
    {
        for (const auto& [height, comp_ptr] : traces_components["bottom"])
        {
            if (comp_ptr->get_hit())
            {
                //si il y a eu un hit, on replace le personnage directement et on coupe la boucle
                glm::vec3 hitPoint = vector3_to_glmvec3(comp_ptr->getHitInfos().hitPoint);
                newLoc.y = hitPoint.y + capsule_radius + capsule_height/2;
                //setActorLocation(newLoc, false);
                break;
            }
        }
    }

    if (hasSideHit(traces_components["right"]) && xVelocity > 0)
    {
        xVelocity = 0;
        for (const auto& [height, comp_ptr] : traces_components["right"])
        {
            if (comp_ptr->get_hit())
            {
                //si il y a eu un hit, on replace le personnage directement et on coupe la boucle
                glm::vec3 hitPoint = vector3_to_glmvec3(comp_ptr->getHitInfos().hitPoint);
                //setActorLocation(glm::vec3(getActorLocation().x, hitPoint.y, getActorLocation().z));
                newLoc.x = hitPoint.x - capsule_radius/2;
                //setActorLocation(newLoc, false);
                break;
            }
        }
    }

    if (hasSideHit(traces_components["left"]) && xVelocity < 0)
    {
        xVelocity = 0;
        for (const auto& [height, comp_ptr] : traces_components["left"])
        {
            if (comp_ptr->get_hit())
            {
                //si il y a eu un hit, on replace le personnage directement et on coupe la boucle
                glm::vec3 hitPoint = vector3_to_glmvec3(comp_ptr->getHitInfos().hitPoint);
                //setActorLocation(glm::vec3(getActorLocation().x, hitPoint.y, getActorLocation().z));
                newLoc.x = hitPoint.x + capsule_radius/2;
                //setActorLocation(newLoc, false);
                break;
            }
        }
    }

    //jump
    if (yVelocity < 0)
    {
        jumping = false;
    }

    //ajouter la xVelocity et yVelocity à la location
    //getRigidBody()->setLinearVelocity(Vector3(xVelocity, yVelocity, 0));
    newLoc.x += xVelocity * dt;
    newLoc.y += yVelocity * dt;
    setActorLocation(newLoc);

    Actor::update(window);
}

bool Character::isFalling() const
{
    return is_falling;
}

//macro pour ajouter les components de line trace pour la detection de collisions
void Character::addLineTracesMacro()
{
    //distance maximum avant que ca overlap le ray opposé
    //LOG_DEBUG("distance : " + std::to_string(capsule_height/2 + capsule_radius));
    float DISTANCE = 5;
    glm::vec3 origin{0};
    glm::vec3 end;
    glm::vec3 direction;

    //-----> bottom <-----//
    origin.y = -capsule_height/2 + DISTANCE - capsule_radius;
    /*on rajoute 1 pour que la fin du ray corresponde aux pied du personnage (car le ray fait 1 vers le bas),
    /*et on ajoute une hémisphere (non comprise par défaut dans le height par rp3d)*/
    direction = {0, -1, 0};
        //bottom center
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_bottom_center", origin, end, capsule_collision_channel);
        traces_components["bottom"]["center"] = static_cast<LineTraceComponent*>(getComponentByID("lt_bottom_center"));

        //bottom left
        origin.x = -capsule_radius/2;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_bottom_left", origin, end, capsule_collision_channel);
        traces_components["bottom"]["left"] = static_cast<LineTraceComponent*>(getComponentByID("lt_bottom_left"));

        //bottom right
        origin.x = capsule_radius/2;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_bottom_right", origin, end, capsule_collision_channel);
        traces_components["bottom"]["right"] = static_cast<LineTraceComponent*>(getComponentByID("lt_bottom_right"));

    //-----> top <-----//
    origin.y = capsule_height/2 - DISTANCE + capsule_radius;  //on enleve 1 pour que la fin du ray corresponde aux pied du personnage (car le ray fait 1 vers le bas)
    direction = {0, 1, 0};
        //top center
        origin.x = 0;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_top_center", origin, end, capsule_collision_channel);
        traces_components["top"]["center"] = static_cast<LineTraceComponent*>(getComponentByID("lt_top_center"));

        //top left
        origin.x = -capsule_radius/2;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_top_left", origin, end, capsule_collision_channel);
        traces_components["top"]["left"] = static_cast<LineTraceComponent*>(getComponentByID("lt_top_left"));

        //top right
        origin.x = capsule_radius/2;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_top_right", origin, end, capsule_collision_channel);
        traces_components["top"]["right"] = static_cast<LineTraceComponent*>(getComponentByID("lt_top_right"));

    //mettre la distance pour les cotés
    DISTANCE = 2;
    //-----> right <-----//
    origin.x = capsule_radius/2 - DISTANCE;  //on enlève 1 pour que la fin du ray corresponde aux pied du personnage (car le ray fait 1 vers le bas)
    direction = {1, 0, 0};
        //right center
        origin.y = 0;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_right_center", origin, end, capsule_collision_channel);
        traces_components["right"]["center"] = static_cast<LineTraceComponent*>(getComponentByID("lt_right_center"));

        //right top
        origin.y = capsule_height/2 + capsule_radius;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_right_top", origin, end, capsule_collision_channel);
        traces_components["right"]["top"] = static_cast<LineTraceComponent*>(getComponentByID("lt_right_top"));

        //right bottom
        origin.y = -capsule_height/2 - capsule_radius;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_right_bottom", origin, end, capsule_collision_channel);
        traces_components["right"]["bottom"] = static_cast<LineTraceComponent*>(getComponentByID("lt_right_bottom"));

    //-----> left <-----//
    origin.x = -capsule_radius/2 + DISTANCE;  //on rajoute 1 pour que la fin du ray corresponde aux pied du personnage (car le ray fait 1 vers le bas)
    direction = {-1, 0, 0};
        //left center
        origin.y = 0;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_left_center", origin, end, capsule_collision_channel);
        traces_components["left"]["center"] = static_cast<LineTraceComponent*>(getComponentByID("lt_left_center"));

        //left top
        origin.y = capsule_height/2 + capsule_radius;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_left_top", origin, end, capsule_collision_channel);
        traces_components["left"]["top"] = static_cast<LineTraceComponent*>(getComponentByID("lt_left_top"));

        //left bottom
        origin.y = -capsule_height/2 - capsule_radius;
        end = origin + direction*DISTANCE;
        addComponent<LineTraceComponent>("lt_left_bottom", origin, end, capsule_collision_channel);
        traces_components["left"]["bottom"] = static_cast<LineTraceComponent*>(getComponentByID("lt_left_bottom"));

    /*
    addComponent<LineTraceComponent>("lt_foot_right", origin, end, capsule_collision_channel);
    traces_components["foot"]["right"] = static_cast<LineTraceComponent*>(getComponentByID("lt_foot_right"));
    addComponent<LineTraceComponent>("lt_foot_left", origin, end, capsule_collision_channel);
    traces_components["foot"]["left"] = static_cast<LineTraceComponent*>(getComponentByID("lt_foot_left"));*/
}

void Character::drawDebugTraces(bool draw)
{
    for (const auto& [side, map] : traces_components)
    {
        for (const auto& [height, traceptr] : map)
        {
            traceptr->drawDebug(draw);
        }
    }
}

NAMEPSACE_AH2D_END;