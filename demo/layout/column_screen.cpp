#include "../demo_context.hpp"
#include "guikit/layout/column.hpp"
#include "guikit/layout/spacer.hpp"

namespace guikit::demo {

namespace {

struct ColumnDemoState {
    keel::usize alignIdx = 1;    // 0=Start, 1=Center, 2=End, 3=Stretch
    keel::usize justifyIdx = 0;  // 0=Start, 1=Center, 2=End, 3=SpaceBetween
    keel::f32 gap = 12.0f;
    keel::f32 padding = 16.0f;
    keel::u32 itemCount = 4;
    bool varySizes = true;
};

ColumnDemoState s_column;

constexpr keel::StaticArray<keel::StringRef, 4> kColAlignNames = {
    "Start", "Center", "End", "Stretch"
};

constexpr keel::StaticArray<keel::StringRef, 4> kColJustifyLabels = {
    "Start", "Center", "End", "Between"
};

constexpr keel::StaticArray<keel::StringRef, 4> kColJustifyNames = {
    "Start", "Center", "End", "SpaceBetween"
};

void RenderColumnTile(
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

void RenderColumnInspector(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutColumn);
    const bool isSpacerDemo = (variant == 1u);

    if (auto el = guikit::Element(ui, {
        .label = "Align (Horizontal)",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_column.alignIdx, kColAlignNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Justify (Vertical)",
        .layout = guikit::ElementLayout::Stacked,
        .disabled = isSpacerDemo
    })) {
        if (guikit::Segmented(ui, s_column.justifyIdx, kColJustifyLabels, {
            .disabled = isSpacerDemo
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Gap"})) {
        if (guikit::Slider(ui, s_column.gap, 0.0f, 32.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Padding"})) {
        if (guikit::Slider(ui, s_column.padding, 0.0f, 36.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (!isSpacerDemo) {
        if (auto el = guikit::Element(ui, {.label = "Item Count"})) {
            if (guikit::Slider(ui, s_column.itemCount, 2u, 6u, {.valueFormat = "%u items", .step = 1.0}).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {
            .label = "Vary Item Sizes",
            .description = "Use heterogeneous dimensions so Align & Justify are clearly visible"
        })) {
            if (guikit::Switch(ui, s_column.varySizes).changed) {
                ui.InvalidateAll();
            }
        }
    }
}

void RenderColumnStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutColumn);
    const auto& pal = ui.GetThemePalette();
    const guikit::Alignment align = keel::from_underlying<guikit::Alignment>(s_column.alignIdx);
    const guikit::Justify justify = keel::from_underlying<guikit::Justify>(s_column.justifyIdx);

    if (auto frame = guikit::Box(ui, {
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .background = {pal.background.WithAlphaF(0.45f)},
        .border = {.width = 1.0f, .radius = ui.GetTheme().GetRadius(guikit::UiSize::Md), .color = {pal.borderSubtle}},
        .clipChildren = true
    })) {
        if (variant == 0u) {
            static constexpr keel::StaticArray<keel::f32, 6> kColW = {200.0f, 320.0f, 240.0f, 360.0f, 220.0f, 280.0f};
            static constexpr keel::StaticArray<keel::f32, 6> kColH = {52.0f, 76.0f, 60.0f, 84.0f, 56.0f, 68.0f};

            if (auto col = guikit::Column(ui, {
                .gap = s_column.gap,
                .padding = guikit::Padding::All(s_column.padding),
                .align = align,
                .justify = justify,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                for (keel::u32 i = 0; i < s_column.itemCount; ++i) {
                    auto scope = ui.IdScope(keel::as<keel::u64>(i));
                    const guikit::AxisSize itemW = (align == guikit::Alignment::Stretch)
                        ? guikit::AxisSize(guikit::SizeMode::Stretch)
                        : guikit::AxisSize::Fixed(s_column.varySizes ? kColW[i] : 260.0f);
                    const guikit::AxisSize itemH = guikit::AxisSize::Fixed(s_column.varySizes ? kColH[i] : 64.0f);
                    RenderColumnTile(ui, ui.Format("Item {}", i + 1), itemW, itemH);
                }
            }
        } else {
            if (auto col = guikit::Column(ui, {
                .gap = s_column.gap,
                .padding = guikit::Padding::All(s_column.padding),
                .align = align,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                const guikit::AxisSize w = (align == guikit::Alignment::Stretch)
                    ? guikit::AxisSize(guikit::SizeMode::Stretch)
                    : guikit::AxisSize::Fixed(280.0f);

                RenderColumnTile(ui, "Item 1", w, guikit::AxisSize::Fixed(56.0f));
                RenderColumnTile(ui, "Item 2", w, guikit::AxisSize::Fixed(56.0f));

                guikit::Spacer(ui);

                RenderColumnTile(ui, "Item 3", w, guikit::AxisSize::Fixed(56.0f));
                RenderColumnTile(ui, "Item 4", w, guikit::AxisSize::Fixed(56.0f));
            }
        }
    }
}

keel::String GenerateColumnCode(const DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::LayoutColumn);
    const keel::StringRef alignName = kColAlignNames[s_column.alignIdx];
    const keel::StringRef justifyName = kColJustifyNames[s_column.justifyIdx];

    if (variant == 0u) {
        return keel::format(
            "if (auto col = guikit::Column(ui, {{\n"
            "    .gap = {:.0f}.0f,\n"
            "    .padding = guikit::Padding::All({:.0f}.0f),\n"
            "    .align = guikit::Alignment::{},\n"
            "    .justify = guikit::Justify::{},\n"
            "    .width = guikit::SizeMode::Stretch,\n"
            "    .height = guikit::SizeMode::Stretch\n"
            "}})) {{\n"
            "    for (keel::u32 i = 0; i < {}; ++i) {{\n"
            "        auto scope = ui.IdScope(i);\n"
            "        if (auto card = guikit::Card(ui, {{.width = {}, .height = 64.0f}})) {{\n"
            "            guikit::Text(ui, ui.Format(\"Item {{}}\", i + 1));\n"
            "        }}\n"
            "    }}\n"
            "}}",
            s_column.gap,
            s_column.padding,
            alignName,
            justifyName,
            s_column.itemCount,
            (s_column.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "260.0f"
        );
    }

    return keel::format(
        "if (auto col = guikit::Column(ui, {{\n"
        "    .gap = {:.0f}.0f,\n"
        "    .padding = guikit::Padding::All({:.0f}.0f),\n"
        "    .align = guikit::Alignment::{},\n"
        "    .width = guikit::SizeMode::Stretch,\n"
        "    .height = guikit::SizeMode::Stretch\n"
        "}})) {{\n"
        "    if (auto c = guikit::Card(ui, {{.width = {}, .height = 56.0f}})) {{ guikit::Text(ui, \"Item 1\"); }}\n"
        "    if (auto c = guikit::Card(ui, {{.width = {}, .height = 56.0f}})) {{ guikit::Text(ui, \"Item 2\"); }}\n"
        "    guikit::Spacer(ui); // Pushes trailing items to the bottom edge\n"
        "    if (auto c = guikit::Card(ui, {{.width = {}, .height = 56.0f}})) {{ guikit::Text(ui, \"Item 3\"); }}\n"
        "    if (auto c = guikit::Card(ui, {{.width = {}, .height = 56.0f}})) {{ guikit::Text(ui, \"Item 4\"); }}\n"
        "}}",
        s_column.gap,
        s_column.padding,
        alignName,
        (s_column.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "280.0f",
        (s_column.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "280.0f",
        (s_column.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "280.0f",
        (s_column.alignIdx == 3u) ? "guikit::SizeMode::Stretch" : "280.0f"
    );
}

} // namespace guikit::demo
