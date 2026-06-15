## SDKs

* XB1 XDK - 180704 (July QFE9 2018)
* Switch SDK - 6.4.0 with NEX: 4.6.2
* PS4 SDK - 6.008.001

## Setup:

1. Make sure you have python 2.7 installed someplace.  The easiest thing is to
use the Visual Studio Installer, pick your version of Visual Studio (2017
currently), and choose "Modify," go to "Individual packages," and check the
64-bit python 2.7 box.  Then you can run python 2.7 via `py.exe -2.7`.
1. Clone repo https://github.com/Mojang/UE4, branch `dungeons-4.22.3` to your PC
1. Run `Setup.bat` to generate, download and install dependencies
1. Right click _Dungeons.uproject_ and Switch UnrealEngine - Select the `UnrealEngine` directory where you cloned the Mojang/UE4 engine.
1. Open _Dungeons.sln_ - if you have the correct SDKs installed you should see
the console configs

## Build:
In Visual Studio:
1. Compile Console(XboxOne/Switch/PS4) Development (need this to generate
Dungeons.target file)
1. Compile Win64 Development Editor
1. **PS4/Switch ONLY - Ensure target is set to default and turned on - it needs
to push the data to the target console.**
1. Run `BuildTools/D11/Cook_XXX.bat` - This will take a while the first time
because it needs to convert all the assets to natvie types.

## Run:

1. **TODO - Add Sandbox setup for XB1**
1. Compile and Run the config you want.



