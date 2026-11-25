#include "../demo_context.hpp"
#include "guikit/layout/wrap.hpp"

namespace guikit::demo {

namespace {

struct WrapDemoState {
    keel::f32 gap = 10.0f;
    keel::f32 runGap = 10.0f;
    keel::f32 padding = 16.0f;
    keel::usize alignIdx = 1;    // 0=Start, 1=Center, 2=End, 3=Stretch
    keel::usize justifyIdx = 0;  // 0=Start, 1=Center, 2=End, 3=SpaceBetween
    keel::u32 itemCount = 12;
    bool varyHeights = true;
};

WrapDemoState s_wrap;

constexpr keel::StaticArray<keel::StringRef, 4> kAlignNames = {
    "Start", "Center", "End", "Stretch"
};

constexpr keel::StaticArray<keel::StringRef, 4> kJustifyLabels = {
    "Start", "Center", "End", "Between"
};

constexpr keel::StaticArray<keel::StringRef, 4> kJustifyNames = {
    "Start", "Center", "End", "SpaceBetween"
};

struct WrapSampleItem {
    keel::f32 width;
    keel::f32 height;
};

constexpr keel::StaticArray<WrapSampleItem, 18> kWrapItems = {{
    {156.0f, 52.0f},
    {112.0f, 44.0f},
    {168.0f, 64.0f},
    {128.0f, 48.0f},
    {164.0f, 56.0f},
    {108.0f, 44.0f},
    {176.0f, 60.0f},
    {162.0f, 48.0f},
    {158.0f, 52.0f},
    {164.0f, 44.0f},
    {142.0f, 60.0f},
    {146.0f, 48.0f},
    {154.0f, 56.0f},
    {152.0f, 44.0f},
    {132.0f, 52.0f},
    {144.0f, 48.0f},
    {148.0f, 60.0f},
    {140.0f, 44.0f}
}};

} // namespace

void RenderWrapInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {
        .label = "Horizontal Gap",
        .description = "Spacing between items on the same wrapped line"
    })) {
        if (guikit::Slider(ui, s_wrap.gap, 0.0f, 28.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Run Gap (Vertical)",
        .description = "Spacing between consecutive wrapped rows"
    })) {
        if (guikit::Slider(ui, s_wrap.runGap, 0.0f, 28.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Padding"})) {
        if (guikit::Slider(ui, s_wrap.padding, 0.0f, 32.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Row Align",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_wrap.alignIdx, kAlignNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Line Justify",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_wrap.justifyIdx, kJustifyLabels).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Item Count"})) {
        if (guikit::Slider(ui, s_wrap.itemCount, 4u, 18u, {.valueFormat = "%u items", .step = 1.0}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Vary Item Heights",
        .description = "Demonstrates Row Align within each wrapped line"
    })) {
        if (guikit::Switch(ui, s_wrap.varyHeights).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderWrapStage(guikit::Context& ui, DemoContext&) {
    const auto& pal = ui.GetThemePalette();
    const guikit::Alignment align = keel::from_underlying<guikit::Alignment>(s_wrap.alignIdx);
    const guikit::Justify justify = keel::from_underlying<guikit::Justify>(s_wrap.justifyIdx);

    if (auto frame = guikit::ScrollArea(ui, {
        .orientation = guikit::Orientation::Vertical,
        .scrollbar = guikit::VisibilityPolicy::Auto,
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .background = {pal.background.WithAlphaF(0.45f)},
        .border = {.width = 1.0f, .radius = ui.GetTheme().GetRadius(guikit::UiSize::Md), .color = {pal.borderSubtle}}
    })) {
        if (auto wrap = guikit::Wrap(ui, {
            .gap = s_wrap.gap,
            .runGap = s_wrap.runGap,
            .padding = guikit::Padding::All(s_wrap.padding),
            .align = align,
            .justify = justify,
            .width = guikit::SizeMode::Stretch
        })) {
            const keel::u32 count = keel::min<keel::u32>(s_wrap.itemCount, keel::as<keel::u32>(kWrapItems.size()));
            for (keel::u32 i = 0; i < count; ++i) {
                auto scope = ui.IdScope(keel::as<keel::u64>(i));
                const WrapSampleItem& it = kWrapItems[i];
                const guikit::AxisSize itemH = (align == guikit::Alignment::Stretch)
                    ? guikit::AxisSize(guikit::SizeMode::Stretch)
                    : guikit::AxisSize::Fixed(s_wrap.varyHeights ? it.height : 48.0f);
                if (auto card = guikit::Card(ui, {
                    .width = guikit::AxisSize::Fixed(it.width),
                    .height = itemH,
                    .padding = guikit::Padding::Symmetric(12.0f, 8.0f)
                })) {
                    guikit::Text(ui, ui.Format("Item {:02u}", i + 1u));
                }
            }
        }
    }
}

keel::String GenerateWrapCode(const DemoContext&) {
    return keel::format(
        "if (auto wrap = guikit::Wrap(ui, {{\n"
        "    .gap = {:.0f}.0f,\n"
        "    .runGap = {:.0f}.0f,\n"
        "    .padding = guikit::Padding::All({:.0f}.0f),\n"
        "    .align = guikit::Alignment::{},\n"
        "    .justify = guikit::Justify::{},\n"
        "    .width = guikit::SizeMode::Stretch\n"
        "}})) {{\n"
        "    for (int i = 0; i < {}; ++i) {{\n"
        "        auto scope = ui.IdScope(i);\n"
        "        if (auto card = guikit::Card(ui, {{.width = 148.0f, .height = {}}})) {{\n"
        "            guikit::Text(ui, ui.Format(\"Item {{:02d}}\", i + 1));\n"
        "        }}\n"
        "    }}\n"
        "}}",
        s_wrap.gap,
        s_wrap.runGap,
        s_wrap.padding,
        kAlignNames[s_wrap.alignIdx],
        kJustifyNames[s_wrap.justifyIdx],
        s_wrap.itemCount,
        (s_wrap.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "48.0f"
    );
}

} // namespace guikit::demo
