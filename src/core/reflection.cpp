#include "reflection.h"
#include <sstream>

#include <utils/log.h>

NAMEPSACE_AH2D_START;

Reflectable::Reflectable()
{
    HPROPERTY(objectID, Access::Editable);
}


Access Reflectable::getPropertyAccess(const std::string& name)
{
    if (properties.find(name) != properties.end())
        return properties[name].access;
    else
    {
        Log::Error("Property " + name + " not found");
        return Access::None;
    }
}

bool Reflectable::propertyExists(const std::string& name)
{
    if (properties.find(name) != properties.end())
        return true;
    else
        return false;
}

PropertyVariantType getValueByString(const std::string& value)
{
    // bool
    if (value == "true") return true;
    if (value == "false") return false;

    // int
    int intVal;
    if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), intVal);
        ec == std::errc() && p == value.data() + value.size())
        return intVal;

    // unsigned int
    unsigned int uintVal;
    if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), uintVal);
        ec == std::errc() && p == value.data() + value.size())
        return uintVal;

    // short
    short shortVal;
    if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), shortVal);
        ec == std::errc() && p == value.data() + value.size())
        return shortVal;

    // float
    float floatVal;
    if (sscanf(value.c_str(), "%f", &floatVal) == 1)
        return floatVal;

    // std::byte (hex format "0xNN")
    if (value.size() > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X'))
    {
        unsigned int tmp;
        if (std::from_chars(value.data() + 2, value.data() + value.size(), tmp, 16).ec == std::errc())
            return std::byte(tmp);
    }

    // glm::vec2, format : "vec2:1.0,2.0"
    if (value.rfind("vec2:", 0) == 0)
    {
        std::string data = value.substr(5); // après "vec2:"
        float x, y;
        char comma;
        std::stringstream ss(data);
        ss >> x >> comma >> y;
        return glm::vec2(x, y);
    }
    // glm::vec3
    if (value.rfind("vec3:", 0) == 0)
    {
        std::string data = value.substr(5);
        float x, y, z;
        char comma;
        std::stringstream ss(data);
        ss >> x >> comma >> y >> comma >> z;
        return glm::vec3(x, y, z);
    }
    // glm::vec4
    if (value.rfind("vec4:", 0) == 0)
    {
        std::string data = value.substr(5);
        float x, y, z, w;
        char comma;
        std::stringstream ss(data);
        ss >> x >> comma >> y >> comma >> z >> comma >> w;
        return glm::vec4(x, y, z, w);
    }

    // uint16_t (pour les flags de collision)
    uint16_t u16Val;
    if (auto [p, ec] = std::from_chars(value.data(), value.data() + value.size(), u16Val);
        ec == std::errc() && p == value.data() + value.size())
    {
        return u16Val;
    }

    /* type pivots pour les widgets, a convertir en enum class
    if (pivot::map.contains(value))
    {
        return pivot::toString(value);
    }*/

    return value;
}

NAMEPSACE_AH2D_END;