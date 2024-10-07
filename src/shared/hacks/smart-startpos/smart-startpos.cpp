#include "smart-startpos.hpp"
#include "../../menu/menu.hpp"

namespace openhack::hacks {

    static std::vector<StartPosObject*> s_startPosObjects;
    static std::vector<GameObject*> s_dualPortals, s_gamemodePortals, s_miniPortals, s_speedChanges, s_mirrorPortals;

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
        s_dualPortals.clear();
        s_gamemodePortals.clear();
        s_miniPortals.clear();
        s_speedChanges.clear();
        s_mirrorPortals.clear();
    }

    GameObject* getClosestObject(std::vector<GameObject*>& vec, StartPosObject* startPos) {
        GameObject* closest = nullptr;

        std::sort(vec.begin(), vec.end(), [] (GameObject* a, GameObject* b) {
            return a->getPositionX() < b->getPositionX();
        });

        for (auto obj : vec) {
            if (obj->getPositionX() - 10 > startPos->getPositionX())
                break;
            else if (obj->getPositionX() - 10 < startPos->getPositionX())
                closest = obj;
        }

        return closest;
    }

    void SmartStartPos::addObject(GameObject* object) {
        auto objID = object->m_objectID;
        switch (objID) {
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
                s_startPosObjects.push_back(static_cast<StartPosObject*>(object));
                break;
            default:
                return;
        }
    }

    void SmartStartPos::updateSmartStartPos(StartPosObject* startPos) {
        LevelSettingsObject* startPosSettings = startPos->m_startSettings;
        LevelSettingsObject* levelSettings = PlayLayer::get()->m_levelSettings;

        startPosSettings->m_startDual = levelSettings->m_startDual;
        startPosSettings->m_startMode = levelSettings->m_startMode;
        startPosSettings->m_startMini = levelSettings->m_startMini;
        startPosSettings->m_startSpeed = levelSettings->m_startSpeed;

        GameObject* obj = getClosestObject(s_dualPortals, startPos);
        if (obj)
            startPosSettings->m_startDual = obj->m_objectID == 286;

        obj = getClosestObject(s_gamemodePortals, startPos);

        if (obj) {
            switch(obj->m_objectID) {
                case 12:
                    startPosSettings->m_startMode = 0;
                    break;
                case 13:
                    startPosSettings->m_startMode = 1;
                    break;
                case 47:
                    startPosSettings->m_startMode = 2;
                    break;
                case 111:
                    startPosSettings->m_startMode = 3;
                    break;
                case 660:
                    startPosSettings->m_startMode = 4;
                    break;
                case 745:
                    startPosSettings->m_startMode = 5;
                    break;
                case 1331:
                    startPosSettings->m_startMode = 6;
                    break;
                case 1933:
                    startPosSettings->m_startMode = 7;
                    break;
            }
        }

        obj = getClosestObject(s_miniPortals, startPos);

        if (obj)
            startPosSettings->m_startMini = obj->m_objectID == 101;

        obj = getClosestObject(s_speedChanges, startPos);
        if (obj) {
            switch(obj->m_objectID) {
                case 200:
                    startPosSettings->m_startSpeed = Speed::Slow;
                    break;
                case 201:
                    startPosSettings->m_startSpeed = Speed::Normal;
                    break;
                case 202:
                    startPosSettings->m_startSpeed = Speed::Fast;
                    break;
                case 203:
                    startPosSettings->m_startSpeed = Speed::Faster;
                    break;
                case 1334:
                    startPosSettings->m_startSpeed = Speed::Fastest;
                    break;
            }
        }
    }

    void SmartStartPos::resetLevel() {
        if (config::get<bool>("hack.smart_startpos.enabled", false)) {
            for (StartPosObject* obj : s_startPosObjects)
                updateSmartStartPos(obj);
        }
    }

}