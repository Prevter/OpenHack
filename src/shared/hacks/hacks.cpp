#include "hacks.hpp"
#include "../openhack.hpp"
#include "hack-list.hpp"

namespace openhack::hacks {
    void ToggleComponent::onInit() {
        // Load the initial state from the configuration
        m_enabled = config::get<bool>("hack." + m_id, false);
        if (m_enabled) applyPatch();

        // Add keybinding callback
        menu::keybinds::setKeybindCallback(m_id, [this]() {
            this->m_enabled = !this->m_enabled;
            this->toggled();
        });
    }

    void ToggleComponent::onDraw() {
        if (m_hasWarning)
            ImGui::BeginDisabled();

        bool pressed = gui::checkbox(m_name.c_str(), &m_enabled);

        if (m_hasWarning) {
            ImGui::EndDisabled();
            gui::tooltip("Your game version is not supported, or you have conflicting mods.");
            return;
        } else if (!m_description.empty()) {
            gui::tooltip(m_description.c_str());
        }

        menu::keybinds::addMenuKeybind(m_id, m_name, [this]() {
            this->m_enabled = !this->m_enabled;
            this->toggled();
        });

        if (pressed) {
            toggled();
        }
    }

    bool ToggleComponent::applyPatch(bool enable) {
        if (m_hasWarning)
            return false;

        bool success = true;
        for (auto &opcode: m_opcodes) {
            if (!applyOpcode(opcode, enable))
                success = false;
        }

        return success;
    }

    void ToggleComponent::toggled() {
        applyPatch();
        config::set("hack." + m_id, m_enabled);
    }

    void TextComponent::onDraw() {
        gui::text(m_text.c_str());
    }

    static std::vector<gui::Window> windows;
    static std::vector<Component *> components;
    static std::vector<ToggleComponent *> hacks;

    /// @brief Read an opcode from a JSON object
    gd::sigscan::Opcode readOpcode(const nlohmann::json &opcode) {
        auto addrStr = opcode.at("addr").get<std::string>();
        auto onStr = opcode.at("on").get<std::string>();
        auto offStr = opcode.at("off").get<std::string>();

        std::vector<uint8_t> on = utils::hexToBytes(onStr);
        std::vector<uint8_t> off = utils::hexToBytes(offStr);

        std::string library;
        if (opcode.contains("lib")) {
            library = opcode.at("lib").get<std::string>();
        }

        return {utils::hexToAddr(addrStr), library, off, on};
    }

    using Opcodes = std::vector<gd::sigscan::Opcode>;
    using MaskMap = std::unordered_map<std::string, Opcodes>;
    using PatternMap = std::unordered_map<std::string, MaskMap>;

