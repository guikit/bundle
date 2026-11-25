#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct SidebarState {
    keel::i32 activityIdx = 0;
    keel::i32 menuIdx = 0;
    bool floatingFrame = true;
    bool showActivityBar = false;
    guikit::ActivityBarStyle activityStyle = guikit::ActivityBarStyle::Indicator;
    guikit::SidebarMode mode = guikit::SidebarMode::Expanded;
    guikit::SidebarMenuStyle menuStyle = guikit::SidebarMenuStyle::Pill;
    guikit::SidebarSide side = guikit::SidebarSide::Left;
    bool resizable = true;
    bool allowCompact = true;
    bool allowCollapse = true;
    bool showBadges = true;
    bool showSections = true;
    bool autoWidth = false;
    keel::f32 sidebarWidth = 210.0f;

    // Optional Workbench shell companions
    bool gearMenuOpen = false;
    bool showBottomPanel = false;
    keel::i32 bottomTabIdx = 0;
    bool showStatusBar = false;
    guikit::StatusBarStyle statusBarStyle = guikit::StatusBarStyle::Surface;
};
SidebarState s_state;

constexpr keel::StringRef ModeEnumName(guikit::SidebarMode m) noexcept {
    switch (m) {
        case guikit::SidebarMode::Compact:   return "Compact";
        case guikit::SidebarMode::Collapsed: return "Collapsed";
        default:                             return "Expanded";
    }
}

constexpr keel::StringRef StatusStyleName(guikit::StatusBarStyle s) noexcept {
    switch (s) {
        case guikit::StatusBarStyle::Header: return "Header";
        case guikit::StatusBarStyle::Accent: return "Accent";
        default:                             return "Surface";
    }
}
} // namespace

