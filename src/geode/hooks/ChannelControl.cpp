#include "../pch.hpp"

#include <Geode/fmod/fmod.hpp>
#include <dash/internal.hpp>
#include <dash/mappings.hpp>
#include "../../shared/hacks/speedhack/speedhack.hpp"

namespace openhack::hooks::ChannelControl {
    FMOD_RESULT setVolume(FMOD::Channel *channel, float volume) {
        hacks::SpeedHack::setVolume(channel);
        return channel->setVolume(volume);
    }
}

$execute {
    (void) geode::Mod::get()->hook(
            (void *) gd::findSignature("FMOD::ChannelControl::setVolume"),
            &openhack::hooks::ChannelControl::setVolume,
            "FMOD::ChannelControl::setVolume",
            tulip::hook::TulipConvention::Stdcall);
}