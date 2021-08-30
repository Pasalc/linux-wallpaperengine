#pragma once

#include <irrlicht/irrlicht.h>

#include "WallpaperEngine/Core/Objects/Images/CMaterial.h"

#include "WallpaperEngine/Core/Core.h"
#include "WallpaperEngine/Core/CObject.h"

#include "WallpaperEngine/Assets/CContainer.h"

namespace WallpaperEngine::Core::Objects
{
    using json = nlohmann::json;
    using namespace WallpaperEngine::Assets;

    class CImage : public CObject
    {
        friend class CObject;

    public:
        static CObject* fromJSON (
                json data,
                CContainer* container,
                bool visible,
                irr::u32 id,
                std::string name,
                const glm::vec3& origin,
                const glm::vec3& scale,
                const glm::vec3& angles
        );

        const Images::CMaterial* getMaterial () const;
        const glm::vec2& getSize () const;
        const std::string& getAlignment () const;

    protected:
        CImage (
                Images::CMaterial* material,
                bool visible,
                irr::u32 id,
                std::string name,
                const glm::vec3& origin,
                const glm::vec3& scale,
                const glm::vec3& angles,
                const glm::vec2& size,
                std::string alignment
        );

        static const std::string Type;

    private:
        glm::vec2 m_size;
        Images::CMaterial* m_material;
        std::string m_alignment;
    };
};
