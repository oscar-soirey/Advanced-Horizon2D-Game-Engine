#pragma once

#include <charconv>
#include <iostream>
#include <variant>
#include <map>
#include <vector>

#include <string>
#include <functional>
#include <memory>

#include <utils/log.h>
#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

//enum access
enum class Access {
    Private,
    Exposed,
    Editable,
    None
};

inline std::string accessToString(Access a)
{
    switch(a)
    {
        case Access::Private:
            return "Private";
        case Access::Exposed:
            return "Exposed";
        case Access::Editable:
            return "Editable";
        case Access::None:
            return "None";
    }
    return "None";
}

using collision_channel = uint16_t;

//structure HProperty pour property et access
using PropertyVariantType = std::variant<int, float, bool, std::string, glm::vec2, glm::vec3, glm::vec4, unsigned int, short, std::byte, reactphysics3d::Vector2, reactphysics3d::Vector3, collision_channel>;
using PropertyVariantPtr = std::variant<int*, float*, bool*, std::string*, glm::vec2*, glm::vec3*, glm::vec4*, unsigned int*, short*, std::byte*, reactphysics3d::Vector2*, reactphysics3d::Vector3*, collision_channel*>;

struct HProperty {
    PropertyVariantPtr property_member{};
    Access access{Access::None};
};

PropertyVariantType getValueByString(const std::string& value);

class Reflectable {
public:
    std::string objectID;

    Reflectable();

    virtual ~Reflectable() = default;

    std::map<std::string, HProperty> properties;

    template<typename T>
    T getPropertyValue(const std::string &name)
    {
        if (auto p = std::get_if<T*>(&properties[name].property_member))
        {
            return **p;
        }
        else
        {
            LOG_ERROR("getPropertyValue, Error with property : " + name);
            return T{};
        }
    }

    Access getPropertyAccess(const std::string& name);

    template<typename T>
    void setPropertyValue(const std::string &name, T value)
    {
        if (auto p = std::get_if<T*>(&properties[name].property_member))
        {
            **p = value;
        }
        else
        {
            LOG_ERROR("setPropertyValue, Error with property : " + name);
        }
    }

    bool propertyExists(const std::string& name);
};


//GLOBAL FACTORY
inline std::map<std::string, std::function<std::unique_ptr<Reflectable>()>>& GlobalFactory()
{
    static std::map<std::string, std::function<std::unique_ptr<Reflectable>()>> factory;
    return factory;
}

//MACROS D'ENREGISTREMENT
//enregister une class
#define HCLASS(ClassName) \
namespace { \
struct Registrar_##ClassName { \
Registrar_##ClassName() { \
GlobalFactory()[#ClassName] = []() { \
return std::make_unique<ClassName>(); \
}; \
} \
}; \
static Registrar_##ClassName s_Registrar_##ClassName; \
}

//macro pour ajouter une property
#define HPROPERTY(name, access) properties[#name] = HProperty(&name, access);

NAMEPSACE_AH2D_END;