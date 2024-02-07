#include "pch.hpp"

$execute
{
    openhack::initialize();
}

$on_mod(Unloaded)
{
    openhack::deinitialize();
}