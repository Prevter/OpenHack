#include "hacks.h"
#include "patterns.h"
#include "../config.h"
#include "../menu/gui.h"

#include "speedhack.h"
#include "discord_rpc.h"
#include "display.h"
#include "auto_safemode.h"
#include "startpos_switch.h"
#include "pickup_coins.h"
#include "shortcuts.h"

namespace hacks
{
    std::vector<Window> windows;
    std::vector<Hack *> hacks;
    std::vector<ToggleComponent *> all_hacks;

    // parse opcode from json
    opcode_t read_opcode(nlohmann::json j)
    {
        auto addr = j["addr"].get<std::string>();
        auto on = j["on"].get<std::string>();
        auto off = j["off"].get<std::string>();

        opcode_t opcode;
        addr = addr.substr(2);
        opcode.address = (void *)std::stoul(addr, nullptr, 16);

        // Optional library name
        if (j.contains("lib"))
            opcode.library = j["lib"].get<std::string>();
        else
            opcode.library = "";

        auto on_bytes = std::vector<uint8_t>();
        auto off_bytes = std::vector<uint8_t>();
        for (size_t i = 2; i < on.size(); i += 2)
        {
            auto byte = on.substr(i, 2);
            uint8_t byte_value = (uint8_t)std::stoul(byte, nullptr, 16);
            on_bytes.push_back(byte_value);
        }
        for (size_t i = 2; i < off.size(); i += 2)
        {
            auto byte = off.substr(i, 2);
            uint8_t byte_value = (uint8_t)std::stoul(byte, nullptr, 16);
            off_bytes.push_back(byte_value);
        }

        opcode.on_bytes = on_bytes;
        opcode.off_bytes = off_bytes;

        return opcode;
    }

    // parse patterned opcode from json
    std::vector<opcode_t> read_pattern(nlohmann::json j)
    {
        auto pattern = j["pattern"].get<std::string>();
        auto mask = j["mask"].get<std::string>();
        std::string library = "";
        if (j.contains("lib"))
            library = j["lib"].get<std::string>();

        std::vector<opcode_t> opcodes;

        // Search for the pattern
        auto result = patterns::match(pattern, library, mask);

        // Check if the pattern was found
        if (result.found)
        {
            // Iterate over all addresses
            for (auto &address : result.opcodes)
            {
                // Create opcode
                opcode_t opcode;
                opcode.address = address.address;
                opcode.library = library;
                opcode.on_bytes = address.on_bytes;
                opcode.off_bytes = address.off_bytes;

                // Add opcode to list
                opcodes.push_back(opcode);
            }
        }
        else
        {
            L_WARN("Pattern not found: {}", pattern);
        }

        return opcodes;
    }

    // applies the opcode to the process
    bool write_opcode(opcode_t opcode, bool enabled)
    {
        // get process handle (or library if specified)
        uintptr_t process_handle;
        if (opcode.library == "")
            process_handle = (uintptr_t)GetModuleHandleA(NULL);
        else
            process_handle = (uintptr_t)GetModuleHandleA(opcode.library.c_str());

        uintptr_t address = process_handle + (uintptr_t)opcode.address;

        // change page protection to allow writing
        DWORD old_protect;
        std::vector<uint8_t> bytes = enabled ? opcode.on_bytes : opcode.off_bytes;
        if (VirtualProtect((void *)address, bytes.size(), PAGE_EXECUTE_READWRITE, &old_protect))
        {
            // write bytes
            memcpy((void *)address, bytes.data(), bytes.size());
            VirtualProtect((void *)address, bytes.size(), old_protect, &old_protect);
            return true;
        }

        // failed to change page protection
        return false;
    }

    // checks if the opcode has correct disable bytes
    bool verify_opcode(opcode_t opcode)
    {
        // get process handle (or library if specified)
        uintptr_t process_handle;
        if (opcode.library == "")
            process_handle = (uintptr_t)GetModuleHandleA(NULL);
        else
            process_handle = (uintptr_t)GetModuleHandleA(opcode.library.c_str());

        uintptr_t address = process_handle + (uintptr_t)opcode.address;

        // enable page protection
        DWORD old_protect;
        if (VirtualProtect((void *)address, opcode.off_bytes.size(), PAGE_EXECUTE_READWRITE, &old_protect))
        {
            // read bytes
            std::vector<uint8_t> bytes;
            bytes.resize(opcode.off_bytes.size());
            memcpy(bytes.data(), (void *)address, opcode.off_bytes.size());

            // restore page protection
            VirtualProtect((void *)address, opcode.off_bytes.size(), old_protect, &old_protect);

            // check if bytes are correct
            return bytes == opcode.off_bytes;
        }

        // failed to change page protection
        return false;
    }

