#include "../demo_context.hpp"
#include "guikit/layout/column.hpp"
#include "guikit/layout/row.hpp"
#include "guikit/layout/spacer.hpp"

namespace guikit::demo {

namespace {

struct FlexDemoState {
    keel::usize alignIdx = 1;    // 0=Start, 1=Center, 2=End, 3=Stretch
    keel::usize justifyIdx = 0;  // 0=Start, 1=Center, 2=End, 3=SpaceBetween
    keel::f32 gap = 12.0f;
    keel::f32 padding = 16.0f;
    keel::u32 itemCount = 4;
    bool equalWidths = false;
    bool varySizes = true;
};

FlexDemoState s_flex;

constexpr keel::StaticArray<keel::StringRef, 4> kAlignNames = {
    "Start", "Center", "End", "Stretch"
};

constexpr keel::StaticArray<keel::StringRef, 4> kJustifyLabels = {
    "Start", "Center", "End", "Between"
};

constexpr keel::StaticArray<keel::StringRef, 4> kJustifyNames = {
    "Start", "Center", "End", "SpaceBetween"
};

void RenderFlexTile(
    guikit::Context& ui,
    keel::StringRef title,
    guikit::AxisSize width,
    guikit::AxisSize height
) {
    if (auto card = guikit::Card(ui, {
        .width = width,
        .height = height,
        .padding = guikit::Padding::All(12.0f)
    })) {
        guikit::Text(ui, title);
    }
}

} // namespace

void RenderFlexInspector(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutFlex);
    const bool isRow = (variant == 0u);
    const bool isSpacerDemo = (variant == 2u);

