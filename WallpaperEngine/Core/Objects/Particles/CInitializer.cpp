#include "CInitializer.h"

#include "Initializers/CLifeTimeRandom.h"
#include "Initializers/CSizeRandom.h"
#include "Initializers/CRotationRandom.h"
#include "Initializers/CVelocityRandom.h"
#include "Initializers/CColorRandom.h"
#include "Initializers/CAlphaRandom.h"
#include "Initializers/CAngularVelocityRandom.h"

using namespace WallpaperEngine::Core::Objects::Particles;

CInitializer* CInitializer::fromJSON (json data)
{
    json::const_iterator id_it = data.find ("id");
    json::const_iterator name_it = data.find ("name");
    irr::u32 id = ((id_it == data.end ()) ? 0 : (irr::u32) (*id_it));

    if (name_it == data.end ())
    {
        throw std::runtime_error ("Particle's initializer must have a name");
    }

    if (*name_it == "lifetimerandom")
    {
        return Initializers::CLifeTimeRandom::fromJSON (data, id);
    }
    else if (*name_it == "sizerandom")
    {
        return Initializers::CSizeRandom::fromJSON (data, id);
    }
    else if (*name_it == "rotationrandom")
    {
        return Initializers::CRotationRandom::fromJSON (data, id);
    }
    else if (*name_it == "velocityrandom")
    {
        return Initializers::CVelocityRandom::fromJSON (data, id);
    }
    else if (*name_it == "colorrandom")
    {
        return Initializers::CColorRandom::fromJSON (data, id);
    }
    else if (*name_it == "alpharandom")
    {
        return Initializers::CAlphaRandom::fromJSON (data, id);
    }
    else if (*name_it == "angularvelocityrandom")
    {
        return Initializers::CAngularVelocityRandom::fromJSON (data, id);
    }
    else
    {
        throw std::runtime_error ("Particle's got an unknown initializer");
    }
}


CInitializer::CInitializer (irr::u32 id, std::string name) :
    m_id (id),
    m_name (std::move(name))
{
}


std::string& CInitializer::getName ()
{
    return this->m_name;
}

irr::u32 CInitializer::getId ()
{
    return this->m_id;
}