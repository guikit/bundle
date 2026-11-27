#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct SliderDemoState {
    keel::f32 val = 65.0f;
    keel::usize stepIdx = 2;
    keel::f32 rangeLow = 22.0f;
    keel::f32 rangeHigh = 78.0f;
    keel::f32 rangeMinSpan = 10.0f;
    bool quantizeStep = false;

    keel::i32 hybridPreset = 0; // 0 = 1 stop (Off | 1..100), 1 = 2 stops (Off | Auto | 1..100)
    bool hybridEnabled = true;
    keel::i32 hybridMode = 1; // 0 = Off, 1 = Auto, 2 = Manual
    keel::f32 hybridVal = 42.0f;
    guikit::Placement hybridValuePlacement = guikit::Placement::Bottom;

    bool showTicks = true;
    bool showLabels = true;
    bool showValue = true;
    guikit::Orientation orientation = guikit::Orientation::Horizontal;
    bool compactVertical = false;
    guikit::Placement valuePlacement = guikit::Placement::Trailing;
    keel::SmallString<64> textLeft{""};
    keel::SmallString<64> textRight{""};
};

SliderDemoState s_state;

constexpr keel::StringRef OrientationEnumName(guikit::Orientation o) noexcept {
    return (o == guikit::Orientation::Vertical) ? "Vertical" : "Horizontal";
}

guikit::AxisSize ResolveSliderHeight(const DemoContext& ctx) noexcept {
    return (s_state.orientation == guikit::Orientation::Vertical &&
            s_state.compactVertical &&
            ctx.ResolveCtrlHeight().IsAuto())
        ? guikit::AxisSize(guikit::SizeMode::Fit)
        : ctx.ResolveCtrlHeight();
}

void RenderSharedSliderInspector(guikit::Context& ui) {
    keel::i32 orientIdx = (s_state.orientation == guikit::Orientation::Vertical) ? 1 : 0;
    static constexpr keel::StaticArray<keel::StringRef, 2> orientNames = {"Horizontal", "Vertical (Fader)"};
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

    if (auto el = guikit::Element(ui, {.label = "Show Value Tag"})) {
        if (guikit::Switch(ui, s_state.showValue).changed) ui.InvalidateAll();
    }
    static constexpr keel::StaticArray<keel::StringRef, 5> valPlaceNames = {
        "Leading", "Trailing", "Top", "Bottom", "Thumb"
    };
    keel::usize valPlaceIdx = keel::as<keel::usize>(s_state.valuePlacement);
    if (auto el = guikit::Element(ui, {.label = "Value Placement", .disabled = !s_state.showValue})) {
        if (guikit::Select(ui, valPlaceIdx, valPlaceNames, {
            .disabled = !s_state.showValue
        }).changed) {
            s_state.valuePlacement = keel::from_underlying<guikit::Placement>(valPlaceIdx);
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Text Left (Min)"})) {
        if (guikit::InputField(ui, s_state.textLeft, {
            .placeholder = "e.g. 0% / Min",
            .clearable = true
        }).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Text Right (Max)"})) {
        if (guikit::InputField(ui, s_state.textRight, {
            .placeholder = "e.g. 100% / Max",
            .clearable = true
        }).changed) ui.InvalidateAll();
    }
}

} // namespace

// ============================================================================
// 1. Slider (Continuous)
// ============================================================================

void RenderSliderInspector(guikit::Context& ui, DemoContext&) {
    RenderSharedSliderInspector(ui);
    if (auto el = guikit::Element(ui, {.label = "Quantize Step (5.0%)"})) {
        if (guikit::Switch(ui, s_state.quantizeStep).changed) ui.InvalidateAll();
    }
}

void RenderSliderStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::Optional<keel::f64> stepOpt = s_state.quantizeStep ? keel::Optional<keel::f64>(5.0) : keel::kNullOpt;
    const guikit::AxisSize wSize = ctx.ResolveCtrlWidth();
    const guikit::AxisSize hSize = ResolveSliderHeight(ctx);

    ctx.RunElementStage(ui, [&]() {
        guikit::SliderOptions opts{};
        opts.orientation = s_state.orientation;
        opts.textLeft = s_state.textLeft;
        opts.textRight = s_state.textRight;
        opts.valueFormat = "%.1f %%";
        opts.step = stepOpt;
        opts.showValue = s_state.showValue;
        opts.valuePlacement = s_state.valuePlacement;
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.width = wSize;
        opts.height = hSize;
        opts.color = ctx.GetControlColor();
        guikit::Slider(ui, s_state.val, 0.0f, 100.0f, opts);
    });
}

