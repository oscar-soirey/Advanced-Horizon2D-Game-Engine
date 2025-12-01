#pragma once

#include <memory>
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

#include <rendering/texture.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

struct SubTexture {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 uvMin;
    glm::vec2 uvMax;
};

class TextureAtlas {
public:
    TextureAtlas(const std::string& packedPath, const std::string& atlasDataPackedPath, GLenum format);
    ~TextureAtlas();

    void bind(GLenum textureUnit = GL_TEXTURE0) const;

    // Récupérer les UV de la sous-texture par son nom
    [[nodiscard]] const SubTexture* getSubTexture(const std::string& name) const;

    int getWidth() const;
    int getHeight() const;

    Texture& getTexture();

private:
    std::unique_ptr<Texture> texture;
    int width, height, nrChannels;

    std::unordered_map<std::string, SubTexture> subTextures;

    void loadAtlasData(const std::string& atlasPackedDataPath);
};

NAMEPSACE_AH2D_END;