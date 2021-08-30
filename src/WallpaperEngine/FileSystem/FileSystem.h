/**
 * @author Alexis Maiquez Murcia <almamu@almamu.com>
 */
#pragma once

#include <string>
#include <vector>
#include <irrlicht/path.h>
#include <nlohmann/json.hpp>

#include "WallpaperEngine/Assets/CContainer.h"

namespace WallpaperEngine::FileSystem
{
    /**
     * Loads a full file into an std::string
     *
     * @param file
     * @return
     */
    std::string loadFullFile (const irr::io::path& file);

    /**
     * Loads a full file into an std::string
     *
     * @param file
     * @return
     */
    std::string loadFullFile (const std::string& file, WallpaperEngine::Assets::CContainer* containers);
}
