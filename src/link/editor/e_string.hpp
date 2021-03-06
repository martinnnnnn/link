#pragma once

#include <vector>
#include <string>
#include <assert.h>
#include <json.hpp>

#include "editor.hpp"
#include "link/types.hpp"
#include "link/file_system.hpp"

namespace link
{
    using nlohmann::json;

    struct EString
    {
        EString(const std::string& label, const std::string& value = "");

        std::string label;
        std::string value;
        std::string previous;

        void set(const std::string& new_value);

        void to_json(json& j, bool is_path)
        {
            if (is_path)
            {
                j["value"] = Path::get_path_relative(LINK_DATA_ROOT, value);

            }
            else
            {
                j["value"] = value;
            }
        }

        void from_json(const json& j, bool is_path)
        {
            if (is_path)
            {
                set(std::string(LINK_DATA_ROOT).append(j["value"]));
            }
            else
            {
                set(j.at("value"));
            }

        }

#ifdef LINK_EDITOR_ENABLED
        bool debug_draw();
#else
        inline bool debug_draw() { return false; }
#endif


    };


}

// 09 77 41 03 81
