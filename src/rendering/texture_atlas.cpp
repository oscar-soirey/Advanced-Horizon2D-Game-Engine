#include "texture_atlas.h"

#include <core/assets_manager.h>
#include <nlohmann/json.hpp>
#include <utils/log.h>

NAMEPSACE_AH2D_START;

TextureAtlas::TextureAtlas(const std::string &packedPath, const std::string &atlasDataPackedPath, GLenum format):
    texture(std::make_unique<Texture>(packedPath.c_str(), format)),
    width(texture->getWidth()),
    height(texture->getHeight()),
    nrChannels(0)
{
    Log::Warning("height initialisé");
    loadAtlasData(atlasDataPackedPath);
    Log::Warning("texture atlas créee");
}

TextureAtlas::~TextureAtlas()
{
    texture->~Texture();
}


void TextureAtlas::bind(GLenum textureUnit) const
{
    texture->bind(textureUnit);
}

const SubTexture* TextureAtlas::getSubTexture(const std::string &name) const
{
    auto it = subTextures.find(name);
    if (it != subTextures.end())
        return &it->second;
    return nullptr;
}

int TextureAtlas::getWidth() const
{
    return width;
}

int TextureAtlas::getHeight() const
{
    return height;
}

void TextureAtlas::loadAtlasData(const std::string &atlasPackedDataPath)
{
    auto dataString = AssetsManager::load_asset(atlasPackedDataPath);

    nlohmann::json doc;
    auto data = doc.parse(dataString);

    //DragonBones exporte souvent dans doc["SubTexture"]
    if (!doc.contains("SubTexture"))
    {
        std::cerr << "Atlas JSON ne contient pas de SubTexture\n";
        return;
    }

    for (auto& sub : doc["SubTexture"])
    {
        std::string name = sub["name"].get<std::string>();
        int x = sub["x"].get<int>();
        int y = sub["y"].get<int>();
        int w = sub["width"].get<int>();
        int h = sub["height"].get<int>();

        SubTexture st;
        st.position = glm::vec2(x, y);
        st.size = glm::vec2(w, h);
        st.uvMin = glm::vec2(float(x) / width, float(y) / height);
        st.uvMax = glm::vec2(float(x + w) / width, float(y + h) / height);

        subTextures[name] = st;
    }
}

Texture& TextureAtlas::getTexture()
{
    return *texture;
}

NAMEPSACE_AH2D_END;