cmake_minimum_required(VERSION 3.15)
project(LDPasswd VERSION 1.0 LANGUAGES C)

# Generate static .a or .lib file
add_library(ldpasswd STATIC 
    src/ldpasswd.c
)

# Tell CMake where the headers are
target_include_directories(ldpasswd PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set properties so the output names are clean
set_target_properties(ldpasswd PROPERTIES 
    OUTPUT_NAME "ldpasswd"
    C_STANDARD 11
)

# Install compiled library
install(TARGETS ldpasswd
    EXPORT LDPasswdConfig
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install the public headers
install(DIRECTORY include/ DESTINATION include)