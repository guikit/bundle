#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct ToolbarDemoState {
    guikit::ToolbarStyle style = guikit::ToolbarStyle::Surface;
    guikit::Orientation orientation = guikit::Orientation::Horizontal;
    guikit::ToolbarOverflow overflow = guikit::ToolbarOverflow::AutoCompact;
    bool showBadges = true;
    bool squareIcons = false;
    keel::i32 activeTool = 0;
    bool snapToGrid = true;
    bool livePreview = false;
    bool syncEnabled = true;
    keel::i32 syncMode = 0;
};
ToolbarDemoState s_tb;

constexpr keel::StringRef StyleEnumName(guikit::ToolbarStyle s) noexcept {
    switch (s) {
        case guikit::ToolbarStyle::Header:   return "Header";
        case guikit::ToolbarStyle::Floating: return "Floating";
        default:                             return "Surface";
    }
}

constexpr keel::StringRef OverflowEnumName(guikit::ToolbarOverflow o) noexcept {
    switch (o) {
        case guikit::ToolbarOverflow::Scroll: return "Scroll";
        case guikit::ToolbarOverflow::Wrap:   return "Wrap";
        case guikit::ToolbarOverflow::Clip:   return "Clip";
        default:                              return "AutoCompact";
    }
}
} // namespace

