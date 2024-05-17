## v2.3.2
* Resolved issue with "Respawn Delay"

## v2.3.1
**Fixes**:
* Fixed vanilla bug with hitboxes, mirror portals and slopes
* Fixed compatibility with some mods
* Fixed wave trail color being wrong with hitboxes enabled
* Attempted to fix "Respawn Delay" not working for some users
* Fixed "Show Minutes" in "Accurate Percentage"
* Fixed some label templates

**Other**:
* Added "{session_time}" label template
* Added ability to see the rotated player hitbox

## v2.3.0
**New**:
* Added "Accurate Percentage"
* Added "Auto Hide Triggers"
* Added "Auto Kill"
* Added "Auto Save"
* Added "Custom Wave Trail"
* Added "Fullscreen"/"Borderless" toggles
* Added "Hide Pause Menu"
* Added "Load Failed Bypass"
* Added "Objects Scale Bypass"

**Fixes**:
* Fixed some typos in the UI
* Fixed "RGB Icons" reverting to default colors

**Other**:
* Added modal windows for some UI elements
* FPS/TPS minimum values are now 5/0 respectively
* Added time/formatted_time to the labels
* Added hold delay to "Frame Stepper"
* Make windows scrollable if they don't fit vertically
* Replay bot no longer triggers "Auto Safe Mode" when inactive

## v2.2.2
* Fixed hitboxes showing up in practice mode when they shouldn't
* Fixed hitboxes not working with mirror portals 
* Fixed player getting stuck in the air on level complete in replays
* Added a toggle to fix player rotation in replays
* Fixed issues with macOS (Wine) and older hardware (OpenGL 2.1)
* Added a toggle into mod settings to enable legacy OpenGL support

**Other**:
* Added symbols export to help with debugging

## v2.2.1
* Fixed crash with corrupted config files (again)
* Added inner hitbox for hitbox trail

## v2.2.0
**New**:
* Added "TPS Bypass"
* Added "Zephyrus" (replays)
* Added "Show Hitboxes" + "on Death"
* Added "Frame Stepper"
* Added "Random Seed"
* Added "Respawn Delay"
* Added "Main Menu Gameplay"
* Added "Auto Deafen"
* Added "Smart StartPos"
* Added API for mod developers
* Added 2 new themes (Modern and Gruvbox)

**Fixes**:
* Fixed "StartPos Switcher" default index being maxed out
* Fixed rare crash when loading a corrupted config file
* Fixed windows getting stuck at the edge of the screen while animating
* Fixed windows spawning in the same spot on the first open
* Fixed search bar width being too big
* Fixed "RGB Icons" not working properly sometimes
* Fixed missing keybind initializer for VSync
* Fixed build errors when building the project with "BUILD_STANDALONE=OFF"
* Fixed "Noclip Accuracy" counting after death/level completion
* Fixed "Verify Hack" not working properly in some cases
* Fixed "Force Platformer" saving level type
* Fixed incompatibility with "Separate Dual Icons" mod when using "Unlock All Icons"

**Other**:
* Improved menu close/open animations
* You can now add custom `.ttf` fonts to `geode/config/prevter.openhack/fonts` folder
* Added keybind/tooltip for toggles with settings
* Added UI scaling for different display resolutions
* Mouse cursor now hides while playing a level

## v2.1.0
**New**:
* Reimplemented "Discord RPC" with x4 more features
* Added "Click Teleport"
* Added "Uncomplete Level"
* Added "Labels"
* Added "Noclip Limit"
* Added "RGB Icons"
* Re-added search bar

**Fixes**:
* Fixed tooltips going off the screen
* Fixed saving settings for non-English languages
* Disabled "Speedhack" on game launch to prevent softlocks
* Various bug fixes
* *[Vanilla]* Fixed label colors for "StartPos Switcher"

## v2.0.1
* Fixed a crash during the game launch if you've unfocused the game window
* Fixed a crash for GDPS users, if executable name is not "GeometryDash.exe"

## v2.0.0
### Full project rewrite for better stability and code quality
**UI**:
* Redesigned UI
* Added theme support
* Improved menu open/close animations
* Added font selection
* More color settings

**Hacks**:
* Added "Audio Speedhack"
* Added "Smooth Editor Trail"
* Added "Default Song Bypass"
* Added "Place Over"
* Added "No "Do Not" Flip"
* Added "No Trail"
* Added "Wave Trail Fix"
* Added "Checkpoint Limit Bypass"
* Added "Show Triggers"
* Added "Moderator Bypass"
* Added "Physics Bypass"
* Added "Instant Complete"
* Added "No Mirror Portal"
* Added "Anticheat Bypass"

**Shortcuts**:
* Added "Apply 4GB patch"
* Added "Inject DLL"
* Added "Show Options"

**Other**:
* Fixed many bugs and crashes
* Keybinds now have an "In-game only" option
* Fixed some hacks descriptions (shoutout to @zkayns and @SpeckyYT on GitHub)
* Added cache for pattern scanner to improve loading times (2nd launch will be faster)
* Added "Reset Camera" to "StartPos Switcher"
* Added label options for "StartPos Switcher"
* [Geode] Mod now uses "early-load", so "Unlock All Icons" should save properly

**Notes**:
* [Vanilla] Support for other GD versions will be added in the future (hopefully until next game update)
* [Vanilla] Crash handler contains more information about the crash
* Discord RPC is temporarily removed for a full rewrite

## v1.6.1
* Fixed some issues with Discord RPC
* Fixed "StartPos Switcher" crashes
* Fixed "Practice Mode" shortcut crash
* Fixed color settings not saving

## v1.6.0
* First Geode release
* Added Shortcuts
* Improved "Stack Windows"
* Added 15 new hacks
* Various bug fixes

# Pre-geode updates

## v1.5.0
* Added "StartPos Switcher"
* Added "Auto Pickup Coins"
* Fixes to keybinds

## v1.4.0
* Added keybinds
* Changes to the "Stack Windows"
* Fixed menu not opening after switching Fullscreen/Borderless mode
* Some bug fixes

## v1.3.0
* Added 13 new hacks
* Added "Discord RPC"
* "Music Unlocker" also unlocks Practice Mode music
* Added descriptions (tooltips) for hacks
* Fixes to old hacks

## v1.2.1
* Added "Auto Safe Mode"
* Added DLL loader
* Added "Enable Menu Buttons" hack
* Code refactoring

## v1.2.0
* Added autoupdater
* Added 2.204 version support
* Small fixes and improvements

## v1.1.1
* Fixed a crash when minimizing/maximizing the game
* Fixed some hooks
* Improved "NoClip" hack
* Restored all hacks to work with 2.202/2.203
* Small GUI improvements
* Added "Display" window to change FPS and toggle VSync

## v1.1.0
* Added support for Geometry Dash 2.202
* Added search bar
* Added pattern scanner for better compatibility

## v1.0.1
* Added snow effect in the main menu
* Fixed random crashes
* Added new hacks
* Fixed UI code

## v1.0.0
* Initial release