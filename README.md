# RaSTA MITM Setup

## Prerequisites

Required Ubuntu/Debian packages:
- cmake
- build-essential (for make, gcc, g++)
- pkg-config (for building unit tests)
- libcunit1-dev (for building unit tests)
- git
- linux-headers-$(uname -r)

Required Alpine/Lab packages:
- cmake
- alpine-sdk (for make, gcc, g++)
- cunit-dev (for building unit tests)
- git
- linux-headers

## Building

Standard CMake build:
```bash
mkdir build
cd build
cmake ..
cmake --build . -v
```

## Executing

### Simple MITM
- run the signal_simulator binary without arguments
- run the interlocking_simulator binary without arguments (to be added)

## Source Layout
- attack-clients: holds the executables that perform the actual attacks, compile into individual executables
- rasta-wrapper: holds a C++ class that wraps the RaSTA protocol, compiles into a static library librasta-wrapper.a
- sci-ls-wrapper: holds a C++ class that wraps the SCI-LS and SCI protocols, on top of the rasta-wrapper. Compiles into librasta-wrapper.a as well.
- CMakeLists.txt.in: configuration to build the RaSTA reference implementation