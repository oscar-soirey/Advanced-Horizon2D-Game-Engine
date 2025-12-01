#include "skeletal_sprite.h"

#include <string>
#include <random>

#include <core/window.h>
#include <nlohmann/json.hpp>
#include <components/sprite.h>

#include <core/assets_manager.h>
#include <utils/log.h>

#include <spine/spine.h>
#include <vector>
#include <string>

NAMEPSACE_AH2D_START;

//parser pour obtenir toutes les regions d'une texture atlas (non pris en charge par spine)
std::vector<std::string> getRegionNames(const char* atlasData)
{
    std::vector<std::string> names;
    if (!atlasData) return names;

    std::istringstream iss(atlasData);
    std::string line;
    bool readingRegion = false;

    while (std::getline(iss, line))
    {
        // supprime les espaces de début/fin
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty())
            continue;

        // ignore les lignes qui contiennent ":" → propriétés
        if (line.find(':') != std::string::npos)
            continue;

        // ignore les lignes de chemins d'images (optionnel)
        if (line.find(".png") != std::string::npos || line.find(".jpg") != std::string::npos)
            continue;

        // reste = nom de region
        names.push_back(line);
    }

    return names;
}

std::string generateRandomID()
{
    static std::mt19937 rng(std::random_device{}()); // moteur aléatoire
    static std::uniform_int_distribution<unsigned long long> dist(0, 0xFFFFFFFFFFFFFFFFULL);

    unsigned long long value = dist(rng);
    return "DB_" + std::to_string(value);
}

SkeletalSpriteComponent::SkeletalSpriteComponent(Render &renderObject, const std::string& atlasPath, const std::string& imagePath, const std::string& skeletonPath, float zOrder)
{
    std::vector<char> atlasBuffer = AssetsManager::load_asset(atlasPath);
    std::vector<char> skeletonBuffer = AssetsManager::load_asset(skeletonPath);

    //charger l'atlas
    atlas_ = new spine::Atlas(
        atlasBuffer.data(),          // data
        static_cast<int>(atlasBuffer.size()), // length
        "",                          //png images dir, pas utile car pas de texture loader
        nullptr
    );
    //atlas_->flipV();
    //spine::Bone::setYDown(true); // Affects all skeletons

    //loading error
    if (!atlas_ || atlas_->getPages().size() == 0)
    {
        std::cerr << "Atlas not loaded correctly!\n";
        return;
    }

    //trouver toutes les regions de l'atlas et creer les sprites correspondants
    std::vector<std::string> regionNames = getRegionNames(atlasBuffer.data());
    for (const auto& name : regionNames)
    {
        //std::cout << " - " << name << "\n";
        auto* region = atlas_->findRegion(name.c_str());
        if (region)
        {
            auto* sprite = new SpriteComponent(renderObject, imagePath, zOrder);
            //flip les coordonnées v
            sprite->uv = gl_UV(region->u, region->u2, 1-region->v, 1-region->v2);
            subsprites[name] = sprite_part_t(sprite, region->degrees);
        }
    }

    //charger le binary du skeleton
    spine::SkeletonBinary binary(atlas_);
    binary.setScale(0.1f);
    spine::SkeletonData* skeletonData = binary.readSkeletonData(reinterpret_cast<const unsigned char *>(skeletonBuffer.data()), static_cast<int>(skeletonBuffer.size()));
    if (!skeletonData)
    {
        std::cerr << "Erreur chargement squelette : " << binary.getError().buffer() << std::endl;
        return;
    }

    //creer le skeleton
    skeleton_ = new spine::Skeleton(skeletonData);
    skeleton_->setToSetupPose();

    //creer l'etat d'animation
    spine::AnimationStateData* stateData = new spine::AnimationStateData(skeletonData);
    state_ = new spine::AnimationState(stateData);


    //initialiser le draw order
    auto& drawOrder = skeleton_->getDrawOrder();
    for (int i = 0; i < drawOrder.size(); i++)
    {
        auto slot = drawOrder[i];
        if (slot->getAttachment())
        {
            std::string name = slot->getAttachment()->getName().buffer();
            auto* sprite = subsprites[name].sprite;
            //on multiplie i par 0.0001 pour que ca modifie tres peu le zOrder pour éviter le clipping
            sprite->setZOrder(static_cast<float>(i)*0.0001 + zOrder);
        }
    }
}

SkeletalSpriteComponent::~SkeletalSpriteComponent()
{
    delete state_;
    for (auto& [name, sprite] : subsprites)
    {
        delete sprite.sprite;
    }
}


void SkeletalSpriteComponent::update(Window &window)
{
    BaseComponent::update(window);

    //make the skeleton follow the component
    skeleton_->setPosition(getWorldLocation().x, getWorldLocation().y);

    state_->update(window.getDeltaTime());
    state_->apply(*skeleton_);

    skeleton_->update(window.getDeltaTime());
    skeleton_->updateWorldTransform();

    //update de chaque slot => chaque sprite qui constitue le skeletal sprite
    for (int i = 0; i < skeleton_->getSlots().size(); ++i)
    {
        spine::Slot& slot = *skeleton_->getSlots()[i];
        if (slot.getAttachment())
        {
            std::string name = slot.getAttachment()->getName().buffer();
            auto* sprite = subsprites[name].sprite;
            int degrees = subsprites[name].degrees;

            glm::vec2 attachmentRelScale{};
            float attachmentRelRotation{};
            glm::vec2 attachmentRelLoc{};
            glm::vec2 offset{};

            // vérifier si c'est exactement un RegionAttachment
            if (slot.getAttachment()->getRTTI().isExactly(spine::RegionAttachment::rtti))
            {
                spine::RegionAttachment* region = static_cast<spine::RegionAttachment*>(slot.getAttachment());
                // safe : tu peux accéder à region->getX(), getY(), etc.
                attachmentRelScale = glm::vec2(region->getScaleX(), region->getScaleY());
                attachmentRelRotation = glm::radians(region->getRotation());
                attachmentRelLoc = glm::vec2(region->getX(), region->getY());
                offset = glm::vec2(region->getRegionOffsetX(), region->getRegionOffsetY());
                std::cout << region->getHeight() << std::endl;
            }

            //-----Pivot-----//
            sprite->setPivot(offset);

            //-----Scale-----//
            glm::vec2 worldScale = glm::vec2(slot.getBone().getWorldScaleX(), slot.getBone().getWorldScaleY());
            sprite->setRelativeScale(glm::vec2(attachmentRelScale * worldScale));

            //-----Rotation (en radians)-----//
            //calculer la rotation a partir de la matrice 2x2 de transformation du bone
            float a = slot.getBone().getA();
            float c = slot.getBone().getC();
            float worldRotation = atan2(c, a);

            //on ajoute la rotation de base de la region dans l'atlas, et on ajoute aussi la rotation de base de l'image par rapport au bone parent
            sprite->setRelativeRotation(worldRotation + glm::radians(static_cast<float>(degrees)) + attachmentRelRotation);

            //-----Location-----//
            //on ajoute l'offset de base de l'image par rapport au bone parent
            sprite->setRelativeLocation(glm::vec3(slot.getBone().getWorldX() + attachmentRelLoc.x, slot.getBone().getWorldY() + attachmentRelLoc.y, getRelativeLocation().z));

        }
    }
}

spine::AnimationState *SkeletalSpriteComponent::getState() const
{
    return state_;
}

NAMEPSACE_AH2D_END;