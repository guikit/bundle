#include "../demo_context.hpp"
#include "guikit/panels/menu_bar.hpp"

namespace guikit::demo {

namespace {

struct MenuBarDemoState {
    guikit::MenuBarStyle style = guikit::MenuBarStyle::Header;
    bool showRightPill = true;
    bool disabled = false;
    float maxMenuHeight = 260.0f;

    // Interactive workspace state driven by MenuBar & ContextMenu
    bool showGrid = true;
    bool showMinimap = true;
    bool showRulers = false;
    keel::i32 densityIdx = 1;
    keel::i32 zoomPct = 100;
    keel::SmallString<128> lastAction{"Click File / Edit / View / Tools / Help or right-click below"};

    // Right-click RAII ContextMenu state
    bool ctxMenuOpen = false;
    guikit::Vec2 ctxMenuPos{0.0f, 0.0f};
};

MenuBarDemoState s_mb;

constexpr keel::StaticArray<keel::StringRef, 3> kStyleLabels = {
    "Header", "Surface", "Compact"
};

keel::StringRef StyleName(guikit::MenuBarStyle st) noexcept {
    switch (st) {
        case guikit::MenuBarStyle::Surface: return "Surface";
        case guikit::MenuBarStyle::Compact: return "Compact";
        default:                            return "Header";
    }
}

void RecordAction(guikit::Context& ui, keel::StringRef action, guikit::UiVariant var = guikit::UiVariant::Info) {
    s_mb.lastAction.assign(action);
    ui.ShowToast({.title = "Menu Command", .message = action, .variant = var, .duration = 2.6f});
    ui.InvalidateAll();
}

} // namespace

void RenderMenuBarInspector(guikit::Context& ui, DemoContext&) {
    keel::i32 styleIdx = static_cast<keel::i32>(s_mb.style);
    if (auto el = ui.Element({.label = "MenuBar Style"})) {
        if (ui.Segmented(styleIdx, kStyleLabels).changed) {
            s_mb.style = static_cast<guikit::MenuBarStyle>(styleIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = ui.Element({
        .label = "Max Dropdown Height",
        .description = "Long menus (e.g. Tools) scroll smoothly when taller than this limit"
    })) {
        if (ui.Slider(s_mb.maxMenuHeight, 140.0f, 420.0f, {
            .valueFormat = "%.0f dp",
            .step = 10.0f
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = ui.Element({.label = "Trailing Status Tag"})) {
        if (ui.Switch(s_mb.showRightPill).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Disabled"})) {
        if (ui.Switch(s_mb.disabled).changed) ui.InvalidateAll();
    }
}

void RenderMenuBarStage(guikit::Context& ui, DemoContext& ctx) {
    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Sm : ctx.controlsSize;
    guikit::ContextMenuOptions dropOpts{};
    dropOpts.size = effSize;
    dropOpts.maxHeight = s_mb.maxMenuHeight;

    if (auto root = ui.Column({
        .gap = 0.0f,
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch
    })) {
        // 1. Desktop MenuBar with hover-scrubbing, cascading SubMenus, Sections, Checkboxes, Radios & Scrolling
        if (auto mb = ui.MenuBar({
            .style = s_mb.style,
            .size = effSize,
            .disabled = s_mb.disabled || ctx.elemDisabled
        })) {
            if (auto file = mb.Menu("File", dropOpts)) {
                if (file.Item("New Workspace", {.shortcut = "Ctrl+N", .icon = guikit::Icon::Plus()})) {
                    RecordAction(ui, "File -> New Workspace", guikit::UiVariant::Primary);
                }
                if (file.Item("Open Project...", {.shortcut = "Ctrl+O", .icon = guikit::Icon::Folder()})) {
                    RecordAction(ui, "File -> Open Project...");
                }
                if (auto recent = file.SubMenu("Recent Projects")) {
                    if (recent.Item("signal_analyzer.gk")) RecordAction(ui, "Opened signal_analyzer.gk");
                    if (recent.Item("marine_chart.gk"))    RecordAction(ui, "Opened marine_chart.gk");
                    if (recent.Item("flight_deck.gk"))     RecordAction(ui, "Opened flight_deck.gk");
                    recent.Separator();
                    if (recent.Item("Clear Recent", {.variant = guikit::UiVariant::Danger})) {
                        RecordAction(ui, "Cleared recent history", guikit::UiVariant::Warning);
                    }
                }
                file.Separator();
                if (auto exp = file.SubMenu("Export", {.icon = guikit::Icon::Save()})) {
                    if (exp.Item("PNG Image", {.shortcut = "Ctrl+E"})) RecordAction(ui, "Export -> PNG Image", guikit::UiVariant::Success);
                    if (exp.Item("SVG Vector"))                        RecordAction(ui, "Export -> SVG Vector", guikit::UiVariant::Success);
                    exp.Separator();
                    if (auto data = exp.SubMenu("Structured Data")) {
                        if (data.Item("CSV Table"))   RecordAction(ui, "Export -> Structured Data -> CSV Table", guikit::UiVariant::Success);
                        if (data.Item("JSON Bundle")) RecordAction(ui, "Export -> Structured Data -> JSON Bundle", guikit::UiVariant::Success);
                    }
                }
                file.Separator();
                if (file.Item("Save", {.shortcut = "Ctrl+S", .icon = guikit::Icon::Save()})) {
                    RecordAction(ui, "File -> Save", guikit::UiVariant::Success);
                }
                if (file.Item("Close Window", {.shortcut = "Alt+F4", .icon = guikit::Icon::Close(), .variant = guikit::UiVariant::Danger})) {
                    RecordAction(ui, "File -> Close Window", guikit::UiVariant::Danger);
                }
            }

            if (auto edit = mb.Menu("Edit", dropOpts)) {
                if (edit.Item("Undo", {.shortcut = "Ctrl+Z", .icon = guikit::Icon::ArrowLeft()}))  RecordAction(ui, "Edit -> Undo");
                if (edit.Item("Redo", {.shortcut = "Ctrl+Y", .icon = guikit::Icon::ArrowRight(), .disabled = true})) {}
                edit.Separator();
                if (edit.Item("Cut",   {.shortcut = "Ctrl+X"}))                                     RecordAction(ui, "Edit -> Cut");
                if (edit.Item("Copy",  {.shortcut = "Ctrl+C", .icon = guikit::Icon::Copy()}))       RecordAction(ui, "Edit -> Copy");
                if (edit.Item("Paste", {.shortcut = "Ctrl+V"}))                                     RecordAction(ui, "Edit -> Paste");
                edit.Separator();
                if (edit.Item("Find & Replace...", {.shortcut = "Ctrl+H", .icon = guikit::Icon::Replace()})) {
                    RecordAction(ui, "Edit -> Find & Replace");
                }
            }

            if (auto view = mb.Menu("View", dropOpts)) {
                view.Section("WORKSPACE");
                (void)view.Checkbox("Show Grid", s_mb.showGrid, {.keepOpen = true});
                (void)view.Checkbox("Show Minimap", s_mb.showMinimap, {.keepOpen = true});
                (void)view.Checkbox("Show Rulers", s_mb.showRulers, {.keepOpen = true});
                view.Separator();
                view.Section("DENSITY");
                (void)view.Radio("Compact", s_mb.densityIdx, 0);
                (void)view.Radio("Standard", s_mb.densityIdx, 1);
                (void)view.Radio("Comfortable", s_mb.densityIdx, 2);
                view.Separator();
                if (auto zoom = view.SubMenu("Zoom Level", {.icon = guikit::Icon::Search()})) {
                    static constexpr keel::i32 kZooms[] = {50, 75, 100, 125, 150, 200};
                    for (keel::i32 z : kZooms) {
                        if (zoom.Radio(ui.Format("{}%", z), s_mb.zoomPct, z)) {
                            RecordAction(ui, ui.Format("Zoom set to {}%", z));
                        }
                    }
                }
            }

            if (auto tools = mb.Menu("Tools", dropOpts)) {
                tools.Section("SCROLLABLE TOOLBOX");
                static constexpr keel::StringRef kTools[] = {
                    "Profiler", "Frame Inspector", "Memory Map", "Asset Browser",
                    "Shader Console", "Audio Mixer", "Network Trace", "Theme Editor",
                    "Font Atlas", "Glyph Metrics", "Layout Debugger", "Keymap Bindings",
                    "Plugin Manager", "Package Cache", "Diagnostic Export", "Reset Settings"
                };
                for (keel::StringRef name : kTools) {
                    if (tools.Item(name)) {
                        RecordAction(ui, ui.Format("Tools -> {}", name));
                    }
                }
            }

            if (auto help = mb.Menu("Help", dropOpts)) {
                if (help.Item("Keyboard Navigation", {.icon = guikit::Icon::Info()})) {
                    RecordAction(ui, "Arrows, Enter, Esc, and first-letter type-ahead supported");
                }
                help.Separator();
                if (help.Item("About GuiKit")) {
                    RecordAction(ui, ui.Format("GuiKit v{}", guikit::Version::string), guikit::UiVariant::Primary);
                }
            }

            if (s_mb.showRightPill) {
                mb.Spacer();
                (void)ui.Tag(ui.Format("Zoom {}%", s_mb.zoomPct), {
                    .variant = guikit::UiVariant::Info,
                    .style = guikit::TagStyle::Soft,
                    .size = guikit::UiSize::Xs
                });
            }
        }

        // 2. Clean workspace area supporting Right-Click RAII ContextMenu with cascading submenus
        if (auto body = ui.Column({
            .gap = 8.0f,
            .padding = guikit::Padding::All(18.0f),
            .align = guikit::Alignment::Center,
            .justify = guikit::Justify::Center,
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch
        })) {
            const auto bodyRes = body.GetResponse();
            if (bodyRes.rightClicked) {
                s_mb.ctxMenuPos = ui.GetMousePos();
                s_mb.ctxMenuOpen = true;
                ui.InvalidateAll();
            }

            ui.Text(s_mb.lastAction.as_ref(), {
                .role = guikit::TextRole::BodySmall,
                .variant = guikit::UiVariant::Muted
            });

            // Right-click RAII ContextMenu using the exact same cascading engine
            if (auto ctxMenu = ui.ContextMenu(s_mb.ctxMenuOpen, {
                .title = "WORKSPACE",
                .anchorPos = s_mb.ctxMenuPos,
                .maxHeight = s_mb.maxMenuHeight,
                .size = effSize
            })) {
                if (auto ins = ctxMenu.SubMenu("Insert Node", {.icon = guikit::Icon::Plus()})) {
                    if (ins.Item("Signal Source")) RecordAction(ui, "Inserted Signal Source");
                    if (ins.Item("FFT Analyzer"))  RecordAction(ui, "Inserted FFT Analyzer");
                    if (ins.Item("Scope Sink"))    RecordAction(ui, "Inserted Scope Sink");
                }
                ctxMenu.Separator();
                (void)ctxMenu.Checkbox("Show Grid", s_mb.showGrid, {.keepOpen = true});
                (void)ctxMenu.Checkbox("Show Rulers", s_mb.showRulers, {.keepOpen = true});
                ctxMenu.Separator();
                if (ctxMenu.Item("Clear Selection", {.icon = guikit::Icon::Trash(), .variant = guikit::UiVariant::Danger})) {
                    RecordAction(ui, "Selection cleared", guikit::UiVariant::Danger);
                }
            }
        }
    }
}

keel::String GenerateMenuBarCode(const DemoContext&) {
    return keel::format(
        "if (auto mb = ui.MenuBar({{.style = guikit::MenuBarStyle::{}}})) {{\n"
        "    if (auto file = mb.Menu(\"File\")) {{\n"
        "        if (file.Item(\"New\", {{.shortcut = \"Ctrl+N\", .icon = guikit::Icon::Plus()}})) {{}}\n"
        "        if (auto exp = file.SubMenu(\"Export\", {{.icon = guikit::Icon::Save()}})) {{\n"
        "            if (exp.Item(\"PNG Image\")) {{}}\n"
        "            if (exp.Item(\"SVG Vector\")) {{}}\n"
        "        }}\n"
        "        file.Separator();\n"
        "        if (file.Item(\"Exit\", {{.variant = guikit::UiVariant::Danger}})) {{}}\n"
        "    }}\n"
        "    if (auto view = mb.Menu(\"View\")) {{\n"
        "        view.Section(\"WORKSPACE\");\n"
        "        view.Checkbox(\"Show Grid\", showGrid, {{.keepOpen = true}});\n"
        "        view.Radio(\"Standard\", densityIdx, 1);\n"
        "    }}\n"
        "    mb.Spacer();\n"
        "    ui.Tag(\"Ready\", {{.variant = guikit::UiVariant::Success, .size = guikit::UiSize::Xs}});\n"
        "}}",
        StyleName(s_mb.style)
    );
}

} // namespace guikit::demo
