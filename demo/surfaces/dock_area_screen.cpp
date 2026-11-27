#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct DockAreaDemoState {
    bool initialized = false;
    guikit::DockLayout layout;

    // DockAreaOptions knobs
    keel::f32 sashSize = 6.0f;
    keel::f32 minPaneSize = 110.0f;
    bool resizable = true;
    bool reorderable = true;
    bool dockable = true;
    bool tearable = true;
    bool expandable = true;
    bool expandCloseOnBackdrop = true;
    bool expandCloseOnEscape = true;
    bool showTabBarForSingle = true;
    bool animateLayout = true;
    guikit::TabBarStyle tabStyle = guikit::TabBarStyle::Underline;

    // Workspace content state (preserved across tab moves & floating via IdScope!)
    keel::i32 selectedFile = 0;
    keel::f32 gainDb = -3.5f;
    keel::f32 cutoffHz = 2400.0f;
    keel::f32 drivePct = 42.0f;
    bool bypassFilter = false;
    bool autoGain = true;
    guikit::ReadoutState readoutState{};
    keel::String editorCode;
    keel::String eventLog;
    keel::u32 eventCount = 0u;

    void ApplyPresetIde() {
        layout.LoadLayout("H(0.22,T(0;files),V(0.70,H(0.72,T(0;editor,preview),T(0;inspector)),T(0;output,problems)))");
        LogEvent("Preset applied: IDE Classic");
    }

    void ApplyPresetThreeCol() {
        layout.LoadLayout("H(0.20,T(0;files),H(0.68,T(0;editor,preview),V(0.55,T(0;inspector),T(0;output,problems))))");
        LogEvent("Preset applied: 3-Column Studio");
    }

    void ApplyPresetFloating() {
        layout.LoadLayout("D2(H(0.24,T(0;files),V(0.72,T(0;editor,preview),T(0;output,problems)));F(inspector,480,48,290,320))");
        LogEvent("Preset applied: Floating Inspector");
    }

    void LogEvent(keel::StringRef msg) {
        ++eventCount;
        if (!eventLog.empty()) eventLog.push_back('\n');
        eventLog.append(keel::format("[{:02}] {}", eventCount, msg).as_ref());
        if (eventLog.size() > 2400u) {
            const keel::usize nl = eventLog.find('\n', eventLog.size() - 1600u);
            if (nl != keel::StringRef::npos) {
                eventLog.assign(eventLog.substr(nl + 1u));
            }
        }
    }

    void EnsureInit() {
        if (initialized) return;
        initialized = true;
        editorCode = keel::String(
            "#include \"guikit/guikit.hpp\"\n\n"
            "void RenderWorkspace(guikit::Context& ui, guikit::DockLayout& layout) {\n"
            "    if (auto dock = guikit::DockArea(ui, layout, {.tearable = true})) {\n"
            "        if (auto p = dock.Panel(\"files\", {.title = \"Explorer\"})) {\n"
            "            // Project tree...\n"
            "        }\n"
            "        if (auto p = dock.Panel(\"editor\", {.title = \"main.cpp\", .closable = true})) {\n"
            "            // Drag any tab to reorder, split, or tear into a floating card!\n"
            "        }\n"
            "    }\n"
            "}\n"
        );
        ApplyPresetIde();
    }
};

DockAreaDemoState s_dock;

} // namespace

void RenderDockAreaInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    s_dock.EnsureInit();

    if (auto el = guikit::Element(ui, {.label = "Workspace Presets", .layout = guikit::ElementLayout::Stacked})) {
        if (auto wrap = guikit::Wrap(ui, {.gap = 6.0f})) {
            if (guikit::Button(ui, {.text = "IDE Classic", .variant = guikit::UiVariant::Secondary}).clicked) {
                s_dock.ApplyPresetIde();
                ui.InvalidateAll();
            }
            if (guikit::Button(ui, {.text = "3-Column", .variant = guikit::UiVariant::Secondary}).clicked) {
                s_dock.ApplyPresetThreeCol();
                ui.InvalidateAll();
            }
            if (guikit::Button(ui, {.text = "Floating Card", .variant = guikit::UiVariant::Secondary}).clicked) {
                s_dock.ApplyPresetFloating();
                ui.InvalidateAll();
            }
            if (guikit::Button(ui, {.text = "Reset", .icon = guikit::Icon::Refresh(), .style = guikit::ButtonStyle::Ghost}).clicked) {
                s_dock.ApplyPresetIde();
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Tab Bar Style", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_dock.tabStyle, {"Underline", "Pill", "Card", "Folder"}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Sash Thickness"})) {
        if (guikit::Slider(ui, s_dock.sashSize, 4.0f, 12.0f, {.valueFormat = "%.0f dp", .step = 1.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Min Pane Size"})) {
        if (guikit::Slider(ui, s_dock.minPaneSize, 72.0f, 180.0f, {.valueFormat = "%.0f dp", .step = 8.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Resizable Sashes", .description = "Drag split dividers (double-click resets to 50%)"})) {
        if (guikit::Switch(ui, s_dock.resizable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Reorderable Tabs", .description = "Drag tabs left/right inside a tab bar"})) {
        if (guikit::Switch(ui, s_dock.reorderable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Dockable Splits", .description = "Drop on 5-way compass or outer 4 edges"})) {
        if (guikit::Switch(ui, s_dock.dockable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Tearable Floating", .description = "Drag tab outside DockArea or click Float button in tab bar"})) {
        if (guikit::Switch(ui, s_dock.tearable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Expandable Panels", .description = "Double-click a tab or click Expand for Modal / Fullscreen"})) {
        if (guikit::Switch(ui, s_dock.expandable).changed) ui.InvalidateAll();
    }
    if (s_dock.expandable) {
        if (auto el = guikit::Element(ui, {.label = "Close Expand on Backdrop", .description = "Clicking dimmed backdrop returns expanded panel to dock"})) {
            if (guikit::Switch(ui, s_dock.expandCloseOnBackdrop).changed) ui.InvalidateAll();
        }
        if (auto el = guikit::Element(ui, {.label = "Close Expand on Escape", .description = "Pressing Escape returns expanded panel to dock"})) {
            if (guikit::Switch(ui, s_dock.expandCloseOnEscape).changed) ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Show Single Tab Bar"})) {
        if (guikit::Switch(ui, s_dock.showTabBarForSingle).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Animate Layout"})) {
        if (guikit::Switch(ui, s_dock.animateLayout).changed) ui.InvalidateAll();
    }

    // Closed panels recovery buttons
    const auto closed = s_dock.layout.ClosedPanels();
    if (!closed.empty()) {
        guikit::Heading(ui, {
            .label = "Closed Panels",
            .level = guikit::HeadingLevel::H6,
            .separator = guikit::HeadingSeparator::Bottom
        });
        if (auto wrap = guikit::Wrap(ui, {.gap = 6.0f})) {
            for (keel::usize i = 0; i < closed.size(); ++i) {
                auto sc = ui.IdScope(closed[i].as_ref());
                const keel::StringRef lbl = ui.Format("Reopen {}", closed[i].as_ref());
                if (guikit::Button(ui, {.text = lbl, .icon = guikit::Icon::Plus(), .variant = guikit::UiVariant::Info}).clicked) {
                    s_dock.layout.ReopenPanel(closed[i].as_ref());
                    s_dock.LogEvent(ui.Format("Reopened panel '{}'", closed[i].as_ref()));
                    ui.InvalidateAll();
                    break;
                }
            }
        }
    }
}

void RenderDockAreaStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    s_dock.EnsureInit();

    if (auto dock = guikit::DockArea(ui, s_dock.layout, {
        .sashSize = s_dock.sashSize,
        .minPaneSize = s_dock.minPaneSize,
        .resizable = s_dock.resizable,
        .reorderable = s_dock.reorderable,
        .dockable = s_dock.dockable,
        .tearable = s_dock.tearable,
        .expandable = s_dock.expandable,
        .expandCloseOnBackdrop = s_dock.expandCloseOnBackdrop,
        .expandCloseOnEscape = s_dock.expandCloseOnEscape,
        .showTabBarForSingle = s_dock.showTabBarForSingle,
        .animateLayout = s_dock.animateLayout,
        .tabStyle = s_dock.tabStyle
    })) {
        if (!dock.ClosedPanelId().empty()) {
            s_dock.LogEvent(ui.Format("Closed panel '{}'", dock.ClosedPanelId()));
        }

        // 1. Explorer panel (TreeView)
        if (auto p = dock.Panel("files", {
            .title = "Explorer",
            .icon = guikit::Icon::Folder(),
            .tooltip = "Project files (drag tab to move or detach)",
            .closable = false,
            .padding = guikit::Padding::All(4.0f)
        })) {
            if (auto tree = guikit::TreeView(ui, {
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch,
                .size = guikit::UiSize::Sm
            })) {
                if (auto root = guikit::TreeItem(ui, {.label = "guikit", .icon = guikit::Icon::Folder(), .defaultOpen = true})) {
                    if (auto surf = guikit::TreeItem(ui, {.label = "surfaces", .icon = guikit::Icon::Folder(), .defaultOpen = true})) {
                        if (guikit::TreeItem(ui, {.label = "dock_area.hpp", .icon = guikit::Icon::Edit(), .selected = s_dock.selectedFile == 0}).response.clicked) {
                            s_dock.selectedFile = 0;
                            s_dock.layout.SetActivePanel("editor");
                        }
                        if (guikit::TreeItem(ui, {.label = "dock_area.cpp", .icon = guikit::Icon::Edit(), .selected = s_dock.selectedFile == 1}).response.clicked) {
                            s_dock.selectedFile = 1;
                            s_dock.layout.SetActivePanel("editor");
                        }
                        if (guikit::TreeItem(ui, {.label = "bottom_panel.cpp", .icon = guikit::Icon::Edit(), .selected = s_dock.selectedFile == 2}).response.clicked) {
                            s_dock.selectedFile = 2;
                            s_dock.layout.SetActivePanel("editor");
                        }
                    }
                    if (auto style = guikit::TreeItem(ui, {.label = "style", .icon = guikit::Icon::Folder(), .defaultOpen = true})) {
                        if (guikit::TreeItem(ui, {.label = "theme.cpp", .icon = guikit::Icon::Settings(), .selected = s_dock.selectedFile == 3}).response.clicked) {
                            s_dock.selectedFile = 3;
                        }
                        if (guikit::TreeItem(ui, {.label = "style_painter.cpp", .icon = guikit::Icon::Eye(), .selected = s_dock.selectedFile == 4}).response.clicked) {
                            s_dock.selectedFile = 4;
                        }
                    }
                    if (guikit::TreeItem(ui, {.label = "CMakeLists.txt", .icon = guikit::Icon::Info(), .selected = s_dock.selectedFile == 5}).response.clicked) {
                        s_dock.selectedFile = 5;
                    }
                }
            }
        }

        // 2. Code Editor panel
        static constexpr keel::StaticArray<keel::StringRef, 6> kFileNames = {
            "dock_area.hpp", "dock_area.cpp", "bottom_panel.cpp", "theme.cpp", "style_painter.cpp", "CMakeLists.txt"
        };
        const keel::usize fi = keel::clamp<keel::usize>(keel::as<keel::usize>(s_dock.selectedFile), 0u, kFileNames.size() - 1u);
        if (auto p = dock.Panel("editor", {
            .title = kFileNames[fi],
            .icon = guikit::Icon::Edit(),
            .tooltip = "Main code editor tab",
            .closable = true,
            .padding = guikit::Padding::All(4.0f)
        })) {
            guikit::TextAreaOptions edOpts{};
            edOpts.showLineNumbers = true;
            edOpts.monospace = true;
            edOpts.size = guikit::UiSize::Sm;
            edOpts.width = guikit::SizeMode::Stretch;
            edOpts.height = guikit::SizeMode::Stretch;
            edOpts.syntax = guikit::SyntaxConfig::Cpp();
            guikit::TextArea(ui, s_dock.editorCode, edOpts);
        }

        // 3. Live Preview panel
        if (auto p = dock.Panel("preview", {
            .title = "Live Preview",
            .icon = guikit::Icon::Eye(),
            .badge = "Live",
            .closable = true,
            .padding = guikit::Padding::All(14.0f),
            .gap = 10.0f,
            .scrollable = true
        })) {
            guikit::Heading(ui, {
                .label = "Signal Chain Monitor",
                .description = "Interactive controls inside a dockable tab — state stays intact when dragged or floated.",
                .separator = guikit::HeadingSeparator::Bottom
            });
            if (auto row = guikit::Row(ui, {.gap = 14.0f, .align = guikit::Alignment::Center})) {
                guikit::Knob(ui, s_dock.gainDb, -24.0f, 12.0f, {.valueFormat = "%.1f dB"});
                guikit::Knob(ui, s_dock.drivePct, 0.0f, 100.0f, {.valueFormat = "%.0f %%"});
                static constexpr keel::StaticArray<guikit::ReadoutUnit, 2> kFreqUnits = {{
                    {.label = "Hz",  .factor = 1.0},
                    {.label = "kHz", .factor = 0.001}
                }};
                const keel::StaticArray<guikit::ReadoutChannel, 1> kChans = {{
                    {
                        .label = "Filter Cutoff",
                        .headerLabel = "CUTOFF",
                        .value = s_dock.cutoffHz,
                        .units = kFreqUnits,
                        .defaultPrecision = 0,
                        .variant = guikit::UiVariant::Primary,
                        .showAction = false
                    }
                }};
                guikit::Readout(ui, s_dock.readoutState, kChans, {.minWidth = 156.0f});
            }
            guikit::ProgressBar(ui, s_dock.drivePct, 0.0, 100.0, {.valueFormat = "%.0f %%", .width = guikit::SizeMode::Stretch});
            guikit::LevelMeter(ui, s_dock.gainDb, -24.0, 12.0, {.width = guikit::SizeMode::Stretch});
        }

        // 4. Inspector panel (can be floated or docked!)
        if (auto p = dock.Panel("inspector", {
            .title = "Inspector",
            .icon = guikit::Icon::Settings(),
            .closable = true,
            .padding = guikit::Padding::All(12.0f),
            .gap = 8.0f,
            .scrollable = true
        })) {
            if (auto el = guikit::Element(ui, {.label = "Output Gain"})) {
                guikit::Slider(ui, s_dock.gainDb, -24.0f, 12.0f, {.valueFormat = "%.1f dB", .step = 0.5f});
            }
            if (auto el = guikit::Element(ui, {.label = "Filter Cutoff"})) {
                guikit::Slider(ui, s_dock.cutoffHz, 120.0f, 12000.0f, {.valueFormat = "%.0f Hz", .step = 40.0f});
            }
            if (auto el = guikit::Element(ui, {.label = "Bypass Filter"})) {
                guikit::Switch(ui, s_dock.bypassFilter);
            }
            if (auto el = guikit::Element(ui, {.label = "Auto Gain"})) {
                guikit::Switch(ui, s_dock.autoGain);
            }
            if (!p.IsFloating() && s_dock.tearable) {
                if (guikit::Button(ui, {
                    .text = "Detach as Floating Card",
                    .icon = guikit::Icon::Copy(),
                    .size = guikit::UiSize::Sm,
                    .variant = guikit::UiVariant::Secondary
                }).clicked) {
                    s_dock.layout.FloatPanel("inspector", guikit::Rect(440.0f, 48.0f, 290.0f, 300.0f));
                    s_dock.LogEvent("Detached 'inspector' as floating card");
                    ui.InvalidateAll();
                }
            }
        }

        // 5. Output Log panel
        const keel::StringRef outBadge = ui.Format("{}", s_dock.eventCount);
        if (auto p = dock.Panel("output", {
            .title = "Output",
            .icon = guikit::Icon::Info(),
            .badge = outBadge,
            .closable = true,
            .padding = guikit::Padding::All(4.0f)
        })) {
            guikit::TextArea(ui, s_dock.eventLog.as_ref(), {
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

        // 6. Problems panel
        if (auto p = dock.Panel("problems", {
            .title = "Problems",
            .icon = guikit::Icon::Warning(),
            .badge = "3",
            .closable = true,
            .padding = guikit::Padding::All(10.0f),
            .gap = 6.0f,
            .scrollable = true
        })) {
            struct Diag { keel::StringRef tag; guikit::UiVariant var; keel::StringRef loc; keel::StringRef msg; };
            static constexpr keel::StaticArray<Diag, 3> kDiags = {{
                {"INFO", guikit::UiVariant::Info,    "dock_area.cpp:1", "Drag any tab onto the 5-way compass or outer tiles (drop in empty space to cancel)"},
                {"INFO", guikit::UiVariant::Success, "dock_area.cpp:2", "Double-click any tab to expand into a Modal overlay, then toggle Fullscreen"},
                {"WARN", guikit::UiVariant::Warning, "dock_area.cpp:3", "Use the tab bar Float button or drag outside DockArea to detach as a floating card"}
            }};
            for (keel::usize i = 0; i < kDiags.size(); ++i) {
                auto sc = ui.IdScope(i);
                if (auto row = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Alignment::Center})) {
                    guikit::Tag(ui, kDiags[i].tag, {.variant = kDiags[i].var, .style = guikit::TagStyle::Soft, .size = guikit::UiSize::Xs});
                    guikit::Text(ui, kDiags[i].loc, {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Secondary});
                    guikit::Text(ui, kDiags[i].msg, {.role = guikit::TextRole::BodySmall});
                }
            }
        }
    }
}

keel::String GenerateDockAreaCode(const DemoContext& /*ctx*/) {
    s_dock.EnsureInit();
    static constexpr keel::StaticArray<keel::StringRef, 4> kTabStyleNames = {
        "Underline", "Pill", "Card", "Folder"
    };
    const keel::StringRef styleName = kTabStyleNames[keel::as<keel::usize>(s_dock.tabStyle)];
    return keel::format(
        "static guikit::DockLayout layout;\n"
        "// layout.LoadLayout(\"{}\");\n\n"
        "if (auto dock = guikit::DockArea(ui, layout, {{\n"
        "    .sashSize = {:.0f}.0f,\n"
        "    .minPaneSize = {:.0f}.0f,\n"
        "    .resizable = {},\n"
        "    .reorderable = {},\n"
        "    .dockable = {},\n"
        "    .tearable = {},\n"
        "    .expandable = {},\n"
        "    .expandCloseOnBackdrop = {},\n"
        "    .expandCloseOnEscape = {},\n"
        "    .showTabBarForSingle = {},\n"
        "    .animateLayout = {},\n"
        "    .tabStyle = guikit::TabBarStyle::{}\n"
        "}})) {{\n"
        "    if (auto p = dock.Panel(\"files\", {{.title = \"Explorer\", .icon = guikit::Icon::Folder()}})) {{\n"
        "        // Explorer TreeView...\n"
        "    }}\n"
        "    if (auto p = dock.Panel(\"editor\", {{.title = \"main.cpp\", .closable = true}})) {{\n"
        "        // Code Editor...\n"
        "    }}\n"
        "    if (auto p = dock.Panel(\"output\", {{.title = \"Output\", .badge = \"12\", .closable = true}})) {{\n"
        "        // Output Log...\n"
        "    }}\n"
        "}}\n",
        s_dock.layout.SaveLayout().as_ref(),
        s_dock.sashSize,
        s_dock.minPaneSize,
        s_dock.resizable ? "true" : "false",
        s_dock.reorderable ? "true" : "false",
        s_dock.dockable ? "true" : "false",
        s_dock.tearable ? "true" : "false",
        s_dock.expandable ? "true" : "false",
        s_dock.expandCloseOnBackdrop ? "true" : "false",
        s_dock.expandCloseOnEscape ? "true" : "false",
        s_dock.showTabBarForSingle ? "true" : "false",
        s_dock.animateLayout ? "true" : "false",
        styleName
    );
}

} // namespace guikit::demo
