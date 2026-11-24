#include "keel/math.h"
#include "../demo_context.hpp"
#include "guikit/display/level_meter.hpp"

namespace guikit::demo {

namespace {

struct LevelMeterDemoState {
    keel::usize orientationIdx = 0; // 0 = Horizontal, 1 = Vertical
    bool compactVertical = false;
    keel::f32 levelDb = -8.5f;
    keel::f32 stereoSpreadDb = 3.5f;
    bool simulateLive = false;
    bool segmented = true;
    keel::u32 segments = 24;
    bool multiZoneColor = true;
    bool autoPeakHold = true;
    bool showClipLed = true;
    bool showScale = true;
    bool showValue = true;
    keel::f32 barThickness = 12.0f;
};

LevelMeterDemoState s_state;

} // namespace

void RenderLevelMeterInspector(guikit::Context& ui, DemoContext& ctx) {
    const bool stereo = (ctx.GetDemoVariant(NavCategory::DisplayLevelMeter) == 0u);

    static constexpr keel::StaticArray<keel::StringRef, 2> orientNames = {"Horizontal", "Vertical"};
    if (auto el = guikit::Element(ui, {.label = "Orientation"})) {
        if (guikit::Segmented(ui, s_state.orientationIdx, orientNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (s_state.orientationIdx == 1) {
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
        .label = "Signal Level (dBFS)",
        .description = "Drive to 0.0 dB to trigger Clip LED (click meter to clear)"
    })) {
        if (guikit::Slider(ui, s_state.levelDb, -60.0f, 0.0f, {
            .valueFormat = "%.1f dB",
            .step = 0.5f
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (stereo) {
        if (auto el = guikit::Element(ui, {.label = "Stereo L/R Offset"})) {
            if (guikit::Slider(ui, s_state.stereoSpreadDb, -12.0f, 12.0f, {
                .valueFormat = "%+.1f dB",
                .step = 0.5f
            }).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Simulate Live Signal",
        .description = "Animate dynamic RF / Audio telemetry modulation"
    })) {
        if (guikit::Switch(ui, s_state.simulateLive).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Discrete LED Segments",
        .description = "Segmented LED bar vs smooth continuous ribbon"
    })) {
        if (guikit::Switch(ui, s_state.segmented).changed) {
            ui.InvalidateAll();
        }
    }

    if (s_state.segmented) {
        if (auto el = guikit::Element(ui, {.label = "LED Segment Count"})) {
            if (guikit::Slider(ui, s_state.segments, 8u, 40u, {
                .valueFormat = "%u LEDs",
                .step = 2.0
            }).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Bar Thickness"})) {
        if (guikit::Slider(ui, s_state.barThickness, 6.0f, 24.0f, {
            .valueFormat = "%.0f dp",
            .step = 1.0f
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "3-Zone Colors (Green / Warn / Danger)"})) {
        if (guikit::Switch(ui, s_state.multiZoneColor).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Auto Peak-Hold & Decay"})) {
        if (guikit::Switch(ui, s_state.autoPeakHold).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show Overload / Clip LED"})) {
        if (guikit::Switch(ui, s_state.showClipLed).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show Calibrated dB Scale"})) {
        if (guikit::Switch(ui, s_state.showScale).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show Numeric Readout"})) {
        if (guikit::Switch(ui, s_state.showValue).changed) ui.InvalidateAll();
    }
}

void RenderLevelMeterStage(guikit::Context& ui, DemoContext& ctx) {
    keel::f32 chLeft = s_state.levelDb;
    keel::f32 chRight = keel::clamp(s_state.levelDb - s_state.stereoSpreadDb, -60.0f, 0.0f);

    if (s_state.simulateLive) {
        const keel::f64 t = ui.GetCurrentTimeSec();
        const keel::f32 envL = keel::as<keel::f32>(
            keel::sin(t * 4.2) * 4.5 +
            keel::sin(t * 11.3) * 2.8 +
            keel::max(0.0, keel::sin(t * 1.9)) * 4.2
        );
        const keel::f32 envR = keel::as<keel::f32>(
            keel::sin(t * 3.8 + 0.9) * 4.5 +
            keel::cos(t * 9.7) * 3.0 +
            keel::max(0.0, keel::sin(t * 1.9 + 0.4)) * 4.0
        );
        chLeft = keel::clamp(s_state.levelDb + envL, -60.0f, 0.0f);
        chRight = keel::clamp(s_state.levelDb - s_state.stereoSpreadDb + envR, -60.0f, 0.0f);
        ui.RequestRepaint(2);
    }

    const guikit::Orientation orient = (s_state.orientationIdx == 0)
        ? guikit::Orientation::Horizontal
        : guikit::Orientation::Vertical;

    static constexpr keel::StaticArray<keel::StringRef, 2> kStereoLabels = {"L", "R"};
    static constexpr keel::StaticArray<keel::StringRef, 1> kMonoLabels = {"IN"};

    const bool stereo = (ctx.GetDemoVariant(NavCategory::DisplayLevelMeter) == 0u);

    guikit::LevelMeterOptions opts{};
    opts.orientation = orient;
    opts.segments = s_state.segmented ? s_state.segments : 0u;
    opts.multiZoneColor = s_state.multiZoneColor;
    opts.autoPeakHold = s_state.autoPeakHold;
    opts.showClipLed = s_state.showClipLed;
    opts.showScale = s_state.showScale;
    opts.showValue = s_state.showValue;
    opts.valueFormat = "%.1f dB";
    opts.channelLabels = stereo
        ? keel::ArrayRef<keel::StringRef>(kStereoLabels)
        : keel::ArrayRef<keel::StringRef>(kMonoLabels);
    opts.barThickness = s_state.barThickness;
    opts.tooltip = "Click meter to reset Peak-Hold & Clip latch";
    opts.disabled = ctx.elemDisabled;
    opts.size = ctx.controlsSize;
    opts.width = ctx.ResolveCtrlWidth();
    opts.height = (orient == guikit::Orientation::Vertical && s_state.compactVertical && ctx.ResolveCtrlHeight().IsAuto())
        ? guikit::AxisSize(guikit::SizeMode::Fit)
        : ctx.ResolveCtrlHeight();
    opts.color = ctx.GetControlColor();

    ctx.RunElementStage(ui, [&]() {
        if (stereo) {
            const keel::StaticArray<keel::f32, 2> stereoVals = {chLeft, chRight};
            guikit::LevelMeter(ui, stereoVals, -60.0f, 0.0f, opts);
        } else {
            guikit::LevelMeter(ui, keel::as<keel::f64>(chLeft), -60.0, 0.0, opts);
        }
    });
}

keel::String GenerateLevelMeterCode(const DemoContext& ctx) {
    const bool stereo = (ctx.GetDemoVariant(NavCategory::DisplayLevelMeter) == 0u);
    const keel::StringRef orientName = (s_state.orientationIdx == 0) ? "Horizontal" : "Vertical";
    const guikit::AxisSize ctrlH =
        (s_state.orientationIdx == 1 && s_state.compactVertical && ctx.ResolveCtrlHeight().IsAuto())
            ? guikit::AxisSize(guikit::SizeMode::Fit)
            : ctx.ResolveCtrlHeight();
    const keel::FixedString<48> heightCode = DemoContext::FormatAxisSizeCode(ctrlH);
    const keel::u32 segVal = s_state.segmented ? s_state.segments : 0u;
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    if (stereo) {
        const keel::String ctrl = keel::format(
            "guikit::LevelMeter(ui, levelsDb, -60.0f, 0.0f, {{\n"
            "    .orientation = guikit::Orientation::{},\n"
            "    .segments = {}u,\n"
            "    .multiZoneColor = {},\n"
            "    .autoPeakHold = {},\n"
            "    .showClipLed = {},\n"
            "    .showScale = {},\n"
            "    .showValue = {},\n"
            "    .valueFormat = \"%.1f dB\",\n"
            "    .channelLabels = kChannels,\n"
            "    .barThickness = {:.1f}f,\n"
            "    .disabled = {},\n"
            "{}"
            "    .width = {},\n"
            "    .height = {},\n"
            "    .color = {}\n"
            "}});",
            orientName,
            segVal,
            s_state.multiZoneColor ? "true" : "false",
            s_state.autoPeakHold ? "true" : "false",
            s_state.showClipLed ? "true" : "false",
            s_state.showScale ? "true" : "false",
            s_state.showValue ? "true" : "false",
            s_state.barThickness,
            ctx.elemDisabled ? "true" : "false",
            sizeLine,
            ctx.FormatCtrlWidthCode(),
            heightCode,
            ctx.FormatControlColorCode()
        );
        keel::String out = keel::format(
            "static constexpr keel::StaticArray<keel::StringRef, 2> kChannels = {{\"L\", \"R\"}};\n"
            "const keel::StaticArray<float, 2> levelsDb = {{{:.1f}f, {:.1f}f}};\n",
            s_state.levelDb,
            keel::clamp(s_state.levelDb - s_state.stereoSpreadDb, -60.0f, 0.0f)
        );
        out.append(ctx.WrapInElementCode(ctrl));
        return out;
    }

    const keel::String ctrl = keel::format(
        "guikit::LevelMeter(ui, signalDb, -60.0, 0.0, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .segments = {}u,\n"
        "    .multiZoneColor = {},\n"
        "    .autoPeakHold = {},\n"
        "    .showClipLed = {},\n"
        "    .showScale = {},\n"
        "    .showValue = {},\n"
        "    .valueFormat = \"%.1f dB\",\n"
        "    .barThickness = {:.1f}f,\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .color = {}\n"
        "}});",
        orientName,
        segVal,
        s_state.multiZoneColor ? "true" : "false",
        s_state.autoPeakHold ? "true" : "false",
        s_state.showClipLed ? "true" : "false",
        s_state.showScale ? "true" : "false",
        s_state.showValue ? "true" : "false",
        s_state.barThickness,
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        heightCode,
        ctx.FormatControlColorCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
