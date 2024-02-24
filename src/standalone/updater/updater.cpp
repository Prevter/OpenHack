#include "updater.hpp"
#include <curl.h>
#include <nlohmann/json.hpp>
#include <thread>
#include "../pch.hpp"

namespace openhack::web {

    bool curl_initialized = false;

    size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *output) {
        output->append((char *) contents, size * nmemb);
        return size * nmemb;
    }

    void get(const std::string &url, const std::function<void(const std::string &)> &callback) {
        if (!curl_initialized) {
            curl_global_init(CURL_GLOBAL_ALL);
            curl_initialized = true;
        }

        std::thread([url, callback]() {
            CURL *curl;
            CURLcode res;
            std::string readBuffer;

            curl = curl_easy_init();
            if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
                curl_easy_setopt(curl, CURLOPT_USERAGENT, "openhack");
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    L_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));
                }

                curl_easy_cleanup(curl);
            }

            callback(readBuffer);
        }).detach();
    }

    struct progress_data {
        float *progress;
        CURL *curl;
        std::ofstream *file;
    };

    size_t write_callback_progress(void *contents, size_t size, size_t nmemb, progress_data *data) {
        data->file->write((char *) contents, size * nmemb);
        if (data->progress != nullptr) {
            double dltotal = 0;
            double dlnow = 0;
            curl_easy_getinfo(data->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &dltotal);
            curl_easy_getinfo(data->curl, CURLINFO_SIZE_DOWNLOAD, &dlnow);
            *data->progress = (float) (dlnow / dltotal);
        }
        return size * nmemb;
    }

    void download(const std::string &url, const std::string &path, const std::function<void(bool)> &callback,
                  float *progress) {
        if (!curl_initialized) {
            curl_global_init(CURL_GLOBAL_ALL);
            curl_initialized = true;
        }

        std::thread([url, path, callback, progress]() {
            CURL *curl;
            CURLcode res;
            std::ofstream file(path, std::ios::binary);

            curl = curl_easy_init();
            if (curl) {
                progress_data data{};
                data.progress = progress;
                data.curl = curl;
                data.file = &file;

                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_progress);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
                curl_easy_setopt(curl, CURLOPT_USERAGENT, "openhack");
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
                curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                res = curl_easy_perform(curl);
                if (res != CURLE_OK) {
                    L_ERROR("curl_easy_perform() failed: {}", curl_easy_strerror(res));
                    curl_easy_cleanup(curl);
                    file.close();
                    callback(false);
                }

                curl_easy_cleanup(curl);
            }

            file.close();

            callback(true);
        }).detach();
    }
}

namespace openhack::updater {

    void check(const std::function<void(bool, Version)> &callback) {
        web::get("https://api.github.com/repos/prevter/gdopenhack/releases/latest",
                 [callback](const std::string &response) {
                     try {
                         nlohmann::json json = nlohmann::json::parse(response);
                         auto version = json["tag_name"].get<std::string>();
                         auto assets = json["assets"];

                         // check every asset for the .zip file
                         // this is to make sure we don't download .geode builds
                         for (auto &asset: assets) {
                             auto name = asset["browser_download_url"].get<std::string>();
                             if (name.find(".zip") != std::string::npos) {
                                 auto download = asset["browser_download_url"].get<std::string>();
                                 auto title = json["name"].get<std::string>();
                                 auto changelog = json["body"].get<std::string>();
                                 callback(true, {version, download, title, changelog});
                                 return;
                             }
                         }
                     } catch (const std::exception &e) {
                         callback(false, {});
                     }
                 });
    }

    void extractZip(const char *zipPath, const char *outputPath) {
        std::string command = "powershell -Command \"Expand-Archive -Path '";
        command += zipPath;
        command += "' -DestinationPath '";
        command += outputPath;
        command += "' -Force\"";
        system(command.c_str());
    }

    void install(const std::string &url, float *progress) {
        web::download(url, "update.zip", [](bool success) {
            if (!success) {
                return;
            }

            // delete old dll if exists
            std::remove("xinput9_1_0.old");

            // rename "xinput9_1_0.dll" to "xinput9_1_0.old"
            std::string oldPath = utils::getCurrentDirectory() + "\\xinput9_1_0.old";
            std::rename("xinput9_1_0.dll", oldPath.c_str());

            // extract the zip
            extractZip("update.zip", ".");

            // delete the zip
            std::remove("update.zip");

            // quit the game
            std::exit(0);

        }, progress);
    }
}