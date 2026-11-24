#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct ContextMenuDemoState {
    bool zoneMenuOpen = false;
    guikit::Vec2 zoneMenuPos{};
    keel::usize triggerModeIdx = 0; // 0 = RightClick, 1 = Click
    keel::f32 minWidth = 200.0f;
    bool itemBookmarked = true;
    bool itemLocked = false;
    bool itemGridSnap = true;
    keel::FixedString<128> lastAction = "Right-click inside the target surface to open ContextMenu";
};

ContextMenuDemoState s_cm;

} // namespace

void RenderContextMenuInspector(guikit::Context& ui, DemoContext& ctx) {
    if (auto el = guikit::Element(ui, {.label = "Control Size"})) {
        if (guikit::StepSlider(ui, ctx.controlsSize, {.showTicks = true, .showLabels = true, .includeNone = true}).changed) {
            ui.InvalidateAll();
        }
    }
    static constexpr keel::StaticArray<keel::StringRef, 2> kTriggerLabels = {"RightClick", "Click"};
    if (auto el = guikit::Element(ui, {.label = "Trigger Mode", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_cm.triggerModeIdx, kTriggerLabels).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Menu Min Width"})) {
        if (guikit::Slider(ui, s_cm.minWidth, 150.0f, 280.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderContextMenuStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::StaticArray<guikit::ContextMenuItem, 6> menuItems = {{
        {.label = "Edit Properties", .shortcut = "Enter",  .icon = guikit::Icon::Edit()},
        {.label = "Copy Config",     .shortcut = "Ctrl+C", .icon = guikit::Icon::Copy(), .separatorAfter = true},
        {.label = "Bookmark Target", .shortcut = "Ctrl+B", .checked = s_cm.itemBookmarked, .hasCheck = true},
        {.label = "Snap to Grid",    .shortcut = "Ctrl+G", .checked = s_cm.itemGridSnap,   .hasCheck = true},
        {.label = "Lock Transform",  .shortcut = "Ctrl+L", .separatorAfter = true, .checked = s_cm.itemLocked, .hasCheck = true},
        {.label = "Delete Node",     .shortcut = "Del",    .icon = guikit::Icon::Trash(), .variant = guikit::UiVariant::Danger}
    }};

    const auto zoneTrigger = (s_cm.triggerModeIdx == 0)
        ? guikit::InteractionTrigger::RightClick
        : guikit::InteractionTrigger::Click;

    if (auto stageCol = guikit::Column(ui, {
        .gap = 14.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center
    })) {
        if (auto zone = guikit::Box(ui, {
            .gap = 4.0f,
            .padding = guikit::Padding::Symmetric(36.0f, 26.0f),
            .background = {ui.GetTheme().palette.surfaceHover.WithAlphaF(0.45f)},
            .border = {.width = 1.0f, .radius = 8.0f, .color = {ui.GetTheme().palette.borderHover}}
        })) {
            guikit::Text(ui,
                (s_cm.triggerModeIdx == 0)
                    ? "Right-Click inside this target zone"
                    : "Left-Click inside this target zone",
                {.role = guikit::TextRole::LabelMedium}
            );
            if (zone.GetResponse().IsTriggered(zoneTrigger)) {
                s_cm.zoneMenuOpen = true;
                s_cm.zoneMenuPos = ui.GetMousePos();
            }
            const auto menuRes = guikit::ContextMenu(ui, s_cm.zoneMenuOpen, menuItems, {
                .anchorPos = s_cm.zoneMenuPos,
                .minWidth = s_cm.minWidth,
                .size = ctx.controlsSize
            });
            if (menuRes.changed && menuRes.clickedIndex >= 0) {
                switch (menuRes.clickedIndex) {
                    case 0: s_cm.lastAction = "Clicked: Edit Properties"; break;
                    case 1: s_cm.lastAction = "Clicked: Copy Config"; break;
                    case 2:
                        s_cm.itemBookmarked = !s_cm.itemBookmarked;
                        s_cm.lastAction = keel::format_fixed<128>("Bookmark = {}", s_cm.itemBookmarked ? "ON" : "OFF");
                        break;
                    case 3:
                        s_cm.itemGridSnap = !s_cm.itemGridSnap;
                        s_cm.lastAction = keel::format_fixed<128>("Snap to Grid = {}", s_cm.itemGridSnap ? "ON" : "OFF");
                        break;
                    case 4:
                        s_cm.itemLocked = !s_cm.itemLocked;
                        s_cm.lastAction = keel::format_fixed<128>("Lock Transform = {}", s_cm.itemLocked ? "ON" : "OFF");
                        break;
                    case 5:
                        s_cm.lastAction = "Clicked: Delete Node";
                        break;
                }
            }
        }

        guikit::Text(ui, s_cm.lastAction.as_ref(), {
            .role = guikit::TextRole::BodySmall,
            .variant = guikit::UiVariant::Secondary
        });
    }
}

keel::String GenerateContextMenuCode(const DemoContext& ctx) {
    const auto szPrefix = FormatUiSizePrefix(ctx.controlsSize);
    const keel::StringRef trigName = (s_cm.triggerModeIdx == 0) ? "RightClick" : "Click";
    return keel::format(
        "const keel::StaticArray<guikit::ContextMenuItem, 6> menuItems = {{{{\n"
        "    {{.label = \"Edit Properties\", .shortcut = \"Enter\",  .icon = guikit::Icon::Edit()}},\n"
        "    {{.label = \"Copy Config\",     .shortcut = \"Ctrl+C\", .icon = guikit::Icon::Copy(), .separatorAfter = true}},\n"
        "    {{.label = \"Bookmark Target\", .shortcut = \"Ctrl+B\", .checked = itemBookmarked, .hasCheck = true}},\n"
        "    {{.label = \"Snap to Grid\",    .shortcut = \"Ctrl+G\", .checked = itemGridSnap,   .hasCheck = true}},\n"
        "    {{.label = \"Lock Transform\",  .shortcut = \"Ctrl+L\", .separatorAfter = true, .checked = itemLocked, .hasCheck = true}},\n"
        "    {{.label = \"Delete Node\",     .shortcut = \"Del\",    .icon = guikit::Icon::Trash(), .variant = guikit::UiVariant::Danger}}\n"
        "}}}};\n\n"
        "if (auto zone = guikit::Box(ui, {{.padding = guikit::Padding::Symmetric(36.0f, 26.0f)}})) {{\n"
        "    if (zone.GetResponse().IsTriggered(guikit::InteractionTrigger::{})) {{\n"
        "        zoneMenuOpen = true;\n"
        "        zoneMenuPos = ui.GetMousePos();\n"
        "    }}\n"
        "    auto zoneMenu = guikit::ContextMenu(ui, zoneMenuOpen, menuItems, {{\n"
        "        .anchorPos = zoneMenuPos,\n"
        "        .minWidth = {:.1f}f{}\n"
        "    }});\n"
        "}}",
        trigName,
        s_cm.minWidth,
        szPrefix
    );
}

} // namespace guikit::demo
