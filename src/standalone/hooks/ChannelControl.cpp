#include "hooks.hpp"
#include "../../shared/hacks/speedhack/speedhack.hpp"

namespace openhack::hooks::ChannelControl {
    FMOD_RESULT setVolume(FMOD::Channel *channel, float volume) {
        hacks::SpeedHack::setVolume(channel);
        return channel->setVolume(volume);
    }

    void installHooks() {
        LOG_HOOK(gd::FMOD::ChannelControl, setVolume);
    }
}
