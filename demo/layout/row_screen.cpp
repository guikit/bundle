#include "../demo_context.hpp"
#include "guikit/layout/row.hpp"
#include "guikit/layout/spacer.hpp"

namespace guikit::demo {

namespace {

struct RowDemoState {
    keel::usize alignIdx = 1;    // 0=Start, 1=Center, 2=End, 3=Stretch
    keel::usize justifyIdx = 0;  // 0=Start, 1=Center, 2=End, 3=SpaceBetween
    keel::f32 gap = 12.0f;
    keel::f32 padding = 16.0f;
    keel::u32 itemCount = 4;
    bool equalWidths = false;
    bool varySizes = true;
};

RowDemoState s_row;

constexpr keel::StaticArray<keel::StringRef, 4> kRowAlignNames = {
    "Start", "Center", "End", "Stretch"
};

constexpr keel::StaticArray<keel::StringRef, 4> kRowJustifyLabels = {
    "Start", "Center", "End", "Between"
};

constexpr keel::StaticArray<keel::StringRef, 4> kRowJustifyNames = {
    "Start", "Center", "End", "SpaceBetween"
};

void RenderRowTile(
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

void RenderRowInspector(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutRow);
    const bool isSpacerDemo = (variant == 1u);

    if (auto el = guikit::Element(ui, {
        .label = "Align (Vertical)",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_row.alignIdx, kRowAlignNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Justify (Horizontal)",
        .layout = guikit::ElementLayout::Stacked,
        .disabled = isSpacerDemo || s_row.equalWidths
    })) {
        if (guikit::Segmented(ui, s_row.justifyIdx, kRowJustifyLabels, {
            .disabled = isSpacerDemo || s_row.equalWidths
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Gap"})) {
        if (guikit::Slider(ui, s_row.gap, 0.0f, 32.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Padding"})) {
        if (guikit::Slider(ui, s_row.padding, 0.0f, 36.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (!isSpacerDemo) {
        if (auto el = guikit::Element(ui, {.label = "Item Count"})) {
            if (guikit::Slider(ui, s_row.itemCount, 2u, 6u, {.valueFormat = "%u items", .step = 1.0}).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {
            .label = "Equal Widths",
            .description = "Distribute equal horizontal space across all Row items"
        })) {
            if (guikit::Switch(ui, s_row.equalWidths).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {
            .label = "Vary Item Sizes",
            .description = "Use heterogeneous dimensions so Align & Justify are clearly visible"
        })) {
            if (guikit::Switch(ui, s_row.varySizes).changed) {
                ui.InvalidateAll();
            }
        }
    }
}

void RenderRowStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutRow);
    const auto& pal = ui.GetThemePalette();
    const guikit::Alignment align = keel::from_underlying<guikit::Alignment>(s_row.alignIdx);
    const guikit::Justify justify = keel::from_underlying<guikit::Justify>(s_row.justifyIdx);

    if (auto frame = guikit::Box(ui, {
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .background = {pal.background.WithAlphaF(0.45f)},
        .border = {.width = 1.0f, .radius = ui.GetTheme().GetRadius(guikit::UiSize::Md), .color = {pal.borderSubtle}},
        .clipChildren = true
    })) {
        if (variant == 0u) {
            static constexpr keel::StaticArray<keel::f32, 6> kRowW = {112.0f, 148.0f, 120.0f, 160.0f, 128.0f, 140.0f};
            static constexpr keel::StaticArray<keel::f32, 6> kRowH = {72.0f, 124.0f, 88.0f, 140.0f, 80.0f, 108.0f};

            if (auto row = guikit::Row(ui, {
                .gap = s_row.gap,
                .padding = guikit::Padding::All(s_row.padding),
                .align = align,
                .justify = justify,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch,
                .equalWidths = s_row.equalWidths
            })) {
                for (keel::u32 i = 0; i < s_row.itemCount; ++i) {
                    auto scope = ui.IdScope(keel::as<keel::u64>(i));
                    const guikit::AxisSize itemW = s_row.equalWidths
                        ? guikit::AxisSize(guikit::SizeMode::Stretch)
                        : guikit::AxisSize::Fixed(s_row.varySizes ? kRowW[i] : 128.0f);
                    const guikit::AxisSize itemH = (align == guikit::Alignment::Stretch)
                        ? guikit::AxisSize(guikit::SizeMode::Stretch)
                        : guikit::AxisSize::Fixed(s_row.varySizes ? kRowH[i] : 96.0f);
                    RenderRowTile(ui, ui.Format("Item {}", i + 1), itemW, itemH);
                }
            }
        } else {
            if (auto row = guikit::Row(ui, {
                .gap = s_row.gap,
                .padding = guikit::Padding::All(s_row.padding),
                .align = align,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                const guikit::AxisSize h = (align == guikit::Alignment::Stretch)
                    ? guikit::AxisSize(guikit::SizeMode::Stretch)
                    : guikit::AxisSize::Fixed(80.0f);

                RenderRowTile(ui, "Item 1", guikit::AxisSize::Fixed(132.0f), h);
                RenderRowTile(ui, "Item 2", guikit::AxisSize::Fixed(132.0f), h);

                guikit::Spacer(ui);

                RenderRowTile(ui, "Item 3", guikit::AxisSize::Fixed(132.0f), h);
                RenderRowTile(ui, "Item 4", guikit::AxisSize::Fixed(132.0f), h);
            }
        }
    }
}

keel::String GenerateRowCode(const DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutRow);
    const keel::StringRef alignName = kRowAlignNames[s_row.alignIdx];
    const keel::StringRef justifyName = kRowJustifyNames[s_row.justifyIdx];

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
            "    for (keel::u32 i = 0; i < {}; ++i) {{\n"
            "        auto scope = ui.IdScope(i);\n"
            "        if (auto card = guikit::Card(ui, {{.width = {}}})) {{\n"
            "            guikit::Text(ui, ui.Format(\"Item {{}}\", i + 1));\n"
            "        }}\n"
            "    }}\n"
            "}}",
            s_row.gap,
            s_row.padding,
            alignName,
            justifyName,
            s_row.equalWidths ? "true" : "false",
            s_row.itemCount,
            s_row.equalWidths ? "guikit::SizeMode::Stretch" : "128.0f"
        );
    }

    return keel::format(
        "if (auto row = guikit::Row(ui, {{\n"
        "    .gap = {:.0f}.0f,\n"
        "    .padding = guikit::Padding::All({:.0f}.0f),\n"
        "    .align = guikit::Alignment::{},\n"
        "    .width = guikit::SizeMode::Stretch,\n"
        "    .height = guikit::SizeMode::Stretch\n"
        "}})) {{\n"
        "    if (auto c = guikit::Card(ui, {{.width = 132.0f}})) {{ guikit::Text(ui, \"Item 1\"); }}\n"
        "    if (auto c = guikit::Card(ui, {{.width = 132.0f}})) {{ guikit::Text(ui, \"Item 2\"); }}\n"
        "    guikit::Spacer(ui); // Pushes trailing items to the right edge\n"
        "    if (auto c = guikit::Card(ui, {{.width = 132.0f}})) {{ guikit::Text(ui, \"Item 3\"); }}\n"
        "    if (auto c = guikit::Card(ui, {{.width = 132.0f}})) {{ guikit::Text(ui, \"Item 4\"); }}\n"
        "}}",
        s_row.gap,
        s_row.padding,
        alignName
    );
}

} // namespace guikit::demo
