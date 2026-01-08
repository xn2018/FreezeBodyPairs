#include "Config.h"
#include <fstream>
#include "SimpleIni.h"
#include "nlohmann/json.hpp"

#include "shim/ryml_compat_shim.h"
#include <articuno/archives/ryml/ryml.h>

using namespace articuno::ryml;
using json = nlohmann::json;

namespace FreezeBodyPairs {

    Config& Config::GetSingleton() noexcept {
        static Config inst;
        return inst;
    }

    void Config::Load() {
        LoadJSON();
    }

    void Config::LoadJSON() {
        const char* path = "Data/SKSE/Plugins/FreezeBodyPairs.json";
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            logger::info("JSON not found (optional): {}", path);
            return;
        }

        json root = json::parse(ifs, nullptr, false);
        if (root.is_discarded()) {
            logger::warn("Invalid JSON file: {}", path);
            return;
        }

        // keys
        if (root.contains("ToggleKey1")) toggleKey1 = root["ToggleKey1"].get<uint32_t>();

        if (root.contains("ToggleKey2")) toggleKey2 = root["ToggleKey2"].get<uint32_t>();

        // bones
        if (root.contains("FrozenBones") && root["FrozenBones"].is_array()) {
            frozenBones.clear();
            for (auto& b : root["FrozenBones"]) frozenBones.push_back(b.get<std::string>());
        }

        logger::info("Loaded JSON: {} bones, keys({}, {})", frozenBones.size(), toggleKey1, toggleKey2);
    }
}
