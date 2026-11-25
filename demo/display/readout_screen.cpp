#include "../demo_context.hpp"
#include "guikit/display/readout.hpp"
#include "keel/math.h"

namespace guikit::demo {

namespace {

struct ReadoutDemoState {
    bool liveSim = true;
    bool showPlusSign = false;
    bool elevated = false;
    bool lockZero = false;
    bool allowChannelMenu = true;
    bool allowPrecisionMenu = true;
    bool allowUnitMenu = true;
    keel::i32 maxPrecision = 5;

    // Single bench instrument telemetry readout slot (all 6 channels switchable via its Channel menu)
    guikit::ReadoutState slot{.channelIndex = 0, .unitIndex = 1, .precision = 2};

    // Per-channel relative zero offsets
    keel::f64 voltageZeroV = 0.0;
    keel::f64 currentZeroA = 0.0;
    keel::f64 powerZeroW = 0.0;
    keel::f64 tempZeroC = 0.0;
};

ReadoutDemoState s_state;

constexpr guikit::ReadoutUnit kVoltageUnits[] = {
    {.label = "mV", .factor = 1000.0},
    {.label = "V",  .factor = 1.0},
    {.label = "kV", .factor = 0.001}
};

constexpr guikit::ReadoutUnit kCurrentUnits[] = {
    {.label = "mA", .factor = 1000.0},
    {.label = "A",  .factor = 1.0}
};

constexpr guikit::ReadoutUnit kPowerUnits[] = {
    {.label = "mW", .factor = 1000.0},
    {.label = "W",  .factor = 1.0},
    {.label = "kW", .factor = 0.001}
};

constexpr guikit::ReadoutUnit kFreqUnits[] = {
    {.label = "Hz",  .factor = 1.0},
    {.label = "kHz", .factor = 0.001},
    {.label = "MHz", .factor = 1e-6}
};

constexpr guikit::ReadoutUnit kTempUnits[] = {
    {.label = "C", .factor = 1.0, .offset = 0.0},
    {.label = "F", .factor = 1.8, .offset = 32.0},
    {.label = "K", .factor = 1.0, .offset = 273.15}
};

} // namespace

void RenderReadoutInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Live Signal Simulation"})) {
        if (guikit::Switch(ui, s_state.liveSim).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show '+' Sign"})) {
        if (guikit::Switch(ui, s_state.showPlusSign).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Elevated Card"})) {
        if (guikit::Switch(ui, s_state.elevated).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Lock Zero Action"})) {
        if (guikit::Switch(ui, s_state.lockZero).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Allow Channel Menu"})) {
        if (guikit::Switch(ui, s_state.allowChannelMenu).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Allow Precision Menu"})) {
        if (guikit::Switch(ui, s_state.allowPrecisionMenu).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Allow Unit Menu"})) {
        if (guikit::Switch(ui, s_state.allowUnitMenu).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Max Precision"})) {
        static constexpr keel::StaticArray<keel::StringRef, 6> kPrecSteps = {
            "1", "2", "3", "4", "5", "6"
        };
        keel::i32 stepIdx = keel::clamp(s_state.maxPrecision - 1, 0, 5);
        if (guikit::StepSlider(ui, stepIdx, kPrecSteps).changed) {
            s_state.maxPrecision = stepIdx + 1;
            ui.InvalidateAll();
        }
    }

    if (guikit::Button(ui, {
        .text = "Clear All Zero Offsets",
        .icon = guikit::Icon::Refresh(),
        .width = guikit::SizeMode::Stretch,
        .variant = guikit::UiVariant::Secondary
    }).clicked) {
        s_state.voltageZeroV = 0.0;
        s_state.currentZeroA = 0.0;
        s_state.powerZeroW = 0.0;
        s_state.tempZeroC = 0.0;
        ui.InvalidateAll();
    }
}

void RenderReadoutStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::f64 t = ui.GetCurrentTime();
    if (s_state.liveSim) {
        ui.RequestRepaint(1);
    }

    const keel::f64 rawVoltageV = s_state.liveSim
        ? (12.04 + 0.35 * keel::sin(t * 0.9) + 0.02 * keel::sin(t * 6.1))
        : 12.04;
    const keel::f64 netVoltageV = rawVoltageV - s_state.voltageZeroV;

    const keel::f64 rawCurrentA = s_state.liveSim
        ? (1.850 + 0.420 * keel::sin(t * 0.65))
        : 1.850;
    const keel::f64 netCurrentA = rawCurrentA - s_state.currentZeroA;

    const keel::f64 rawPowerW = rawVoltageV * rawCurrentA;
    const keel::f64 netPowerW = rawPowerW - s_state.powerZeroW;

    const keel::f64 freqHz = s_state.liveSim
        ? (48000.0 + 125.0 * keel::sin(t * 0.5))
        : 48000.0;

    const keel::f64 rawTempC = s_state.liveSim
        ? (24.6 + 1.4 * keel::sin(t * 0.35))
        : 24.6;
    const keel::f64 netTempC = rawTempC - s_state.tempZeroC;

    const keel::f64 elapsed = s_state.liveSim ? t : 83.4;
    const keel::i32 totalSec = keel::as<keel::i32>(elapsed);
    const keel::i32 mins = (totalSec / 60) % 60;
    const keel::i32 secs = totalSec % 60;
    const keel::i32 tenths = keel::as<keel::i32>(elapsed * 10.0) % 10;
    const keel::StringRef uptimeStr = ui.Format("{:02}:{:02}.{}", mins, secs, tenths);

    const bool overCurrent = (netCurrentA > 2.15);

    guikit::ReadoutChannel channels[6] = {
        {
            .label = "Supply Voltage",
            .headerLabel = "VOLTAGE",
            .value = netVoltageV,
            .units = kVoltageUnits,
            .defaultUnitIndex = 1,
            .defaultPrecision = 2,
            .variant = guikit::UiVariant::Primary,
            .showAction = true,
            .actionActive = (keel::abs(s_state.voltageZeroV) > 1e-4),
            .actionDisabled = s_state.lockZero,
            .actionTooltip = "Set relative zero (REL)"
        },
        {
            .label = "Output Current",
            .headerLabel = "CURRENT",
            .value = netCurrentA,
            .units = kCurrentUnits,
            .defaultUnitIndex = 1,
            .defaultPrecision = 3,
            .variant = overCurrent ? guikit::UiVariant::Danger : guikit::UiVariant::Info,
            .showAction = true,
            .actionActive = (keel::abs(s_state.currentZeroA) > 1e-4),
            .actionDisabled = s_state.lockZero,
            .actionTooltip = "Zero current offset"
        },
        {
            .label = "Active Power",
            .headerLabel = "POWER",
            .value = netPowerW,
            .units = kPowerUnits,
            .defaultUnitIndex = 1,
            .defaultPrecision = 2,
            .variant = guikit::UiVariant::Success,
            .showAction = true,
            .actionActive = (keel::abs(s_state.powerZeroW) > 1e-4),
            .actionDisabled = s_state.lockZero,
            .actionTooltip = "Zero power baseline"
        },
        {
            .label = "Carrier Frequency",
            .headerLabel = "FREQUENCY",
            .value = freqHz,
            .units = kFreqUnits,
            .defaultUnitIndex = 1,
            .defaultPrecision = 3,
            .variant = guikit::UiVariant::Primary,
            .showAction = false
        },
        {
            .label = "Board Temperature",
            .headerLabel = "TEMPERATURE",
            .value = netTempC,
            .units = kTempUnits,
            .defaultUnitIndex = 0,
            .defaultPrecision = 1,
            .variant = guikit::UiVariant::Warning,
            .showAction = true,
            .actionActive = (keel::abs(s_state.tempZeroC) > 1e-4),
            .actionDisabled = s_state.lockZero,
            .actionTooltip = "Measure delta temperature"
        },
        {
            .label = "Session Uptime",
            .headerLabel = "UPTIME",
            .value = elapsed,
            .formattedValue = uptimeStr,
            .variant = guikit::UiVariant::Default,
            .showAction = false
        }
    };

    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Md : ctx.controlsSize;

    guikit::ReadoutOptions ro{};
    ro.size = effSize;
    ro.width = ctx.ResolveCtrlWidth();
    ro.height = ctx.ResolveCtrlHeight();
    ro.minWidth = 204.0f;
    ro.surface = s_state.elevated ? guikit::SurfaceRole::Raised : guikit::SurfaceRole::Flat;
    ro.disabled = ctx.elemDisabled;
    ro.maxPrecision = s_state.maxPrecision;
    ro.allowChannelMenu = s_state.allowChannelMenu;
    ro.allowPrecisionMenu = s_state.allowPrecisionMenu;
    ro.allowUnitMenu = s_state.allowUnitMenu;
    ro.showPlusSign = s_state.showPlusSign;
    ro.channelMenuTitle = "CHANNEL";
    ro.precisionMenuTitle = "PRECISION";
    ro.unitMenuTitle = "UNITS";

    const auto resp = guikit::Readout(ui, s_state.slot, channels, ro);
    if (resp.actionClicked) {
        const keel::i32 ch = s_state.slot.channelIndex;
        if (ch == 0) {
            s_state.voltageZeroV = (keel::abs(s_state.voltageZeroV) > 1e-4) ? 0.0 : rawVoltageV;
        } else if (ch == 1) {
            s_state.currentZeroA = (keel::abs(s_state.currentZeroA) > 1e-4) ? 0.0 : rawCurrentA;
        } else if (ch == 2) {
            s_state.powerZeroW = (keel::abs(s_state.powerZeroW) > 1e-4) ? 0.0 : rawPowerW;
        } else if (ch == 4) {
            s_state.tempZeroC = (keel::abs(s_state.tempZeroC) > 1e-4) ? 0.0 : rawTempC;
        }
        guikit::ShowToast(ui, {
            .title = "Relative Zero Updated",
            .message = channels[keel::as<keel::usize>(ch)].label,
            .variant = guikit::UiVariant::Success
        });
        ui.InvalidateAll();
    }
}

keel::String GenerateReadoutCode(const DemoContext& ctx) {
    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Md : ctx.controlsSize;
    return keel::format(
        "static constexpr guikit::ReadoutUnit kVoltageUnits[] = {{\n"
        "    {{.label = \"mV\", .factor = 1000.0}},\n"
        "    {{.label = \"V\",  .factor = 1.0}},\n"
        "    {{.label = \"kV\", .factor = 0.001}}\n"
        "}};\n\n"
        "guikit::ReadoutChannel channels[] = {{\n"
        "    {{.label = \"Supply Voltage\", .headerLabel = \"VOLTAGE\",\n"
        "      .value = voltageV, .units = kVoltageUnits,\n"
        "      .defaultUnitIndex = 1, .defaultPrecision = 2}}\n"
        "}};\n\n"
        "auto resp = guikit::Readout(ui, slotState, channels, {{\n"
        "    .size = guikit::UiSize::{},\n"
        "    .minWidth = 204.0f,\n"
        "    .surface = guikit::SurfaceRole::{},\n"
        "    .disabled = {},\n"
        "    .maxPrecision = {},\n"
        "    .showPlusSign = {}\n"
        "}});\n"
        "if (resp.actionClicked) {{ ZeroChannel(slotState.channelIndex); }}",
        SizeName(effSize),
        s_state.elevated ? "Raised" : "Flat",
        ctx.elemDisabled ? "true" : "false",
        s_state.maxPrecision,
        s_state.showPlusSign ? "true" : "false"
    );
}

} // namespace guikit::demo
