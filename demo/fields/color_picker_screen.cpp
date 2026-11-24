#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct ColorPickerDemoState {
    guikit::Color color{59, 130, 246, 235};
    bool showHex = true;
    bool showAlpha = true;
    bool showSwatches = true;
    bool inlinePanel = false;
};
ColorPickerDemoState s_state;
} // namespace

void RenderColorPickerInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {
        .label = "Inline Picker Panel",
        .description = "Embed full SV + Hue/Alpha panel inline vs Popover button"
    })) {
        if (guikit::Switch(ui, s_state.inlinePanel).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {
        .label = "Show #HEX Readout",
        .description = "Show formatted hex code inside closed swatch button",
        .disabled = s_state.inlinePanel
    })) {
        if (guikit::Switch(ui, s_state.showHex, {.disabled = s_state.inlinePanel}).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {
        .label = "Enable Alpha Channel",
        .description = "Include opacity slider and #RRGGBBAA format"
    })) {
        if (guikit::Switch(ui, s_state.showAlpha).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {
        .label = "Show Preset Swatches",
        .description = "Display quick-pick color palette row"
    })) {
        if (guikit::Switch(ui, s_state.showSwatches).changed) ui.InvalidateAll();
    }
}

void RenderColorPickerStage(guikit::Context& ui, DemoContext& ctx) {
    ctx.RunElementStage(ui, [&]() {
        guikit::ColorPickerOptions opts{};
        opts.showHex = s_state.showHex;
        opts.showAlpha = s_state.showAlpha;
        opts.showSwatches = s_state.showSwatches;
        opts.inlinePanel = s_state.inlinePanel;
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.width = ctx.ResolveCtrlWidth();
        opts.height = ctx.ResolveCtrlHeight();
        opts.background = ctx.GetControlBackground(NavCategory::FieldColorPicker);
        opts.border = ctx.GetCtrlBorder();
        guikit::ColorPicker(ui, s_state.color, opts);
    });
}

keel::String GenerateColorPickerCode(const DemoContext& ctx) {
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::String ctrl = keel::format(
        "ui.ColorPicker(traceColor, {{\n"
        "    .showHex = {},\n"
        "    .showAlpha = {},\n"
        "    .showSwatches = {},\n"
        "    .inlinePanel = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .background = {},\n"
        "    .border = {}\n"
        "}});",
        s_state.showHex ? "true" : "false",
        s_state.showAlpha ? "true" : "false",
        s_state.showSwatches ? "true" : "false",
        s_state.inlinePanel ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        ctx.FormatCtrlHeightCode(),
        ctx.FormatControlBackgroundCode(NavCategory::FieldColorPicker),
        ctx.FormatCtrlBorderCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
