#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct MenuButtonDemoState {
    keel::usize apiModeIdx = 0; // 0 = MenuScope (RAII), 1 = Item Array
    guikit::ButtonStyle style = guikit::ButtonStyle::Default;
    keel::SmallString<64> label{"Export"};
    bool hasIcon = true;
    bool square = false;
    bool showSubmenu = true;
    keel::f32 menuMinWidth = 200.0f;
    bool includeGrid = true;
    keel::i32 dpiIdx = 1; // 0 = 1x, 1 = 2x, 2 = 4x
    keel::FixedString<128> lastAction = "Click button to open dropdown menu";
};

MenuButtonDemoState s_mb;

} // namespace

void RenderMenuButtonInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 2> kApiModes = {"MenuScope", "Item Array"};
    if (auto el = guikit::Element(ui, {.label = "API Style", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_mb.apiModeIdx, kApiModes).changed) {
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 5> kStyleNames = {
        "Default", "Solid", "Soft", "Outline", "Ghost"
    };
    if (auto el = guikit::Element(ui, {.label = "Button Style"})) {
        if (guikit::Segmented(ui, s_mb.style, kStyleNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Button Label"})) {
        if (guikit::InputField(ui, s_mb.label, {.placeholder = "Actions...", .clearable = true}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Leading Icon"})) {
        if (guikit::Switch(ui, s_mb.hasIcon).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Square Icon-Only"})) {
        if (guikit::Switch(ui, s_mb.square).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Include Submenu"})) {
        if (guikit::Switch(ui, s_mb.showSubmenu).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Menu Min Width"})) {
        if (guikit::Slider(ui, s_mb.menuMinWidth, 150.0f, 300.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderMenuButtonStage(guikit::Context& ui, DemoContext& ctx) {
    guikit::ButtonOptions opts{};
    opts.text = s_mb.label.as_ref();
    opts.icon = (s_mb.hasIcon || s_mb.square) ? guikit::Icon::Save() : guikit::Icon{};
    opts.align = ctx.GetCtrlAlign(NavCategory::ActionMenuButton);
    opts.square = s_mb.square;
    opts.disabled = ctx.elemDisabled;
    opts.size = ctx.controlsSize;
    opts.width = ctx.ResolveCtrlWidth();
    opts.height = ctx.ResolveCtrlHeight();
    opts.style = s_mb.style;
    opts.variant = (ctx.btnVariant == guikit::UiVariant::Ghost) ? guikit::UiVariant::Default : ctx.btnVariant;
    opts.background = ctx.GetControlBackground(NavCategory::ActionMenuButton);
    opts.border = ctx.GetCtrlBorder();

    guikit::ContextMenuOptions menuOpts{};
    menuOpts.minWidth = s_mb.menuMinWidth;

    ctx.RunElementStage(ui, [&]() {
        if (s_mb.apiModeIdx == 0) {
            if (auto menu = guikit::MenuButton(ui, opts, menuOpts)) {
                if (menu.Item("Export PNG", {.shortcut = "Ctrl+E", .icon = guikit::Icon::Save()})) {
                    s_mb.lastAction = "MenuScope: Export PNG";
                }
                if (menu.Item("Export SVG", {.shortcut = "Ctrl+Shift+E", .icon = guikit::Icon::Copy()})) {
                    s_mb.lastAction = "MenuScope: Export SVG";
                }
                if (s_mb.showSubmenu) {
                    if (auto sub = menu.SubMenu("Resolution", {.icon = guikit::Icon::Settings()})) {
                        if (sub.Radio("1x (72 DPI)", s_mb.dpiIdx, 0, {.keepOpen = true})) s_mb.lastAction = "Resolution = 1x";
                        if (sub.Radio("2x (Retina)", s_mb.dpiIdx, 1, {.keepOpen = true})) s_mb.lastAction = "Resolution = 2x";
                        if (sub.Radio("4x (Print)",  s_mb.dpiIdx, 2, {.keepOpen = true})) s_mb.lastAction = "Resolution = 4x";
                    }
                }
                menu.Separator();
                if (menu.Checkbox("Include Grid Overlay", s_mb.includeGrid, {.keepOpen = true})) {
                    s_mb.lastAction = keel::format_fixed<128>("Include Grid = {}", s_mb.includeGrid ? "ON" : "OFF");
                }
                menu.Separator();
                if (menu.Item("Reset Export Presets", {.icon = guikit::Icon::Trash(), .variant = guikit::UiVariant::Danger})) {
                    s_mb.includeGrid = true;
                    s_mb.dpiIdx = 1;
                    s_mb.lastAction = "MenuScope: Reset Export Presets";
                }
            }
        } else {
            static constexpr keel::StaticArray<guikit::ContextMenuItem, 3> kResSubItems = {{
                {.label = "1x (72 DPI)", .id = 101},
                {.label = "2x (Retina)", .id = 102},
                {.label = "4x (Print)",  .id = 103}
            }};
            const keel::StaticArray<guikit::ContextMenuItem, 5> items = {{
                {.label = "Export PNG", .shortcut = "Ctrl+E", .icon = guikit::Icon::Save(), .id = 1},
                {.label = "Export SVG", .shortcut = "Ctrl+Shift+E", .icon = guikit::Icon::Copy(), .id = 2},
                {.label = "Resolution", .icon = guikit::Icon::Settings(), .separatorAfter = true,
                 .disabled = !s_mb.showSubmenu,
                 .children = s_mb.showSubmenu ? keel::ArrayRef<guikit::ContextMenuItem>(kResSubItems) : keel::ArrayRef<guikit::ContextMenuItem>{}},
                {.label = "Include Grid Overlay", .separatorAfter = true, .checked = s_mb.includeGrid, .hasCheck = true, .keepOpenOnClick = true, .id = 3},
                {.label = "Reset Export Presets", .icon = guikit::Icon::Trash(), .variant = guikit::UiVariant::Danger, .id = 4}
            }};

            const auto res = guikit::MenuButton(ui, items, opts, menuOpts);
            if (res.changed) {
                switch (res.clickedId) {
                    case 1: s_mb.lastAction = "Array: Export PNG"; break;
                    case 2: s_mb.lastAction = "Array: Export SVG"; break;
                    case 3:
                        s_mb.includeGrid = !s_mb.includeGrid;
                        s_mb.lastAction = keel::format_fixed<128>("Include Grid = {}", s_mb.includeGrid ? "ON" : "OFF");
                        break;
                    case 4: s_mb.lastAction = "Array: Reset Export Presets"; break;
                    case 101: s_mb.lastAction = "Submenu: 1x (72 DPI)"; break;
                    case 102: s_mb.lastAction = "Submenu: 2x (Retina)"; break;
                    case 103: s_mb.lastAction = "Submenu: 4x (Print)"; break;
                    default: break;
                }
            }
        }
    });

    guikit::Text(ui, s_mb.lastAction.as_ref(), {
        .role = guikit::TextRole::BodySmall,
        .variant = guikit::UiVariant::Secondary
    });
}

keel::String GenerateMenuButtonCode(const DemoContext& ctx) {
    const guikit::UiVariant effVar = (ctx.btnVariant == guikit::UiVariant::Ghost) ? guikit::UiVariant::Default : ctx.btnVariant;
    const keel::String sizeLine = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md)
        ? keel::format("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::String{};

    keel::String ctrl;
    if (s_mb.apiModeIdx == 0) {
        ctrl = keel::format(
            "if (auto menu = guikit::MenuButton(ui, {{\n"
            "    .text = \"{}\",\n"
            "    .icon = {},\n"
            "    .square = {},\n"
            "    .disabled = {},\n"
            "{}"
            "    .width = {},\n"
            "    .height = {},\n"
            "    .style = guikit::ButtonStyle::{},\n"
            "    .variant = guikit::UiVariant::{}\n"
            "}}, {{.minWidth = {:.1f}f}})) {{\n"
            "    if (menu.Item(\"Export PNG\", {{.shortcut = \"Ctrl+E\", .icon = guikit::Icon::Save()}})) {{}}\n"
            "    if (menu.Item(\"Export SVG\", {{.shortcut = \"Ctrl+Shift+E\", .icon = guikit::Icon::Copy()}})) {{}}\n"
            "    menu.Separator();\n"
            "    menu.Checkbox(\"Include Grid Overlay\", includeGrid, {{.keepOpen = true}});\n"
            "}}",
            s_mb.label,
            (s_mb.hasIcon || s_mb.square) ? "guikit::Icon::Save()" : "{}",
            s_mb.square ? "true" : "false",
            ctx.elemDisabled ? "true" : "false",
            sizeLine,
            ctx.FormatCtrlWidthCode(),
            ctx.FormatCtrlHeightCode(),
            ButtonStyleEnumName(s_mb.style),
            VariantName(effVar),
            s_mb.menuMinWidth
        );
    } else {
        ctrl = keel::format(
            "auto res = guikit::MenuButton(ui, menuItems, {{\n"
            "    .text = \"{}\",\n"
            "    .icon = {},\n"
            "    .square = {},\n"
            "    .disabled = {},\n"
            "{}"
            "    .width = {},\n"
            "    .height = {},\n"
            "    .style = guikit::ButtonStyle::{},\n"
            "    .variant = guikit::UiVariant::{}\n"
            "}}, {{.minWidth = {:.1f}f}});\n"
            "if (res.changed) {{\n"
            "    HandleCommand(res.clickedId);\n"
            "}}",
            s_mb.label,
            (s_mb.hasIcon || s_mb.square) ? "guikit::Icon::Save()" : "{}",
            s_mb.square ? "true" : "false",
            ctx.elemDisabled ? "true" : "false",
            sizeLine,
            ctx.FormatCtrlWidthCode(),
            ctx.FormatCtrlHeightCode(),
            ButtonStyleEnumName(s_mb.style),
            VariantName(effVar),
            s_mb.menuMinWidth
        );
    }
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
