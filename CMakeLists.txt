cmake_minimum_required(VERSION 3.21)
project(guikit_bundle VERSION 1.0.0 LANGUAGES CXX C)

if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    endif()

    if (MSVC)
        # Enable multi-processor translation-unit compilation across all bundle targets & submodules
        add_compile_options(/MP)
    endif()
endif()

option(GUIKIT_BUNDLE_BUILD_TEXT "Build standalone GuiKit FreeType text engine (guikit::text)" ON)
option(GUIKIT_BUNDLE_BUILD_SDL "Build GuiKit SDL3 + ThorVG backend module (guikit::sdl)" ON)
option(GUIKIT_BUNDLE_BUILD_NATIVE "Build GuiKit Native platform + OpenGL ES3 backend module (guikit::native)" ON)
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
set(GUIKIT_TEXT_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_SDL_BUILD_EXAMPLES ${GUIKIT_BUNDLE_BUILD_DEMO})
set(GUIKIT_SDL_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_NATIVE_BUILD_EXAMPLES ${GUIKIT_BUNDLE_BUILD_DEMO})
set(GUIKIT_NATIVE_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_GEO_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_SIGNAL_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_COCKPIT_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})
set(GUIKIT_DIAGRAM_BUILD_TESTS ${GUIKIT_BUNDLE_BUILD_TESTS})

add_subdirectory(guikit)
if (GUIKIT_BUNDLE_BUILD_TEXT AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/text/CMakeLists.txt")
    add_subdirectory(text)
endif()
if (GUIKIT_BUNDLE_BUILD_SDL AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/sdl/CMakeLists.txt")
    add_subdirectory(sdl)
endif()
if (GUIKIT_BUNDLE_BUILD_NATIVE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/native/CMakeLists.txt")
    add_subdirectory(native)
endif()
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
if (TARGET guikit::text)
    target_link_libraries(guikit_all INTERFACE guikit::text)
endif()
if (TARGET guikit::sdl)
    target_link_libraries(guikit_all INTERFACE guikit::sdl)
endif()
if (TARGET guikit::native)
    target_link_libraries(guikit_all INTERFACE guikit::native)
endif()

if (GUIKIT_BUNDLE_BUILD_DEMO AND (TARGET guikit::backend_sdl3 OR TARGET guikit::native))
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
    add_custom_target(guikit_demo_assets DEPENDS "${GUIKIT_DEMO_GENERATED_ASSETS_CPP}")

    set(GUIKIT_DEMO_SOURCES
        demo/main.cpp
        demo/demo_window.cpp
        # 1. STYLE
        demo/style/theme_studio_screen.cpp
        # 2. LAYOUT
        demo/layout/split_view_screen.cpp
        demo/layout/element_screen.cpp
        # 3. SURFACES
        demo/surfaces/card_screen.cpp
        demo/surfaces/menu_bar_screen.cpp
        demo/surfaces/toolbar_screen.cpp
        demo/surfaces/sidebar_screen.cpp
        # 4. OVERLAYS
        demo/overlays/modal_screen.cpp
        demo/overlays/popover_screen.cpp
        demo/overlays/context_menu_screen.cpp
        demo/overlays/toast_screen.cpp
        demo/overlays/command_palette_screen.cpp
        demo/overlays/tooltip_screen.cpp
        # 5. ACTIONS
        demo/actions/button_screen.cpp
        demo/actions/segmented_screen.cpp
        demo/actions/tab_bar_screen.cpp
        demo/actions/breadcrumb_screen.cpp
        demo/actions/pagination_screen.cpp
        # 6. FIELDS
        demo/fields/input_screen.cpp
        demo/fields/code_editor_screen.cpp
        demo/fields/select_screen.cpp
        demo/fields/spin_box_screen.cpp
        demo/fields/date_picker_screen.cpp
        demo/fields/tag_input_screen.cpp
        demo/fields/color_picker_screen.cpp
        # 7. CONTROLS
        demo/controls/checkbox_screen.cpp
        demo/controls/radio_screen.cpp
        demo/controls/switch_screen.cpp
        demo/controls/slider_screen.cpp
        demo/controls/knob_screen.cpp
        # 8. DISPLAY
        demo/display/heading_screen.cpp
        demo/display/tag_screen.cpp
        demo/display/progress_bar_screen.cpp
        demo/display/level_meter_screen.cpp
        demo/display/readout_screen.cpp
        demo/display/image_screen.cpp
        # 9. VIEWS
        demo/views/table_screen.cpp
        demo/views/tree_view_screen.cpp
        demo/views/markdown_screen.cpp
        demo/views/image_view_screen.cpp
        demo/views/schematic_screen.cpp
        # 10. MODULES
        demo/modules/osm_map_screen.cpp
        demo/modules/signal_screen.cpp
        demo/modules/sonar_screen.cpp
        demo/modules/waterfall_screen.cpp
        demo/modules/attitude_screen.cpp
        demo/modules/car_dashboard_screen.cpp
        demo/modules/bpmn_screen.cpp
        "${GUIKIT_DEMO_GENERATED_ASSETS_CPP}"
    )

    if (TARGET guikit::backend_sdl3)
        add_executable(guikit_demo ${GUIKIT_DEMO_SOURCES})
        add_dependencies(guikit_demo guikit_demo_assets)
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

    if (TARGET guikit::native)
        add_executable(guikit_demo_native ${GUIKIT_DEMO_SOURCES})
        add_dependencies(guikit_demo_native guikit_demo_assets)
        target_include_directories(guikit_demo_native PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/demo"
        )
        target_compile_definitions(guikit_demo_native PRIVATE
            GUIKIT_DEMO_USE_NATIVE=1
        )
        target_link_libraries(guikit_demo_native PRIVATE
            guikit::guikit
            guikit::native
            guikit::geo
            guikit::signal
            guikit::cockpit
            guikit::diagram
        )
        if (TARGET guikit::text)
            target_link_libraries(guikit_demo_native PRIVATE guikit::text)
        endif()
        guikit_enforce_guidelines(guikit_demo_native)
        if (EMSCRIPTEN)
            set_target_properties(guikit_demo_native PROPERTIES SUFFIX ".html")
            target_link_options(guikit_demo_native PRIVATE
                "-Oz"
                "-flto"
                "-sSTACK_SIZE=1048576"
                "-Wl,--stack-first"
                "-sALLOW_MEMORY_GROWTH=1"
                "-sINITIAL_MEMORY=33554432"
                "-sMAX_WEBGL_VERSION=2"
                "-sMIN_WEBGL_VERSION=2"
                "-sEXIT_RUNTIME=0"
                "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/demo/shell.html"
            )
        endif()
    endif()
endif()
