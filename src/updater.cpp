#include "updater.h"
#include <thread>
#include <nlohmann/json.hpp>
#include <curl/curl.h>

#include "logger.h"
#include "utils.h"

namespace updater
{
    void check_update(const char *repo, std::function<void(version_t)> callback)
    {
        std::thread([=]()
                    {
                        std::string url = "https://api.github.com/repos/";
                        url += repo;
                        url += "/releases/latest";

                        auto response = utils::get_request(url.c_str());
                        try
                        {
                            auto json = nlohmann::json::parse(response);

                            auto version = json["tag_name"].get<std::string>();
                            auto download = json["assets"][0]["browser_download_url"].get<std::string>();
                            auto title = json["name"].get<std::string>();
                            auto changelog = json["body"].get<std::string>();
                            callback({version, download, title, changelog});
                        }
                        catch (...)
                        {
                            L_ERROR("Failed to check for updates!");
                        } })
            .detach();
    }

    void install_update(const char *download_url, float *progress)
    {
        std::thread([=]()
                    {
                        std::string path = utils::get_current_directory();
                        path += "\\update.zip";
                        
                        utils::download_file(download_url, path.c_str(), progress);

                        L_INFO("Update installed successfully!"); })
            .detach();
    }
}