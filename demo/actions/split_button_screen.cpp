#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct SplitButtonDemoState {
    keel::usize apiModeIdx = 0; // 0 = Item Array, 1 = MenuScope
    guikit::ButtonStyle style = guikit::ButtonStyle::Default;
    keel::SmallString<64> label{"Build & Run"};
    bool hasIcon = true;
    bool loading = false;
    keel::f32 menuMinWidth = 200.0f;
    bool cleanFirst = false;
    keel::i32 primaryCount = 0;
    keel::FixedString<128> lastAction = "Click left zone for primary action, right chevron for menu";
};

SplitButtonDemoState s_sb;

constexpr keel::StringRef StyleName(guikit::ButtonStyle s) noexcept {
    switch (s) {
        case guikit::ButtonStyle::Default: return "Default";
        case guikit::ButtonStyle::Solid:   return "Solid";
        case guikit::ButtonStyle::Soft:    return "Soft";
        case guikit::ButtonStyle::Outline: return "Outline";
        case guikit::ButtonStyle::Ghost:   return "Ghost";
    }
    return "Default";
}

} // namespace

void RenderSplitButtonInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 2> kApiModes = {"Item Array", "MenuScope"};
    if (auto el = guikit::Element(ui, {.label = "API Style", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_sb.apiModeIdx, kApiModes).changed) {
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 5> kStyleNames = {
        "Default", "Solid", "Soft", "Outline", "Ghost"
    };
    if (auto el = guikit::Element(ui, {.label = "Button Style"})) {
        if (guikit::Segmented(ui, s_sb.style, kStyleNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Primary Label"})) {
        if (guikit::InputField(ui, s_sb.label, {.placeholder = "Action...", .clearable = true}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Leading Icon"})) {
        if (guikit::Switch(ui, s_sb.hasIcon).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Loading Spinner"})) {
        if (guikit::Switch(ui, s_sb.loading).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Menu Min Width"})) {
        if (guikit::Slider(ui, s_sb.menuMinWidth, 150.0f, 300.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderSplitButtonStage(guikit::Context& ui, DemoContext& ctx) {
    guikit::ButtonOptions opts{};
    opts.text = s_sb.label.as_ref();
    opts.icon = s_sb.hasIcon ? guikit::Icon::Play() : guikit::Icon{};
    opts.align = ctx.GetCtrlAlign(NavCategory::ActionSplitButton);
    opts.loading = s_sb.loading;
    opts.disabled = ctx.elemDisabled;
    opts.size = ctx.controlsSize;
    opts.width = ctx.ResolveCtrlWidth();
    opts.height = ctx.ResolveCtrlHeight();
    opts.style = s_sb.style;
    opts.variant = (ctx.btnVariant == guikit::UiVariant::Ghost) ? guikit::UiVariant::Default : ctx.btnVariant;
    opts.background = ctx.GetControlBackground(NavCategory::ActionSplitButton);
    opts.border = ctx.GetCtrlBorder();

    guikit::ContextMenuOptions menuOpts{};
    menuOpts.minWidth = s_sb.menuMinWidth;

    if (auto stageCol = guikit::Column(ui, {
        .gap = 12.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center
    })) {
        if (s_sb.apiModeIdx == 0) {
            const keel::StaticArray<guikit::ContextMenuItem, 4> items = {{
                {.label = "Build Debug",       .shortcut = "F7",      .icon = guikit::Icon::Play(),     .id = 1},
                {.label = "Build Release",     .shortcut = "Ctrl+F7", .icon = guikit::Icon::Flash(),    .id = 2},
                {.label = "Clean Before Build", .separatorAfter = true, .checked = s_sb.cleanFirst, .hasCheck = true, .keepOpenOnClick = true, .id = 3},
                {.label = "Clear Build Cache", .icon = guikit::Icon::Trash(), .variant = guikit::UiVariant::Danger, .id = 4}
            }};

            const auto res = guikit::SplitButton(ui, items, opts, menuOpts);
            if (res.clicked) {
                ++s_sb.primaryCount;
                s_sb.lastAction = keel::format_fixed<128>("Primary action triggered #{}", s_sb.primaryCount);
            } else if (res.selectionChanged) {
                switch (res.clickedId) {
                    case 1: s_sb.lastAction = "Dropdown: Build Debug"; break;
                    case 2: s_sb.lastAction = "Dropdown: Build Release"; break;
                    case 3:
                        s_sb.cleanFirst = !s_sb.cleanFirst;
                        s_sb.lastAction = keel::format_fixed<128>("Clean Before Build = {}", s_sb.cleanFirst ? "ON" : "OFF");
                        break;
                    case 4: s_sb.lastAction = "Dropdown: Clear Build Cache"; break;
                    default: break;
                }
            }
        } else {
            bool primaryClicked = false;
            if (auto menu = guikit::SplitButton(ui, primaryClicked, opts, menuOpts)) {
                if (menu.Item("Build Debug", {.shortcut = "F7", .icon = guikit::Icon::Play()})) {
                    s_sb.lastAction = "MenuScope: Build Debug";
                }
                if (menu.Item("Build Release", {.shortcut = "Ctrl+F7", .icon = guikit::Icon::Flash()})) {
                    s_sb.lastAction = "MenuScope: Build Release";
                }
                menu.Separator();
                if (menu.Checkbox("Clean Before Build", s_sb.cleanFirst, {.keepOpen = true})) {
                    s_sb.lastAction = keel::format_fixed<128>("Clean Before Build = {}", s_sb.cleanFirst ? "ON" : "OFF");
                }
                menu.Separator();
                if (menu.Item("Clear Build Cache", {.icon = guikit::Icon::Trash(), .variant = guikit::UiVariant::Danger})) {
                    s_sb.lastAction = "MenuScope: Clear Build Cache";
                }
            }
            if (primaryClicked) {
                ++s_sb.primaryCount;
                s_sb.lastAction = keel::format_fixed<128>("Primary action triggered #{}", s_sb.primaryCount);
            }
        }

        guikit::Text(ui, s_sb.lastAction.as_ref(), {
            .role = guikit::TextRole::BodySmall,
            .variant = guikit::UiVariant::Secondary
        });
    }
}

keel::String GenerateSplitButtonCode(const DemoContext& ctx) {
    const guikit::UiVariant effVar = (ctx.btnVariant == guikit::UiVariant::Ghost) ? guikit::UiVariant::Default : ctx.btnVariant;
    const keel::String sizeLine = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md)
        ? keel::format("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::String{};

    if (s_sb.apiModeIdx == 0) {
        return keel::format(
            "auto res = guikit::SplitButton(ui, buildItems, {{\n"
            "    .text = \"{}\",\n"
            "    .icon = {},\n"
            "    .loading = {},\n"
            "    .disabled = {},\n"
            "{}"
            "    .style = guikit::ButtonStyle::{},\n"
            "    .variant = guikit::UiVariant::{}\n"
            "}}, {{.minWidth = {:.1f}f}});\n"
            "if (res.clicked) {{\n"
            "    RunPrimaryBuild();\n"
            "}} else if (res.selectionChanged) {{\n"
            "    RunBuildVariant(res.clickedId);\n"
            "}}",
            s_sb.label,
            s_sb.hasIcon ? "guikit::Icon::Play()" : "{}",
            s_sb.loading ? "true" : "false",
            ctx.elemDisabled ? "true" : "false",
            sizeLine,
            StyleName(s_sb.style),
            VariantName(effVar),
            s_sb.menuMinWidth
        );
    }

    return keel::format(
        "bool runPrimary = false;\n"
        "if (auto menu = guikit::SplitButton(ui, runPrimary, {{\n"
        "    .text = \"{}\",\n"
        "    .icon = {},\n"
        "    .loading = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .style = guikit::ButtonStyle::{},\n"
        "    .variant = guikit::UiVariant::{}\n"
        "}}, {{.minWidth = {:.1f}f}})) {{\n"
        "    if (menu.Item(\"Build Debug\", {{.shortcut = \"F7\", .icon = guikit::Icon::Play()}})) {{}}\n"
        "    if (menu.Item(\"Build Release\", {{.shortcut = \"Ctrl+F7\", .icon = guikit::Icon::Flash()}})) {{}}\n"
        "    menu.Separator();\n"
        "    menu.Checkbox(\"Clean Before Build\", cleanFirst, {{.keepOpen = true}});\n"
        "}}\n"
        "if (runPrimary) {{\n"
        "    RunPrimaryBuild();\n"
        "}}",
        s_sb.label,
        s_sb.hasIcon ? "guikit::Icon::Play()" : "{}",
        s_sb.loading ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        StyleName(s_sb.style),
        VariantName(effVar),
        s_sb.menuMinWidth
    );
}

} // namespace guikit::demo
