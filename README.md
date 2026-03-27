# LDPasswd
LDPasswd is a C library providing functions to achieve ϵ-LDP protection when collecting passwords in a simulated Red vs. Blue cybersecurity competition

# Compiling
## On Linux
1. Install prerequisites
```bash
sudo apt install mingw-w64 cmake build-essentials
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

4. Retrieve compiled header and move it to the correct location
```bash
# Look inside build/ and build-windows/ for the .a file and .lib file respectively
# On Linux, move the file to a global library directory if not there already
mv ldpasswd.a /usr/local/lib/
```
```powershell
# On Windows, move .lib file to project root for static compilation
```

## For developers
To compile C to EXE using mingw
```bash
x86_64-w64-mingw32-gcc hello.c -o hello.exe
```