#include "../demo_context.hpp"
#include "guikit/layout/grid.hpp"
#include "guikit/layout/scroll_area.hpp"

namespace guikit::demo {

namespace {

struct GridDemoState {
    keel::u32 equalColumns = 3;
    keel::u32 equalItemCount = 6;
    keel::f32 minColWidth = 0.0f;
    keel::f32 gap = 12.0f;
    keel::f32 runGap = 12.0f;
    keel::f32 padding = 16.0f;
    keel::usize alignIdx = 3; // 0=Start, 1=Center, 2=End, 3=Stretch
};

GridDemoState s_grid;

constexpr keel::StaticArray<keel::StringRef, 4> kAlignNames = {
    "Start", "Center", "End", "Stretch"
};

constexpr keel::StaticArray<guikit::UiVariant, 6> kCellVariants = {
    guikit::UiVariant::Primary,
    guikit::UiVariant::Info,
    guikit::UiVariant::Success,
    guikit::UiVariant::Warning,
    guikit::UiVariant::Danger,
    guikit::UiVariant::Secondary
};

void RenderGridCard(
    guikit::Context& ui,
    keel::StringRef title,
    keel::StringRef desc,
    keel::StringRef badge,
    guikit::UiVariant variant,
    keel::f32 minHeight
) {
    const guikit::StateColor tint = guikit::StateColor::FromVariant(variant).Tinted(0.12f);
    if (auto card = guikit::Card(ui, {
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .minHeight = minHeight,
        .gap = 6.0f,
        .padding = guikit::Padding::All(14.0f),
        .background = tint,
        .border = {.width = 1.0f, .radius = 8.0f, .color = guikit::StateColor::FromVariant(variant).WithAlphaF(0.45f)}
    })) {
        guikit::Heading(ui, {
            .label = title,
            .description = desc,
            .badge = {badge, guikit::StateColor::FromVariant(variant)},
            .level = guikit::HeadingLevel::H5
        });
    }
}

} // namespace

void RenderGridInspector(guikit::Context& ui, DemoContext& ctx) {
    const bool isEqualCols = (ctx.GetDemoVariant(NavCategory::LayoutGrid) == 1u);

    if (isEqualCols) {
        if (auto el = guikit::Element(ui, {.label = "Columns"})) {
            if (guikit::Slider(ui, s_grid.equalColumns, 1u, 6u, {.valueFormat = "%u cols", .step = 1.0}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Card Count"})) {
            if (guikit::Slider(ui, s_grid.equalItemCount, 3u, 12u, {.valueFormat = "%u cards", .step = 1.0}).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Min Column Width",
        .description = "Auto-wrap cells when track drops below threshold (0 = fixed tracks)"
    })) {
        if (guikit::Slider(ui, s_grid.minColWidth, 0.0f, 220.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Column Gap"})) {
        if (guikit::Slider(ui, s_grid.gap, 0.0f, 28.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Row Gap"})) {
        if (guikit::Slider(ui, s_grid.runGap, 0.0f, 28.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Padding"})) {
        if (guikit::Slider(ui, s_grid.padding, 0.0f, 32.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Row Align",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_grid.alignIdx, kAlignNames).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderGridStage(guikit::Context& ui, DemoContext& ctx) {
    const bool isEqualCols = (ctx.GetDemoVariant(NavCategory::LayoutGrid) == 1u);
    const auto& pal = ui.GetThemePalette();
    const guikit::Alignment align = keel::from_underlying<guikit::Alignment>(s_grid.alignIdx);

    if (auto frame = guikit::ScrollArea(ui, {
        .orientation = guikit::Orientation::Vertical,
        .scrollbar = guikit::VisibilityPolicy::Auto,
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .background = {pal.background.WithAlphaF(0.45f)},
        .border = {.width = 1.0f, .radius = 8.0f, .color = {pal.borderSubtle}}
    })) {
        if (!isEqualCols) {
            if (auto grid = guikit::Grid(ui, {
                .columns = 12,
                .minColWidth = s_grid.minColWidth,
                .gap = s_grid.gap,
                .runGap = s_grid.runGap,
                .padding = guikit::Padding::All(s_grid.padding),
                .align = align
            })) {
                if (auto c = grid.Cell8()) {
                    RenderGridCard(ui, "Block A", "8 of 12 columns", "Cell8", guikit::UiVariant::Primary, 112.0f);
                }
                if (auto c = grid.Cell4()) {
                    RenderGridCard(ui, "Block B", "4 of 12 columns", "Cell4", guikit::UiVariant::Info, 84.0f);
                }
                if (auto c = grid.Cell6()) {
                    RenderGridCard(ui, "Block C", "6 of 12 columns", "Cell6", guikit::UiVariant::Success, 92.0f);
                }
                if (auto c = grid.Cell6()) {
                    RenderGridCard(ui, "Block D", "6 of 12 columns", "Cell6", guikit::UiVariant::Warning, 72.0f);
                }
                if (auto c = grid.Cell4()) {
                    RenderGridCard(ui, "Block E", "4 of 12 columns", "Cell4", guikit::UiVariant::Secondary, 76.0f);
                }
                if (auto c = grid.Cell4()) {
                    RenderGridCard(ui, "Block F", "4 of 12 columns", "Cell4", guikit::UiVariant::Success, 96.0f);
                }
                if (auto c = grid.Cell4()) {
                    RenderGridCard(ui, "Block G", "4 of 12 columns", "Cell4", guikit::UiVariant::Danger, 76.0f);
                }
            }
        } else {
            if (auto grid = guikit::Grid(ui, {
                .columns = keel::as<keel::i32>(s_grid.equalColumns),
                .minColWidth = s_grid.minColWidth,
                .gap = s_grid.gap,
                .runGap = s_grid.runGap,
                .padding = guikit::Padding::All(s_grid.padding),
                .align = align
            })) {
                for (keel::u32 i = 0; i < s_grid.equalItemCount; ++i) {
                    auto scope = ui.IdScope(keel::as<keel::u64>(i));
                    const guikit::UiVariant var = kCellVariants[i % kCellVariants.size()];
                    const keel::f32 minH = (i % 2u == 0u) ? 88.0f : 68.0f;
                    if (auto c = grid.Cell(1)) {
                        RenderGridCard(
                            ui,
                            ui.Format("Cell {:02u}", i + 1u),
                            "Equal-width track",
                            ui.Format("1/{}", s_grid.equalColumns),
                            var,
                            minH
                        );
                    }
                }
            }
        }
    }
}

keel::String GenerateGridCode(const DemoContext& ctx) {
    const bool isEqualCols = (ctx.GetDemoVariant(NavCategory::LayoutGrid) == 1u);
    const keel::StringRef alignName = kAlignNames[s_grid.alignIdx];

    if (!isEqualCols) {
        return keel::format(
            "if (auto grid = guikit::Grid(ui, {{\n"
            "    .columns = 12,\n"
            "    .minColWidth = {:.0f}.0f,\n"
            "    .gap = {:.0f}.0f,\n"
            "    .runGap = {:.0f}.0f,\n"
            "    .padding = guikit::Padding::All({:.0f}.0f),\n"
            "    .align = guikit::Alignment::{}\n"
            "}})) {{\n"
            "    if (auto c = grid.Cell8()) {{ /* Block A */ }}\n"
            "    if (auto c = grid.Cell4()) {{ /* Block B */ }}\n"
            "    if (auto c = grid.Cell6()) {{ /* Block C */ }}\n"
            "    if (auto c = grid.Cell6()) {{ /* Block D */ }}\n"
            "    if (auto c = grid.Cell4()) {{ /* Block E */ }}\n"
            "    if (auto c = grid.Cell4()) {{ /* Block F */ }}\n"
            "    if (auto c = grid.Cell4()) {{ /* Block G */ }}\n"
            "}}",
            s_grid.minColWidth,
            s_grid.gap,
            s_grid.runGap,
            s_grid.padding,
            alignName
        );
    }

    return keel::format(
        "if (auto grid = guikit::Grid(ui, {{\n"
        "    .columns = {},\n"
        "    .minColWidth = {:.0f}.0f,\n"
        "    .gap = {:.0f}.0f,\n"
        "    .runGap = {:.0f}.0f,\n"
        "    .padding = guikit::Padding::All({:.0f}.0f),\n"
        "    .align = guikit::Alignment::{}\n"
        "}})) {{\n"
        "    for (int i = 0; i < {}; ++i) {{\n"
        "        auto scope = ui.IdScope(i);\n"
        "        if (auto cell = grid.Cell(1)) {{\n"
        "            if (auto card = guikit::Card(ui)) {{\n"
        "                guikit::Text(ui, ui.Format(\"Cell {{:02d}}\", i + 1));\n"
        "            }}\n"
        "        }}\n"
        "    }}\n"
        "}}",
        s_grid.equalColumns,
        s_grid.minColWidth,
        s_grid.gap,
        s_grid.runGap,
        s_grid.padding,
        alignName,
        s_grid.equalItemCount
    );
}

} // namespace guikit::demo
