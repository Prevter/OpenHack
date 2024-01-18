#include "../pch.h"

namespace keybinds
{

    struct Keybind
    {
        std::string name;
        std::string id;

        uint32_t keycode;
        std::function<void()> callback;

        Keybind(std::string id, std::string name, std::function<void()> callback = nullptr, uint32_t default_key = 0)
        {
            this->id = id;
            this->name = name;
            this->callback = callback;
            this->keycode = default_key;
        }
    };

    void init();      // called after all hacks are loaded
    void update();    // called every frame, registers pressed keys
    void draw_menu(); // draws the "Keybinds" window

    void add_keybind(Keybind keybind);
    void remove_keybind(std::string id);
    void set_keybind(std::string id, uint32_t key);
    bool has_keybind(std::string id);

    void load_keybinds();

}