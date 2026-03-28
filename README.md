# LDPasswd
LDPasswd is a C library providing functions to achieve ϵ-LDP protection when collecting passwords in a simulated Red vs. Blue cybersecurity competition

# Compiling
## On Linux
1. Install prerequisites
```bash
sudo apt install mingw-w64 cmake
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
export VCPKG_ROOT=$(pwd)/vcpkg
```

2. Compiling library for Linux
```bash
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build
```

3. Compiling library for Windows
```bash
cmake -B build-windows -S . \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$PWD/toolchains/mingw-w64-x86_64.cmake" \
  -DVCPKG_TARGET_TRIPLET=x64-mingw-static
  -DCMAKE_SHELL=/bin/sh

cmake --build build-windows
```

4. Install the compiled library with its header (optional, you can also just retrieve the now compiled library from the build directory to do what you want with it)
```bash
# Linux
sudo cmake --install build

# Windows
sudo cmake --install build-windows
```

5. Build the testing binary
```bash
# Linux
cmake --build build --target testldp
```

5. Clean up environment
```bash
rm -rf build/
rm -rf build-windows/
rm -rf vcpkg_installed/
```

## For developers
To compile C to EXE using mingw
```bash
x86_64-w64-mingw32-gcc hello.c -o hello.exe
```