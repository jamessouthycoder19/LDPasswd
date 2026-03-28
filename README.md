# LDPasswd
LDPasswd is a C library providing functions to achieve ϵ-LDP protection when collecting passwords in a simulated Red vs. Blue cybersecurity competition

# Compiling
## On Linux
1. Install prerequisites
```bash
sudo apt install mingw-w64 cmake build-essential pkg-config
```

2. Compiling library for Linux
```bash
cmake -B build
cmake --build build
```

3. Compiling library for Windows
```bash
cmake -B build-windows -DCMAKE_TOOLCHAIN_FILE=toolchains/mingw-w64-x86_64.cmake
cmake --build build-windows
```

4. Install the compiled library with its header (optional, you can also just retrieve the now compiled library from the build directory to do what you want with it)
```bash
# Linux
sudo cmake --install build

# Windows
sudo cmake --install build-windows
```

5. Clean up environment
```bash
rm -rf build/
rm -rf build-windows/
```

## For developers
To compile C to EXE using mingw
```bash
x86_64-w64-mingw32-gcc hello.c -o hello.exe
```