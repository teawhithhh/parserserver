cmake_minimum_required(VERSION 3.10)

project(Http)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
find_package(spdlog REQUIRED)
find_package(Crow REQUIRED)
find_package(yaml-cpp REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_executable(Http main.cxx Server.cxx ParsersController.cxx Node.cxx NodeTranslator.cxx)
target_include_directories(Http PRIVATE .)

target_link_libraries(Http PRIVATE ws2_32 mswsock spdlog::spdlog Crow::Crow yaml-cpp)
