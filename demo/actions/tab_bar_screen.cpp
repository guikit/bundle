#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct TabBarState {
    keel::i32 idx = 0;
    guikit::TabBarStyle style = guikit::TabBarStyle::Underline;
    keel::f32 gap = 4.0f;
    bool showSep = true;
    bool fullWidth = true;
    bool scrollable = false;
    bool hasBg = true;
    keel::f32 radius = 8.0f;
    keel::f32 padding = 3.0f;
    keel::f32 borderWidth = 1.0f;
};
TabBarState s_state;
} // namespace

void RenderTabBarInspector(guikit::Context& ui, DemoContext& ctx) {
    if (auto el = guikit::Element(ui, {.label = "TabBar Size"})) {
        if (guikit::StepSlider(ui, ctx.controlsSize, {.showTicks = true, .showLabels = true, .includeNone = true}).changed) {
            ui.InvalidateAll();
        }
    }
    static constexpr keel::StaticArray<keel::StringRef, 4> styleNames = {"Underline", "Pill", "Card", "Folder"};
    if (auto el = guikit::Element(ui, {.label = "TabBar Style"})) {
        if (guikit::Segmented(ui, s_state.style, styleNames).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Container Background Fill"})) {
        if (guikit::Switch(ui, s_state.hasBg).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Container Corner Radius", .disabled = !s_state.hasBg})) {
        if (guikit::Slider(ui, s_state.radius, 0.0f, 24.0f, {.valueFormat = "%.0f px", .disabled = !s_state.hasBg}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Container Internal Padding", .disabled = !s_state.hasBg})) {
        if (guikit::Slider(ui, s_state.padding, 0.0f, 12.0f, {.valueFormat = "%.0f px", .disabled = !s_state.hasBg}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Container Border Width", .disabled = !s_state.hasBg})) {
        if (guikit::Slider(ui, s_state.borderWidth, 0.0f, 4.0f, {.valueFormat = "%.1f px", .disabled = !s_state.hasBg}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Tab Spacing (Gap)"})) {
        if (guikit::Slider(ui, s_state.gap, 0.0f, 24.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Baseline Separator"})) {
        if (guikit::Switch(ui, s_state.showSep).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Full Width Stretch"})) {
        if (guikit::Switch(ui, s_state.fullWidth).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Scrollable Ribbon"})) {
        if (guikit::Switch(ui, s_state.scrollable).changed) ui.InvalidateAll();
    }
}

void RenderTabBarStage(guikit::Context& ui, DemoContext& ctx) {
    static const keel::StaticArray<guikit::TabItem, 3> demoTabs = {{
        {.label = "First", .icon = guikit::Icon::Settings()},
        {.label = "Second", .icon = guikit::Icon::Eye()},
        {.label = "Third", .icon = guikit::Icon::Play()}
    }};
    auto& th = ctx.previewTheme;
    guikit::TabBar(ui, s_state.idx, demoTabs, {
        .style = s_state.style,
        .showSeparator = s_state.showSep,
        .fullWidth = s_state.fullWidth,
        .scrollable = s_state.scrollable,
        .gap = s_state.gap,
        .padding = s_state.hasBg ? guikit::Padding::All(s_state.padding) : guikit::Padding{},
        .size = ctx.controlsSize,
        .background = s_state.hasBg ? guikit::StateColor{th.palette.surfaceElevated} : guikit::StateColor::None(),
        .border = {
            .width = s_state.hasBg ? s_state.borderWidth : 0.0f,
            .radius = s_state.hasBg ? keel::Optional<keel::f32>(s_state.radius) : keel::kNullOpt,
            .color = s_state.hasBg ? guikit::StateColor{th.palette.borderSubtle} : guikit::StateColor{}
        }
    });
}

keel::String GenerateTabBarCode(const DemoContext& ctx) {
    static constexpr keel::StaticArray<keel::StringRef, 4> kStyleCodeNames = {"Underline", "Pill", "Card", "Folder"};
    const keel::usize styleIdx = keel::min<keel::usize>(keel::as<keel::usize>(s_state.style), 3);
    return keel::format(
        "guikit::TabBar(ui, activeTab, tabItems, {{\n"
        "    .style = guikit::TabBarStyle::{},\n"
        "    .showSeparator = {},\n"
        "    .fullWidth = {},\n"
        "    .scrollable = {},\n"
        "    .gap = {:.1f}f,\n"
        "    .padding = guikit::Padding::All({:.1f}f){},\n"
        "    .background = {},\n"
        "    .border = {{.width = {:.1f}f, .radius = {:.1f}f, .color = {}}}\n"
        "}});",
        kStyleCodeNames[styleIdx],
        s_state.showSep ? "true" : "false",
        s_state.fullWidth ? "true" : "false",
        s_state.scrollable ? "true" : "false",
        s_state.gap,
        s_state.hasBg ? s_state.padding : 0.0f,
        FormatUiSizePrefix(ctx.controlsSize),
        s_state.hasBg ? "guikit::StateColor{theme.palette.surfaceElevated}" : "guikit::StateColor::None()",
        s_state.hasBg ? s_state.borderWidth : 0.0f,
        s_state.hasBg ? s_state.radius : 0.0f,
        s_state.hasBg ? "guikit::StateColor{theme.palette.borderSubtle}" : "guikit::StateColor{}"
    );
}

} // namespace guikit::demo
