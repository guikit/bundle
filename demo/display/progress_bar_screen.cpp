#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct ProgressBarDemoState {
    keel::f32 val = 65.0f;
    bool indeterminate = false;
    bool segmented = false;
    keel::u32 segments = 20u;
    bool multiZone = false;
    bool showPeak = false;
    keel::f32 peakVal = 86.0f;
    keel::f32 barHeight = 10.0f;

    bool showValue = true;
    guikit::Orientation orientation = guikit::Orientation::Horizontal;
    bool compactVertical = false;
    guikit::Placement valuePlacement = guikit::Placement::Trailing;
    keel::SmallString<64> textLeft{""};
    keel::SmallString<64> textRight{""};
};
ProgressBarDemoState s_state;

constexpr keel::StringRef OrientationEnumName(guikit::Orientation o) noexcept {
    return (o == guikit::Orientation::Vertical) ? "Vertical" : "Horizontal";
}
} // namespace

void RenderProgressBarInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {
        .label = "Indeterminate",
        .description = "Animated shuttle bar for unknown duration"
    })) {
        if (guikit::Switch(ui, s_state.indeterminate).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Progress Value",
        .disabled = s_state.indeterminate
    })) {
        if (guikit::Slider(ui, s_state.val, 0.0f, 100.0f, {
            .valueFormat = "%.1f %%",
            .disabled = s_state.indeterminate
        }).changed) {
            if (s_state.val > s_state.peakVal) {
                s_state.peakVal = s_state.val;
            }
            ui.InvalidateAll();
        }
    }

    keel::i32 orientIdx = (s_state.orientation == guikit::Orientation::Vertical) ? 1 : 0;
    static constexpr keel::StaticArray<keel::StringRef, 2> orientNames = {"Horizontal", "Vertical"};
    if (auto el = guikit::Element(ui, {.label = "Orientation"})) {
        if (guikit::Segmented(ui, orientIdx, orientNames).changed) {
            s_state.orientation = (orientIdx == 1)
                ? guikit::Orientation::Vertical
                : guikit::Orientation::Horizontal;
            ui.InvalidateAll();
        }
    }

    if (s_state.orientation == guikit::Orientation::Vertical) {
        if (auto el = guikit::Element(ui, {
            .label = "Compact Vertical Height (Fit)",
            .description = "Pass .height = SizeMode::Fit instead of stretching"
        })) {
            if (guikit::Switch(ui, s_state.compactVertical).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Segmented LED / VU Meter",
        .description = "Discrete LED cells vs smooth continuous bar"
    })) {
        if (guikit::Switch(ui, s_state.segmented).changed) {
            ui.InvalidateAll();
        }
    }
    if (s_state.segmented) {
        if (auto el = guikit::Element(ui, {.label = "LED Segment Count"})) {
            if (guikit::Slider(ui, s_state.segments, 8u, 36u, {
                .valueFormat = "%u segs"
            }).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Bar Thickness"})) {
        if (guikit::Slider(ui, s_state.barHeight, 4.0f, 20.0f, {
            .valueFormat = "%.0f dp",
            .step = 1.0f
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "3-Zone Colors (Green/Warn/Danger)"})) {
        if (guikit::Switch(ui, s_state.multiZone).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Peak-Hold Marker", .disabled = s_state.indeterminate})) {
        if (guikit::Switch(ui, s_state.showPeak, {.disabled = s_state.indeterminate}).changed) {
            ui.InvalidateAll();
        }
    }
    if (s_state.showPeak && !s_state.indeterminate) {
        if (auto el = guikit::Element(ui, {.label = "Peak-Hold Level"})) {
            if (guikit::Slider(ui, s_state.peakVal, 0.0f, 100.0f, {
                .valueFormat = "%.1f %%"
            }).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Value Tag", .disabled = s_state.indeterminate})) {
        if (guikit::Switch(ui, s_state.showValue, {.disabled = s_state.indeterminate}).changed) {
            ui.InvalidateAll();
        }
    }
    static constexpr keel::StaticArray<keel::StringRef, 5> valPlaceNames = {
        "Leading", "Trailing", "Top", "Bottom", "Thumb"
    };
    keel::usize valPlaceIdx = keel::as<keel::usize>(s_state.valuePlacement);
    const bool disablePlacement = !s_state.showValue || s_state.indeterminate;
    if (auto el = guikit::Element(ui, {.label = "Value Placement", .disabled = disablePlacement})) {
        if (guikit::Select(ui, valPlaceIdx, valPlaceNames, {
            .disabled = disablePlacement
        }).changed) {
            s_state.valuePlacement = keel::from_underlying<guikit::Placement>(valPlaceIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Text Left (Min)"})) {
        if (guikit::InputField(ui, s_state.textLeft, {
            .placeholder = "e.g. 0% / Min",
            .clearable = true
        }).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Text Right (Max)"})) {
        if (guikit::InputField(ui, s_state.textRight, {
            .placeholder = "e.g. 100% / Max",
            .clearable = true
        }).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderProgressBarStage(guikit::Context& ui, DemoContext& ctx) {
    const guikit::AxisSize wSize = ctx.ResolveCtrlWidth();
    const guikit::AxisSize hSize =
        (s_state.orientation == guikit::Orientation::Vertical && s_state.compactVertical && ctx.ResolveCtrlHeight().IsAuto())
            ? guikit::AxisSize(guikit::SizeMode::Fit)
            : ctx.ResolveCtrlHeight();

    ctx.RunElementStage(ui, [&]() {
        guikit::ProgressBarOptions opts{};
        opts.orientation = s_state.orientation;
        opts.textLeft = s_state.textLeft;
        opts.textRight = s_state.textRight;
        opts.valueFormat = "%.1f %%";
        opts.peakHoldValue = (s_state.showPeak && !s_state.indeterminate)
            ? keel::Optional<keel::f64>(s_state.peakVal)
            : keel::kNullOpt;
        opts.segments = s_state.segmented ? s_state.segments : 0u;
        opts.trackHeight = s_state.barHeight;
        opts.showValue = s_state.showValue && !s_state.indeterminate;
        opts.indeterminate = s_state.indeterminate;
        opts.valuePlacement = s_state.valuePlacement;
        opts.multiZoneColor = s_state.multiZone;
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.width = wSize;
        opts.height = hSize;
        opts.color = ctx.GetControlColor();
        guikit::ProgressBar(ui, s_state.val, 0.0, 100.0, opts);
    });
}

keel::String GenerateProgressBarCode(const DemoContext& ctx) {
    const guikit::AxisSize ctrlH =
        (s_state.orientation == guikit::Orientation::Vertical && s_state.compactVertical && ctx.ResolveCtrlHeight().IsAuto())
            ? guikit::AxisSize(guikit::SizeMode::Fit)
            : ctx.ResolveCtrlHeight();
    const keel::FixedString<48> heightCode = DemoContext::FormatAxisSizeCode(ctrlH);
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    const keel::String ctrl = keel::format(
        "guikit::ProgressBar(ui, progressValue, 0.0, 100.0, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .textLeft = \"{}\",\n"
        "    .textRight = \"{}\",\n"
        "    .valueFormat = \"%.1f %%\",\n"
        "    .peakHoldValue = {},\n"
        "    .segments = {},\n"
        "    .trackHeight = {:.1f}f,\n"
        "    .showValue = {},\n"
        "    .indeterminate = {},\n"
        "    .valuePlacement = guikit::Placement::{},\n"
        "    .multiZoneColor = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .color = {}\n"
        "}});",
        OrientationEnumName(s_state.orientation),
        s_state.textLeft,
        s_state.textRight,
        (s_state.showPeak && !s_state.indeterminate)
            ? keel::format_fixed<32>("{:.1f}", s_state.peakVal)
            : keel::FixedString<32>("keel::kNullOpt"),
        s_state.segmented ? s_state.segments : 0u,
        s_state.barHeight,
        (s_state.showValue && !s_state.indeterminate) ? "true" : "false",
        s_state.indeterminate ? "true" : "false",
        PlacementName(s_state.valuePlacement),
        s_state.multiZone ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        heightCode,
        ctx.FormatControlColorCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