    /// @brief Used to get cached addresses for patterns and masks
    /// @param id The ID of the cache (usually the window title)
    /// @return The cached addresses
    PatternMap tryGetCache(const std::string &id) {
        auto filePath = utils::getModHacksDirectory() / fmt::format("{}.json.cache", id);
        if (!std::filesystem::exists(filePath)) {
            L_TRACE("Cache file does not exist for {}.", id);
            return {};
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            L_WARN("Failed to open cache file: {}", filePath.string());
            return {};
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        try {
            auto json = nlohmann::json::parse(buffer.str());
            PatternMap map;
            for (const auto &[pattern, masks]: json.items()) {
                MaskMap maskMap;
                for (const auto &[mask, opcodes]: masks.items()) {
                    Opcodes opcodeList;
                    for (const auto &opcode: opcodes) {
                        opcodeList.push_back(readOpcode(opcode));
                    }
                    maskMap[mask] = opcodeList;
                }
                map[pattern] = maskMap;
            }
            return map;
        } catch (const std::exception &e) {
            L_ERROR("Failed to parse file: {}", filePath.string());
            L_ERROR("{}", e.what());
            return {};
        }
    }

    /// @brief Used to search for a pattern and mask in the cache
    /// @param pattern The pattern to search for
    /// @param mask The mask to search for
    /// @param map The map to search in
    /// @param opcodes The opcodes to fill if found
    /// @return Whether the pattern and mask were found in the cache
    bool findInCache(const std::string &pattern, const std::string &mask, const PatternMap &map, Opcodes &opcodes) {
        auto it = map.find(pattern);
        if (it == map.end()) {
            return false;
        }

        auto &maskMap = it->second;
        auto maskIt = maskMap.find(mask);
        if (maskIt == maskMap.end()) {
            return false;
        }

        opcodes = maskIt->second;
        return true;
    }

    /// @brief Used to add a pattern and mask to the cache
    /// @param map The map to add to
    /// @param pattern The pattern to add
    /// @param mask The mask to add
    /// @param opcodes The opcodes to add
    void insertIntoCache(PatternMap &map, const std::string &pattern, const std::string &mask, const Opcodes &opcodes) {
        auto it = map.find(pattern);
        if (it == map.end()) {
            map[pattern] = {{mask, opcodes}};
        } else {
            it->second[mask] = opcodes;
        }
    }

    /// @brief Used to save the cache to a file
    /// @param id The ID of the cache (usually the window title)
    /// @param map The map to save
    void saveCache(const std::string &id, const PatternMap &map) {
        auto filePath = utils::getModHacksDirectory() / fmt::format("{}.json.cache", id);
        std::ofstream file(filePath);
        if (!file.is_open()) {
            L_ERROR("Failed to open file: {}", filePath.string());
            return;
        }

        nlohmann::json json;
        for (const auto &[pattern, masks]: map) {
            nlohmann::json maskMap;
            for (const auto &[mask, opcodes]: masks) {
                nlohmann::json opcodeList;
                for (const auto &opcode: opcodes) {
                    nlohmann::json opcodeJson;
                    opcodeJson["addr"] = fmt::format("0x{:X}", opcode.address);
                    opcodeJson["on"] = utils::bytesToHex(opcode.patched);
                    opcodeJson["off"] = utils::bytesToHex(opcode.original);
                    opcodeJson["lib"] = opcode.library;
                    opcodeList.push_back(opcodeJson);
                }
                maskMap[mask] = opcodeList;
            }
            json[pattern] = maskMap;
        }

        file << json.dump(4);
        file.close();
    }

    void initialize() {
        auto hacksDir = utils::getModHacksDirectory();
        if (!std::filesystem::exists(hacksDir)) {
            L_ERROR("Hacks directory does not exist: {}", hacksDir.string());
            return;
        }

        // Add the embedded hacks
        std::vector<EmbeddedHack*> embeddedHacks = hacks::s_allHacks;

        for (const auto &entry: std::filesystem::directory_iterator(hacksDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                // Load the JSON file
                std::ifstream file(entry.path());
                if (!file.is_open()) {
                    L_ERROR("Failed to open file: {}", entry.path().string());
                    continue;
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                file.close();

                std::vector<Component *> windowComponents;

                try {
                    nlohmann::json json = nlohmann::json::parse(buffer.str());
                    std::string title = json.at("title").get<std::string>();

                    // Load the cached addresses
                    PatternMap cache = tryGetCache(title);
                    PatternMap newCache;

                    L_BENCHMARK(title,
                    for (const auto &component: json.at("items")) {
                        if (component.contains("version")) {
                            auto version = component.at("version").get<std::string>();
                            if (!utils::compareVersion(version))
                                continue;
                        }

                        auto type = component.at("type").get<std::string>();
                        if (type == "text") {
                            auto text = component.at("text").get<std::string>();
                            windowComponents.push_back(new TextComponent(text));
                        } else if (type == "toggle") {
                            auto toggleTitle = component.at("title").get<std::string>();
                            auto id = component.at("id").get<std::string>();
                            std::vector<gd::sigscan::Opcode> opcodes;
                            bool warn = false;
                            for (const auto &opcode: component.at("opcodes")) {
                                if (opcode.contains("version")) {
                                    auto version = opcode.at("version").get<std::string>();
                                    if (!utils::compareVersion(version)) {
                                        continue;
                                    }
                                }

                                if (opcode.contains("pattern")) {
                                    auto pattern = opcode.at("pattern").get<std::string>();
                                    auto mask = opcode.at("mask").get<std::string>();
                                    std::string library;
                                    if (opcode.contains("lib")) {
                                        library = opcode.at("lib").get<std::string>();
                                    }

                                    Opcodes opc;
                                    bool found = findInCache(pattern, mask, cache, opc);
                                    bool verified = true;
                                    if (found) {
                                        // Revalidate the cached addresses
                                        for (auto &o: opc) {
                                            if (!verifyOpcode(o)) {
                                                verified = false;
                                                break;
                                            }
                                        }

                                        if (!verified) {
                                            L_TRACE("Cached addresses are invalid for: {}", pattern);
                                        }
                                    }

                                    if (!found || !verified) {
                                        // Scan for the addresses
                                        opc = gd::sigscan::match(pattern, mask, library);
                                        if (opc.empty()) {
                                            warn = true;
                                            break;
                                        }

                                        // Verify the addresses
                                        for (auto &o: opc) {
                                            if (!verifyOpcode(o)) {
                                                warn = true;
                                                break;
                                            }
                                        }
                                    }

                                    if (!warn) {
                                        insertIntoCache(newCache, pattern, mask, opc);
                                        opcodes.insert(opcodes.end(), opc.begin(), opc.end());
                                    }
                                } else {
                                    auto opc = readOpcode(opcode);
                                    if (!verifyOpcode(opc))
                                        warn = true;
                                    opcodes.push_back(opc);
                                }
                            }

                            if (opcodes.empty()) {
                                L_WARN("No opcodes found for: {}", toggleTitle);
                                warn = true;
                            }

                            if (warn)
                                L_WARN("{} has invalid opcodes!", toggleTitle);

                            auto toggle = new ToggleComponent(toggleTitle, id, opcodes);
                            toggle->setWarnings(warn);

                            if (component.contains("description")) {
                                toggle->setDescription(component.at("description").get<std::string>());
                            }

                            if (component.contains("cheat")) {
                                toggle->setIsCheat(component.at("cheat").get<bool>());
                            }

                            windowComponents.push_back(toggle);
                            hacks.push_back(toggle);
                        } else if (type == "embedded") {
                            auto hack = component.at("hack").get<std::string>();
                            auto it = std::find_if(embeddedHacks.begin(), embeddedHacks.end(), [hack](const auto &h) {
                                return h->getId() == hack;
                            });

                            if (it != embeddedHacks.end()) {
                                windowComponents.push_back(*it);
                                embeddedHacks.erase(it);
                            } else {
                                L_WARN("Failed to find embedded hack: {}", hack);
                            }
                        }
                    }
                    );

                    // Sort the components
                    std::sort(windowComponents.begin(), windowComponents.end(), [](const auto &a, const auto &b) {
                        std::string aName = a->getName();
                        std::string bName = b->getName();

                        std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
                        std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

                        return aName < bName;
                    });

                    // Initialize the components
                    for (auto &component: windowComponents) {
                        component->onInit();
                    }

                    // Add the window
                    windows.emplace_back(title, [windowComponents]() {
                        for (auto &component: windowComponents) {
                            component->onDraw();
                        }
                    });

                    // Save the new cache
                    if (!newCache.empty()) {
                        saveCache(title, newCache);
                    }

                } catch (const std::exception &e) {
                    L_ERROR("Failed to parse file: {}", entry.path().string());
                    L_ERROR("{}", e.what());
                    continue;
                }
            }
        }

        // Initialize the embedded hacks
        for (auto &hack: embeddedHacks) {
            hack->onInit();
        }
    }

    std::vector<gui::Window> &getWindows() {
        return windows;
    }

    std::vector<Component *> &getComponents() {
        return components;
    }

    std::vector<ToggleComponent *> &getHacks() {
        return hacks;
    }

    std::vector<EmbeddedHack *> &getEmbeddedHacks() {
        return s_allHacks;
    }

    bool applyOpcode(const gd::sigscan::Opcode &opcode, bool enable) {
        uintptr_t handle;
        if (opcode.library.empty()) {
            handle = utils::getModuleHandle();
        } else {
            handle = utils::getModuleHandle(opcode.library.c_str());
        }

        uintptr_t address = handle + opcode.address;

        std::vector<uint8_t> bytes = enable ? opcode.patched : opcode.original;
        return utils::patchMemory(address, bytes);
    }

    bool verifyOpcode(const gd::sigscan::Opcode &opcode) {
        uintptr_t handle;
        if (opcode.library.empty()) {
            handle = utils::getModuleHandle();
        } else {
            handle = utils::getModuleHandle(opcode.library.c_str());
        }

        uintptr_t address = handle + opcode.address;

        std::vector<uint8_t> bytes = utils::readMemory(address, opcode.original.size());
        return bytes == opcode.original;
    }
}