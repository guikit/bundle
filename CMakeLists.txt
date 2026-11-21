cmake_minimum_required(VERSION 3.21)
project(guikit_bundle VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(GUIKIT_BUNDLE_BUILD_DEMO "Build full GuiKit Bundle interactive showcase demo" ON)
option(GUIKIT_BUNDLE_BUILD_TESTS "Build and run unit tests for all GuiKit ecosystem repositories" ON)

if (GUIKIT_BUNDLE_BUILD_TESTS)
    enable_testing()
endif()

# Forward unified test/demo switches into submodules so a single CMake + CTest covers the entire ecosystem
set(GUIKIT_BUILD_DEMO ${GUIKIT_BUNDLE_BUILD_DEMO} CACHE BOOL "" FORCE)
set(GUIKIT_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS} CACHE BOOL "" FORCE)
set(GUIKIT_GEO_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS} CACHE BOOL "" FORCE)
set(GUIKIT_SIGNAL_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS} CACHE BOOL "" FORCE)
set(GUIKIT_COCKPIT_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS} CACHE BOOL "" FORCE)
set(GUIKIT_DIAGRAM_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS} CACHE BOOL "" FORCE)

add_subdirectory(guikit)
add_subdirectory(geo)
add_subdirectory(signal)
add_subdirectory(cockpit)
add_subdirectory(diagram)

# Umbrella target aggregating the entire GuiKit ecosystem
add_library(guikit_all INTERFACE)
add_library(guikit::all ALIAS guikit_all)
target_link_libraries(guikit_all INTERFACE
    guikit::guikit
    guikit::geo
    guikit::signal
    guikit::cockpit
    guikit::diagram
)

if (GUIKIT_BUNDLE_BUILD_DEMO)
    add_executable(guikit_bundle_demo
        demo/main.cpp
        demo/scenes/osm_map_screen.cpp
        demo/scenes/signal_screen.cpp
        demo/scenes/sonar_screen.cpp
        demo/scenes/waterfall_screen.cpp
        demo/scenes/attitude_screen.cpp
        demo/scenes/car_dashboard_screen.cpp
        demo/scenes/bpmn_screen.cpp
    )
    target_include_directories(guikit_bundle_demo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/demo"
    )
    target_link_libraries(guikit_bundle_demo PRIVATE
        guikit::demo_lib
        guikit::all
    )
    guikit_enforce_guidelines(guikit_bundle_demo
        HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/demo"
    )
    if (EMSCRIPTEN)
        set_target_properties(guikit_bundle_demo PROPERTIES SUFFIX ".html")
        target_link_options(guikit_bundle_demo PRIVATE
            "-Oz"
            "-flto"
            "-sSTACK_SIZE=1048576"
            "-Wl,--stack-first"
            "-sALLOW_MEMORY_GROWTH=1"
            "-sINITIAL_MEMORY=67108864"
            "-sMAX_WEBGL_VERSION=2"
            "-sMIN_WEBGL_VERSION=1"
            "-sEXIT_RUNTIME=0"
            "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/guikit/examples/demo/shell.html"
        )
    endif()
endif()
