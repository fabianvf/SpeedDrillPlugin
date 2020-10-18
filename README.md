# SpeedDrillPlugin
BakkesMod Rocket League plugin to measure your performance doing the speed drill. Measures the time between ball hits and keeps track of averages over a freeplay session.

## Installation

### Using Bakkesmod Installer 

Click `Install with Bakkesmod` https://bakkesplugins.com/plugins/view/154

### From Source
* Set an environment variable
    1. Add a BAKKES_MOD environment variable that has the path to the bakkesmod folder (e.g. 'STEAMLIBRARY/steamapps/common/rocketleague/Binaries/Win64/bakkesmod')
    2. Restart computer
    3. Build the solution in Release | 64-bit
* Edit the project properties to correct the paths (replace BAKKES_MOD environment variable with the path to the bakkesmod folder in the rocket league 64 bin folder)
    1. C/C++ -> General -> Additional Include Directories
    2. Linker -> General -> Additional Library Directories
    3. Build Events -> Post-Build Event -> Command Line
        1. SpeedDrillPlugin.dll
        2. SpeedDrillPlugin.set
