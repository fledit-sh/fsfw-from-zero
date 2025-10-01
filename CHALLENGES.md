# Encountered Challenges (Happy Mistakes)

1. Really Frustrating:
Spending hours on the build file when cloning the repo. correct etl library is important and not in the readme.
Proposal: Add this to the actual repositories.
```bash
git clone https://github.com/ETLCPP/etl.git
cd etl
cmake -B build .
cmake --install build/
```

2. Making the webcam work. This requires opencv and some fiddling with the hardware. In this project a hello world has been generated with GPT5 to get the basic hardware running.
```bash
sudo pacman -S --needed gcc make cmake pkgconf opencv v4l-utils
```

3. Deriving the DeviceHandlerBase as a WebcamHandler. Requires a lot of fiddling, but the dependencies of the fsfw make quite clear what you need and provide sufficient documentation.
Introduced scaffolding and somehow made it work. Oriented on the ThermalDeviceHandler from sourceobsw

4. Its very important to override **ALL** the methods. Otherwise the baseclass stays virtual.
5. Encountered some problems with commandmessagecleaner. 
6. ERROR | 13:12:56.641 | DeviceHandlerBase::initialize: Object ID 0x57000001 | Passed communication IF invalid
7. Tried to schedule the DeviceHandler... Still not working properly
8. Stupid, forgot the ComIF.
9. ALWAYS remember to include files in the cmakelists.txt. Nothing works automatically
10. Endianness plays an important role between windows and linux.
11. Fixing endianness
12. 