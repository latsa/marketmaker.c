cmake_minimum_required(VERSION 3.7.2)

include("cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.19.173.tar.gz"
        SHA1 "750fb1d621af971fad6f303356b13017ad9f5e15"
        LOCAL
)

project(MarketMaker)

set(HUNTER_CONFIGURATION_TYPES Release)
set(HUNTER_JOBS_NUMBER 4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS Off)

set(DEPS_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/cpp-ethereum/.git OR
    NOT EXISTS ${CMAKE_SOURCE_DIR}/nanomsg/.git)
    message(FATAL_ERROR "Git submodules not initialized, execute:\n  git submodule update --init --recursive")
endif()

hunter_add_package(ZLIB)
find_package(ZLIB CONFIG REQUIRED)

hunter_add_package(OpenSSL)
find_package(OpenSSL REQUIRED)

hunter_add_package(CURL)
find_package(CURL CONFIG REQUIRED)

add_subdirectory("${CMAKE_SOURCE_DIR/sources}")
include_directories("${CMAKE_SOURCE_DIR}/include")

target_compile_definitions(nanomsg PUBLIC NN_STATIC_LIB=ON NN_ENABLE_GETADDRINFO_A=OFF -DNN_ENABLE_DOC=OFF -DNN_TESTS=OFF NN_TOOLS=OFF NN_ENABLE_NANOCAT=OFF CMAKE_INSTALL_PREFIX=${DEPS_INSTALL_PREFIX})
add_subdirectory("${CMAKE_SOURCE_DIR}/external/nanomsg")
add_subdirectory("${CMAKE_SOURCE_DIR}/external/cpp-ethereum")

# ********************************************************************

#target_link_libraries(... ZLIB::zlib)
#target_link_libraries(... PUBLIC OpenSSL::SSL OpenSSL::Crypto)
#target_link_libraries(... CURL::libcurl)
