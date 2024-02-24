#pragma once

#include <functional>
#include <string>

namespace openhack::web {
    /// @brief Get the contents of a URL.
    /// @param url URL to get the contents from.
    /// @param callback Callback to call when the request is done.
    void get(const std::string &url, const std::function<void(const std::string &)> &callback);

    /// @brief Download a file from the internet.
    /// @param url URL to download from.
    /// @param path Path to save the file to.
    /// @param callback Callback to call when the download is done.
    /// @param progress A pointer to a float that will be updated with the progress.
    void download(const std::string &url, const std::string &path, const std::function<void(bool)> &callback, float* progress = nullptr);
}

namespace openhack::updater {

    /// @brief Metadata about version from GitHub.
    struct Version {
        std::string version;
        std::string downloadUrl;
        std::string title;
        std::string changelog;
    };

    /// @brief Check if a new version is available.
    /// @param callback Callback to call when the check is done.
    void check(const std::function<void(bool, Version)> &callback);

    /// @brief Installs the update.
    /// @param url URL to download the update from.
    /// @param progress A pointer to a float that will be updated with the progress.
    void install(const std::string &url, float *progress = nullptr);
}