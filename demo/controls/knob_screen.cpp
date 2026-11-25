#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct KnobDemoState {
    keel::f32 val = 65.0f;
    keel::f32 bipolarVal = 18.0f;
    bool bipolar = false;
    bool quantizeStep = false;
    bool showArc = true;
    bool showTicks = true;
    keel::u32 tickCount = 11;
    bool showValue = true;
    guikit::Placement valuePlacement = guikit::Placement::Bottom;
};
KnobDemoState s_state;
} // namespace

void RenderKnobInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {
        .label = "Bipolar Center-Zero Sweep",
        .description = "Fill arc from 12 o'clock center (-50..+50)"
    })) {
        if (guikit::Switch(ui, s_state.bipolar).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show 270° Active Arc"})) {
        if (guikit::Switch(ui, s_state.showArc).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show Radial Ticks"})) {
        if (guikit::Switch(ui, s_state.showTicks).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Radial Tick Count", .disabled = !s_state.showTicks})) {
        if (guikit::Slider(ui, s_state.tickCount, 5u, 21u, {
            .valueFormat = "%u ticks",
            .step = 2.0,
            .disabled = !s_state.showTicks
        }).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Quantize Step (5.0)"})) {
        if (guikit::Switch(ui, s_state.quantizeStep).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show Value Readout"})) {
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
}

void RenderKnobStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::Optional<keel::f64> stepOpt = s_state.quantizeStep ? keel::Optional<keel::f64>(5.0) : keel::kNullOpt;

    ctx.RunElementStage(ui, [&]() {
        guikit::KnobOptions opts{};
        opts.step = stepOpt;
        opts.tickCount = s_state.tickCount;
        opts.showValue = s_state.showValue;
        opts.showTicks = s_state.showTicks;
        opts.showArc = s_state.showArc;
        opts.bipolar = s_state.bipolar;
        opts.valuePlacement = s_state.valuePlacement;
        opts.diameter = ctx.ResolveKnobDiameter();
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.color = ctx.GetControlColor();

        if (s_state.bipolar) {
            opts.valueFormat = "%+.1f %%";
            opts.defaultValue = 0.0;
            guikit::Knob(ui, s_state.bipolarVal, -50.0f, 50.0f, opts);
        } else {
            opts.valueFormat = "%.1f %%";
            opts.defaultValue = 50.0;
            guikit::Knob(ui, s_state.val, 0.0f, 100.0f, opts);
        }
    });
}

keel::String GenerateKnobCode(const DemoContext& ctx) {
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::String ctrl = keel::format(
        "guikit::Knob(ui, knobValue, {}, {}, {{\n"
        "    .valueFormat = \"{}\",\n"
        "    .step = {},\n"
        "    .defaultValue = {},\n"
        "    .tickCount = {},\n"
        "    .showValue = {},\n"
        "    .showTicks = {},\n"
        "    .showArc = {},\n"
        "    .bipolar = {},\n"
        "    .valuePlacement = guikit::Placement::{},\n"
        "    .diameter = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .color = {}\n"
        "}});",
        s_state.bipolar ? "-50.0f" : "0.0f",
        s_state.bipolar ? "50.0f" : "100.0f",
        s_state.bipolar ? "%+.1f %%" : "%.1f %%",
        s_state.quantizeStep ? "5.0" : "keel::kNullOpt",
        s_state.bipolar ? "0.0" : "50.0",
        s_state.tickCount,
        s_state.showValue ? "true" : "false",
        s_state.showTicks ? "true" : "false",
        s_state.showArc ? "true" : "false",
        s_state.bipolar ? "true" : "false",
        PlacementName(s_state.valuePlacement),
        ctx.FormatKnobDiameterCode(),
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatControlColorCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
