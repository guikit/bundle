#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct ButtonState {
    keel::SmallString<128> text{"Click me"};
    guikit::ButtonStyle style = guikit::ButtonStyle::Default;
    bool hasIcon = true;
    guikit::Placement iconPlacement = guikit::Placement::Leading;
    bool square = false;
    bool loading = false;
    keel::i32 clicks = 0;
};
ButtonState s_state;
} // namespace

void RenderButtonInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 5> kStyleNames = {
        "Default", "Solid", "Soft", "Outline", "Ghost"
    };
    if (auto el = guikit::Element(ui, {.label = "Button Style"})) {
        if (guikit::Segmented(ui, s_state.style, kStyleNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Capsule Text"})) {
        if (guikit::InputField(ui, s_state.text, {
            .placeholder = "Icon-only...",
            .clearable = true
        }).changed) {
            s_state.clicks = 0;
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Capsule Icon"})) {
        if (guikit::Switch(ui, s_state.hasIcon).changed) ui.InvalidateAll();
    }

    static constexpr keel::StaticArray<keel::StringRef, 4> placeNames = {"Leading", "Trailing", "Top", "Bottom"};
    keel::usize placeIdx = (s_state.iconPlacement == guikit::Placement::Trailing)
        ? 1
        : ((s_state.iconPlacement == guikit::Placement::Top)
            ? 2
            : ((s_state.iconPlacement == guikit::Placement::Bottom) ? 3 : 0));
    const bool placeDisabled = !s_state.hasIcon || s_state.square;
    if (auto el = guikit::Element(ui, {.label = "Icon Placement", .disabled = placeDisabled})) {
        if (guikit::Segmented(ui, placeIdx, placeNames, {
            .disabled = placeDisabled
        }).changed) {
            s_state.iconPlacement = (placeIdx == 1)
                ? guikit::Placement::Trailing
                : ((placeIdx == 2)
                    ? guikit::Placement::Top
                    : ((placeIdx == 3) ? guikit::Placement::Bottom : guikit::Placement::Leading));
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Loading Spinner"})) {
        if (guikit::Switch(ui, s_state.loading).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Square ToolButton"})) {
        if (guikit::Switch(ui, s_state.square).changed) ui.InvalidateAll();
    }
}

void RenderButtonStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::StringRef btnText = (s_state.clicks > 0 && !s_state.text.empty())
        ? ui.Format("{} ({}x)", s_state.text, s_state.clicks)
        : s_state.text.as_ref();

    guikit::ButtonOptions opts{};
    opts.text = btnText;
    opts.icon = s_state.hasIcon ? guikit::Icon::Play() : guikit::Icon{};
    opts.iconPlacement = s_state.iconPlacement;
    opts.align = ctx.GetCtrlAlign(NavCategory::ActionButton);
    opts.loading = s_state.loading;
    opts.square = s_state.square;
    opts.disabled = ctx.elemDisabled;
    opts.size = ctx.controlsSize;
    opts.width = ctx.ResolveCtrlWidth();
    opts.height = ctx.ResolveCtrlHeight();
    opts.style = s_state.style;
    opts.variant = (ctx.btnVariant == guikit::UiVariant::Ghost) ? guikit::UiVariant::Default : ctx.btnVariant;
    if (ctx.btnVariant == guikit::UiVariant::Ghost && s_state.style == guikit::ButtonStyle::Default) {
        opts.style = guikit::ButtonStyle::Ghost;
    }
    opts.background = ctx.GetControlBackground(NavCategory::ActionButton);
    opts.border = ctx.GetCtrlBorder();

    if (guikit::Button(ui, opts)) {
        ++s_state.clicks;
        ui.InvalidateAll();
    }
}

keel::String GenerateButtonCode(const DemoContext& ctx) {
    const guikit::Align ctrlAlign = ctx.GetCtrlAlign(NavCategory::ActionButton);
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const guikit::ButtonStyle effStyle = (ctx.btnVariant == guikit::UiVariant::Ghost && s_state.style == guikit::ButtonStyle::Default)
        ? guikit::ButtonStyle::Ghost
        : s_state.style;
    const guikit::UiVariant effVariant = (ctx.btnVariant == guikit::UiVariant::Ghost)
        ? guikit::UiVariant::Default
        : ctx.btnVariant;
    return keel::format(
        "if (guikit::Button(ui, {{\n"
        "    .text = \"{}\",\n"
        "    .icon = {},\n"
        "    .iconPlacement = guikit::Placement::{},\n"
        "    .align = guikit::Align::{},\n"
        "    .loading = {},\n"
        "    .square = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .style = guikit::ButtonStyle::{},\n"
        "    .variant = guikit::UiVariant::{},\n"
        "    .background = {},\n"
        "    .border = {}\n"
        "}})) {{\n"
        "    // On click handler\n"
        "}}",
        s_state.text,
        s_state.hasIcon ? "guikit::Icon::Play()" : "{}",
        PlacementName(s_state.iconPlacement),
        AlignName(ctrlAlign),
        s_state.loading ? "true" : "false",
        s_state.square ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatControlWidthCode(),
        ctx.FormatControlHeightCode(),
        ButtonStyleEnumName(effStyle),
        VariantName(effVariant),
        ctx.FormatControlBackgroundCode(NavCategory::ActionButton),
        ctx.FormatCtrlBorderCode()
    );
}

} // namespace guikit::demo