void RenderToolbarInspector(guikit::Context& ui, DemoContext& ctx) {
    static constexpr keel::StaticArray<keel::StringRef, 3> kStyleLabels = {"Surface", "Header", "Floating"};
    keel::usize styleIdx = (s_tb.style == guikit::ToolbarStyle::Header)
        ? 1
        : (s_tb.style == guikit::ToolbarStyle::Floating ? 2 : 0);
    if (auto el = guikit::Element(ui, {.label = "Toolbar Style", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, styleIdx, kStyleLabels).changed) {
            s_tb.style = (styleIdx == 1)
                ? guikit::ToolbarStyle::Header
                : (styleIdx == 2 ? guikit::ToolbarStyle::Floating : guikit::ToolbarStyle::Surface);
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 2> kOrientLabels = {"Horizontal", "Vertical"};
    keel::usize orientIdx = (s_tb.orientation == guikit::Orientation::Vertical) ? 1 : 0;
    if (auto el = guikit::Element(ui, {.label = "Orientation", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, orientIdx, kOrientLabels).changed) {
            s_tb.orientation = (orientIdx == 1)
                ? guikit::Orientation::Vertical
                : guikit::Orientation::Horizontal;
            ui.InvalidateAll();
        }
    }

    if (s_tb.orientation == guikit::Orientation::Horizontal) {
        static constexpr keel::StaticArray<keel::StringRef, 4> kOverflowLabels = {"Compact", "Scroll", "Wrap", "Clip"};
        keel::usize overflowIdx = keel::as<keel::usize>(s_tb.overflow);
        if (auto el = guikit::Element(ui, {.label = "Overflow Policy", .layout = guikit::ElementLayout::Stacked})) {
            if (guikit::Segmented(ui, overflowIdx, kOverflowLabels).changed) {
                s_tb.overflow = keel::from_underlying<guikit::ToolbarOverflow>(overflowIdx);
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Toolbar Size"})) {
        if (guikit::StepSlider(ui, ctx.controlsSize, {.showTicks = true, .showLabels = true, .includeNone = true}).changed) {
            ui.InvalidateAll();
        }
    }
    ctx.RenderVariantPicker(ui);

    if (auto el = guikit::Element(ui, {.label = "Icon-Only Square Tools"})) {
        if (guikit::Switch(ui, s_tb.squareIcons).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Action Badges"})) {
        if (guikit::Switch(ui, s_tb.showBadges).changed) ui.InvalidateAll();
    }
}

void RenderToolbarStage(guikit::Context& ui, DemoContext& ctx) {
    const bool isVert = (s_tb.orientation == guikit::Orientation::Vertical);
    const bool isSq = s_tb.squareIcons || isVert;

    const guikit::UiVariant toolVar = (ctx.btnVariant == guikit::UiVariant::Ghost)
        ? guikit::UiVariant::Primary
        : ctx.btnVariant;
    if (auto tb = guikit::Toolbar(ui, {
        .style = s_tb.style,
        .orientation = s_tb.orientation,
        .overflow = s_tb.overflow,
        .size = ctx.controlsSize
    })) {
        tb.Item(s_tb.activeTool, 0, {
            .text = "Select",
            .tooltip = "Select Tool (V)",
            .icon = guikit::Icon::Check(),
            .variant = toolVar,
            .square = isSq
        });
        tb.Item(s_tb.activeTool, 1, {
            .text = "Pan",
            .tooltip = "Pan Viewport (H)",
            .icon = guikit::Icon::Folder(),
            .variant = toolVar,
            .square = isSq
        });
        tb.Item(s_tb.activeTool, 2, {
            .text = "Measure",
            .tooltip = "Measure Cursor (M)",
            .icon = guikit::Icon::Search(),
            .variant = toolVar,
            .square = isSq
        });

        tb.Separator();

        tb.Toggle(s_tb.snapToGrid, {
            .text = "Snap",
            .tooltip = "Toggle Grid Snapping",
            .icon = guikit::Icon::Settings(),
            .variant = guikit::UiVariant::Info,
            .square = isSq
        });
        tb.Toggle(s_tb.livePreview, {
            .text = s_tb.livePreview ? "Live" : "Preview",
            .tooltip = "Toggle Live Preview",
            .icon = s_tb.livePreview ? guikit::Icon::Pause() : guikit::Icon::Play(),
            .variant = s_tb.livePreview ? guikit::UiVariant::Success : guikit::UiVariant::Warning,
            .square = isSq,
            .badge = (s_tb.showBadges && s_tb.livePreview) ? "LIVE" : ""
        });

        static constexpr keel::StaticArray<keel::StringRef, 3> kSyncModes = {
            "Auto Sync", "Manual Sync", "Offline"
        };
        tb.SplitToggle(s_tb.syncEnabled, s_tb.syncMode, kSyncModes, {
            .text = kSyncModes[keel::as<keel::usize>(keel::clamp(s_tb.syncMode, 0, 2))],
            .tooltip = "Toggle Sync & Select Mode",
            .icon = guikit::Icon::Flash(),
            .square = isSq
        });

        tb.Separator();

        tb.Dropdown({
            .text = "Export",
            .tooltip = "Export Snapshot",
            .icon = guikit::Icon::Save(),
            .square = isSq,
            .badge = s_tb.showBadges ? "3" : ""
        }, {
            {.label = "Export Vector SVG...", .shortcut = "Ctrl+E",       .icon = guikit::Icon::Folder()},
            {.label = "Export JSON Data...",  .shortcut = "Ctrl+Shift+E", .icon = guikit::Icon::Copy(), .separatorAfter = true},
            {.label = "Copy Clipboard Summary", .shortcut = "Ctrl+C",     .icon = guikit::Icon::Check()}
        });
    }
}

keel::String GenerateToolbarCode(const DemoContext& ctx) {
    return keel::format(
        "if (auto tb = guikit::Toolbar(ui, {{\n"
        "    .style = guikit::ToolbarStyle::{},\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .overflow = guikit::ToolbarOverflow::{}{}\n"
        "}})) {{\n"
        "    tb.Item(activeTool, 0, {{.text = \"Select\",  .icon = guikit::Icon::Check(),  .variant = guikit::UiVariant::{}, .square = {}}});\n"
        "    tb.Item(activeTool, 1, {{.text = \"Pan\",     .icon = guikit::Icon::Folder(), .variant = guikit::UiVariant::{}, .square = {}}});\n"
        "    tb.Item(activeTool, 2, {{.text = \"Measure\", .icon = guikit::Icon::Search(), .variant = guikit::UiVariant::{}, .square = {}}});\n"
        "    tb.Separator();\n"
        "    tb.Toggle(snapToGrid,  {{.text = \"Snap\",    .icon = guikit::Icon::Settings(), .variant = guikit::UiVariant::Info,    .square = {}}});\n"
        "    tb.Toggle(livePreview, {{.text = \"Preview\", .icon = guikit::Icon::Play(),     .variant = guikit::UiVariant::Success, .square = {}}});\n"
        "    tb.SplitToggle(syncEnabled, syncMode, {{\"Auto Sync\", \"Manual Sync\", \"Offline\"}}, {{\n"
        "        .text = \"Auto Sync\", .icon = guikit::Icon::Flash(), .square = {}\n"
        "    }});\n"
        "    tb.Separator();\n"
        "    auto menu = tb.Dropdown({{.text = \"Export\"{}}}, {{\n"
        "        {{.label = \"Export Vector SVG...\", .shortcut = \"Ctrl+E\", .icon = guikit::Icon::Folder()}},\n"
        "        {{.label = \"Copy Summary\",         .shortcut = \"Ctrl+C\", .icon = guikit::Icon::Check()}}\n"
        "    }});\n"
        "}}",
        StyleEnumName(s_tb.style),
        s_tb.orientation == guikit::Orientation::Vertical ? "Vertical" : "Horizontal",
        OverflowEnumName(s_tb.overflow),
        FormatUiSizePrefix(ctx.controlsSize),
        VariantName(ctx.btnVariant),
        s_tb.squareIcons ? "true" : "false",
        VariantName(ctx.btnVariant),
        s_tb.squareIcons ? "true" : "false",
        VariantName(ctx.btnVariant),
        s_tb.squareIcons ? "true" : "false",
        s_tb.squareIcons ? "true" : "false",
        s_tb.squareIcons ? "true" : "false",
        s_tb.squareIcons ? "true" : "false",
        s_tb.showBadges ? ", .badge = \"3\"" : ""
    );
}

} // namespace guikit::demo
