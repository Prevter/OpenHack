#include "smart-startpos.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    static std::vector<gd::StartPosObject*> s_startPosObjects;
    static std::vector<gd::GameObject*> s_gravityPortals, s_dualPortals, s_gamemodePortals, s_miniPortals, s_speedChanges, s_mirrorPortals;

    void SmartStartPos::onInit() {
        // Set default values
        config::setIfEmpty("hack.smart_startpos.enabled", false);

        // Initialize keybind
        menu::keybinds::setKeybindCallback("smart_startpos.enabled", []() {
            bool enabled = !config::get<bool>("hack.smart_startpos.enabled");
            config::set("hack.smart_startpos.enabled", enabled);
        });
    }

    void SmartStartPos::onDraw() {
        gui::checkbox("Smart StartPos", "hack.smart_startpos.enabled");
        gui::tooltip("Tries to pick the settings for the StartPos objects automatically.\n"
                     "Note: This only evaluates portals, so it might not work in all cases.");
        menu::keybinds::addMenuKeybind("smart_startpos.enabled", "Smart StartPos", [](){
            bool enabled = !config::get<bool>("hack.smart_startpos.enabled");
            config::set("hack.smart_startpos.enabled", enabled);
        });
    }

    void SmartStartPos::initLevel() {
        // Clear all vectors
        s_startPosObjects.clear();
        s_gravityPortals.clear();
        s_dualPortals.clear();
        s_gamemodePortals.clear();
        s_miniPortals.clear();
        s_speedChanges.clear();
        s_mirrorPortals.clear();
    }

    void SmartStartPos::addObject(gd::GameObject* object) {
        auto objID = object->m_objectID();
        switch (objID) {
            case 10: // Blue Gravity Portal
            case 11: // Yellow Gravity Portal
            case 2926: // Green Gravity Portal
                s_gravityPortals.push_back(object);
                break;
            case 12: // Cube Portal
            case 13: // Ship Portal
            case 47: // Ball Portal
            case 111: // UFO Portal
            case 660: // Wave Portal
            case 745: // Robot Portal
            case 1331: // Spider Portal
            case 1933: // SwingCopter Portal
                s_gamemodePortals.push_back(object);
                break;
            case 286: // Orange Dual Portal
            case 287: // Blue Dual Portal
                s_dualPortals.push_back(object);
                break;
            case 99: // Normal Size Portal
            case 101: // Mini Portal
                s_miniPortals.push_back(object);
                break;
            case 45: // Orange Mirror Portal
            case 46: // Blue Mirror Portal
                s_mirrorPortals.push_back(object);
                break;
            case 200: // -1x Speed Change
            case 201: // 1x Speed Change
            case 202: // 2x Speed Change
            case 203: // 3x Speed Change
            case 1334: // 4x Speed Change
                s_speedChanges.push_back(object);
                break;
            case 31: // StartPos
                s_startPosObjects.push_back(reinterpret_cast<gd::StartPosObject*>(object));
                break;
            default:
                return;
        }

        // Recalculate start positions when a new object is added
        updateSmartStartPos();
    }

    /// @brief Get all objects that are before the given x position
    std::vector<gd::GameObject*> findBefore(std::vector<gd::GameObject*>& vec, float x) {
        std::vector<gd::GameObject*> result;
        for (auto* obj : vec) {
            if (obj->getPositionX() < x) {
                result.push_back(obj);
            }
        }
        return result;
    }

    gd::GameObject* findLastBefore(std::vector<gd::GameObject*>& vec, float x) {
        gd::GameObject* result = nullptr;
        for (auto* obj : vec) {
            if (obj->getPositionX() < x) {
                result = obj;
            }
        }
        return result;
    }

    void SmartStartPos::updateSmartStartPos() {
        if (!config::get<bool>("hack.smart_startpos.enabled")) return;
        if (s_startPosObjects.empty()) return;

        for (auto* startPos : s_startPosObjects) {
            auto x = startPos->getPositionX();
            auto* startPosSettings = startPos->m_levelSettings();
            bool upsideDown = startPosSettings->m_isFlipped();
            bool dual = startPosSettings->m_startDual();
            bool mini = startPosSettings->m_startMini();
            bool mirror = startPosSettings->m_mirrorMode();
            int speed = startPosSettings->m_startSpeed();
            int gamemode = startPosSettings->m_startMode();

            // Get all objects that are before the current StartPos
            auto gravityPortals = findBefore(s_gravityPortals, x + 10);
            auto dualPortal = findLastBefore(s_dualPortals, x + 10);
            auto gamemodePortal = findLastBefore(s_gamemodePortals, x + 10);
            auto miniPortal = findLastBefore(s_miniPortals, x + 10);
            auto speedChange = findLastBefore(s_speedChanges, x + 50);
            auto mirrorPortal = findLastBefore(s_mirrorPortals, x + 10);

            // Iterate over all objects and set the settings accordingly
            for (auto* gravityPortal : gravityPortals) {
                switch (gravityPortal->m_objectID()) {
                    case 10: // Blue Gravity Portal
                        upsideDown = false;
                        break;
                    case 11: // Yellow Gravity Portal
                        upsideDown = true;
                        break;
                    case 2926: // Green Gravity Portal
                        upsideDown = !upsideDown;
                        break;
                }
            }

            if (dualPortal) {
                switch (dualPortal->m_objectID()) {
                    case 286: // Orange Dual Portal
                        dual = true;
                        break;
                    case 287: // Blue Dual Portal
                        dual = false;
                        break;
                }
            }

            if (dualPortal) {
                switch (gamemodePortal->m_objectID()) {
                    case 12: // Cube Portal
                        gamemode = 0;
                        break;
                    case 13: // Ship Portal
                        gamemode = 1;
                        break;
                    case 47: // Ball Portal
                        gamemode = 2;
                        break;
                    case 111: // UFO Portal
                        gamemode = 3;
                        break;
                    case 660: // Wave Portal
                        gamemode = 4;
                        break;
                    case 745: // Robot Portal
                        gamemode = 5;
                        break;
                    case 1331: // Spider Portal
                        gamemode = 6;
                        break;
                    case 1933: // SwingCopter Portal
                        gamemode = 7;
                        break;
                }
            }

            if (miniPortal) {
                switch (miniPortal->m_objectID()) {
                    case 99: // Normal Size Portal
                        mini = false;
                        break;
                    case 101: // Mini Portal
                        mini = true;
                        break;
                }
            }

            if (speedChange) {
                switch (speedChange->m_objectID()) {
                    case 200: // -1x Speed Change
                        speed = 1;
                        break;
                    case 201: // 1x Speed Change
                        speed = 0;
                        break;
                    case 202: // 2x Speed Change
                        speed = 2;
                        break;
                    case 203: // 3x Speed Change
                        speed = 3;
                        break;
                    case 1334: // 4x Speed Change
                        speed = 4;
                        break;
                }
            }

            if (mirrorPortal) {
                switch (mirrorPortal->m_objectID()) {
                    case 45: // Orange Mirror Portal
                        mirror = true;
                        break;
                    case 46: // Blue Mirror Portal
                        mirror = false;
                        break;
                }
            }

            startPosSettings->m_startMode() = gamemode;
            startPosSettings->m_startSpeed() = speed;
            startPosSettings->m_startMini() = mini;
            startPosSettings->m_startDual() = dual;
            startPosSettings->m_mirrorMode() = mirror;
            startPosSettings->m_isFlipped() = upsideDown;
        }

    }

}