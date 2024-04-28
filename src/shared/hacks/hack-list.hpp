#pragma once

#include "speedhack/speedhack.hpp"
#include "display/display.hpp"
#include "shortcuts/shortcuts.hpp"
#include "auto-safemode/auto-safemode.hpp"
#include "instant-complete/instant-complete.hpp"
#include "auto-pickup-coins/auto-pickup-coins.hpp"
#include "startpos-switcher/startpos-switcher.hpp"
#include "discord-rpc/discord-rpc.hpp"
#include "click-tp/click-tp.hpp"
#include "labels/labels.hpp"
#include "noclip-limit/noclip-limit.hpp"
#include "rgb-icons/rgb-icons.hpp"
#include "zephyrus/replays.hpp"
#include "frame-stepper/frame-stepper.hpp"
#include "random-seed/random-seed.hpp"
#include "menu-gameplay/menu-gameplay.hpp"
#include "hitboxes/hitboxes.hpp"
#include "respawn-delay/respawn-delay.hpp"
#include "auto-deafen/auto-deafen.hpp"
#include "smart-startpos/smart-startpos.hpp"
#include "auto-kill/auto-kill.hpp"
#include "auto-save/auto-save.hpp"
#include "hide-triggers/hide-triggers.hpp"
#include "accurate-percentage/accurate-percentage.hpp"
#include "custom-wave-trail/custom-wave-trail.hpp"
#include "hide-pause/hide-pause.hpp"

#include <vector>
#include <memory>

namespace openhack::hacks {

    /// @brief List of all hacks.
    static std::vector<EmbeddedHack*> s_allHacks = {
        new SpeedHack(),
        new Display(),
        new Shortcuts(),
        new AutoSafemode(),
        new InstantComplete(),
        new AutoPickupCoins(),
        new StartPosSwitcher(),
        new DiscordRPC(),
        new ClickTeleport(),
        new Labels(),
        new NoclipLimit(),
        new RGBIcons(),
        new Zephyrus(),
        new FrameStepper(),
        new RandomSeed(),
        new MenuGameplay(),
        new Hitboxes(),
        new RespawnDelay(),
        new AutoDeafen(),
        new SmartStartPos(),
        new AutoKill(),
        new AutoSave(),
        new HideTriggers(),
        new AccuratePercentage(),
        new CustomWaveTrail(),
        new HidePause()
    };

}