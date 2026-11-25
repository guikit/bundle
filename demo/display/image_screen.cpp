#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct ImageState {
    keel::usize sourceIdx = 0;
    static constexpr keel::StaticArray<keel::StringRef, 2> sources = {
        "assets/images/lena.png",
        "assets/images/missing_preview.png"
    };
    static constexpr keel::StaticArray<keel::StringRef, 2> sourceNames = {
        "Lena (assets/images/lena.png)",
        "Missing Asset (Alt Fallback)"
    };
    keel::SmallString<128> altText{"Fallback: asset unavailable"};
    guikit::ObjectFit fit = guikit::ObjectFit::Contain;
    static constexpr keel::StaticArray<keel::StringRef, 5> fitModeNames = {
        "Contain", "Cover", "Fill", "ScaleDown", "Original"
    };
    guikit::Align alignX = guikit::Align::Center;
    guikit::Align alignY = guikit::Align::Center;
    keel::f32 opacityPct = 100.0f;
    bool checkerboard = true;
};

ImageState s_state;

} // namespace

void RenderImageInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Image Source"})) {
        if (guikit::Select(ui, s_state.sourceIdx, ImageState::sourceNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Alt Fallback Text"})) {
        if (guikit::InputField(ui, s_state.altText, {
            .placeholder = "Fallback text when missing...",
            .clearable = true
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "ObjectFit Policy"})) {
        if (guikit::Select(ui, s_state.fit, ImageState::fitModeNames).changed) {
            ui.InvalidateAll();
        }
    }

    keel::usize axIdx = keel::as<keel::usize>(s_state.alignX);
    if (auto el = guikit::Element(ui, {.label = "Horizontal Fit Align"})) {
        if (guikit::Segmented(ui, axIdx, kLabelAlignNames).changed) {
            s_state.alignX = keel::from_underlying<guikit::Align>(axIdx);
            ui.InvalidateAll();
        }
    }

    keel::usize ayIdx = keel::as<keel::usize>(s_state.alignY);
    if (auto el = guikit::Element(ui, {.label = "Vertical Fit Align"})) {
        if (guikit::Segmented(ui, ayIdx, kLabelAlignNames).changed) {
            s_state.alignY = keel::from_underlying<guikit::Align>(ayIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Opacity"})) {
        if (guikit::Slider(ui, s_state.opacityPct, 10.0f, 100.0f, {
            .valueFormat = "%.0f %%",
            .step = 5.0f
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Checkerboard Backdrop"})) {
        if (guikit::Switch(ui, s_state.checkerboard).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderImageStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::u8 alpha = keel::as<keel::u8>(keel::clamp(keel::round_i32(s_state.opacityPct * 2.55f), 0, 255));
    guikit::ImageOptions opts{};
    opts.alt = s_state.altText;
    opts.fit = s_state.fit;
    opts.alignX = s_state.alignX;
    opts.alignY = s_state.alignY;
    opts.checkerboard = s_state.checkerboard;
    opts.opacity = alpha;
    opts.disabled = ctx.elemDisabled;
    opts.size = ctx.controlsSize;
    opts.width = ctx.ResolveCtrlWidth();
    opts.height = ctx.ResolveCtrlHeight();
    opts.background = ctx.GetControlBackground(NavCategory::DisplayImage);
    opts.border = ctx.GetCtrlBorder();
    guikit::Image(ui, ImageState::sources[s_state.sourceIdx % ImageState::sources.size()], opts);
}

keel::String GenerateImageCode(const DemoContext& ctx) {
    const keel::i32 alpha = keel::clamp(keel::round_i32(s_state.opacityPct * 2.55f), 0, 255);
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    return keel::format(
        "guikit::Image(ui, \"{}\", {{\n"
        "    .alt = \"{}\",\n"
        "    .fit = guikit::ObjectFit::{},\n"
        "    .alignX = guikit::Align::{},\n"
        "    .alignY = guikit::Align::{},\n"
        "    .checkerboard = {},\n"
        "    .opacity = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .background = {},\n"
        "    .border = {}\n"
        "}});",
        ImageState::sources[s_state.sourceIdx % ImageState::sources.size()],
        s_state.altText,
        ImageState::fitModeNames[keel::as<keel::usize>(s_state.fit)],
        AlignName(s_state.alignX),
        AlignName(s_state.alignY),
        s_state.checkerboard ? "true" : "false",
        alpha,
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        ctx.FormatCtrlHeightCode(),
        ctx.FormatControlBackgroundCode(NavCategory::DisplayImage),
        ctx.FormatCtrlBorderCode()
    );
}

} // namespace guikit::demo