    if (auto el = guikit::Element(ui, {
        .label = "Align (Cross Axis)",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_flex.alignIdx, kAlignNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Justify (Main Axis)",
        .layout = guikit::ElementLayout::Stacked,
        .disabled = isSpacerDemo || (isRow && s_flex.equalWidths)
    })) {
        if (guikit::Segmented(ui, s_flex.justifyIdx, kJustifyLabels, {
            .disabled = isSpacerDemo || (isRow && s_flex.equalWidths)
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Gap"})) {
        if (guikit::Slider(ui, s_flex.gap, 0.0f, 32.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Padding"})) {
        if (guikit::Slider(ui, s_flex.padding, 0.0f, 36.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (!isSpacerDemo) {
        if (auto el = guikit::Element(ui, {.label = "Item Count"})) {
            if (guikit::Slider(ui, s_flex.itemCount, 2u, 6u, {.valueFormat = "%u items", .step = 1.0}).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (isRow) {
        if (auto el = guikit::Element(ui, {
            .label = "Equal Widths",
            .description = "Distribute equal horizontal space across all Row items"
        })) {
            if (guikit::Switch(ui, s_flex.equalWidths).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (!isSpacerDemo) {
        if (auto el = guikit::Element(ui, {
            .label = "Vary Item Sizes",
            .description = "Use heterogeneous dimensions so Align & Justify are clearly visible"
        })) {
            if (guikit::Switch(ui, s_flex.varySizes).changed) {
                ui.InvalidateAll();
            }
        }
    }
}

void RenderFlexStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutFlex);
    const auto& pal = ui.GetThemePalette();
    const guikit::Alignment align = keel::from_underlying<guikit::Alignment>(s_flex.alignIdx);
    const guikit::Justify justify = keel::from_underlying<guikit::Justify>(s_flex.justifyIdx);

    if (auto frame = guikit::Box(ui, {
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .background = {pal.background.WithAlphaF(0.45f)},
        .border = {.width = 1.0f, .radius = ui.GetTheme().GetRadius(guikit::UiSize::Md), .color = {pal.borderSubtle}},
        .clipChildren = true
    })) {
        if (variant == 0u) {
            // Row
            static constexpr keel::StaticArray<keel::f32, 6> kRowW = {112.0f, 148.0f, 120.0f, 160.0f, 128.0f, 140.0f};
            static constexpr keel::StaticArray<keel::f32, 6> kRowH = {72.0f, 124.0f, 88.0f, 140.0f, 80.0f, 108.0f};

            if (auto row = guikit::Row(ui, {
                .gap = s_flex.gap,
                .padding = guikit::Padding::All(s_flex.padding),
                .align = align,
                .justify = justify,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch,
                .equalWidths = s_flex.equalWidths
            })) {
                for (keel::u32 i = 0; i < s_flex.itemCount; ++i) {
                    auto scope = ui.IdScope(keel::as<keel::u64>(i));
                    const guikit::AxisSize itemW = s_flex.equalWidths
                        ? guikit::AxisSize(guikit::SizeMode::Stretch)
                        : guikit::AxisSize::Fixed(s_flex.varySizes ? kRowW[i] : 128.0f);
                    const guikit::AxisSize itemH = (align == guikit::Alignment::Stretch)
                        ? guikit::AxisSize(guikit::SizeMode::Stretch)
                        : guikit::AxisSize::Fixed(s_flex.varySizes ? kRowH[i] : 96.0f);
                    RenderFlexTile(ui, ui.Format("Item {}", i + 1), itemW, itemH);
                }
            }
        } else if (variant == 1u) {
            // Column
            static constexpr keel::StaticArray<keel::f32, 6> kColW = {200.0f, 320.0f, 240.0f, 360.0f, 220.0f, 280.0f};
            static constexpr keel::StaticArray<keel::f32, 6> kColH = {52.0f, 76.0f, 60.0f, 84.0f, 56.0f, 68.0f};

            if (auto col = guikit::Column(ui, {
                .gap = s_flex.gap,
                .padding = guikit::Padding::All(s_flex.padding),
                .align = align,
                .justify = justify,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                for (keel::u32 i = 0; i < s_flex.itemCount; ++i) {
                    auto scope = ui.IdScope(keel::as<keel::u64>(i));
                    const guikit::AxisSize itemW = (align == guikit::Alignment::Stretch)
                        ? guikit::AxisSize(guikit::SizeMode::Stretch)
                        : guikit::AxisSize::Fixed(s_flex.varySizes ? kColW[i] : 260.0f);
                    const guikit::AxisSize itemH = guikit::AxisSize::Fixed(s_flex.varySizes ? kColH[i] : 64.0f);
                    RenderFlexTile(ui, ui.Format("Item {}", i + 1), itemW, itemH);
                }
            }
        } else {
            // Row + Spacer (elastic split bar)
            if (auto row = guikit::Row(ui, {
                .gap = s_flex.gap,
                .padding = guikit::Padding::All(s_flex.padding),
                .align = align,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                const guikit::AxisSize h = (align == guikit::Alignment::Stretch)
                    ? guikit::AxisSize(guikit::SizeMode::Stretch)
                    : guikit::AxisSize::Fixed(80.0f);

                RenderFlexTile(ui, "Item 1", guikit::AxisSize::Fixed(132.0f), h);
                RenderFlexTile(ui, "Item 2", guikit::AxisSize::Fixed(132.0f), h);

                guikit::Spacer(ui);

                RenderFlexTile(ui, "Item 3", guikit::AxisSize::Fixed(132.0f), h);
                RenderFlexTile(ui, "Item 4", guikit::AxisSize::Fixed(132.0f), h);
            }
        }
    }
}

keel::String GenerateFlexCode(const DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutFlex);
    const keel::StringRef alignName = kAlignNames[s_flex.alignIdx];
    const keel::StringRef justifyName = kJustifyNames[s_flex.justifyIdx];

    if (variant == 0u) {
        return keel::format(
            "if (auto row = guikit::Row(ui, {{\n"
            "    .gap = {:.0f}.0f,\n"
            "    .padding = guikit::Padding::All({:.0f}.0f),\n"
            "    .align = guikit::Alignment::{},\n"
            "    .justify = guikit::Justify::{},\n"
            "    .width = guikit::SizeMode::Stretch,\n"
            "    .height = guikit::SizeMode::Stretch,\n"
            "    .equalWidths = {}\n"
            "}})) {{\n"
            "    for (int i = 0; i < {}; ++i) {{\n"
            "        auto scope = ui.IdScope(i);\n"
            "        if (auto card = guikit::Card(ui, {{.width = {}}})) {{\n"
            "            guikit::Text(ui, ui.Format(\"Item {{}}\", i + 1));\n"
            "        }}\n"
            "    }}\n"
            "}}",
            s_flex.gap,
            s_flex.padding,
            alignName,
            justifyName,
            s_flex.equalWidths ? "true" : "false",
            s_flex.itemCount,
            s_flex.equalWidths ? "guikit::SizeMode::Stretch" : "128.0f"
        );
    }

    if (variant == 1u) {
        return keel::format(
            "if (auto col = guikit::Column(ui, {{\n"
            "    .gap = {:.0f}.0f,\n"
            "    .padding = guikit::Padding::All({:.0f}.0f),\n"
            "    .align = guikit::Alignment::{},\n"
            "    .justify = guikit::Justify::{},\n"
            "    .width = guikit::SizeMode::Stretch,\n"
            "    .height = guikit::SizeMode::Stretch\n"
            "}})) {{\n"
            "    for (int i = 0; i < {}; ++i) {{\n"
            "        auto scope = ui.IdScope(i);\n"
            "        if (auto card = guikit::Card(ui, {{.width = {}, .height = 64.0f}})) {{\n"
            "            guikit::Text(ui, ui.Format(\"Item {{}}\", i + 1));\n"
            "        }}\n"
            "    }}\n"
            "}}",
            s_flex.gap,
            s_flex.padding,
            alignName,
            justifyName,
            s_flex.itemCount,
            (s_flex.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "260.0f"
        );
    }

    const keel::StringRef spacerCardH = (s_flex.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "80.0f";
    return keel::format(
        "if (auto row = guikit::Row(ui, {{\n"
        "    .gap = {:.0f}.0f,\n"
        "    .padding = guikit::Padding::All({:.0f}.0f),\n"
        "    .align = guikit::Alignment::{},\n"
        "    .width = guikit::SizeMode::Stretch,\n"
        "    .height = guikit::SizeMode::Stretch\n"
        "}})) {{\n"
        "    if (auto card = guikit::Card(ui, {{.width = 132.0f, .height = {}}})) {{\n"
        "        guikit::Text(ui, \"Item 1\");\n"
        "    }}\n"
        "    if (auto card = guikit::Card(ui, {{.width = 132.0f, .height = {}}})) {{\n"
        "        guikit::Text(ui, \"Item 2\");\n"
        "    }}\n"
        "    guikit::Spacer(ui);\n"
        "    if (auto card = guikit::Card(ui, {{.width = 132.0f, .height = {}}})) {{\n"
        "        guikit::Text(ui, \"Item 3\");\n"
        "    }}\n"
        "    if (auto card = guikit::Card(ui, {{.width = 132.0f, .height = {}}})) {{\n"
        "        guikit::Text(ui, \"Item 4\");\n"
        "    }}\n"
        "}}",
        s_flex.gap,
        s_flex.padding,
        alignName,
        spacerCardH,
        spacerCardH,
        spacerCardH,
        spacerCardH
    );
}

} // namespace guikit::demo
