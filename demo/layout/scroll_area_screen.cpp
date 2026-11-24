#include "../demo_context.hpp"
#include "guikit/layout/row.hpp"
#include "guikit/layout/scroll_area.hpp"

namespace guikit::demo {

namespace {

struct ScrollAreaDemoState {
    keel::usize orientationIdx = 0; // 0=Vertical, 1=Horizontal, 2=Both
    keel::usize policyIdx = 0;      // 0=Auto, 1=Always, 2=Never
    guikit::UiSize scrollbarSize = guikit::UiSize::Md;
    keel::f32 gap = 10.0f;
    keel::f32 padding = 14.0f;
    bool overflowContent = true;
};

ScrollAreaDemoState s_scroll;

constexpr keel::StaticArray<keel::StringRef, 3> kOrientationNames = {
    "Vertical", "Horizontal", "Both"
};

constexpr keel::StaticArray<keel::StringRef, 3> kPolicyNames = {
    "Auto", "Always", "Never"
};

constexpr keel::StaticArray<guikit::UiVariant, 5> kRowVariants = {
    guikit::UiVariant::Primary,
    guikit::UiVariant::Info,
    guikit::UiVariant::Success,
    guikit::UiVariant::Warning,
    guikit::UiVariant::Secondary
};

} // namespace

void RenderScrollAreaInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {
        .label = "Orientation",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_scroll.orientationIdx, kOrientationNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Scrollbar Visibility",
        .layout = guikit::ElementLayout::Stacked
    })) {
        if (guikit::Segmented(ui, s_scroll.policyIdx, kPolicyNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Scrollbar Size"})) {
        if (guikit::StepSlider(ui, s_scroll.scrollbarSize, {
            .showTicks = true,
            .showLabels = true
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Item Gap"})) {
        if (guikit::Slider(ui, s_scroll.gap, 0.0f, 24.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Padding"})) {
        if (guikit::Slider(ui, s_scroll.padding, 0.0f, 28.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Overflow Content",
        .description = "Toggle large vs compact content to test Auto vs Always scrollbars"
    })) {
        if (guikit::Switch(ui, s_scroll.overflowContent).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderScrollAreaStage(guikit::Context& ui, DemoContext&) {
    const auto& pal = ui.GetThemePalette();
    const guikit::Orientation orient = keel::from_underlying<guikit::Orientation>(s_scroll.orientationIdx);
    const guikit::VisibilityPolicy policy = keel::from_underlying<guikit::VisibilityPolicy>(s_scroll.policyIdx);

    if (auto scroll = guikit::ScrollArea(ui, {
        .orientation = orient,
        .scrollbar = policy,
        .align = (orient == guikit::Orientation::Horizontal) ? guikit::Alignment::Center : guikit::Alignment::Stretch,
        .gap = s_scroll.gap,
        .padding = guikit::Padding::All(s_scroll.padding),
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch,
        .background = {pal.background.WithAlphaF(0.45f)},
        .border = {.width = 1.0f, .radius = 8.0f, .color = {pal.borderSubtle}},
        .size = s_scroll.scrollbarSize
    })) {
        if (orient == guikit::Orientation::Vertical) {
            const keel::u32 count = s_scroll.overflowContent ? 24u : 3u;
            for (keel::u32 i = 0; i < count; ++i) {
                auto scope = ui.IdScope(keel::as<keel::u64>(i));
                const guikit::UiVariant var = kRowVariants[i % kRowVariants.size()];
                if (auto card = guikit::Card(ui, {
                    .width = guikit::SizeMode::Stretch,
                    .padding = guikit::Padding::Symmetric(14.0f, 10.0f)
                })) {
                    guikit::Heading(ui, {
                        .label = ui.Format("Telemetry Channel {:02u}", i + 1u),
                        .description = ui.Format("Sample rate: {} kHz • Buffer nominal", 48u + (i % 4u) * 16u),
                        .badge = {ui.Format("CH-{:02u}", i + 1u), guikit::StateColor::FromVariant(var)},
                        .level = guikit::HeadingLevel::H5
                    });
                }
            }
        } else if (orient == guikit::Orientation::Horizontal) {
            const keel::u32 count = s_scroll.overflowContent ? 16u : 2u;
            for (keel::u32 i = 0; i < count; ++i) {
                auto scope = ui.IdScope(keel::as<keel::u64>(i));
                const guikit::UiVariant var = kRowVariants[i % kRowVariants.size()];
                const guikit::StateColor tint = guikit::StateColor::FromVariant(var).Tinted(0.12f);
                if (auto card = guikit::Card(ui, {
                    .width = 192.0f,
                    .height = 200.0f,
                    .gap = 8.0f,
                    .padding = guikit::Padding::All(14.0f),
                    .background = tint,
                    .border = {.width = 1.0f, .radius = 8.0f, .color = guikit::StateColor::FromVariant(var).WithAlphaF(0.45f)}
                })) {
                    guikit::Heading(ui, {
                        .label = ui.Format("Slot {:02u}", i + 1u),
                        .description = "Horizontal strip card",
                        .badge = {ui.Format("#{:02u}", i + 1u), guikit::StateColor::FromVariant(var)},
                        .level = guikit::HeadingLevel::H5
                    });
                    guikit::Text(ui, "Use horizontal scrollbar or Shift + MouseWheel to pan.", {
                        .role = guikit::TextRole::BodySmall,
                        .variant = guikit::UiVariant::Muted
                    });
                }
            }
        } else {
            // 2D matrix (Orientation::Both)
            const keel::u32 rows = s_scroll.overflowContent ? 12u : 2u;
            const keel::u32 cols = s_scroll.overflowContent ? 8u : 2u;
            for (keel::u32 r = 0; r < rows; ++r) {
                auto rowScope = ui.IdScope(keel::as<keel::u64>(r));
                if (auto row = guikit::Row(ui, {
                    .gap = s_scroll.gap,
                    .width = guikit::SizeMode::Fit
                })) {
                    for (keel::u32 c = 0; c < cols; ++c) {
                        auto colScope = ui.IdScope(keel::as<keel::u64>(c));
                        const guikit::UiVariant var = kRowVariants[(r + c) % kRowVariants.size()];
                        const guikit::StateColor tint = guikit::StateColor::FromVariant(var).Tinted(0.10f);
                        if (auto cell = guikit::Card(ui, {
                            .width = 160.0f,
                            .height = 84.0f,
                            .gap = 4.0f,
                            .padding = guikit::Padding::All(10.0f),
                            .background = tint,
                            .border = {.width = 1.0f, .radius = 6.0f, .color = guikit::StateColor::FromVariant(var).WithAlphaF(0.40f)}
                        })) {
                            guikit::Heading(ui, {
                                .label = ui.Format("R{:02u} · C{:02u}", r + 1u, c + 1u),
                                .description = "2D matrix tile",
                                .level = guikit::HeadingLevel::H6
                            });
                        }
                    }
                }
            }
        }
    }
}

keel::String GenerateScrollAreaCode(const DemoContext&) {
    const keel::StringRef orientName = kOrientationNames[s_scroll.orientationIdx];
    const keel::StringRef policyName = kPolicyNames[s_scroll.policyIdx];

    if (s_scroll.orientationIdx == 2u) {
        return keel::format(
            "if (auto scroll = guikit::ScrollArea(ui, {{\n"
            "    .orientation = guikit::Orientation::Both,\n"
            "    .scrollbar = guikit::VisibilityPolicy::{},\n"
            "    .gap = {:.0f}.0f,\n"
            "    .padding = guikit::Padding::All({:.0f}.0f),\n"
            "    .width = guikit::SizeMode::Stretch,\n"
            "    .height = guikit::SizeMode::Stretch,\n"
            "    .size = guikit::UiSize::{}\n"
            "}})) {{\n"
            "    for (int r = 0; r < 12; ++r) {{\n"
            "        auto rowScope = ui.IdScope(r);\n"
            "        if (auto row = guikit::Row(ui, {{.gap = {:.0f}.0f, .width = guikit::SizeMode::Fit}})) {{\n"
            "            for (int c = 0; c < 8; ++c) {{\n"
            "                auto colScope = ui.IdScope(c);\n"
            "                if (auto cell = guikit::Card(ui, {{.width = 160.0f, .height = 84.0f}})) {{\n"
            "                    guikit::Text(ui, ui.Format(\"R{{}} C{{}}\", r + 1, c + 1));\n"
            "                }}\n"
            "            }}\n"
            "        }}\n"
            "    }}\n"
            "}}",
            policyName,
            s_scroll.gap,
            s_scroll.padding,
            SizeName(s_scroll.scrollbarSize),
            s_scroll.gap
        );
    }

    return keel::format(
        "if (auto scroll = guikit::ScrollArea(ui, {{\n"
        "    .orientation = guikit::Orientation::{},\n"
        "    .scrollbar = guikit::VisibilityPolicy::{},\n"
        "    .gap = {:.0f}.0f,\n"
        "    .padding = guikit::Padding::All({:.0f}.0f),\n"
        "    .width = guikit::SizeMode::Stretch,\n"
        "    .height = guikit::SizeMode::Stretch,\n"
        "    .size = guikit::UiSize::{}\n"
        "}})) {{\n"
        "    for (int i = 0; i < {}; ++i) {{\n"
        "        auto scope = ui.IdScope(i);\n"
        "        if (auto card = guikit::Card(ui, {{{}}})) {{\n"
        "            guikit::Text(ui, ui.Format(\"Item {{}}\", i + 1));\n"
        "        }}\n"
        "    }}\n"
        "}}",
        orientName,
        policyName,
        s_scroll.gap,
        s_scroll.padding,
        SizeName(s_scroll.scrollbarSize),
        s_scroll.orientationIdx == 0u ? (s_scroll.overflowContent ? 24 : 3) : (s_scroll.overflowContent ? 16 : 2),
        s_scroll.orientationIdx == 0u ? ".width = guikit::SizeMode::Stretch" : ".width = 192.0f, .height = 200.0f"
    );
}

} // namespace guikit::demo
