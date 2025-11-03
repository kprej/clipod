include(../cmake/CPM.cmake)
CPMAddPackage("gh:jarro2783/cxxopts#master")

set(SQLITECPP_INCLUDE_SCRIPT OFF CACHE INTERNAL "No include script SQLitecpp")
CPMAddPackage("gh:SRombauts/SQLiteCpp#master")
CPMAddPackage("gh:arthursonzogni/ftxui#main")
CPMAddPackage("gh:gershnik/modern-uuid#master")
CPMAddPackage("gh:kcat/openal-soft#master")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        message("Using MSVC & clang")
        set(CMAKE_CXX_SIMULATE_ID "MSVC2")
    endif()
endif()

CPMAddPackage("gh:palacaze/sigslot#master")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
        set(CMAKE_CXX_SIMULATE_ID "MSVC")
    endif()
endif()


set(BUILD_TESTING OFF CACHE INTERNAL "Disable tests")
set(ENABLE_EXTERNAL_LIBS ON CACHE INTERNAL "Enable EXTERNAL Libs")

CPMAddPackage("gh:gabime/spdlog@1.8.2")
CPMAddPackage("gh:taglib/taglib#master")

CPMAddPackage("gh:marzer/tomlplusplus#master")

CPMAddPackage(
    NAME ctrlc
    GIT_REPOSITORY https://github.com/evgenykislov/ctrl-c.git
    GIT_TAG master
    DOWNLOAD_ONLY YES
)
if (ctrlc_ADDED)
  # ctrlc has no CMake support, so we create our own target

  add_library(ctrlc STATIC ${ctrlc_SOURCE_DIR}/src/ctrl-c.h ${ctrlc_SOURCE_DIR}/src/ctrl-c.cpp)

  target_include_directories(ctrlc
    PUBLIC
      $<BUILD_INTERFACE:${ctrlc_SOURCE_DIR}/src>
  )
endif()

find_package(FFMPEG REQUIRED)
