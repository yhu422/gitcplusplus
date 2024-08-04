cmake_minimum_required(VERSION 3.27)
project(gitlet)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_CXX_STANDARD 17)
set(OPENSSL_ROOT_DIR "/opt/homebrew/Cellar/openssl@3/3.3.1")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARY_DIR "${OPENSSL_ROOT_DIR}/lib")
set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libssl.a")  # or libssl.so on Linux
set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/libcrypto.a")
find_package(OpenSSL 3.3.1 REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization)
include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARY_DIR}")
message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
add_executable(gitlet main.cpp
        src/Object.cpp
        include/Object.h
        include/common.h
        include/Blob.h
        include/Commit.h
        src/Util.cpp
        include/Util.h
        include/vars.h
)
target_link_libraries(gitlet ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${Boost_LIBRARIES})