#include "../pch.hpp"

#include <Geode/fmod/fmod.hpp>
#include "../../shared/hacks/speedhack/speedhack.hpp"

namespace openhack::hooks::ChannelControl {
    FMOD_RESULT setVolume(FMOD::Channel *channel, float volume) {
        hacks::SpeedHack::setVolume(channel);
        return channel->setVolume(volume);
    }
}

$execute {
    auto setVolumeAddr = gd::utils::findExport(
            (uintptr_t)GetModuleHandleA("fmod.dll"),
            "?setVolume@ChannelControl@FMOD@@QAG?AW4FMOD_RESULT@@M@Z");
    geode::Mod::get()->hook(
            (void*)setVolumeAddr,
            &openhack::hooks::ChannelControl::setVolume,
            "FMOD::ChannelControl::setVolume",
            tulip::hook::TulipConvention::Stdcall);
}