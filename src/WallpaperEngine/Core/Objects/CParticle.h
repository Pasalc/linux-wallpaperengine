#pragma once

#include <irrlicht/irrlicht.h>

#include "WallpaperEngine/Core/Objects/Particles/CControlPoint.h"
#include "WallpaperEngine/Core/Objects/Particles/CEmitter.h"
#include "WallpaperEngine/Core/Objects/Particles/CInitializer.h"

#include "WallpaperEngine/Core/Core.h"
#include "WallpaperEngine/Core/CObject.h"

namespace WallpaperEngine::Core::Objects
{
    using json = nlohmann::json;

    class CParticle : public CObject
    {
        friend class CObject;

    public:
        static CParticle* fromFile (
                const std::string& filename,
                CContainer* container,
                irr::u32 id,
                std::string name,
                const glm::vec3& origin,
                const glm::vec3& scale
        );

        const std::vector<Particles::CEmitter*>& getEmitters () const;
        const std::vector<Particles::CControlPoint*>& getControlPoints () const;
        const std::vector<Particles::CInitializer*>& getInitializers () const;

    protected:
        CParticle (
            irr::u32 starttime,
            irr::u32 maxcount,
            irr::u32 id,
            std::string name,
            const glm::vec3& origin,
            const glm::vec3& scale
        );
        void insertControlPoint (Particles::CControlPoint* controlpoint);
        void insertEmitter (Particles::CEmitter* emitter);
        void insertInitializer (Particles::CInitializer* initializer);

        static const std::string Type;
    private:
        irr::u32 m_starttime;
        irr::u32 m_maxcount;
        std::vector<Particles::CControlPoint*> m_controlpoints;
        std::vector<Particles::CEmitter*> m_emitters;
        std::vector<Particles::CInitializer*> m_initializers;
    };
};
