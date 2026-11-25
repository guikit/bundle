#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct MenuButtonDemoState {
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

    if (auto menu = guikit::MenuButton(ui, opts, menuOpts)) {
        if (menu.Item("Export PNG", {.shortcut = "Ctrl+E", .icon = guikit::Icon::Save()})) {
            s_mb.lastAction = "Export PNG";
        }
        if (menu.Item("Export SVG", {.shortcut = "Ctrl+Shift+E", .icon = guikit::Icon::Copy()})) {
            s_mb.lastAction = "Export SVG";
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
            s_mb.lastAction = "Reset Export Presets";
        }
    }

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

    return keel::format(
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
}

} // namespace guikit::demo
