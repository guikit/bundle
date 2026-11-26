#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct BottomPanelDemoState {
    guikit::BottomPanelMode mode = guikit::BottomPanelMode::Expanded;
    keel::f32 defaultHeight = 170.0f;
    keel::f32 maxRatio = 0.70f;
    bool resizable = true;
    bool allowCollapse = true;
    bool showToggleButton = true;
    bool scrollable = true;
    bool floatingFrame = false;
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

    static constexpr keel::StaticArray<keel::StringRef, 2> kFrameNames = {"Docked", "Floating"};
    keel::usize frameIdx = s_bp.floatingFrame ? 1u : 0u;
    if (auto el = guikit::Element(ui, {.label = "Shell Frame"})) {
        if (guikit::Segmented(ui, frameIdx, kFrameNames).changed) {
            s_bp.floatingFrame = (frameIdx == 1u);
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
    if (auto el = guikit::Element(ui, {.label = "Scrollable Body"})) {
        if (guikit::Switch(ui, s_bp.scrollable).changed) ui.InvalidateAll();
    }
}

void RenderBottomPanelStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    guikit::UiTheme stageTheme = ui.GetTheme();
    stageTheme.SetShellStyle(s_bp.floatingFrame ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked);

    if (auto stageZone = ui.Zone({.theme = stageTheme})) {
        if (auto shell = guikit::Column(ui, {
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch
        })) {
            // Main workspace area above the bottom panel
            if (auto mainArea = guikit::Column(ui, {
                .gap = 10.0f,
                .padding = guikit::Padding::All(20.0f),
                .align = guikit::Alignment::Start,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                guikit::Heading(ui, {
                    .label = "Workspace Viewport",
                    .description = "Drag the bottom panel's top border to resize or snap-collapse.",
                    .separator = guikit::HeadingSeparator::Bottom
                });

                if (auto row = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Alignment::Center})) {
                    guikit::Tag(ui, s_bp.mode == guikit::BottomPanelMode::Expanded ? "EXPANDED" : "COLLAPSED", {
                        .variant = s_bp.mode == guikit::BottomPanelMode::Expanded ? guikit::UiVariant::Success : guikit::UiVariant::Secondary,
                        .style = guikit::TagStyle::Soft,
                        .size = guikit::UiSize::Sm
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
                if (auto hdr = guikit::Toolbar(ui, {.style = guikit::ToolbarStyle::Header, .size = guikit::UiSize::Sm})) {
                    static const keel::StaticArray<guikit::TabItem, 3> kTabs = {{
                        {.label = "Output",   .icon = guikit::Icon::Info()},
                        {.label = "Problems", .icon = guikit::Icon::Warning(), .badge = "2"},
                        {.label = "Terminal", .icon = guikit::Icon::Play()}
                    }};
                    guikit::TabBar(ui, s_bp.activeTab, kTabs, {.style = guikit::TabBarStyle::Underline, .size = guikit::UiSize::Sm});
                    hdr.Spacer();
                    if (hdr.ToolButton({.tooltip = "Close Panel", .icon = guikit::Icon::Close(), .square = true}).clicked) {
                        s_bp.mode = guikit::BottomPanelMode::Collapsed;
                        ui.InvalidateAll();
                    }
                }

                if (auto body = guikit::Column(ui, {
                    .gap = 4.0f,
                    .padding = guikit::Padding::Symmetric(12.0f, 8.0f),
                    .width = guikit::SizeMode::Stretch,
                    .height = guikit::SizeMode::Fit
                })) {
                    if (s_bp.activeTab == 0u) {
                        guikit::Text(ui, "[12:04:01] Configuring CMake workspace...", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Muted});
                        guikit::Text(ui, "[12:04:03] Building guikit_core (C++20, zero warnings)", {.role = guikit::TextRole::BodySmall});
                        guikit::Text(ui, "[12:04:06] Linking guikit_demo_native -> build_native/Release", {.role = guikit::TextRole::BodySmall});
                        guikit::Text(ui, "[12:04:07] 21/21 test suites passed (100%).", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Success});
                        guikit::Text(ui, "[12:04:08] Ready for interactive preview.", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary});
                    } else if (s_bp.activeTab == 1u) {
                        guikit::Text(ui, "WARN  layout/split_view.cpp:142 — Unused layout hint token", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Warning});
                        guikit::Text(ui, "INFO  style/theme.cpp:88 — Custom accent fallback verified", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Info});
                    } else {
                        guikit::Text(ui, "$ ctest --test-dir build --output-on-failure --parallel 8", {.role = guikit::TextRole::BodySmall});
                        guikit::Text(ui, "100% tests passed, 0 tests failed out of 21", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Success});
                    }
                }
            }
        }
    }
}

keel::String GenerateBottomPanelCode(const DemoContext& /*ctx*/) {
    const keel::String shellComment = s_bp.floatingFrame
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
        "    if (auto hdr = guikit::Toolbar(ui, {{.style = guikit::ToolbarStyle::Header, .size = guikit::UiSize::Sm}})) {{\n"
        "        guikit::TabBar(ui, activeTab, tabs, {{.style = guikit::TabBarStyle::Underline, .size = guikit::UiSize::Sm}});\n"
        "        hdr.Spacer();\n"
        "        if (hdr.ToolButton({{.tooltip = \"Close Panel\", .icon = guikit::Icon::Close(), .square = true}}).clicked) {{\n"
        "            panelMode = guikit::BottomPanelMode::Collapsed;\n"
        "        }}\n"
        "    }}\n"
        "    // Panel body content...\n"
        "}}",
        shellComment,
        (s_bp.mode == guikit::BottomPanelMode::Collapsed) ? "Collapsed" : "Expanded",
        s_bp.defaultHeight,
        s_bp.maxRatio,
        s_bp.resizable ? "true" : "false",
        s_bp.allowCollapse ? "true" : "false",
        s_bp.showToggleButton ? "true" : "false",
        s_bp.scrollable ? "true" : "false"
    );
}

} // namespace guikit::demo
