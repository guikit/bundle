#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct SegmentedDemoState {
    keel::usize selectedIdx = 1;
    keel::u32 selectedMask = 0b0101u;
    bool multiSelect = false;
    bool showIcons = true;
    bool showText = true;
    guikit::Placement iconPlacement = guikit::Placement::Leading;
    bool equalWidth = true;
    bool showDividers = true;
    bool disableLastSegment = false;
};
SegmentedDemoState s_state;
} // namespace

void RenderSegmentedInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {
        .label = "Multi-Toggle Mask Mode",
        .description = "Toggle multiple segments independently (guikit::SegmentedMask)"
    })) {
        if (guikit::Switch(ui, s_state.multiSelect).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show Segment Icons"})) {
        if (guikit::Switch(ui, s_state.showIcons).changed) {
            if (!s_state.showIcons && !s_state.showText) s_state.showText = true;
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Segment Labels"})) {
        if (guikit::Switch(ui, s_state.showText).changed) {
            if (!s_state.showText && !s_state.showIcons) s_state.showIcons = true;
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 4> placeNames = {"Leading", "Trailing", "Top", "Bottom"};
    keel::usize placeIdx = keel::as<keel::usize>(s_state.iconPlacement);
    if (placeIdx > 3) placeIdx = 0;
    if (auto el = guikit::Element(ui, {
        .label = "Icon Placement",
        .disabled = !(s_state.showIcons && s_state.showText)
    })) {
        if (guikit::Segmented(ui, placeIdx, placeNames, {
            .disabled = !(s_state.showIcons && s_state.showText)
        }).changed) {
            s_state.iconPlacement = keel::from_underlying<guikit::Placement>(placeIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Equal Segment Widths",
        .description = "Uniform segment width vs natural content width"
    })) {
        if (guikit::Switch(ui, s_state.equalWidth).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Show Track Dividers"})) {
        if (guikit::Switch(ui, s_state.showDividers).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "Disable Last Segment"})) {
        if (guikit::Switch(ui, s_state.disableLastSegment).changed) ui.InvalidateAll();
    }
}

void RenderSegmentedStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::StaticArray<guikit::SegmentedItem, 4> items = {{
        {
            .text = s_state.showText ? "Overview" : "",
            .icon = s_state.showIcons ? guikit::Icon::Eye() : guikit::Icon{},
            .iconPlacement = s_state.iconPlacement,
            .tooltip = "Summary overview layout"
        },
        {
            .text = s_state.showText ? "Editor" : "",
            .icon = s_state.showIcons ? guikit::Icon::Edit() : guikit::Icon{},
            .iconPlacement = s_state.iconPlacement,
            .tooltip = "Interactive document editor"
        },
        {
            .text = s_state.showText ? "Preview" : "",
            .icon = s_state.showIcons ? guikit::Icon::Play() : guikit::Icon{},
            .iconPlacement = s_state.iconPlacement,
            .tooltip = "Live rendered preview"
        },
        {
            .text = s_state.showText ? "Archive" : "",
            .icon = s_state.showIcons ? guikit::Icon::Folder() : guikit::Icon{},
            .iconPlacement = s_state.iconPlacement,
            .tooltip = "Archived revisions",
            .disabled = s_state.disableLastSegment
        }
    }};

    guikit::SegmentedOptions segOpts{};
    segOpts.equalWidth = s_state.equalWidth;
    segOpts.showDividers = s_state.showDividers;
    segOpts.disabled = ctx.elemDisabled;
    segOpts.size = ctx.controlsSize;
    segOpts.width = ctx.ResolveCtrlWidth();
    segOpts.height = ctx.ResolveCtrlHeight();
    segOpts.variant = ctx.btnVariant;
    segOpts.background = ctx.GetControlBackground(NavCategory::ActionSegmented);
    segOpts.border = ctx.GetCtrlBorder();

    if (s_state.multiSelect) {
        guikit::SegmentedMask(ui, s_state.selectedMask, items, segOpts);
    } else {
        guikit::Segmented(ui, s_state.selectedIdx, items, segOpts);
    }
}

keel::String GenerateSegmentedCode(const DemoContext& ctx) {
    const keel::StringRef fnName = s_state.multiSelect ? "SegmentedMask" : "Segmented";
    const keel::StringRef varName = s_state.multiSelect ? "activeMask" : "selectedMode";
    const keel::FixedString<64> variantCode = keel::format_fixed<64>("guikit::UiVariant::{}", VariantName(ctx.btnVariant));
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    return keel::format(
        "guikit::{}(ui, {}, {{\n"
        "    {{.text = \"Overview\", .icon = guikit::Icon::Eye()}},\n"
        "    {{.text = \"Editor\", .icon = guikit::Icon::Edit()}},\n"
        "    {{.text = \"Preview\", .icon = guikit::Icon::Play()}},\n"
        "    {{.text = \"Archive\", .icon = guikit::Icon::Folder(), .disabled = {}}}\n"
        "}}, {{\n"
        "    .equalWidth = {},\n"
        "    .showDividers = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .variant = {},\n"
        "    .background = {},\n"
        "    .border = {}\n"
        "}});",
        fnName,
        varName,
        s_state.disableLastSegment ? "true" : "false",
        s_state.equalWidth ? "true" : "false",
        s_state.showDividers ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        ctx.FormatCtrlHeightCode(),
        variantCode,
        ctx.FormatControlBackgroundCode(NavCategory::ActionSegmented),
        ctx.FormatCtrlBorderCode()
    );
}

} // namespace guikit::demo
