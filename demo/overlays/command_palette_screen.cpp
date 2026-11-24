#include "../demo_context.hpp"
#include "guikit/overlays/command_palette.hpp"

namespace guikit::demo {

namespace {

struct CommandPaletteDemoState {
    bool open = false;
    bool autoWidth = true;
    keel::f32 width = 660.0f;
    keel::i32 maxVisible = 8;
    keel::f32 backdropOpacity = 0.55f;
    bool closeOnBackdrop = true;
    bool closeOnEscape = true;
    bool showFooter = true;
    bool confineToStage = false;
    keel::FixedString<160> lastCommand = "None (press Ctrl+K or click Open)";
};

CommandPaletteDemoState s_cp;

} // namespace

void RenderCommandPaletteInspector(guikit::Context& ui, DemoContext& ctx) {
    if (auto el = guikit::Element(ui, {.label = "Control Size"})) {
        if (guikit::StepSlider(ui, ctx.controlsSize, {.showTicks = true, .showLabels = true, .includeNone = true}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Auto-Fit Content Width"})) {
        if (guikit::Switch(ui, s_cp.autoWidth).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Custom Palette Width", .disabled = s_cp.autoWidth})) {
        if (guikit::Slider(ui, s_cp.width, 380.0f, 820.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Max Visible Rows"})) {
        if (guikit::Slider(ui, s_cp.maxVisible, 3, 12, {.valueFormat = "%d rows", .step = 1}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Backdrop Dimming"})) {
        if (guikit::Slider(ui, s_cp.backdropOpacity, 0.0f, 0.90f, {.valueFormat = "%.2f", .step = 0.05f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Close on Backdrop Click"})) {
        if (guikit::Switch(ui, s_cp.closeOnBackdrop).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Close on Escape Key"})) {
        if (guikit::Switch(ui, s_cp.closeOnEscape).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Footer Hints"})) {
        if (guikit::Switch(ui, s_cp.showFooter).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Confine Palette to Stage"})) {
        if (guikit::Switch(ui, s_cp.confineToStage).changed) ui.InvalidateAll();
    }
}

void RenderCommandPaletteStage(guikit::Context& ui, DemoContext& ctx) {
    if (ui.ConsumeShortcut(guikit::Key::K, true)) {
        s_cp.open = !s_cp.open;
        ui.InvalidateAll();
    }

    if (auto stageCol = guikit::Column(ui, {
        .gap = 14.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center
    })) {
        if (guikit::Button(ui, {
            .text = "Open Command Palette (Ctrl+K)...",
            .icon = guikit::Icon::Search(),
            .variant = guikit::UiVariant::Primary
        }).clicked) {
            s_cp.open = true;
        }
        guikit::Text(ui, s_cp.lastCommand.as_ref(), {
            .role = guikit::TextRole::BodySmall,
            .variant = guikit::UiVariant::Secondary
        });
    }

    static const keel::StaticArray<guikit::CommandPaletteItem, 6> kPaletteCommands = {{
        {.label = "Configure Telemetry Endpoint", .category = "Dialog",    .description = "Open station profile dialog",         .shortcut = "Ctrl+E", .icon = guikit::Icon::Settings()},
        {.label = "Spawn Info Notification",      .category = "Toast",     .description = "Emit a streaming info toast",         .shortcut = "Alt+I",  .icon = guikit::Icon::Flash()},
        {.label = "Toggle Bookmark Target",       .category = "Workspace", .description = "Pin or unpin the active target node", .shortcut = "Ctrl+B", .icon = guikit::Icon::Check()},
        {.label = "Copy Station Configuration",   .category = "Clipboard", .description = "Serialize JSON profile to clipboard", .shortcut = "Ctrl+C", .icon = guikit::Icon::Copy()},
        {.label = "Reset DSP Pipeline",           .category = "Signal",    .description = "Clear FFT buffers & peak history",    .shortcut = "Alt+R",  .icon = guikit::Icon::Refresh()},
        {.label = "Delete Station Node...",       .category = "Danger",    .description = "Permanently purge active node",       .shortcut = "Del",    .icon = guikit::Icon::Trash()}
    }};

    auto cpRes = guikit::CommandPalette(ui, s_cp.open, kPaletteCommands, {
        .placeholder = "Type a command or search...",
        .width = s_cp.autoWidth ? keel::kNullOpt : keel::Optional<keel::f32>(s_cp.width),
        .maxVisibleItems = keel::as<keel::u32>(keel::max(1, s_cp.maxVisible)),
        .backdropOpacity = s_cp.backdropOpacity,
        .closeOnBackdropClick = s_cp.closeOnBackdrop,
        .closeOnEscape = s_cp.closeOnEscape,
        .showFooterHints = s_cp.showFooter,
        .confineToParent = s_cp.confineToStage,
        .size = ctx.controlsSize
    });

    if (cpRes.changed && cpRes.selectedIndex >= 0 && keel::as<keel::usize>(cpRes.selectedIndex) < kPaletteCommands.size()) {
        const auto& cmd = kPaletteCommands[keel::as<keel::usize>(cpRes.selectedIndex)];
        s_cp.lastCommand = keel::format_fixed<160>("Executed: [{}] {}", cmd.category, cmd.label);
    }
}

keel::String GenerateCommandPaletteCode(const DemoContext& ctx) {
    const auto szPrefix = FormatUiSizePrefix(ctx.controlsSize);
    const keel::String widthLine = s_cp.autoWidth
        ? keel::String::from("    .width = keel::kNullOpt,\n")
        : keel::format("    .width = {:.1f}f,\n", s_cp.width);
    return keel::format(
        "if (ui.ConsumeShortcut(guikit::Key::K, true) ||\n"
        "    guikit::Button(ui, {{.text = \"Open Command Palette (Ctrl+K)...\", .icon = guikit::Icon::Search()}}).clicked) {{\n"
        "    commandPaletteOpen = !commandPaletteOpen;\n"
        "}}\n\n"
        "auto cpRes = guikit::CommandPalette(ui, commandPaletteOpen, kPaletteCommands, {{\n"
        "    .placeholder = \"Type a command or search...\",\n"
        "{}"
        "    .maxVisibleItems = {},\n"
        "    .backdropOpacity = {:.2f}f,\n"
        "    .closeOnBackdropClick = {},\n"
        "    .closeOnEscape = {},\n"
        "    .showFooterHints = {},\n"
        "    .confineToParent = {}{}\n"
        "}});",
        widthLine,
        keel::max(1, s_cp.maxVisible),
        s_cp.backdropOpacity,
        s_cp.closeOnBackdrop ? "true" : "false",
        s_cp.closeOnEscape ? "true" : "false",
        s_cp.showFooter ? "true" : "false",
        s_cp.confineToStage ? "true" : "false",
        szPrefix
    );
}

} // namespace guikit::demo