    /* ===== Window ===== */

    Window::Window(std::string title)
    {
        m_title = title;
    }

    void Window::draw()
    {
        gui::Begin(m_title.c_str());
        for (auto &component : m_components)
        {
            component->draw();
        }
        gui::End();
    }

    void Window::add_component(Component *component)
    {
        m_components.push_back(component);
    }

    std::string Window::get_title()
    {
        return m_title;
    }

    void Window::load(nlohmann::json *data)
    {
        for (auto &component : m_components)
        {
            component->load(data);
        }
    }

    void Window::save(nlohmann::json *data)
    {
        for (auto &component : m_components)
        {
            component->save(data);
        }
    }

    /* ===== ToggleComponent ===== */

    ToggleComponent::ToggleComponent(std::string &title, std::string id, std::function<void()> callback, std::vector<opcode_t> opcodes)
    {
        m_title = title;
        m_id = id;
        m_callback = callback;
        m_opcodes = opcodes;
        m_enabled = false;
        m_description = "";
    }

    void ToggleComponent::toggle()
    {
        bool success = apply_patch();
        if (!success)
        {
            L_ERROR("Failed to apply patch {}!", m_title);
        }
        if (m_callback != nullptr)
        {
            m_callback();
        }
    }

    void ToggleComponent::draw()
    {
        if (m_has_warnings)
        {
            ImGui::BeginDisabled();
        }

        bool toggled = gui::ImToggleButton(m_title.c_str(), &m_enabled);

        if (m_has_warnings)
        {
            ImGui::EndDisabled();
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImVec2 pos = ImGui::GetMousePos();
                pos.x += 10;
                pos.y += 10;
                ImGui::SetNextWindowPos(pos);
                ImGui::SetTooltip("This hack won't work on current version of Geometry Dash!");
            }
        }
        else if (m_description != "")
        {
            if (ImGui::IsItemHovered())
            {
                ImVec2 pos = ImGui::GetMousePos();
                pos.x += 10;
                pos.y += 10;
                ImGui::SetNextWindowPos(pos);
                ImGui::SetTooltip(m_description.c_str());
            }
        }

        keybinds::add_menu_keybind(
            m_id.c_str(), m_title.c_str(),
            [this]()
            {
                this->m_enabled = !this->m_enabled;
                this->toggle();
            });

