cmake_minimum_required(VERSION 3.21)
project(guikit_bundle VERSION 1.0.0 LANGUAGES CXX C)

if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    endif()
endif()

option(GUIKIT_BUNDLE_BUILD_DEMO "Build full GuiKit interactive showcase demo (guikit_demo)" ${PROJECT_IS_TOP_LEVEL})
option(GUIKIT_BUNDLE_BUILD_TESTS "Build and run unit tests for all GuiKit ecosystem repositories" ${PROJECT_IS_TOP_LEVEL})

if (GUIKIT_BUNDLE_BUILD_TESTS)
    enable_testing()
endif()

# Forward unified example/test switches into submodules via normal variables (CMP0077)
# so including guikit/bundle via add_subdirectory() never pollutes parent project cache.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(GUIKIT_BUILD_EXAMPLES ${GUIKIT_BUNDLE_BUILD_DEMO})
set(GUIKIT_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_GEO_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_SIGNAL_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_COCKPIT_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_DIAGRAM_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})

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
    set(GUIKIT_DEMO_GENERATED_ASSETS_CPP "${CMAKE_CURRENT_BINARY_DIR}/generated/demo_embedded_assets.cpp")
    file(GLOB_RECURSE GUIKIT_DEMO_ASSET_FILES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/demo/assets/*"
    )
    add_custom_command(
        OUTPUT "${GUIKIT_DEMO_GENERATED_ASSETS_CPP}"
        COMMAND "${CMAKE_COMMAND}"
            "-DASSETS_DIR=${CMAKE_CURRENT_SOURCE_DIR}/demo/assets"
            "-DOUTPUT_FILE=${GUIKIT_DEMO_GENERATED_ASSETS_CPP}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateDemoAssets.cmake"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateDemoAssets.cmake"
            ${GUIKIT_DEMO_ASSET_FILES}
        COMMENT "Embedding GuiKit Demo showcase assets into C++ binary"
        VERBATIM
    )

    add_executable(guikit_demo
        demo/main.cpp
        demo/demo_window.cpp
        demo/elements/heading_screen.cpp
        demo/elements/element_screen.cpp
        demo/elements/button_screen.cpp
        demo/elements/slider_screen.cpp
        demo/elements/knob_screen.cpp
        demo/elements/spin_box_screen.cpp
        demo/elements/input_screen.cpp
        demo/elements/select_screen.cpp
        demo/elements/segmented_screen.cpp
        demo/elements/color_picker_screen.cpp
        demo/elements/date_picker_screen.cpp
        demo/elements/level_meter_screen.cpp
        demo/elements/readout_screen.cpp
        demo/elements/tag_screen.cpp
        demo/elements/tag_input_screen.cpp
        demo/elements/switch_screen.cpp
        demo/elements/checkbox_screen.cpp
        demo/elements/radio_screen.cpp
        demo/elements/image_screen.cpp
        demo/panels/card_screen.cpp
        demo/panels/split_view_screen.cpp
        demo/panels/tab_bar_screen.cpp
        demo/panels/breadcrumb_screen.cpp
        demo/panels/pagination_screen.cpp
        demo/panels/menu_bar_screen.cpp
        demo/panels/toolbar_screen.cpp
        demo/panels/sidebar_screen.cpp
        demo/panels/tree_view_screen.cpp
        demo/panels/table_screen.cpp
        demo/panels/overlays_screen.cpp
        demo/content/markdown_screen.cpp
        demo/content/code_editor_screen.cpp
        demo/scenes/schematic_screen.cpp
        demo/scenes/image_view_screen.cpp
        demo/scenes/osm_map_screen.cpp
        demo/scenes/signal_screen.cpp
        demo/scenes/sonar_screen.cpp
        demo/scenes/waterfall_screen.cpp
        demo/scenes/attitude_screen.cpp
        demo/scenes/car_dashboard_screen.cpp
        demo/scenes/bpmn_screen.cpp
        "${GUIKIT_DEMO_GENERATED_ASSETS_CPP}"
    )
    target_include_directories(guikit_demo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/demo"
    )
    target_link_libraries(guikit_demo PRIVATE
        guikit::all
    )
    guikit_enforce_guidelines(guikit_demo
        HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/demo"
    )
    if (EMSCRIPTEN)
        set_target_properties(guikit_demo PROPERTIES SUFFIX ".html")
        target_link_options(guikit_demo PRIVATE
            "-Oz"
            "-flto"
            "-sSTACK_SIZE=1048576"
            "-Wl,--stack-first"
            "-sALLOW_MEMORY_GROWTH=1"
            "-sINITIAL_MEMORY=67108864"
            "-sMAX_WEBGL_VERSION=2"
            "-sMIN_WEBGL_VERSION=1"
            "-sEXIT_RUNTIME=0"
            "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/demo/shell.html"
        )
    endif()
endif()
