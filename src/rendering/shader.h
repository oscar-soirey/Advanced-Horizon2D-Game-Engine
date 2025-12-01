#pragma once

#include <string>
#include <glm/glm.hpp>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    void use() const;
    void update() const;

    //sert à envoyer différents types de variables au shader, nom est le nom de la variable et value la valeur
    //&name = référence de variable, interdit de la modifier
    //const car la méthode ne modifie aucune variable de l'objet courant
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
};

NAMEPSACE_AH2D_END;