keel::String GenerateSliderCode(const DemoContext& ctx) {
    const keel::FixedString<48> heightCode = DemoContext::FormatAxisSizeCode(ResolveSliderHeight(ctx));
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    const keel::String ctrl = keel::format(
        "guikit::Slider(ui, value, 0.0f, 100.0f, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .textLeft = \"{}\",\n"
        "    .textRight = \"{}\",\n"
        "    .valueFormat = \"%.1f %%\",\n"
        "    .step = {},\n"
        "    .showValue = {},\n"
        "    .valuePlacement = guikit::Placement::{},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .color = {}\n"
        "}});",
        OrientationEnumName(s_state.orientation),
        s_state.textLeft,
        s_state.textRight,
        s_state.quantizeStep ? "5.0" : "keel::kNullOpt",
        s_state.showValue ? "true" : "false",
        PlacementName(s_state.valuePlacement),
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        heightCode,
        ctx.FormatControlColorCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

// ============================================================================
// 2. StepSlider (Discrete Labeled Steps)
// ============================================================================

void RenderStepSliderInspector(guikit::Context& ui, DemoContext&) {
    RenderSharedSliderInspector(ui);
    if (auto el = guikit::Element(ui, {.label = "Show Step Ticks"})) {
        if (guikit::Switch(ui, s_state.showTicks).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Step Labels"})) {
        if (guikit::Switch(ui, s_state.showLabels).changed) ui.InvalidateAll();
    }
}

void RenderStepSliderStage(guikit::Context& ui, DemoContext& ctx) {
    const guikit::AxisSize wSize = ctx.ResolveCtrlWidth();
    const guikit::AxisSize hSize = ResolveSliderHeight(ctx);

    ctx.RunElementStage(ui, [&]() {
        static constexpr keel::StaticArray<keel::StringRef, 5> stepItems = {"Off", "Low", "Mid", "High", "Max"};
        guikit::StepSliderOptions opts{};
        opts.orientation = s_state.orientation;
        opts.textLeft = s_state.textLeft;
        opts.textRight = s_state.textRight;
        opts.showTicks = s_state.showTicks;
        opts.showLabels = s_state.showLabels;
        opts.showValue = s_state.showValue;
        opts.valuePlacement = s_state.valuePlacement;
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.width = wSize;
        opts.height = hSize;
        opts.color = ctx.GetControlColor();
        guikit::StepSlider(ui, s_state.stepIdx, stepItems, opts);
    });
}

keel::String GenerateStepSliderCode(const DemoContext& ctx) {
    const keel::FixedString<48> heightCode = DemoContext::FormatAxisSizeCode(ResolveSliderHeight(ctx));
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    const keel::String ctrl = keel::format(
        "guikit::StepSlider(ui, presetIndex, stepItems, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .textLeft = \"{}\",\n"
        "    .textRight = \"{}\",\n"
        "    .showTicks = {},\n"
        "    .showLabels = {},\n"
        "    .showValue = {},\n"
        "    .valuePlacement = guikit::Placement::{},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .color = {}\n"
        "}});",
        OrientationEnumName(s_state.orientation),
        s_state.textLeft,
        s_state.textRight,
        s_state.showTicks ? "true" : "false",
        s_state.showLabels ? "true" : "false",
        s_state.showValue ? "true" : "false",
        PlacementName(s_state.valuePlacement),
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        heightCode,
        ctx.FormatControlColorCode()
    );
    keel::String out = keel::String::from(
        "static constexpr keel::StaticArray<keel::StringRef, 5> stepItems = "
        "{\"Off\", \"Low\", \"Mid\", \"High\", \"Max\"};\n"
    );
    out.append(ctx.WrapInElementCode(ctrl));
    return out;
}

// ============================================================================
// 3. RangeSlider (Dual-Thumb Interval)
// ============================================================================

void RenderRangeSliderInspector(guikit::Context& ui, DemoContext&) {
    RenderSharedSliderInspector(ui);
    if (auto el = guikit::Element(ui, {.label = "Quantize Step (5.0%)"})) {
        if (guikit::Switch(ui, s_state.quantizeStep).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Minimum Span"})) {
        if (guikit::Slider(ui, s_state.rangeMinSpan, 0.0f, 40.0f, {
            .valueFormat = "%.0f %%",
            .step = 5.0f
        }).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderRangeSliderStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::Optional<keel::f64> stepOpt = s_state.quantizeStep ? keel::Optional<keel::f64>(5.0) : keel::kNullOpt;
    const guikit::AxisSize wSize = ctx.ResolveCtrlWidth();
    const guikit::AxisSize hSize = ResolveSliderHeight(ctx);

    ctx.RunElementStage(ui, [&]() {
        guikit::RangeSliderOptions opts{};
        opts.orientation = s_state.orientation;
        opts.textLeft = s_state.textLeft;
        opts.textRight = s_state.textRight;
        opts.valueFormat = "%.0f %%";
        opts.step = stepOpt;
        opts.minSpan = keel::as<keel::f64>(s_state.rangeMinSpan);
        opts.showValue = s_state.showValue;
        opts.valuePlacement = s_state.valuePlacement;
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.width = wSize;
        opts.height = hSize;
        opts.color = ctx.GetControlColor();
        guikit::RangeSlider(ui, s_state.rangeLow, s_state.rangeHigh, 0.0f, 100.0f, opts);
    });
}

keel::String GenerateRangeSliderCode(const DemoContext& ctx) {
    const keel::FixedString<48> heightCode = DemoContext::FormatAxisSizeCode(ResolveSliderHeight(ctx));
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    const keel::String ctrl = keel::format(
        "guikit::RangeSlider(ui, rangeLow, rangeHigh, 0.0f, 100.0f, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .textLeft = \"{}\",\n"
        "    .textRight = \"{}\",\n"
        "    .valueFormat = \"%.0f %%\",\n"
        "    .step = {},\n"
        "    .minSpan = {:.1f},\n"
        "    .showValue = {},\n"
        "    .valuePlacement = guikit::Placement::{},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .color = {}\n"
        "}});",
        OrientationEnumName(s_state.orientation),
        s_state.textLeft,
        s_state.textRight,
        s_state.quantizeStep ? "5.0" : "keel::kNullOpt",
        s_state.rangeMinSpan,
        s_state.showValue ? "true" : "false",
        PlacementName(s_state.valuePlacement),
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        heightCode,
        ctx.FormatControlColorCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

// ============================================================================
// 4. HybridSlider (Discrete Leading Stops + Continuous Range)
// ============================================================================

void RenderHybridSliderInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 2> kPresetNames = {
        "Off | 1..100",
        "Off | Auto | 1..100"
    };
    if (auto el = guikit::Element(ui, {.label = "Stop Preset"})) {
        if (guikit::Segmented(ui, s_state.hybridPreset, kPresetNames).changed) {
            ui.InvalidateAll();
        }
    }

    keel::i32 orientIdx = (s_state.orientation == guikit::Orientation::Vertical) ? 1 : 0;
    static constexpr keel::StaticArray<keel::StringRef, 2> orientNames = {"Horizontal", "Vertical (Fader)"};
    if (auto el = guikit::Element(ui, {.label = "Orientation"})) {
        if (guikit::Segmented(ui, orientIdx, orientNames).changed) {
            s_state.orientation = (orientIdx == 1)
                ? guikit::Orientation::Vertical
                : guikit::Orientation::Horizontal;
            ui.InvalidateAll();
        }
    }
    if (s_state.orientation == guikit::Orientation::Vertical) {
        if (auto el = guikit::Element(ui, {.label = "Compact Vertical Height (Fit)"})) {
            if (guikit::Switch(ui, s_state.compactVertical).changed) ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Show Stop Ticks"})) {
        if (guikit::Switch(ui, s_state.showTicks).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Stop Labels"})) {
        if (guikit::Switch(ui, s_state.showLabels).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Manual Value"})) {
        if (guikit::Switch(ui, s_state.showValue).changed) ui.InvalidateAll();
    }
    static constexpr keel::StaticArray<keel::StringRef, 5> valPlaceNames = {
        "Leading", "Trailing", "Top", "Bottom", "Thumb"
    };
    keel::usize valPlaceIdx = keel::as<keel::usize>(s_state.hybridValuePlacement);
    if (auto el = guikit::Element(ui, {.label = "Value Placement", .disabled = !s_state.showValue})) {
        if (guikit::Select(ui, valPlaceIdx, valPlaceNames, {
            .disabled = !s_state.showValue
        }).changed) {
            s_state.hybridValuePlacement = keel::from_underlying<guikit::Placement>(valPlaceIdx);
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Quantize Step (5.0%)"})) {
        if (guikit::Switch(ui, s_state.quantizeStep).changed) ui.InvalidateAll();
    }
}

void RenderHybridSliderStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::Optional<keel::f64> stepOpt = s_state.quantizeStep ? keel::Optional<keel::f64>(5.0) : keel::kNullOpt;
    const guikit::AxisSize wSize = ctx.ResolveCtrlWidth();
    const guikit::AxisSize hSize = ResolveSliderHeight(ctx);

    ctx.RunElementStage(ui, [&]() {
        guikit::HybridSliderOptions opts{};
        opts.orientation = s_state.orientation;
        opts.valueFormat = "%.0f %%";
        opts.step = stepOpt;
        opts.showTicks = s_state.showTicks;
        opts.showStopLabels = s_state.showLabels;
        opts.showValue = s_state.showValue;
        opts.valuePlacement = s_state.hybridValuePlacement;
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.width = wSize;
        opts.height = hSize;
        opts.color = ctx.GetControlColor();

        if (s_state.hybridPreset == 0) {
            guikit::HybridSlider(ui, s_state.hybridEnabled, s_state.hybridVal, 1.0f, 100.0f, opts);
        } else {
            static constexpr keel::StaticArray<keel::StringRef, 2> kStops = {"Off", "Auto"};
            guikit::HybridSlider(ui, s_state.hybridMode, s_state.hybridVal, 1.0f, 100.0f, kStops, opts);
        }
    });
}

keel::String GenerateHybridSliderCode(const DemoContext& ctx) {
    const keel::FixedString<48> heightCode = DemoContext::FormatAxisSizeCode(ResolveSliderHeight(ctx));
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    if (s_state.hybridPreset == 0) {
        const keel::String ctrl = keel::format(
            "guikit::HybridSlider(ui, enabled, value, 1.0f, 100.0f, {{\n"
            "    .orientation = guikit::Orientation::{},\n"
            "    .valueFormat = \"%.0f %%\",\n"
            "    .step = {},\n"
            "    .showTicks = {},\n"
            "    .showStopLabels = {},\n"
            "    .showValue = {},\n"
            "    .valuePlacement = guikit::Placement::{},\n"
            "    .disabled = {},\n"
            "{}"
            "    .width = {},\n"
            "    .height = {},\n"
            "    .color = {}\n"
            "}});",
            OrientationEnumName(s_state.orientation),
            s_state.quantizeStep ? "5.0" : "keel::kNullOpt",
            s_state.showTicks ? "true" : "false",
            s_state.showLabels ? "true" : "false",
            s_state.showValue ? "true" : "false",
            PlacementName(s_state.hybridValuePlacement),
            ctx.elemDisabled ? "true" : "false",
            sizeLine,
            ctx.FormatCtrlWidthCode(),
            heightCode,
            ctx.FormatControlColorCode()
        );
        return ctx.WrapInElementCode(ctrl);
    }

    const keel::String ctrl = keel::format(
        "guikit::HybridSlider(ui, mode, value, 1.0f, 100.0f, stops, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .valueFormat = \"%.0f %%\",\n"
        "    .step = {},\n"
        "    .showTicks = {},\n"
        "    .showStopLabels = {},\n"
        "    .showValue = {},\n"
        "    .valuePlacement = guikit::Placement::{},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .color = {}\n"
        "}});",
        OrientationEnumName(s_state.orientation),
        s_state.quantizeStep ? "5.0" : "keel::kNullOpt",
        s_state.showTicks ? "true" : "false",
        s_state.showLabels ? "true" : "false",
        s_state.showValue ? "true" : "false",
        PlacementName(s_state.hybridValuePlacement),
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        heightCode,
        ctx.FormatControlColorCode()
    );
    keel::String out = keel::String::from(
        "static constexpr keel::StaticArray<keel::StringRef, 2> stops = {\"Off\", \"Auto\"};\n"
    );
    out.append(ctx.WrapInElementCode(ctrl));
    return out;
}

} // namespace guikit::demo
