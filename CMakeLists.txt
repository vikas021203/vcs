cmake_minimum_required(VERSION 3.10)
project(vcs)

set(CMAKE_CXX_STANDARD 17)

# Manually specify OpenSSL paths
set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libcrypto.lib")
set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/x64/MD/libssl.lib")

# Include headers
include_directories(include)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Define executable
add_executable(vcs ${SOURCES})

#add OpenSSL to your executables
target_include_directories(vcs PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(vcs PRIVATE ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
