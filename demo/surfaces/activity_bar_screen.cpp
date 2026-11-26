#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct ActivityBarDemoState {
    keel::i32 activeIdx = 0;
    guikit::SidebarSide side = guikit::SidebarSide::Left;
    guikit::ActivityBarStyle style = guikit::ActivityBarStyle::Indicator;
    guikit::ButtonStyle buttonStyle = guikit::ButtonStyle::Ghost;
    guikit::UiSize size = guikit::UiSize::Md;
    bool floatingFrame = false;
    bool linkSidebar = true;
    guikit::SidebarMode linkedMode = guikit::SidebarMode::Expanded;
    bool showBadges = true;
    bool gearMenuOpen = false;
    keel::FixedString<64> lastAction{};
};

ActivityBarDemoState s_actBar;

constexpr keel::StaticArray<keel::StringRef, 4> kActivityTitles = {
    "Explorer", "Search", "Source Control", "Extensions"
};

constexpr keel::StaticArray<keel::StringRef, 4> kActivityDescs = {
    "Project workspace tree, open editors, and outline.",
    "Full-text search and regex replace across all files.",
    "3 staged changes ready to commit on branch main.",
    "Installed plugins, themes, and language packs."
};

} // namespace

void RenderActivityBarInspector(guikit::Context& ui, DemoContext& ctx) {
    static constexpr keel::StaticArray<keel::StringRef, 2> kStyleNames = {"Indicator", "Pill"};
    keel::usize styleIdx = (s_actBar.style == guikit::ActivityBarStyle::Pill) ? 1u : 0u;
    if (auto el = guikit::Element(ui, {.label = "Active Style"})) {
        if (guikit::Segmented(ui, styleIdx, kStyleNames).changed) {
            s_actBar.style = (styleIdx == 1u) ? guikit::ActivityBarStyle::Pill : guikit::ActivityBarStyle::Indicator;
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 4> kBtnStyleNames = {"Ghost", "Default", "Soft", "Outline"};
    keel::usize btnStyleIdx = 0u;
    if (s_actBar.buttonStyle == guikit::ButtonStyle::Default) btnStyleIdx = 1u;
    else if (s_actBar.buttonStyle == guikit::ButtonStyle::Soft) btnStyleIdx = 2u;
    else if (s_actBar.buttonStyle == guikit::ButtonStyle::Outline) btnStyleIdx = 3u;
    if (auto el = guikit::Element(ui, {.label = "Button Style", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, btnStyleIdx, kBtnStyleNames).changed) {
            switch (btnStyleIdx) {
                case 1u: s_actBar.buttonStyle = guikit::ButtonStyle::Default; break;
                case 2u: s_actBar.buttonStyle = guikit::ButtonStyle::Soft;    break;
                case 3u: s_actBar.buttonStyle = guikit::ButtonStyle::Outline; break;
                default: s_actBar.buttonStyle = guikit::ButtonStyle::Ghost;   break;
            }
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 2> kSideNames = {"Left", "Right"};
    keel::usize sideIdx = (s_actBar.side == guikit::SidebarSide::Right) ? 1u : 0u;
    if (auto el = guikit::Element(ui, {.label = "Dock Side"})) {
        if (guikit::Segmented(ui, sideIdx, kSideNames).changed) {
            s_actBar.side = (sideIdx == 1u) ? guikit::SidebarSide::Right : guikit::SidebarSide::Left;
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Rail Size"})) {
        if (guikit::StepSlider(ui, s_actBar.size, {.showTicks = true, .showLabels = true}).changed) {
            ui.InvalidateAll();
        }
    }

    ctx.RenderVariantPicker(ui);

    static constexpr keel::StaticArray<keel::StringRef, 2> kFrameNames = {"Docked", "Floating"};
    keel::usize frameIdx = s_actBar.floatingFrame ? 1u : 0u;
    if (auto el = guikit::Element(ui, {.label = "Shell Frame"})) {
        if (guikit::Segmented(ui, frameIdx, kFrameNames).changed) {
            s_actBar.floatingFrame = (frameIdx == 1u);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Link Companion Sidebar",
        .description = "Clicking active icon collapses/expands the linked Sidebar"
    })) {
        if (guikit::Switch(ui, s_actBar.linkSidebar).changed) {
            if (s_actBar.linkSidebar && s_actBar.linkedMode == guikit::SidebarMode::Collapsed) {
                s_actBar.linkedMode = guikit::SidebarMode::Expanded;
            }
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Corner Badges"})) {
        if (guikit::Switch(ui, s_actBar.showBadges).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderActivityBarStage(guikit::Context& ui, DemoContext& ctx) {
    guikit::UiTheme stageTheme = ui.GetTheme();
    stageTheme.SetShellStyle(s_actBar.floatingFrame ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked);

    if (auto stageZone = ui.Zone({.theme = stageTheme})) {
        if (auto shell = guikit::Box(ui, {
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch
        })) {
            guikit::ActivityBarOptions barOpts{
                .side = s_actBar.side,
                .style = s_actBar.style,
                .buttonStyle = s_actBar.buttonStyle,
                .color = ctx.ResolveControlColor(),
                .linkedSidebarMode = s_actBar.linkSidebar ? &s_actBar.linkedMode : nullptr,
                .size = s_actBar.size
            };

            if (auto bar = guikit::ActivityBar(ui, barOpts)) {
                bar.Item(s_actBar.activeIdx, 0, {
                    .icon = guikit::Icon::Folder(),
                    .label = "Explorer",
                    .tooltip = "Explorer (click active to toggle sidebar)"
                });
                bar.Item(s_actBar.activeIdx, 1, {
                    .icon = guikit::Icon::Search(),
                    .label = "Search",
                    .tooltip = "Search Workspace",
                    .badge = s_actBar.showBadges ? "2" : ""
                });
                bar.Item(s_actBar.activeIdx, 2, {
                    .icon = guikit::Icon::Refresh(),
                    .label = "Source Control",
                    .tooltip = "Source Control (3 pending changes)",
                    .badge = s_actBar.showBadges ? "3" : ""
                });
                bar.Item(s_actBar.activeIdx, 3, {
                    .icon = guikit::Icon::Plus(),
                    .label = "Extensions",
                    .tooltip = "Extensions Marketplace",
                    .badge = s_actBar.showBadges ? "9+" : ""
                });

                bar.Spacer();
                bar.Separator();

                const guikit::Response gearBtn = bar.Action({
                    .icon = guikit::Icon::Settings(),
                    .label = "Manage",
                    .tooltip = "Settings & Quick Actions",
                    .active = s_actBar.gearMenuOpen
                });
                if (gearBtn.clicked) {
                    s_actBar.gearMenuOpen = !s_actBar.gearMenuOpen;
                }
                const auto gearMenu = guikit::ContextMenu(ui, s_actBar.gearMenuOpen, {
                    {.label = "Preferences...",  .shortcut = "Ctrl+,", .icon = guikit::Icon::Settings()},
                    {.label = "Color Theme",     .shortcut = "Ctrl+K", .icon = guikit::Icon::Eye(), .separatorAfter = true},
                    {.label = "Check Updates",   .icon = guikit::Icon::Refresh()}
                }, {
                    .anchorRect = gearBtn.rect,
                    .minWidth = 200.0f
                });
                if (gearMenu.clickedIndex >= 0) {
                    static constexpr keel::StaticArray<keel::StringRef, 3> kGearLabels = {
                        "Opened Preferences", "Opened Color Theme", "Checked for Updates"
                    };
                    s_actBar.lastAction = kGearLabels[keel::as<keel::usize>(gearMenu.clickedIndex) % kGearLabels.size()];
                }
            }

            if (s_actBar.linkSidebar) {
                if (auto sb = guikit::Sidebar(ui, s_actBar.linkedMode, {
                    .side = s_actBar.side,
                    .width = 180.0f,
                    .resizable = true,
                    .allowCompact = false,
                    .allowCollapse = true,
                    .padding = guikit::Padding::All(12.0f)
                })) {
                    const keel::usize idx = keel::as<keel::usize>(keel::clamp(s_actBar.activeIdx, 0, 3));
                    guikit::Heading(ui, {
                        .label = kActivityTitles[idx],
                        .level = guikit::HeadingLevel::H6,
                        .separator = guikit::HeadingSeparator::Bottom
                    });
                    guikit::Text(ui, {kActivityDescs[idx], guikit::TextWrap::Word()}, {
                        .role = guikit::TextRole::BodySmall,
                        .variant = guikit::UiVariant::Secondary
                    });
                }
            }

            if (auto mainCol = guikit::Column(ui, {
                .gap = 10.0f,
                .padding = guikit::Padding::All(20.0f),
                .align = guikit::Alignment::Start,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                const keel::usize idx = keel::as<keel::usize>(keel::clamp(s_actBar.activeIdx, 0, 3));
                guikit::Heading(ui, {
                    .label = kActivityTitles[idx],
                    .description = kActivityDescs[idx],
                    .separator = guikit::HeadingSeparator::Bottom
                });

                guikit::Text(ui, {
                    s_actBar.linkSidebar
                        ? "Click the active ActivityBar icon again to collapse or re-expand the linked Sidebar."
                        : "Standalone icon rail — click any item or the bottom Manage gear action.",
                    guikit::TextWrap::Word()
                }, {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary});

                if (!s_actBar.lastAction.empty()) {
                    guikit::Tag(ui, s_actBar.lastAction.as_ref(), {
                        .variant = guikit::UiVariant::Info,
                        .style = guikit::TagStyle::Soft,
                        .size = guikit::UiSize::Sm
                    });
                }
            }
        }
    }
}

keel::String GenerateActivityBarCode(const DemoContext& ctx) {
    const keel::String openCall = s_actBar.linkSidebar
        ? keel::String("guikit::ActivityBar(ui, sidebarMode, {")
        : keel::String("guikit::ActivityBar(ui, {");
    const keel::String colorLine = (ctx.btnVariant != guikit::UiVariant::Default)
        ? keel::format(",\n    .color = {}", ctx.FormatControlColorCode())
        : keel::String{};
    const keel::String shellComment = s_actBar.floatingFrame
        ? keel::String("// theme.SetShellStyle(guikit::ShellStyle::Floating);\n")
        : keel::String{};
    const keel::String badge2 = s_actBar.showBadges ? keel::String(", .badge = \"2\"") : keel::String{};
    const keel::String badge3 = s_actBar.showBadges ? keel::String(", .badge = \"3\"") : keel::String{};

    return keel::format(
        "{}"
        "if (auto bar = {}\n"
        "    .side = guikit::SidebarSide::{},\n"
        "    .style = guikit::ActivityBarStyle::{},\n"
        "    .buttonStyle = guikit::ButtonStyle::{}{}{}\n"
        "}})) {{\n"
        "    bar.Item(activeIdx, 0, {{.icon = guikit::Icon::Folder(), .label = \"Explorer\"}});\n"
        "    bar.Item(activeIdx, 1, {{.icon = guikit::Icon::Search(), .label = \"Search\"{}}});\n"
        "    bar.Item(activeIdx, 2, {{.icon = guikit::Icon::Refresh(), .label = \"Source Control\"{}}});\n"
        "    bar.Spacer();\n"
        "    bar.Separator();\n"
        "    if (bar.Action({{.icon = guikit::Icon::Settings(), .label = \"Manage\"}}).clicked) {{\n"
        "        // Open settings menu\n"
        "    }}\n"
        "}}",
        shellComment,
        openCall,
        (s_actBar.side == guikit::SidebarSide::Right) ? "Right" : "Left",
        (s_actBar.style == guikit::ActivityBarStyle::Pill) ? "Pill" : "Indicator",
        ButtonStyleEnumName(s_actBar.buttonStyle),
        colorLine,
        FormatUiSizePrefix(s_actBar.size),
        badge2,
        badge3
    );
}

} // namespace guikit::demo
