#include "../demo_context.hpp"
#include "guikit/widgets/readout.hpp"
#include "keel/math.h"

namespace guikit::demo {

namespace {

struct ReadoutDemoState {
    bool liveSim = true;
    bool showPlusSign = false;
    bool elevated = false;
    bool disableTare = false;
    bool allowChannelMenu = true;
    bool allowPrecisionMenu = true;
    bool allowUnitMenu = true;
    keel::i32 maxPrecision = 5;

    // 3 telemetry slots like an industrial tensile testing machine header bar
    guikit::ReadoutState slots[3] = {
        {.channelIndex = 0, .unitIndex = 1, .precision = 2}, // LOAD (kN)
        {.channelIndex = 1, .unitIndex = 1, .precision = 2}, // PEAK LOAD (kN)
        {.channelIndex = 2, .unitIndex = 0, .precision = 2}  // DISPLACEMENT (mm)
    };

    // Per-channel tare offsets & simulated raw sensor values
    double loadTareN = 0.0;
    double peakLoadN = 18420.0;
    double dispTareMm = 0.0;
    double extTareUm = 0.0;

    // Single-channel chamber temperature demo
    keel::i32 tempUnit = 0;
    keel::i32 tempPrec = 1;
};

ReadoutDemoState s_state;

constexpr guikit::ReadoutUnit kForceUnits[] = {
    {.label = "N",   .factor = 1.0},
    {.label = "kN",  .factor = 0.001},
    {.label = "kgf", .factor = 0.101971621},
    {.label = "lbf", .factor = 0.224808943}
};

constexpr guikit::ReadoutUnit kDispUnits[] = {
    {.label = "mm", .factor = 1.0},
    {.label = "cm", .factor = 0.1},
    {.label = "m",  .factor = 0.001},
    {.label = "in", .factor = 0.0393700787}
};

constexpr guikit::ReadoutUnit kStrainUnits[] = {
    {.label = "um", .factor = 1.0},
    {.label = "mm", .factor = 0.001},
    {.label = "%",  .factor = 0.002} // 50 mm gauge length
};

constexpr guikit::ReadoutUnit kPressureUnits[] = {
    {.label = "MPa", .factor = 1.0},
    {.label = "bar", .factor = 10.0},
    {.label = "psi", .factor = 145.037738}
};

constexpr guikit::ReadoutUnit kTempUnits[] = {
    {.label = "C", .factor = 1.0, .offset = 0.0},
    {.label = "F", .factor = 1.8, .offset = 32.0},
    {.label = "K", .factor = 1.0, .offset = 273.15}
};

} // namespace

void RenderReadoutInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = ui.Element({.label = "Live Sensor Simulation"})) {
        if (ui.Switch(s_state.liveSim).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Show '+' Sign"})) {
        if (ui.Switch(s_state.showPlusSign).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Elevated Card"})) {
        if (ui.Switch(s_state.elevated).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Lock Tare Action"})) {
        if (ui.Switch(s_state.disableTare).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Allow Channel Menu"})) {
        if (ui.Switch(s_state.allowChannelMenu).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Allow Precision Menu"})) {
        if (ui.Switch(s_state.allowPrecisionMenu).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Allow Unit Menu"})) {
        if (ui.Switch(s_state.allowUnitMenu).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Max Precision"})) {
        static constexpr keel::StaticArray<keel::StringRef, 6> kPrecSteps = {
            "1", "2", "3", "4", "5", "6"
        };
        keel::i32 stepIdx = keel::clamp(s_state.maxPrecision - 1, 0, 5);
        if (ui.StepSlider(stepIdx, kPrecSteps).changed) {
            s_state.maxPrecision = stepIdx + 1;
            ui.InvalidateAll();
        }
    }

    if (ui.Button({
        .text = "Reset All Tare Offsets",
        .icon = guikit::Icon::Refresh(),
        .width = guikit::SizeMode::Stretch,
        .background = guikit::StateColor::Secondary()
    }).clicked) {
        s_state.loadTareN = 0.0;
        s_state.dispTareMm = 0.0;
        s_state.extTareUm = 0.0;
        s_state.peakLoadN = 18420.0;
        ui.InvalidateAll();
    }
}

void RenderReadoutStage(guikit::Context& ui, DemoContext& ctx) {
    const double t = ui.GetCurrentTime();
    if (s_state.liveSim) {
        ui.RequestRepaint(1);
    }

    const double rawLoadN = s_state.liveSim
        ? (12500.0 + 6200.0 * keel::sin(t * 0.85) + 180.0 * keel::sin(t * 5.3))
        : 12450.0;
    const double netLoadN = rawLoadN - s_state.loadTareN;
    if (netLoadN > s_state.peakLoadN) {
        s_state.peakLoadN = netLoadN;
    }

    const double rawDispMm = s_state.liveSim
        ? (14.25 + 6.5 * keel::sin(t * 0.45))
        : 14.25;
    const double netDispMm = rawDispMm - s_state.dispTareMm;

    const double rawExtUm = s_state.liveSim
        ? (420.0 + 185.0 * keel::sin(t * 0.85))
        : 420.0;
    const double netExtUm = rawExtUm - s_state.extTareUm;

    const double oilMPa = s_state.liveSim
        ? (21.4 + 1.2 * keel::sin(t * 0.6))
        : 21.4;

    const keel::i32 totalSec = static_cast<keel::i32>(s_state.liveSim ? t : 83.4);
    const keel::i32 mins = (totalSec / 60) % 60;
    const keel::i32 secs = totalSec % 60;
    const keel::i32 tenths = static_cast<keel::i32>((s_state.liveSim ? t : 83.4) * 10.0) % 10;
    const keel::StringRef timeStr = ui.Format("{:02}:{:02}.{}", mins, secs, tenths);

    const bool overload = (netLoadN > 17500.0);

    guikit::ReadoutChannel channels[6] = {
        {
            .label = "Load",
            .headerLabel = "LOAD",
            .value = netLoadN,
            .units = kForceUnits,
            .defaultUnitIndex = 1,
            .defaultPrecision = 2,
            .variant = overload ? guikit::UiVariant::Danger : guikit::UiVariant::Primary,
            .showAction = true,
            .actionActive = (keel::abs(s_state.loadTareN) > 1e-3),
            .actionDisabled = s_state.disableTare,
            .actionTooltip = "Tare load cell (zero)"
        },
        {
            .label = "Peak Load",
            .headerLabel = "PEAK LOAD",
            .value = s_state.peakLoadN,
            .units = kForceUnits,
            .defaultUnitIndex = 1,
            .defaultPrecision = 2,
            .variant = guikit::UiVariant::Info,
            .showAction = true,
            .actionDisabled = s_state.disableTare,
            .actionTooltip = "Reset peak hold"
        },
        {
            .label = "Crosshead Displacement",
            .headerLabel = "DISPLACEMENT",
            .value = netDispMm,
            .units = kDispUnits,
            .defaultUnitIndex = 0,
            .defaultPrecision = 2,
            .variant = guikit::UiVariant::Primary,
            .showAction = true,
            .actionActive = (keel::abs(s_state.dispTareMm) > 1e-3),
            .actionDisabled = s_state.disableTare,
            .actionTooltip = "Zero crosshead displacement"
        },
        {
            .label = "Extensometer",
            .headerLabel = "EXTENSOMETER",
            .value = netExtUm,
            .units = kStrainUnits,
            .defaultUnitIndex = 0,
            .defaultPrecision = 1,
            .variant = guikit::UiVariant::Success,
            .showAction = true,
            .actionActive = (keel::abs(s_state.extTareUm) > 1e-3),
            .actionDisabled = s_state.disableTare,
            .actionTooltip = "Zero extensometer"
        },
        {
            .label = "Test Time",
            .headerLabel = "TEST TIME",
            .value = s_state.liveSim ? t : 83.4,
            .formattedValue = timeStr,
            .variant = guikit::UiVariant::Default,
            .showAction = false
        },
        {
            .label = "Hydraulic Oil Pressure",
            .headerLabel = "OIL PRESSURE",
            .value = oilMPa,
            .units = kPressureUnits,
            .defaultUnitIndex = 0,
            .defaultPrecision = 2,
            .variant = guikit::UiVariant::Warning,
            .showAction = false
        }
    };

    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Md : ctx.controlsSize;

    if (auto col = ui.Column({.gap = 16.0f, .width = guikit::SizeMode::Stretch})) {
        ui.Heading({
            .label = "Telemetry Readout Bar",
            .description = "Click channel header, numeric value, or unit to open its dropdown; click ◎ to tare/reset",
            .level = guikit::HeadingLevel::H4
        });

        if (auto bar = ui.Wrap({.gap = 10.0f})) {
            for (keel::usize i = 0; i < 3; ++i) {
                guikit::ReadoutOptions ro{};
                ro.size = effSize;
                ro.minWidth = 210.0f;
                ro.elevated = s_state.elevated;
                ro.disabled = ctx.elemDisabled;
                ro.maxPrecision = s_state.maxPrecision;
                ro.allowChannelMenu = s_state.allowChannelMenu;
                ro.allowPrecisionMenu = s_state.allowPrecisionMenu;
                ro.allowUnitMenu = s_state.allowUnitMenu;
                ro.showPlusSign = s_state.showPlusSign;
                ro.channelMenuTitle = "SELECT CHANNEL";
                ro.precisionMenuTitle = "PRECISION (DIGITS)";
                ro.unitMenuTitle = "UNITS";

                const auto resp = ui.Readout(s_state.slots[i], channels, ro);
                if (resp.actionClicked) {
                    const keel::i32 ch = s_state.slots[i].channelIndex;
                    if (ch == 0) {
                        s_state.loadTareN = rawLoadN;
                        ui.ShowToast({.title = "Load Tared", .message = "Load cell zeroed at current reading", .variant = guikit::UiVariant::Success});
                    } else if (ch == 1) {
                        s_state.peakLoadN = netLoadN;
                        ui.ShowToast({.title = "Peak Reset", .message = "Peak load latch cleared", .variant = guikit::UiVariant::Info});
                    } else if (ch == 2) {
                        s_state.dispTareMm = rawDispMm;
                        ui.ShowToast({.title = "Displacement Zeroed", .message = "Crosshead reference zeroed", .variant = guikit::UiVariant::Success});
                    } else if (ch == 3) {
                        s_state.extTareUm = rawExtUm;
                        ui.ShowToast({.title = "Extensometer Zeroed", .message = "Strain reference zeroed", .variant = guikit::UiVariant::Success});
                    }
                    ui.InvalidateAll();
                }
            }
        }

        // Single-channel affine conversion readout (°C / °F / K)
        if (auto singleRow = ui.Row({.gap = 12.0f, .align = guikit::Alignment::Center})) {
            const double tempC = s_state.liveSim ? (23.5 + 1.8 * keel::sin(t * 0.3)) : 23.5;
            guikit::ReadoutOptions singleOpts{};
            singleOpts.size = effSize;
            singleOpts.minWidth = 210.0f;
            singleOpts.elevated = s_state.elevated;
            singleOpts.disabled = ctx.elemDisabled;
            singleOpts.allowPrecisionMenu = s_state.allowPrecisionMenu;
            singleOpts.allowUnitMenu = s_state.allowUnitMenu;
            singleOpts.showPlusSign = s_state.showPlusSign;
            ui.Readout("CHAMBER TEMP", tempC, s_state.tempUnit, s_state.tempPrec, kTempUnits, singleOpts);
        }
    }
}

keel::String GenerateReadoutCode(const DemoContext& ctx) {
    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Md : ctx.controlsSize;
    return keel::format(
        "static constexpr guikit::ReadoutUnit kForceUnits[] = {{\n"
        "    {{.label = \"N\",   .factor = 1.0}},\n"
        "    {{.label = \"kN\",  .factor = 0.001}},\n"
        "    {{.label = \"kgf\", .factor = 0.101971621}},\n"
        "    {{.label = \"lbf\", .factor = 0.224808943}}\n"
        "}};\n\n"
        "guikit::ReadoutChannel channels[] = {{\n"
        "    {{.label = \"Load\", .headerLabel = \"LOAD\", .value = loadN,\n"
        "      .units = kForceUnits, .defaultUnitIndex = 1, .defaultPrecision = 2}}\n"
        "}};\n\n"
        "auto resp = ui.Readout(slotState, channels, {{\n"
        "    .size = guikit::UiSize::{},\n"
        "    .minWidth = 210.0f,\n"
        "    .elevated = {},\n"
        "    .maxPrecision = {},\n"
        "    .showPlusSign = {}\n"
        "}});\n"
        "if (resp.actionClicked) {{ TareChannel(slotState.channelIndex); }}",
        SizeName(effSize),
        s_state.elevated ? "true" : "false",
        s_state.maxPrecision,
        s_state.showPlusSign ? "true" : "false"
    );
}

} // namespace guikit::demo
