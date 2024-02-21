#include "hooks.hpp"
#include "../../shared/hacks/speedhack/speedhack.hpp"
#include <dash/hook/fmod/ChannelControl.hpp>

namespace openhack::hooks::ChannelControl {
    FMOD_RESULT setVolume(FMOD::Channel *channel, float volume) {
        hacks::SpeedHack::setVolume(channel);
        return hook::ChannelControl::setVolume(channel, volume);
    }

    void installHooks() {
        LOG_HOOK(ChannelControl, setVolume);
    }
}