void RenderSidebarInspector(guikit::Context& ui, DemoContext& ctx) {
    if (auto el = guikit::Element(ui, {.label = "Sidebar Mode", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_state.mode, {"Expanded", "Compact", "Collapsed"}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Menu Size"})) {
        if (guikit::StepSlider(ui, ctx.controlsSize, {.showTicks = true, .showLabels = true, .includeNone = true}).changed) ui.InvalidateAll();
    }
    ctx.RenderVariantPicker(ui);

    static constexpr keel::StaticArray<keel::StringRef, 2> sbMenuStyleNames = {"Pill", "Indicator"};
    keel::usize sbStyleIdx = (s_state.menuStyle == guikit::SidebarMenuStyle::Indicator) ? 1 : 0;
    if (auto el = guikit::Element(ui, {.label = "Menu Style"})) {
        if (guikit::Segmented(ui, sbStyleIdx, sbMenuStyleNames).changed) {
            s_state.menuStyle = (sbStyleIdx == 1) ? guikit::SidebarMenuStyle::Indicator : guikit::SidebarMenuStyle::Pill;
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 2> sbSideNames = {"Left", "Right"};
    keel::usize sbSideIdx = (s_state.side == guikit::SidebarSide::Right) ? 1 : 0;
    if (auto el = guikit::Element(ui, {.label = "Dock Side"})) {
        if (guikit::Segmented(ui, sbSideIdx, sbSideNames).changed) {
            s_state.side = (sbSideIdx == 1) ? guikit::SidebarSide::Right : guikit::SidebarSide::Left;
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Mouse Resizable (Sash)",
        .description = "Drag inner border to resize or snap"
    })) {
        if (guikit::Switch(ui, s_state.resizable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {
        .label = "Allow Compact (Icons)",
        .description = "Snap/toggle to icons-only rail"
    })) {
        if (guikit::Switch(ui, s_state.allowCompact).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {
        .label = "Allow Collapse (0px)",
        .description = "Snap/toggle to completely minimized"
    })) {
        if (guikit::Switch(ui, s_state.allowCollapse).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Section Headers"})) {
        if (guikit::Switch(ui, s_state.showSections).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Badges"})) {
        if (guikit::Switch(ui, s_state.showBadges).changed) ui.InvalidateAll();
    }
    const bool notExpanded = (s_state.mode != guikit::SidebarMode::Expanded);
    if (auto el = guikit::Element(ui, {.label = "Auto-Fit Width", .disabled = notExpanded})) {
        if (guikit::Switch(ui, s_state.autoWidth, {.disabled = notExpanded}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Default Width", .disabled = notExpanded || s_state.autoWidth})) {
        if (guikit::Slider(ui, s_state.sidebarWidth, 140.0f, 320.0f, {.valueFormat = "%.0f px", .disabled = notExpanded || s_state.autoWidth}).changed) {
            ui.InvalidateAll();
        }
    }

    guikit::Heading(ui, {.label = "SHELL COMPANIONS", .level = guikit::HeadingLevel::H6, .separator = guikit::HeadingSeparator::Bottom});

    keel::usize frameIdx = s_state.floatingFrame ? 1 : 0;
    if (auto el = guikit::Element(ui, {.label = "Shell Frame", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, frameIdx, {"Docked (Flush)", "Floating (Island)"}).changed) {
            s_state.floatingFrame = (frameIdx == 1);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "ActivityBar Rail",
        .description = "Primary icon rail docked flush to Sidebar"
    })) {
        if (guikit::Switch(ui, s_state.showActivityBar).changed) ui.InvalidateAll();
    }

    if (s_state.showActivityBar) {
        static constexpr keel::StaticArray<keel::StringRef, 2> abStyleNames = {"Indicator", "Pill"};
        keel::usize abStyleIdx = (s_state.activityStyle == guikit::ActivityBarStyle::Pill) ? 1 : 0;
        if (auto el = guikit::Element(ui, {.label = "ActivityBar Style"})) {
            if (guikit::Segmented(ui, abStyleIdx, abStyleNames).changed) {
                s_state.activityStyle = (abStyleIdx == 1) ? guikit::ActivityBarStyle::Pill : guikit::ActivityBarStyle::Indicator;
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "BottomPanel Dock",
        .description = "Resizable bottom panel below workspace"
    })) {
        if (guikit::Switch(ui, s_state.showBottomPanel).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {
        .label = "StatusBar Strip",
        .description = "Compact status strip at window bottom"
    })) {
        if (guikit::Switch(ui, s_state.showStatusBar).changed) ui.InvalidateAll();
    }

    if (s_state.showStatusBar) {
        if (auto el = guikit::Element(ui, {.label = "StatusBar Style", .layout = guikit::ElementLayout::Stacked})) {
            if (guikit::Segmented(ui, s_state.statusBarStyle, {"Surface", "Header", "Accent"}).changed) {
                ui.InvalidateAll();
            }
        }
    }
}

void RenderSidebarStage(guikit::Context& ui, DemoContext& ctx) {
    const bool isFloating = s_state.floatingFrame;
    guikit::UiTheme stageTheme = ui.GetTheme(); // the zone keeps its own copy for the frame
    stageTheme.SetShellStyle(isFloating ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked);

    if (auto stageZone = ui.Zone({.theme = stageTheme})) {
        if (auto workbenchShell = guikit::Column(ui, {
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch
        })) {
            if (auto demoWorkspace = guikit::Box(ui, {
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                // 1. Optional VS Code style ActivityBar docked on the outer edge, linked to s_state.mode
                if (s_state.showActivityBar) {
                    if (auto bar = guikit::ActivityBar(ui, s_state.mode, {
                        .side = s_state.side,
                        .style = s_state.activityStyle,
                        .color = ctx.ResolveControlColor(),
                        .size = ctx.controlsSize
                    })) {
                        if (bar.Item(s_state.activityIdx, 0, {
                            .icon = guikit::Icon::Folder(),
                            .label = "Explorer",
                            .tooltip = "Explorer (Click active icon to toggle Sidebar)"
                        }).changed) {
                            s_state.menuIdx = 0;
                        }
                        if (bar.Item(s_state.activityIdx, 1, {
                            .icon = guikit::Icon::Search(),
                            .label = "Analytics",
                            .tooltip = "Search & Real-Time Analytics",
                            .badge = s_state.showBadges ? "2" : ""
                        }).changed) {
                            s_state.menuIdx = 1;
                        }
                        if (bar.Item(s_state.activityIdx, 2, {
                            .icon = guikit::Icon::Play(),
                            .label = "Deployments",
                            .tooltip = "Deployments & CI/CD Pipelines",
                            .badge = s_state.showBadges ? "3" : ""
                        }).changed) {
                            s_state.menuIdx = 2;
                        }

                        bar.Spacer();
                        bar.Separator();

                        const guikit::Response gearBtn = bar.Action({
                            .icon = guikit::Icon::Settings(),
                            .label = "Settings",
                            .tooltip = "Settings & Quick Actions",
                            .active = s_state.gearMenuOpen
                        });
                        if (gearBtn.clicked) {
                            s_state.gearMenuOpen = !s_state.gearMenuOpen;
                        }
                        const auto gearMenu = guikit::ContextMenu(ui, s_state.gearMenuOpen, {
                            {.label = "Command Palette...", .shortcut = "Ctrl+Shift+P", .icon = guikit::Icon::Search()},
                            {.label = "Settings",           .shortcut = "Ctrl+,",       .icon = guikit::Icon::Settings(), .separatorAfter = true},
                            {.label = "Toggle Sidebar",     .shortcut = "Ctrl+B",       .icon = guikit::Icon::ChevronLeft()},
                            {.label = "Toggle BottomPanel", .shortcut = "Ctrl+J",       .icon = guikit::Icon::ChevronUp()}
                        }, {
                            .anchorRect = gearBtn.rect,
                            .minWidth = 220.0f
                        });
                        if (gearMenu.clickedIndex == 1) {
                            s_state.menuIdx = 3;
                            if (s_state.mode == guikit::SidebarMode::Collapsed) {
                                s_state.mode = guikit::SidebarMode::Expanded;
                            }
                            ui.InvalidateAll();
                        } else if (gearMenu.clickedIndex == 2) {
                            s_state.mode = (s_state.mode == guikit::SidebarMode::Collapsed)
                                ? guikit::SidebarMode::Expanded
                                : guikit::SidebarMode::Collapsed;
                            ui.InvalidateAll();
                        } else if (gearMenu.clickedIndex == 3) {
                            s_state.showBottomPanel = !s_state.showBottomPanel;
                            ui.InvalidateAll();
                        }
                    }
                }

                // 2. Primary Sidebar docked flush next to ActivityBar (or on outer edge when standalone)
                if (auto sb = guikit::Sidebar(ui, s_state.mode, {
                    .side = s_state.side,
                    .width = s_state.autoWidth ? keel::kNullOpt : keel::Optional<keel::f32>(s_state.sidebarWidth),
                    .resizable = s_state.resizable,
                    .allowCompact = s_state.allowCompact,
                    .allowCollapse = s_state.allowCollapse,
                    .scrollable = true
                })) {
                    const keel::i32 prevIdx = s_state.menuIdx;
                    if (auto menu = guikit::SidebarMenu(ui, {
                        .style = s_state.menuStyle,
                        .variant = ctx.btnVariant,
                        .size = ctx.controlsSize
                    })) {
                        static constexpr keel::StaticArray<keel::StringRef, 3> actHeaders = {
                            "EXPLORER", "ANALYTICS", "PIPELINES"
                        };
                        if (s_state.showSections) {
                            menu.Section(s_state.showActivityBar
                                ? actHeaders[keel::as<keel::usize>(keel::clamp(s_state.activityIdx, 0, 2))]
                                : keel::StringRef("WORKSPACE"));
                        }
                        menu.Item(s_state.menuIdx, 0, {
                            .text = "Overview",
                            .icon = guikit::Icon::Folder(),
                            .badge = s_state.showBadges ? "3" : ""
                        });
                        menu.Item(s_state.menuIdx, 1, {
                            .text = "Analytics",
                            .icon = guikit::Icon::Search(),
                            .badge = s_state.showBadges ? "New" : ""
                        });
                        menu.Item(s_state.menuIdx, 2, {
                            .text = "Deployments",
                            .icon = guikit::Icon::Play()
                        });
                        if (s_state.showSections) {
                            menu.Section("SYSTEM");
                        }
                        menu.Item(s_state.menuIdx, 3, {
                            .text = "Settings",
                            .icon = guikit::Icon::Settings()
                        });
                    }
                    if (s_state.menuIdx != prevIdx) {
                        if (s_state.menuIdx <= 2) {
                            s_state.activityIdx = s_state.menuIdx;
                        }
                        ui.InvalidateAll();
                    }
                }

                // 3. Minimal Center Area (keeps visual focus on the Sidebar)
                if (auto mainPane = guikit::Column(ui, {
                    .padding = guikit::Padding::All(24.0f),
                    .align = guikit::Alignment::Center,
                    .justify = guikit::Justify::Center,
                    .width = guikit::SizeMode::Stretch,
                    .height = guikit::SizeMode::Stretch
                })) {
                    static constexpr keel::StaticArray<keel::StringRef, 4> pageTitles = {
                        "Overview", "Analytics", "Deployments", "Settings"
                    };
                    guikit::Heading(ui, {
                        .label = pageTitles[keel::as<keel::usize>(keel::clamp(s_state.menuIdx, 0, 3))],
                        .description = "Drag the Sidebar border sash to resize or snap between Expanded, Compact, and Collapsed modes.",
                        .badge = {ModeEnumName(s_state.mode), ctx.ResolveControlColor()},
                        .level = guikit::HeadingLevel::H5
                    });
                }

                // 4. Resizable & Collapsible BottomPanel (docked below Main Pane)
                if (auto bp = guikit::BottomPanel(ui, s_state.showBottomPanel, {
                    .height = 145.0f,
                    .resizable = s_state.resizable,
                    .allowCollapse = s_state.allowCollapse
                })) {
                    if (auto bpTb = guikit::Toolbar(ui, {
                        .style = guikit::ToolbarStyle::Header,
                        .size = guikit::UiSize::Sm
                    })) {
                        bpTb.Item(s_state.bottomTabIdx, 0, {.text = "Output", .icon = guikit::Icon::Play()});
                        bpTb.Item(s_state.bottomTabIdx, 1, {
                            .text = "Problems",
                            .icon = guikit::Icon::Warning(),
                            .badge = s_state.showBadges ? "2" : ""
                        });
                        bpTb.Item(s_state.bottomTabIdx, 2, {.text = "Preview", .icon = guikit::Icon::Eye()});
                        bpTb.Spacer();
                        guikit::Text(ui, ui.Format("H: {:.0f}px", bp.Height()), {
                            .role = guikit::TextRole::LabelSmall,
                            .variant = guikit::UiVariant::Muted
                        });
                        if (bpTb.Action({
                            .text = "Close",
                            .tooltip = "Collapse BottomPanel",
                            .icon = guikit::Icon::Close(),
                            .square = true
                        }).clicked) {
                            s_state.showBottomPanel = false;
                            ui.InvalidateAll();
                        }
                    }

                    if (auto bpBody = guikit::Box(ui, {
                        .gap = 6.0f,
                        .width = guikit::SizeMode::Stretch,
                        .height = guikit::SizeMode::Stretch,
                        .padding = guikit::Padding::All(10.0f)
                    })) {
                        static constexpr keel::StaticArray<keel::StringRef, 3> tabContents = {
                            "[Build] Workspace compiled cleanly (0 warnings, 0 errors) in 142 ms.",
                            "[Warn] 2 layout rules adjusted to fit narrow viewport width.",
                            "[Preview] Drag the top border of this BottomPanel up/down to resize the panel."
                        };
                        guikit::Text(ui, tabContents[keel::as<keel::usize>(keel::clamp(s_state.bottomTabIdx, 0, 2))], {
                            .role = guikit::TextRole::BodySmall,
                            .variant = guikit::UiVariant::Secondary
                        });
                    }
                }
            }

            // 5. Optional Bottom StatusBar spanning the full workbench width
            if (s_state.showStatusBar) {
                if (auto sb = guikit::StatusBar(ui, {
                    .style = s_state.statusBarStyle,
                    .size = ctx.controlsSize
                })) {
                    sb.Item({
                        .text = "Workbench Ready",
                        .dotColor = ctx.previewTheme.GetVariant(guikit::UiVariant::Success).bg,
                        .color = guikit::StateColor::Success(),
                        .badge = s_state.showBadges ? "GPU" : ""
                    });
                    sb.Separator();
                    sb.Item({
                        .text = ui.Format("Sidebar: {}", ModeEnumName(s_state.mode)),
                        .icon = guikit::Icon::Folder()
                    });
                    sb.Spacer();
                    if (sb.Action({
                        .text = s_state.showBottomPanel ? "Hide Panel" : "Show Panel",
                        .icon = s_state.showBottomPanel ? guikit::Icon::ChevronDown() : guikit::Icon::ChevronUp(),
                        .tooltip = "Toggle BottomPanel (Ctrl+J)",
                        .active = s_state.showBottomPanel
                    }).clicked) {
                        s_state.showBottomPanel = !s_state.showBottomPanel;
                        ui.InvalidateAll();
                    }
                }
            }
        }
    }
}

keel::String GenerateSidebarCode(const DemoContext& ctx) {
    const keel::StringRef sideStr = (s_state.side == guikit::SidebarSide::Right) ? "Right" : "Left";
    keel::String out;

    keel::format_into(
        out,
        "// Theme-driven application shell framing (Docked vs Floating)\n"
        "theme.SetShellStyle(guikit::ShellStyle::{});\n"
        "ui.SetTheme(theme);\n\n",
        s_state.floatingFrame ? "Floating" : "Docked"
    );

    if (s_state.showActivityBar) {
        keel::format_into(
            out,
            "if (auto bar = guikit::ActivityBar(ui, sidebarMode, {{\n"
            "    .side = guikit::SidebarSide::{},\n"
            "    .style = guikit::ActivityBarStyle::{},\n"
            "    .color = guikit::StateColor::{}(){}\n"
            "}})) {{\n"
            "    bar.Item(activeTool, 0, {{.icon = guikit::Icon::Folder(), .tooltip = \"Explorer\"}});\n"
            "    bar.Item(activeTool, 1, {{.icon = guikit::Icon::Search(), .tooltip = \"Analytics\"{}}});\n"
            "    bar.Spacer();\n"
            "    bar.Separator();\n"
            "    auto gear = bar.Action({{.icon = guikit::Icon::Settings(), .tooltip = \"Settings\"}});\n"
            "}}\n\n",
            sideStr,
            s_state.activityStyle == guikit::ActivityBarStyle::Pill ? "Pill" : "Indicator",
            VariantName(ctx.btnVariant),
            FormatUiSizePrefix(ctx.controlsSize),
            s_state.showBadges ? ", .badge = \"2\"" : ""
        );
    }

    const auto widthExpr = s_state.autoWidth
        ? keel::FixedString<32>("keel::kNullOpt")
        : keel::format_fixed<32>("{:.1f}f", s_state.sidebarWidth);

    keel::format_into(
        out,
        "if (auto sb = guikit::Sidebar(ui, sidebarMode, {{\n"
        "    .side = guikit::SidebarSide::{},\n"
        "    .width = {},\n"
        "    .resizable = {},\n"
        "    .allowCompact = {},\n"
        "    .allowCollapse = {},\n"
        "    .scrollable = true\n"
        "}})) {{\n"
        "    if (auto menu = guikit::SidebarMenu(ui, {{\n"
        "        .style = guikit::SidebarMenuStyle::{},\n"
        "        .variant = guikit::UiVariant::{}{}\n"
        "    }})) {{\n"
        "        menu.Section(\"WORKSPACE\");\n"
        "        menu.Item(selected, 0, {{.text = \"Overview\", .icon = guikit::Icon::Folder(){}}});\n"
        "        menu.Item(selected, 1, {{.text = \"Analytics\", .icon = guikit::Icon::Search(){}}});\n"
        "    }}\n"
        "}}",
        sideStr,
        widthExpr,
        s_state.resizable ? "true" : "false",
        s_state.allowCompact ? "true" : "false",
        s_state.allowCollapse ? "true" : "false",
        s_state.menuStyle == guikit::SidebarMenuStyle::Indicator ? "Indicator" : "Pill",
        VariantName(ctx.btnVariant),
        FormatUiSizePrefix(ctx.controlsSize),
        s_state.showBadges ? ", .badge = \"3\"" : "",
        s_state.showBadges ? ", .badge = \"New\"" : ""
    );

    if (s_state.showBottomPanel) {
        keel::format_into(
            out,
            "\n\nif (auto bp = guikit::BottomPanel(ui, showBottomPanel, {{\n"
            "    .height = 180.0f,\n"
            "    .resizable = {},\n"
            "    .allowCollapse = {}\n"
            "}})) {{\n"
            "    if (auto tb = guikit::Toolbar(ui, {{.style = guikit::ToolbarStyle::Header}})) {{\n"
            "        tb.Item(activeTab, 0, {{.text = \"Output\", .icon = guikit::Icon::Play()}});\n"
            "        tb.Spacer();\n"
            "        if (tb.Action({{.icon = guikit::Icon::Close(), .square = true}}).clicked) showBottomPanel = false;\n"
            "    }}\n"
            "    // Bottom panel content (charts, logs, preview pane)\n"
            "}}",
            s_state.resizable ? "true" : "false",
            s_state.allowCollapse ? "true" : "false"
        );
    }

    if (s_state.showStatusBar) {
        keel::format_into(
            out,
            "\n\nif (auto sb = guikit::StatusBar(ui, {{.style = guikit::StatusBarStyle::{}}})) {{\n"
            "    sb.Item({{.text = \"Ready\", .dotColor = theme.variants.success.base}});\n"
            "    sb.Spacer();\n"
            "    if (sb.Action({{.text = \"Bottom Panel\", .active = showBottomPanel}}).clicked) {{\n"
            "        showBottomPanel = !showBottomPanel;\n"
            "    }}\n"
            "}}",
            StatusStyleName(s_state.statusBarStyle)
        );
    }

    return out;
}

} // namespace guikit::demo
