#include "../demo_context.hpp"
#include "guikit/layout/split_view.hpp"

namespace guikit::demo {

namespace {

struct SplitViewDemoState {
    keel::usize orientationIdx = 0;     // 0 = Horizontal, 1 = Vertical
    keel::f32 primaryRatio = 0.35f;
    keel::f32 secondaryRatio = 0.55f;
    keel::f32 sashSize = 6.0f;
    keel::f32 minRatio = 0.12f;
    keel::f32 maxRatio = 0.88f;
    keel::f32 minPaneSize = 48.0f;
    bool showGrip = true;
    bool collapsible = true;
    bool showOuterBorder = true;
};

SplitViewDemoState s_split;

constexpr keel::StaticArray<keel::StringRef, 2> kOrientations = {
    "Horizontal", "Vertical"
};

void RenderPanePlaceholder(
    guikit::Context& ui,
    keel::StringRef title,
    keel::f32 ratioPct,
    guikit::UiVariant badgeVariant
) {
    if (auto col = guikit::Column(ui, {
        .gap = 6.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center,
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch
    })) {
        guikit::Heading(ui, {
            .label = title,
            .badge = {ui.Format("{:.0f}%", ratioPct), guikit::StateColor::FromVariant(badgeVariant)},
            .align = guikit::Align::Center,
            .width = guikit::SizeMode::Fit
        });
        guikit::Text(ui, "Drag sash to resize • Double-click sash to reset", {
            .role = guikit::TextRole::LabelSmall,
            .variant = guikit::UiVariant::Muted
        });
    }
}

} // namespace

void RenderSplitViewInspector(guikit::Context& ui, DemoContext& ctx) {
    const bool isNested = (ctx.GetDemoVariant(NavCategory::LayoutSplitView) == 0u);

    if (auto el = guikit::Element(ui, {.label = "Orientation", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_split.orientationIdx, kOrientations).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Primary Ratio"})) {
        if (guikit::Slider(ui, s_split.primaryRatio, 0.0f, 1.0f, {.valueFormat = "%.2f", .step = 0.01f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (isNested) {
        if (auto el = guikit::Element(ui, {.label = "Secondary Ratio"})) {
            if (guikit::Slider(ui, s_split.secondaryRatio, 0.0f, 1.0f, {.valueFormat = "%.2f", .step = 0.01f}).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Ratio Bounds"})) {
        if (guikit::RangeSlider(ui, s_split.minRatio, s_split.maxRatio, 0.05f, 0.95f, {
            .valueFormat = "%.2f",
            .step = 0.05f,
            .minSpan = 0.05
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Min Pane Size"})) {
        if (guikit::Slider(ui, s_split.minPaneSize, 24.0f, 140.0f, {.valueFormat = "%.0f dp", .step = 4.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Sash Thickness"})) {
        if (guikit::Slider(ui, s_split.sashSize, 4.0f, 14.0f, {.valueFormat = "%.0f dp", .step = 1.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Sash Grip Pill"})) {
        if (guikit::Switch(ui, s_split.showGrip).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Collapsible Edge Snap"})) {
        if (guikit::Switch(ui, s_split.collapsible).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Outer Frame"})) {
        if (guikit::Switch(ui, s_split.showOuterBorder).changed) {
            ui.InvalidateAll();
        }
    }

    if (guikit::Button(ui, {
        .text = "Reset Ratios",
        .icon = guikit::Icon::Refresh(),
        .width = guikit::SizeMode::Stretch,
        .variant = guikit::UiVariant::Secondary
    }).clicked) {
        s_split.primaryRatio = 0.35f;
        s_split.secondaryRatio = 0.55f;
        ui.InvalidateAll();
    }
    (void)ctx;
}

void RenderSplitViewStage(guikit::Context& ui, DemoContext& ctx) {
    const bool isNested = (ctx.GetDemoVariant(NavCategory::LayoutSplitView) == 0u);
    const auto& pal = ui.GetTheme().palette;
    const guikit::Orientation primaryOrient = (s_split.orientationIdx == 0)
        ? guikit::Orientation::Horizontal
        : guikit::Orientation::Vertical;
    const guikit::Orientation secondaryOrient = (primaryOrient == guikit::Orientation::Horizontal)
        ? guikit::Orientation::Vertical
        : guikit::Orientation::Horizontal;

    const guikit::SplitViewOptions primaryOpts{
        .orientation = primaryOrient,
        .defaultRatio = 0.35f,
        .minRatio = s_split.minRatio,
        .maxRatio = s_split.maxRatio,
        .minFirstSize = s_split.minPaneSize,
        .minSecondSize = s_split.minPaneSize,
        .sashSize = s_split.sashSize,
        .showGrip = s_split.showGrip,
        .collapsible = s_split.collapsible,
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .background = {pal.surface},
        .border = s_split.showOuterBorder
            ? guikit::Border{.width = 1.0f, .radius = 8.0f, .color = {pal.borderSubtle}}
            : guikit::Border::None(),
        .clipChildren = true
    };

    if (auto outerSplit = guikit::SplitView(ui, s_split.primaryRatio, primaryOpts)) {
        if (auto pane1 = outerSplit.First({
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch,
            .padding = guikit::Padding::All(16.0f),
            .background = {pal.surface},
            .clipChildren = true
        })) {
            RenderPanePlaceholder(ui, "Pane 1", outerSplit.Ratio() * 100.0f, guikit::UiVariant::Primary);
        }

        if (auto pane2 = outerSplit.Second({
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch,
            .clipChildren = true
        })) {
            if (isNested) {
                const guikit::SplitViewOptions nestedOpts{
                    .orientation = secondaryOrient,
                    .defaultRatio = 0.55f,
                    .minRatio = s_split.minRatio,
                    .maxRatio = s_split.maxRatio,
                    .minFirstSize = s_split.minPaneSize,
                    .minSecondSize = s_split.minPaneSize,
                    .sashSize = s_split.sashSize,
                    .showGrip = s_split.showGrip,
                    .collapsible = s_split.collapsible,
                    .width = guikit::SizeMode::Stretch,
                    .height = guikit::SizeMode::Stretch,
                    .clipChildren = true
                };

                if (auto innerSplit = guikit::SplitView(ui, s_split.secondaryRatio, nestedOpts)) {
                    if (auto topPane = innerSplit.First({
                        .width = guikit::SizeMode::Stretch,
                        .height = guikit::SizeMode::Stretch,
                        .padding = guikit::Padding::All(16.0f),
                        .background = {pal.background.WithAlphaF(0.55f)},
                        .clipChildren = true
                    })) {
                        RenderPanePlaceholder(ui, "Pane 2A", innerSplit.Ratio() * 100.0f, guikit::UiVariant::Info);
                    }

                    if (auto bottomPane = innerSplit.Second({
                        .width = guikit::SizeMode::Stretch,
                        .height = guikit::SizeMode::Stretch,
                        .padding = guikit::Padding::All(16.0f),
                        .background = {pal.surface},
                        .clipChildren = true
                    })) {
                        RenderPanePlaceholder(ui, "Pane 2B", (1.0f - innerSplit.Ratio()) * 100.0f, guikit::UiVariant::Success);
                    }
                }
            } else {
                if (auto singleRight = guikit::Box(ui, {
                    .width = guikit::SizeMode::Stretch,
                    .height = guikit::SizeMode::Stretch,
                    .padding = guikit::Padding::All(16.0f),
                    .background = {pal.background.WithAlphaF(0.55f)},
                    .clipChildren = true
                })) {
                    RenderPanePlaceholder(ui, "Pane 2", (1.0f - outerSplit.Ratio()) * 100.0f, guikit::UiVariant::Info);
                }
            }
        }
    }
}

keel::String GenerateSplitViewCode(const DemoContext& ctx) {
    const bool isNested = (ctx.GetDemoVariant(NavCategory::LayoutSplitView) == 0u);
    const keel::StringRef orientName = kOrientations[s_split.orientationIdx];
    if (!isNested) {
        return keel::format(
            "float primaryRatio = {:.2f}f;\n\n"
            "if (auto split = guikit::SplitView(ui, primaryRatio, {{\n"
            "    .orientation = guikit::Orientation::{},\n"
            "    .defaultRatio = 0.35f,\n"
            "    .minRatio = {:.2f}f,\n"
            "    .maxRatio = {:.2f}f,\n"
            "    .minFirstSize = {:.0f}f,\n"
            "    .minSecondSize = {:.0f}f,\n"
            "    .sashSize = {:.0f}f,\n"
            "    .showGrip = {},\n"
            "    .collapsible = {}\n"
            "}})) {{\n"
            "    if (auto first = split.First()) {{\n"
            "        // First pane content\n"
            "    }}\n"
            "    if (auto second = split.Second()) {{\n"
            "        // Second pane content\n"
            "    }}\n"
            "}}",
            s_split.primaryRatio,
            orientName,
            s_split.minRatio,
            s_split.maxRatio,
            s_split.minPaneSize,
            s_split.minPaneSize,
            s_split.sashSize,
            s_split.showGrip ? "true" : "false",
            s_split.collapsible ? "true" : "false"
        );
    }

    const keel::StringRef nestedOrientName = (s_split.orientationIdx == 0) ? "Vertical" : "Horizontal";
    return keel::format(
        "float primaryRatio = {:.2f}f;\n"
        "float secondaryRatio = {:.2f}f;\n\n"
        "if (auto split = guikit::SplitView(ui, primaryRatio, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .defaultRatio = 0.35f,\n"
        "    .minRatio = {:.2f}f,\n"
        "    .maxRatio = {:.2f}f,\n"
        "    .minFirstSize = {:.0f}f,\n"
        "    .minSecondSize = {:.0f}f,\n"
        "    .sashSize = {:.0f}f,\n"
        "    .showGrip = {},\n"
        "    .collapsible = {}\n"
        "}})) {{\n"
        "    if (auto first = split.First()) {{ /* Pane 1 */ }}\n"
        "    if (auto second = split.Second()) {{\n"
        "        if (auto inner = guikit::SplitView(ui, secondaryRatio, {{\n"
        "            .orientation = guikit::Orientation::{},\n"
        "            .defaultRatio = 0.55f,\n"
        "            .minRatio = {:.2f}f,\n"
        "            .maxRatio = {:.2f}f,\n"
        "            .minFirstSize = {:.0f}f,\n"
        "            .minSecondSize = {:.0f}f,\n"
        "            .sashSize = {:.0f}f,\n"
        "            .showGrip = {},\n"
        "            .collapsible = {}\n"
        "        }})) {{\n"
        "            if (auto paneA = inner.First())  {{ /* Pane 2A */ }}\n"
        "            if (auto paneB = inner.Second()) {{ /* Pane 2B */ }}\n"
        "        }}\n"
        "    }}\n"
        "}}",
        s_split.primaryRatio,
        s_split.secondaryRatio,
        orientName,
        s_split.minRatio,
        s_split.maxRatio,
        s_split.minPaneSize,
        s_split.minPaneSize,
        s_split.sashSize,
        s_split.showGrip ? "true" : "false",
        s_split.collapsible ? "true" : "false",
        nestedOrientName,
        s_split.minRatio,
        s_split.maxRatio,
        s_split.minPaneSize,
        s_split.minPaneSize,
        s_split.sashSize,
        s_split.showGrip ? "true" : "false",
        s_split.collapsible ? "true" : "false"
    );
}

} // namespace guikit::demo
