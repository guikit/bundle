#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct RadioState {
    keel::usize modeIdx = 0; // 0 = 3-Option Group, 1 = Single Radio
    keel::i32 selectedIdx = 0;
    bool singleChecked = true;
};
RadioState s_state;
} // namespace

void RenderRadioInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 2> kModeNames = {"3-Option Group", "Single Radio"};
    if (auto el = guikit::Element(ui, {.label = "Mode"})) {
        if (guikit::Segmented(ui, s_state.modeIdx, kModeNames).changed) {
            ui.InvalidateAll();
        }
    }
    const bool groupDemo = (s_state.modeIdx == 0u);
    if (groupDemo) {
        static constexpr keel::StaticArray<keel::StringRef, 3> optNames = {"Option A", "Option B", "Option C"};
        keel::usize idx = keel::as<keel::usize>(keel::clamp(s_state.selectedIdx, 0, 2));
        if (auto el = guikit::Element(ui, {.label = "Active Selection"})) {
            if (guikit::Segmented(ui, idx, optNames).changed) {
                s_state.selectedIdx = keel::as<keel::i32>(idx);
                ui.InvalidateAll();
            }
        }
    } else {
        if (auto el = guikit::Element(ui, {.label = "Selected State"})) {
            if (guikit::Switch(ui, s_state.singleChecked).changed) {
                ui.InvalidateAll();
            }
        }
    }
}

void RenderRadioStage(guikit::Context& ui, DemoContext& ctx) {
    const bool groupDemo = (s_state.modeIdx == 0u);
    guikit::RadioOptions ctrlOpts{};
    ctrlOpts.disabled = ctx.elemDisabled;
    ctrlOpts.size = ctx.controlsSize;
    ctrlOpts.color = ctx.GetControlColor();

    if (groupDemo) {
        if (auto col = guikit::Column(ui, {.gap = 14.0f})) {
            ctx.RunElementStage(ui, "Option A", [&]() {
                guikit::Radio(ui, s_state.selectedIdx, 0, ctrlOpts);
            });
            ctx.RunElementStage(ui, "Option B", [&]() {
                guikit::Radio(ui, s_state.selectedIdx, 1, ctrlOpts);
            });
            ctx.RunElementStage(ui, "Option C", [&]() {
                guikit::Radio(ui, s_state.selectedIdx, 2, ctrlOpts);
            });
        }
    } else {
        ctx.RunElementStage(ui, [&]() {
            guikit::Radio(ui, s_state.singleChecked, ctrlOpts);
        });
    }
}

keel::String GenerateRadioCode(const DemoContext& ctx) {
    const bool groupDemo = (s_state.modeIdx == 0u);
    const keel::StringRef callHead = groupDemo
        ? "Radio(ui, selectedMode, 0"
        : "Radio(ui, radioSelected";
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::String ctrl = keel::format(
        "guikit::{}, {{\n"
        "    .disabled = {},\n"
        "{}"
        "    .color = {}\n"
        "}});",
        callHead,
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatControlColorCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
