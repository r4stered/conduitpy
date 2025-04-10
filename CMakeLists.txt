# Disable in-source builds to prevent source tree corruption
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message(
        FATAL_ERROR
        "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
"
    )
endif()

cmake_minimum_required(VERSION 3.21)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
)

# Make `GIT_SUBMODULES ""` initialize no submodules
cmake_policy(SET CMP0097 NEW)

# Set default build type to release with debug info (i.e. release mode
# optimizations are performed, but debug info still exists).
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

project(Conduit LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# When building, don't use the install RPATH already (but later on when
# installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

# Add the automatically determined parts of the RPATH which point to directories
# outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# The RPATH to be used when installing, but only if it's not a system directory
list(
    FIND
    CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
    "${CMAKE_INSTALL_PREFIX}/lib"
    isSystemDir
)
if("${isSystemDir}" STREQUAL "-1")
    list(APPEND CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

# Generate compile_commands.json by default
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "YES" CACHE STRING "" FORCE)
endif()

include(BuildTypes)

# Control where the static and shared libraries are built so that on Windows,
# we don't need to tinker with the path to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)

option(BUILD_PYTHON "Build Python module" ON)

include(CompilerFlags)

file(GLOB_RECURSE Conduit_src src/*.cc)
add_library(Conduit ${Conduit_src})
add_library(Condui::Conduit ALIAS Conduit)
compiler_flags(Conduit)
target_include_directories(Conduit PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

set_target_properties(Conduit PROPERTIES DEBUG_POSTFIX "d")

set_property(TARGET Conduit PROPERTY FOLDER "libraries")
target_compile_definitions(Conduit PRIVATE CONDUIT_EXPORTS)

include(CTest)
include(FetchContent)

option(USE_SYSTEM_NANOBIND "Use system nanobind" OFF)

set(WITH_TESTS OFF CACHE INTERNAL "Turn off tests")
set(WITH_CSCORE OFF CACHE INTERNAL "Turn off cscore")
set(WITH_GUI OFF CACHE INTERNAL "Turn off gui")
set(WITH_PROTOBUF OFF CACHE INTERNAL "Turn off protobuf")

fetchcontent_declare(
    allwpilib
    GIT_REPOSITORY https://github.com/wpilibsuite/allwpilib.git
    GIT_TAG v2025.3.2
)
fetchcontent_makeavailable(allwpilib)

target_link_libraries(Conduit PUBLIC hal)

target_include_directories(
    Conduit
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

install(
    TARGETS Conduit
    COMPONENT Conduit
    EXPORT ConduitTargets
    LIBRARY
    DESTINATION lib
    ARCHIVE
    DESTINATION lib
    RUNTIME
    DESTINATION bin
    INCLUDES DESTINATION include
)
export(TARGETS Conduit FILE Conduit.cmake NAMESPACE Conduit::)
install(DIRECTORY include/ COMPONENT Conduit DESTINATION "include")
install(
    EXPORT ConduitTargets
    FILE Conduit.cmake
    NAMESPACE Conduit::
    DESTINATION lib/cmake/Conduit
)

include(CMakePackageConfigHelpers)

# Generate the config file that includes the exports
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ConduitConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ConduitConfig.cmake
    INSTALL_DESTINATION "lib/cmake/Conduit"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Install the config file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/ConduitConfig.cmake
    COMPONENT Conduit
    DESTINATION lib/cmake/Conduit
)

if(BUILD_PYTHON)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    if(DEFINED PY_BUILD_CMAKE_MODULE_NAME)
        set(PY_DEST ${PY_BUILD_CMAKE_MODULE_NAME})
    else()
        set(PY_DEST lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR})
    endif()

    # nanobind dependency
    if(NOT USE_SYSTEM_NANOBIND)
        fetchcontent_declare(
            nanobind
            GIT_REPOSITORY https://github.com/wjakob/nanobind.git
            GIT_TAG v2.6.1
        )
        fetchcontent_makeavailable(nanobind)
    else()
        find_package(nanobind CONFIG REQUIRED)
    endif()

    file(GLOB_RECURSE conduitpy_src conduitpy/cpp/*.cpp)

    # Build Conduit dependency directly into the wheel to avoid having to
    # configure RPATHs
    nanobind_add_module(_conduitpy ${conduitpy_src} ${Conduit_src})
    compiler_flags(_conduitpy)
    target_compile_definitions(_conduitpy PRIVATE CONDUITPY=1)
    target_link_libraries(_conduitpy PRIVATE hal wpiutil)
    target_include_directories(
        _conduitpy
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/conduitpy/cpp
    )

    # Suppress compiler warnings in nanobind
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        # nanobind/include/nanobind/nb_attr.h:212:14: warning: ISO C++ forbids zero-size array
        #  212 |     arg_data args[Size];
        #      |              ^~~~
        target_compile_options(_conduitpy PRIVATE -Wno-pedantic)
    endif()
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
        target_compile_options(nanobind-static PRIVATE -Wno-array-bounds)
        target_compile_options(
            _conduitpy
            PRIVATE -Wno-nested-anon-types -Wno-zero-length-array
        )
    endif()

    install(
        TARGETS _conduitpy
        COMPONENT python_modules
        LIBRARY
        DESTINATION ${PY_DEST}
    )

    nanobind_add_stub(
        _conduitpy_stub
        INSTALL_TIME
        MARKER_FILE conduitpy/py.typed
        MODULE _conduitpy
        OUTPUT conduitpy/__init__.pyi
        PYTHON_PATH $<TARGET_FILE_DIR:_conduitpy>
        DEPENDS _conduitpy
        COMPONENT python_modules
    )
endif()
