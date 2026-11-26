#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct BottomPanelDemoState {
    guikit::BottomPanelMode mode = guikit::BottomPanelMode::Expanded;
    keel::f32 defaultHeight = 190.0f;
    keel::f32 maxRatio = 0.70f;
    bool resizable = true;
    bool allowCollapse = true;
    bool showToggleButton = true;
    bool scrollable = false;
    keel::usize activeTab = 0;
};

BottomPanelDemoState s_bp;

} // namespace

void RenderBottomPanelInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    if (auto el = guikit::Element(ui, {.label = "Panel Mode", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_bp.mode, {"Expanded", "Collapsed"}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Default Height"})) {
        if (guikit::Slider(ui, s_bp.defaultHeight, 100.0f, 300.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Max Height Ratio"})) {
        if (guikit::Slider(ui, s_bp.maxRatio, 0.35f, 0.85f, {.valueFormat = "%.2f", .step = 0.05f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Resizable Top Sash",
        .description = "Drag the top border up/down to resize"
    })) {
        if (guikit::Switch(ui, s_bp.resizable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {
        .label = "Allow Snap-Collapse",
        .description = "Dragging below minimum height collapses to 0px"
    })) {
        if (guikit::Switch(ui, s_bp.allowCollapse).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {
        .label = "Edge Toggle Pill",
        .description = "Show interactive collapse/expand button on top edge"
    })) {
        if (guikit::Switch(ui, s_bp.showToggleButton).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {
        .label = "Scrollable Body",
        .description = "Enables panel scrollbar (see Problems tab list)"
    })) {
        if (guikit::Switch(ui, s_bp.scrollable).changed) ui.InvalidateAll();
    }
}

void RenderBottomPanelStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    const bool isFloating = (ui.GetTheme().shellStyle == guikit::ShellStyle::Floating);
    if (auto shell = guikit::Column(ui, {
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch
    })) {
            // Main workspace area above the bottom panel (Card adapts seamlessly to Docked vs Floating shell)
            if (auto mainArea = guikit::Card(ui, {
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch,
                .gap = 10.0f,
                .padding = guikit::Padding::All(20.0f)
            })) {
                guikit::Heading(ui, {
                    .label = "Workspace Viewport",
                    .description = "Drag the bottom panel's top border to resize or snap-collapse.",
                    .separator = guikit::HeadingSeparator::Bottom
                });

                if (guikit::Button(ui, {
                    .text = s_bp.mode == guikit::BottomPanelMode::Expanded ? "Collapse Panel" : "Expand Panel",
                    .icon = s_bp.mode == guikit::BottomPanelMode::Expanded ? guikit::Icon::ChevronDown() : guikit::Icon::ChevronUp(),
                    .size = guikit::UiSize::Sm,
                    .variant = guikit::UiVariant::Secondary
                }).clicked) {
                    s_bp.mode = (s_bp.mode == guikit::BottomPanelMode::Expanded)
                        ? guikit::BottomPanelMode::Collapsed
                        : guikit::BottomPanelMode::Expanded;
                    ui.InvalidateAll();
                }
            }

            // Dockable BottomPanel below
            if (auto panel = guikit::BottomPanel(ui, s_bp.mode, {
                .height = s_bp.defaultHeight,
                .maxRatio = s_bp.maxRatio,
                .resizable = s_bp.resizable,
                .allowCollapse = s_bp.allowCollapse,
                .showToggleButton = s_bp.showToggleButton,
                .scrollable = s_bp.scrollable
            })) {
                if (auto bar = guikit::TabBar(ui, {
                    .style = guikit::TabBarStyle::Underline,
                    .padding = guikit::Padding{isFloating ? 6.0f : 0.0f, 4.0f, 4.0f, 0.0f},
                    .size = guikit::UiSize::Sm
                })) {
                    static const keel::StaticArray<guikit::TabItem, 3> kTabs = {{
                        {.label = "Output",   .icon = guikit::Icon::Info()},
                        {.label = "Problems", .icon = guikit::Icon::Warning(), .badge = "6"},
                        {.label = "Terminal", .icon = guikit::Icon::Play()}
                    }};
                    bar.Items(s_bp.activeTab, kTabs);
                    if (bar.Action({.tooltip = "Close Panel", .icon = guikit::Icon::Close(), .square = true}).clicked) {
                        s_bp.mode = guikit::BottomPanelMode::Collapsed;
                        ui.InvalidateAll();
                    }
                }

                if (s_bp.activeTab == 1u) {
                    struct DiagItem {
                        keel::StringRef severity;
                        guikit::UiVariant variant;
                        keel::StringRef location;
                        keel::StringRef message;
                    };
                    static constexpr keel::StaticArray<DiagItem, 6> kDiags = {{
                        {"WARN", guikit::UiVariant::Warning, "layout/split_view.cpp:142",  "Unused layout hint token in fallback branch"},
                        {"INFO", guikit::UiVariant::Info,    "style/theme.cpp:88",         "Custom accent fallback verified against WCAG AA"},
                        {"WARN", guikit::UiVariant::Warning, "surfaces/toolbar.cpp:312",   "Explicit gap overrides density token"},
                        {"INFO", guikit::UiVariant::Info,    "actions/tab_bar.cpp:210",    "Scrollable strip active with trailing action slot"},
                        {"WARN", guikit::UiVariant::Warning, "views/table.cpp:1184",       "Virtual row estimate adjusted on DPI change"},
                        {"INFO", guikit::UiVariant::Success, "tests/test_context.cpp:1",   "All 21 ecosystem CTest suites passed (0 warnings)"}
                    }};
                    if (auto list = guikit::Column(ui, {
                        .gap = 6.0f,
                        .padding = guikit::Padding::Symmetric(10.0f, 8.0f),
                        .width = guikit::SizeMode::Stretch,
                        .height = guikit::SizeMode::Fit
                    })) {
                        for (keel::usize i = 0u; i < kDiags.size(); ++i) {
                            auto rowScope = ui.IdScope(i);
                            if (auto row = guikit::Row(ui, {
                                .gap = 8.0f,
                                .align = guikit::Alignment::Center,
                                .width = guikit::SizeMode::Stretch
                            })) {
                                guikit::Tag(ui, kDiags[i].severity, {
                                    .variant = kDiags[i].variant,
                                    .style = guikit::TagStyle::Soft,
                                    .size = guikit::UiSize::Xs
                                });
                                guikit::Text(ui, kDiags[i].location, {
                                    .role = guikit::TextRole::CodeSmall,
                                    .variant = guikit::UiVariant::Secondary
                                });
                                guikit::Text(ui, kDiags[i].message, {
                                    .role = guikit::TextRole::BodySmall
                                });
                            }
                        }
                    }
                } else {
                    static constexpr keel::StringRef kOutputText =
                        "[12:04:01] Configuring CMake workspace (Visual Studio 17 2022)...\n"
                        "[12:04:02] Checking keel guidelines for guikit_core -> OK\n"
                        "[12:04:03] Building guikit_core (C++20, zero warnings, zero STL headers)\n"
                        "[12:04:05] Building guikit_native & guikit_demo_native (Release)\n"
                        "[12:04:06] Linking guikit_demo_native -> build_native/Release/guikit_demo_native.exe\n"
                        "[12:04:07] Running CTest: 21/21 test suites passed (100%)\n"
                        "[12:04:08] Ready for interactive preview.";

                    static constexpr keel::StringRef kTerminalText =
                        "$ cmake --build build --config Release --parallel 8\n"
                        "[100%] Built target guikit_demo_native\n"
                        "$ ctest --test-dir build -C Debug --output-on-failure --parallel\n"
                        "100% tests passed, 0 tests failed out of 21\n"
                        "Total Test time (real) = 28.55 sec\n"
                        "$ _";

                    const keel::StringRef consoleText = (s_bp.activeTab == 2u) ? kTerminalText : kOutputText;
                    if (auto body = guikit::Column(ui, {
                        .gap = 0.0f,
                        .padding = guikit::Padding::All(6.0f),
                        .width = guikit::SizeMode::Stretch,
                        .height = guikit::SizeMode::Stretch
                    })) {
                        auto tabScope = ui.IdScope(s_bp.activeTab);
                        guikit::TextArea(ui, consoleText, {
                            .readOnly = true,
                            .showLineNumbers = false,
                            .showCopyButton = true,
                            .monospace = true,
                            .size = guikit::UiSize::Sm,
                            .width = guikit::SizeMode::Stretch,
                            .height = guikit::SizeMode::Stretch,
                            .showIndentGuides = false
                        });
                    }
                }
            }
        }
}

keel::String GenerateBottomPanelCode(const DemoContext& ctx) {
    const bool isFloating = (ctx.studio.shellStyle == guikit::ShellStyle::Floating);
    const keel::String shellComment = isFloating
        ? keel::String("// theme.SetShellStyle(guikit::ShellStyle::Floating);\n")
        : keel::String{};
    return keel::format(
        "{}"
        "guikit::BottomPanelMode panelMode = guikit::BottomPanelMode::{};\n"
        "if (auto panel = guikit::BottomPanel(ui, panelMode, {{\n"
        "    .height = {:.0f}.0f,\n"
        "    .maxRatio = {:.2f}f,\n"
        "    .resizable = {},\n"
        "    .allowCollapse = {},\n"
        "    .showToggleButton = {},\n"
        "    .scrollable = {}\n"
        "}})) {{\n"
        "    if (auto bar = guikit::TabBar(ui, {{\n"
        "        .style = guikit::TabBarStyle::Underline,\n"
        "        .padding = guikit::Padding{{{}, 4.0f, 4.0f, 0.0f}},\n"
        "        .size = guikit::UiSize::Sm\n"
        "    }})) {{\n"
        "        bar.Items(activeTab, tabs);\n"
        "        if (bar.Action({{.tooltip = \"Close Panel\", .icon = guikit::Icon::Close(), .square = true}}).clicked) {{\n"
        "            panelMode = guikit::BottomPanelMode::Collapsed;\n"
        "        }}\n"
        "    }}\n"
        "    if (auto body = guikit::Column(ui, {{\n"
        "        .padding = guikit::Padding::All(6.0f),\n"
        "        .width = guikit::SizeMode::Stretch,\n"
        "        .height = guikit::SizeMode::Stretch\n"
        "    }})) {{\n"
        "        guikit::TextArea(ui, consoleText, {{\n"
        "            .readOnly = true,\n"
        "            .showLineNumbers = false,\n"
        "            .showCopyButton = true,\n"
        "            .monospace = true,\n"
        "            .size = guikit::UiSize::Sm,\n"
        "            .width = guikit::SizeMode::Stretch,\n"
        "            .height = guikit::SizeMode::Stretch,\n"
        "            .showIndentGuides = false\n"
        "        }});\n"
        "    }}\n"
        "}}",
        shellComment,
        (s_bp.mode == guikit::BottomPanelMode::Collapsed) ? "Collapsed" : "Expanded",
        s_bp.defaultHeight,
        s_bp.maxRatio,
        s_bp.resizable ? "true" : "false",
        s_bp.allowCollapse ? "true" : "false",
        s_bp.showToggleButton ? "true" : "false",
        s_bp.scrollable ? "true" : "false",
        isFloating ? "6.0f" : "0.0f"
    );
}

} // namespace guikit::demo
