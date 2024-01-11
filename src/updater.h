#pragma once
#include <functional>
#include <string>

namespace updater
{

    struct version_t
    {
        std::string version;
        std::string download;
        std::string title;
        std::string changelog;
    };

    void check_update(const char *repo, std::function<void(version_t)> callback);

}