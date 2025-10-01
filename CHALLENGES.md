# Getting Started

1. Really Frustrating:
Spending hours on the build file when cloning the repo. correct etl library is important and not in the readme.
Proposal: Add this to the actual repositories.
```bash
git clone https://github.com/ETLCPP/etl.git
cd etl
git checkout <targetVersion>
cmake -B build .
cmake --install build/
```

2. Making the webcam work. This requires opencv and some fiddling with the hardware. In this project a hello world has been generated with GPT5 to get the basic hardware running.
```bash
sudo pacman -S --needed gcc make cmake pkgconf opencv v4l-utils
```

3. Deriving the DeviceHandlerBase as a WebcamHandler. Requires a lot of fiddling, but the dependencies of the fsfw make quite clear what you need and provide sufficient documentation.
Introduced scaffolding and somehow made it work. Oriented on the ThermalDeviceHandler from sourceobsw

