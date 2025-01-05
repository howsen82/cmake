## Installation

[![Minimal CMake Build](https://github.com/howsen82/cmake/actions/workflows/cmake.yml/badge.svg)](https://github.com/howsen82/cmake/actions/workflows/cmake.yml)

**Ubuntu**

```sh
sudo apt update && sudo apt upgrade -y
sudo apt install build-essential checkinstall zlib1g-dev libssl-dev -y
wget https://github.com/Kitware/CMake/releases/download/v3.30.3/cmake-3.30.3.tar.gz # Download source code version from https://cmake.org/download/
tar -zxvf cmake-3.30.3.tar.gz
cd cmake-3.30.3
./bootstrap
make
sudo make install
cmake --version
```

**Windows**

```sh
wget https://github.com/Kitware/CMake/releases/download/v3.30.3/cmake-3.30.3-windows-x86_64.msi
```

Check CMake version

```sh
cmake --version
```

Build from current directory

```sh
# Generate build file into new 'build' folder
cmake -S . -B build
# Start Build current project
cmake --build ./build
```

Or

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

Generate build files

```sh
cmake -G "Unix Makefiles" -S . -B ./build
```

Generate Ninja build systems

```sh
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -S . -B ./build
```

Generate G++ build systems

```sh
cmake -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=/usr/bin/g++-13 -S . -B ./build
```

Build with add compiler flag

```sh
cmake -G "Unix Makefiles" -DCMAKE_CXX_FLAGS="-Wall -Werror" -S . -B ./build
```

Build with `Debug` build systems

```sh
cmake -G "Unix Makefiles" -DCMAKE_CXX_FLAGS="-Wall -Werror" -DCMAKE_CXX_FLAGS_RELEASE="-fpermissive" -DCMAKE_BUILD_TYPE=Debug -S . -B ./build
```

Build with `Release` build systems

```sh
cmake -G "Unix Makefiles" -DCMAKE_CXX_FLAGS="-Wall -Werror" -DCMAKE_CXX_FLAGS_RELEASE="-fpermissive" -DCMAKE_BUILD_TYPE=Release -S . -B ./build
```

To list all available presets

```sh
cmake --list-presets
```

Build with parallel build

```sh
cmake --build ./build/ --parallel $(($(nproc)-1))
```

Build with single core

```sh
time cmake --build ./build --parallel 1 2>/dev/null > /dev/null
```

Build with 4 cores

```sh
time cmake --build ./build --parallel 4 2>/dev/null > /dev/null
```

Removing previous build artifacts before the build

```sh
cmake --build ./build/ --clean-first
```

Debugging your build process

```sh
cmake --build ./build/ --verbose
```

Passing command-line arguments to the build tool

```sh
cmake --build ./build/ -- --trace
```

Installing a project via the CLI

```sh
sudo cmake --install ./build
```

Changing the default installation path

```sh
cmake --install build --prefix /tmp/example
```

Stripping binaries while installing

```sh
cmake --install build --strip
```

Installing specific components only (component-based install)

```sh
cmake --install build --component ch2.executables
```

Installing a specific configuration (for multiple-configuration generators only)

```sh
cmake --install build --config Debug
```

Build a Release project

```sh
cmake -S . -B ./build -DCMAKE_BUILD_TYPE="Release"
cmake --build ./build # Default using Debug
cmake --build ./build --config Release # Compile with Release
cmake --install ./build --prefix /tmp/install-test
cmake --install ./build --prefix /tmp/install-test --config Debug # Using Debug to install
```

---

## Creating an installable package with CPack

| **Generator Name** | **Description**                   |
|--------------------|-----------------------------------|
| 7Z                 | 7-zip archive                     |
| DEB                | Debian package                    |
| External           | CPack external package            |
| IFW                | Qt Installer Framework            |
| NSIS               | Null Soft Installer               |
| NSIS64             | Null Soft Installer (64-bit)      |
| NuGet              | NuGet packages                    |
| RPM                | RPM packages                      |
| STGZ               | Self-extracting TAR gzip archive  |
| TBZ2               | Tar BZip2 archive                 |
| TGZ                | Tar GZip archive                  |
| TXZ                | Tar XZ archive                    |
| TZ                 | Tar Compress archive              |
| TZST               | Tar Zstandard archive             |
| ZIP                | Zip archive                       |

Variables

| **Variable Name**           | **Description**                                        | **Default Value**     |
|-----------------------------|--------------------------------------------------------|-----------------------|
| CPACK_PACKAGE_NAME          | Package name                                           | Project name          |
| CPACK_PACKAGE_VENDOR        | Package vendor name                                    | “Humanity”            |
| CPACK_PACKAGE_VERSION_MAJOR | Package major version                                  | Project major version |
| CPACK_PACKAGE_VERSION_MINOR | Package minor version                                  | Project minor version |
| CPACK_PACKAGE_VERSION_PATCH | Package patch version                                  | Project patch version |
| CPACK_GENERATOR             | List of CPack generators to use                        | N/A                   |
| CPACK_THREADS               | Number of threads to use when parallelism is supported | 1                     |

```sh
cmake –S . -B build/
cmake --build build/
cpack --config build/CPackConfig.cmake -B build/
# Install package
sudo dpkg -i build/ch4_ex06_pack-1.0-Linux.deb
# Run package
ch4_ex06_executable
```

---

## Generating documentation from your code

