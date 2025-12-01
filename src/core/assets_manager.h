#pragma once

#include <physfs/physfs.h>
#include <string>
#include <vector>
#include <core/h2d_namespace.h>

NAMEPSACE_AH2D_START;

namespace AssetsManager
{
    bool init(const char* argv0);
    void deinit();

    std::vector<char> load_asset(const std::string& packedPath);
    PHYSFS_sint64 get_asset_size(const std::string& packedPath);
    bool file_exists(const std::string& packedPath);
    std::vector<std::vector<char>> load_assets_from_pak(std::vector<char> openedPakFile);
    std::vector<std::string> find_assets_from_path(const std::string& packedPath);
}

NAMEPSACE_AH2D_END;