        if (toggled)
        {
            toggle();
        }
    }

    bool ToggleComponent::apply_patch()
    {
        if (m_has_warnings)
            return false;

        bool success = true;
        for (auto &opcode : m_opcodes)
        {
            success = write_opcode(opcode, m_enabled);
            if (!success)
            {
                break;
            }
        }

        return success;
    }

    bool ToggleComponent::is_enabled()
    {
        return m_enabled;
    }

    std::string ToggleComponent::get_title()
    {
        return m_title;
    }

    void ToggleComponent::load(nlohmann::json *data)
    {
        m_enabled = data->value(m_id, false);
        apply_patch();
    }

    void ToggleComponent::save(nlohmann::json *data)
    {
        data->emplace(m_id, m_enabled);
    }

    void ToggleComponent::create_keybind(keybinds::Keybind *keybind)
    {
        keybind->callback = [this]()
        {
            this->m_enabled = !this->m_enabled;
            this->toggle();
        };
    }

    /* ===== TextComponent ===== */

    TextComponent::TextComponent(std::string text)
    {
        m_text = text;
    }

    void TextComponent::draw()
    {
        gui::ImText(m_text.c_str());
    }

    /* ===== EmbeddedHackComponent ===== */

    EmbeddedHackComponent::EmbeddedHackComponent(Hack *hack, const char *id)
    {
        m_hack = hack;
        m_id = std::string(id);
    }

    void EmbeddedHackComponent::draw()
    {
        m_hack->draw(true);
    }

    void EmbeddedHackComponent::create_keybind(keybinds::Keybind *keybind)
    {
        m_hack->load_keybind(keybind);
    }

    void init()
    {
        // Initialize all hacks
        hacks.push_back(new Speedhack());
        hacks.push_back(new DiscordRPC());
        hacks.push_back(new DisplayHack());
        hacks.push_back(new AutoSafeMode());
        hacks.push_back(new StartposSwitcher());
        hacks.push_back(new PickupCoins());
        hacks.push_back(new Shortcuts());

        for (auto &hack : hacks)
        {
            hack->init();
        }
        
        // clear windows
        windows.clear();

        // get geode mod resources directory
        std::string resources_dir = geode::Mod::get()->getResourcesDir().string();

        // iterate over all files in "openhack/hacks" directory
        for (auto &p : std::filesystem::directory_iterator(resources_dir))
        {
            // check if file is a json file
            if (p.path().extension() == ".json")
            {
                // load json file
                std::ifstream file(p.path());
                if (!file.is_open())
                {
                    L_ERROR("Failed to open config file!");
                    return;
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                file.close();

                std::string json = buffer.str();

                // parse json
                try
                {
                    nlohmann::json j = nlohmann::json::parse(json);

                    // create window
                    Window window(j["title"].get<std::string>());

                    // iterate over all components
                    for (auto &component : j["items"])
                    {
                        if (component.contains("version"))
                        {
                            auto version = component["version"].get<std::string>();
                            if (!utils::compare_version(version.c_str()))
                            {
                                continue;
                            }
                        }

                        auto type = component["type"].get<std::string>();
                        if (type == "toggle")
                        {
                            auto title = component["title"].get<std::string>();
                            auto id = component["id"].get<std::string>();
                            auto opcodes = std::vector<opcode_t>();
                            bool warn = false;
                            for (auto &opcode : component["opcodes"])
                            {
                                if (opcode.contains("version"))
                                {
                                    auto version = opcode["version"].get<std::string>();
                                    if (!utils::compare_version(version.c_str()))
                                    {
                                        continue;
                                    }
                                }

                                if (opcode.contains("pattern"))
                                {
                                    auto pattern_opcodes = read_pattern(opcode);
                                    if (pattern_opcodes.empty())
                                        warn = true;

                                    for (auto &pattern_opcode : pattern_opcodes)
                                    {
                                        if (!verify_opcode(pattern_opcode))
                                            warn = true;
                                        opcodes.push_back(pattern_opcode);
                                    }
                                }
                                else
                                {
                                    opcode_t opc = read_opcode(opcode);
                                    if (!verify_opcode(opc))
                                        warn = true;
                                    opcodes.push_back(opc);
                                }
                            }

                            if (warn)
                            {
                                L_WARN("{} has invalid opcodes!", title);
                            }

                            auto toggle = new ToggleComponent(title, id, nullptr, opcodes);
                            toggle->set_warnings(warn);

                            if (component.contains("cheat"))
                            {
                                auto cheat = component["cheat"].get<bool>();
                                toggle->set_is_cheat(cheat);
                            }

                            if (component.contains("description"))
                            {
                                auto description = component["description"].get<std::string>();
                                toggle->set_description(description);
                            }

                            window.add_component(toggle);
                            all_hacks.push_back(toggle);
                        }
                        else if (type == "text")
                        {
                            auto text = component["text"].get<std::string>();
                            auto text_component = new TextComponent(text);
                            window.add_component(text_component);
                        }
                        else if (type == "embedded")
                        {
                            auto hack_name = component["hack"].get<std::string>();
                            Hack *hack = find_hack<Hack>(hack_name);

                            if (hack != nullptr)
                            {
                                auto embedded = new EmbeddedHackComponent(hack, hack_name.c_str());
                                window.add_component(embedded);
                            }
                        }
                    }

                    // sort toggle components alphabetically
                    std::sort(window.m_components.begin(), window.m_components.end(), [](Component *a, Component *b)
                              {
                                std::string a_key = a->get_sort_key();
                                std::string b_key = b->get_sort_key();

                                // compare lower case strings
                                std::transform(a_key.begin(), a_key.end(), a_key.begin(), ::tolower);
                                std::transform(b_key.begin(), b_key.end(), b_key.begin(), ::tolower);
                                
                                return a_key < b_key; });

                    // add window to list of windows
                    windows.push_back(window);
                }
                catch (const std::exception &e)
                {
                    L_ERROR("Failed to parse config file: {}", e.what());
                }
            }
        }

        // late hack init
        for (auto &hack : hacks)
        {
            hack->late_init();
        }
    }

    void draw()
    {
        for (auto &hack : hacks)
        {
            hack->draw();
        }

        for (auto &window : windows)
        {
            window.draw();
        }
    }

    void update()
    {
        for (auto &hack : hacks)
        {
            hack->update();
        }
    }

    void load(nlohmann::json *data)
    {
        for (auto &hack : hacks)
        {
            hack->load(data);
        }

        for (auto &window : windows)
        {
            window.load(data);
        }
    }

    void save(nlohmann::json *data)
    {
        for (auto &hack : hacks)
        {
            hack->save(data);
        }

        for (auto &window : windows)
        {
            window.save(data);
        }
    }
}