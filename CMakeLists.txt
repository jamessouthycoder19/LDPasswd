cmake_minimum_required(VERSION 3.15)
project(LDPasswd VERSION 1.0 LANGUAGES C)

# Build Directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Define the Static Library
add_library(ldpasswd STATIC src/ldpasswd.c)

# Configure Library Headers
target_include_directories(ldpasswd PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(ldpasswd PROPERTIES 
    OUTPUT_NAME "ldpasswd"
    C_STANDARD 11
)

# Add the executable (After the library is defined)
add_executable(testldp tests/test.c)

# Link the executable with the library
target_link_libraries(testldp PRIVATE ldpasswd)

# Install compiled library
install(TARGETS ldpasswd
    EXPORT LDPasswdConfig
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install the public headers
install(DIRECTORY include/ DESTINATION include)