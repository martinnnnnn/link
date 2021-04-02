#include "string_utils.hpp"

namespace link
{
    void string::replace(std::string& data, const std::string& to_search, const std::string& replace)
    {
        size_t pos = data.find(to_search);

        while (pos != std::string::npos)
        {
            data.replace(pos, to_search.size(), replace);
            pos = data.find(to_search, pos + to_search.size());
        }
    }
}