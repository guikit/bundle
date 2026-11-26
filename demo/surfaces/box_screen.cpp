#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct BoxDemoState {
    guikit::SurfaceRole surface = guikit::SurfaceRole::Flat;
    keel::usize bgIdx = 0;     // 0=Default (Theme role fill)
    keel::usize borderIdx = 0; // 0=Default
    keel::f32 borderWidth = 1.0f;
    keel::f32 radius = -1.0f;  // <0 = Auto
    keel::f32 padding = 16.0f;
    keel::f32 gap = 10.0f;
    bool fullWidth = false;
    keel::f32 minWidth = 300.0f;
    bool clipChildren = true;
};

BoxDemoState s_box;

constexpr keel::StaticArray<keel::StringRef, 4> kBoxSurfaceNames = {
    "None", "Flat", "Raised", "Sunken"
};

guikit::SurfaceRole BoxRoleFromIdx(keel::usize idx) noexcept {
    switch (idx) {
        case 1:  return guikit::SurfaceRole::Flat;
        case 2:  return guikit::SurfaceRole::Raised;
        case 3:  return guikit::SurfaceRole::Sunken;
        default: return guikit::SurfaceRole::None;
    }
}

keel::usize BoxRoleToIdx(guikit::SurfaceRole r) noexcept {
    switch (r) {
        case guikit::SurfaceRole::Flat:   return 1u;
        case guikit::SurfaceRole::Raised: return 2u;
        case guikit::SurfaceRole::Sunken: return 3u;
        default:                          return 0u;
    }
}

constexpr keel::StringRef BoxRoleCodeName(guikit::SurfaceRole r) noexcept {
    switch (r) {
        case guikit::SurfaceRole::Flat:   return "Flat";
        case guikit::SurfaceRole::Raised: return "Raised";
        case guikit::SurfaceRole::Sunken: return "Sunken";
        default:                          return "None";
    }
}

guikit::StateColor ResolveBoxBorderColor() noexcept {
    return BorderPresetToStateColor(s_box.borderIdx);
}

keel::StringRef BoxBorderCodeName() noexcept {
    return BorderPresetToCode(s_box.borderIdx);
}

} // namespace

void RenderBoxInspector(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::SurfaceBox);
    const bool isGallery = (variant == 1u);

    if (!isGallery) {
        keel::usize roleIdx = BoxRoleToIdx(s_box.surface);
        if (auto el = guikit::Element(ui, {.label = "Surface Role", .layout = guikit::ElementLayout::Stacked})) {
            if (guikit::Segmented(ui, roleIdx, kBoxSurfaceNames).changed) {
                s_box.surface = BoxRoleFromIdx(roleIdx);
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Background"})) {
            if (guikit::Select(ui, s_box.bgIdx, kBgPresetNames).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Border Width"})) {
            if (guikit::Slider(ui, s_box.borderWidth, 0.0f, 4.0f, {.valueFormat = "%.1f px", .step = 0.5f}).changed) {
                ui.InvalidateAll();
            }
        }
        const bool hasBorder = (s_box.borderWidth > 0.0f);
        if (auto el = guikit::Element(ui, {.label = "Border Color", .disabled = !hasBorder})) {
            if (guikit::Select(ui, s_box.borderIdx, kUiVariantNames, {.disabled = !hasBorder}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Corner Radius"})) {
            static constexpr keel::StaticArray<keel::StringRef, 2> kRadiusVariants = {{"Auto", "%.0f px"}};
            if (guikit::Slider(ui, s_box.radius, -1.0f, 24.0f, {
                .valueFormat = (s_box.radius < 0.0f)
                    ? guikit::UiText{"Auto", kRadiusVariants}
                    : guikit::UiText{"%.0f px", kRadiusVariants},
                .step = 1.0f
            }).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Stretch Full Width"})) {
            if (guikit::Switch(ui, s_box.fullWidth).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Minimum Width", .disabled = s_box.fullWidth})) {
            if (guikit::Slider(ui, s_box.minWidth, 200.0f, 520.0f, {.valueFormat = "%.0f px", .step = 10.0f, .disabled = s_box.fullWidth}).changed) {
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Padding"})) {
        if (guikit::Slider(ui, s_box.padding, 0.0f, 32.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Gap"})) {
        if (guikit::Slider(ui, s_box.gap, 0.0f, 24.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {
        .label = "Clip Children",
        .description = "Mask child drawing strictly within box bounds"
    })) {
        if (guikit::Switch(ui, s_box.clipChildren).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderBoxStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::SurfaceBox);
    const auto& pal = ui.GetThemePalette();

    if (variant == 0u) {
        if (auto box = guikit::Box(ui, {
            .gap = s_box.gap,
            .width = s_box.fullWidth ? guikit::SizeMode::Stretch : guikit::SizeMode::Fit,
            .minWidth = s_box.fullWidth ? keel::Optional<keel::f32>{keel::kNullOpt} : keel::Optional<keel::f32>{s_box.minWidth},
            .padding = guikit::Padding::All(s_box.padding),
            .background = BgPresetToStateColor(s_box.bgIdx),
            .border = {
                .width = s_box.borderWidth,
                .radius = (s_box.radius >= 0.0f) ? keel::Optional<keel::f32>(s_box.radius) : keel::kNullOpt,
                .color = ResolveBoxBorderColor()
            },
            .surface = s_box.surface,
            .clipChildren = s_box.clipChildren
        })) {
            guikit::Text(ui, "Box Container", {.role = guikit::TextRole::TitleSmall});
            guikit::Text(ui, {
                "Lightweight primitive surface with configurable background, border, surface role, and clipping.",
                guikit::TextWrap::Word()
            }, {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary});

            if (auto inner = guikit::Box(ui, {
                .gap = 6.0f,
                .width = guikit::SizeMode::Stretch,
                .padding = guikit::Padding::All(10.0f),
                .background = {pal.background.WithAlphaF(0.6f)},
                .border = {.width = 1.0f, .color = {pal.borderSubtle}},
                .surface = guikit::SurfaceRole::Sunken,
                .clipChildren = s_box.clipChildren
            })) {
                guikit::Text(ui, "Nested Sunken Box", {.role = guikit::TextRole::LabelMedium});
                guikit::Text(ui, "Ideal for inset panels, code blocks, or custom viewports.", {
                    .role = guikit::TextRole::BodySmall,
                    .variant = guikit::UiVariant::Muted
                });
            }
        }
    } else {
        // SurfaceRole comparison gallery
        static constexpr keel::StaticArray<guikit::SurfaceRole, 4> kRoles = {
            guikit::SurfaceRole::None,
            guikit::SurfaceRole::Flat,
            guikit::SurfaceRole::Raised,
            guikit::SurfaceRole::Sunken
        };
        static constexpr keel::StaticArray<keel::StringRef, 4> kDescs = {
            "Raw container without chrome elevation",
            "Standard flat card/panel surface",
            "Elevated surface (3D bevel or shadow)",
            "Inset well (recessed field/list frame)"
        };

        if (auto grid = guikit::Grid(ui, {.gap = 14.0f, .width = guikit::SizeMode::Stretch})) {
            for (keel::usize i = 0; i < kRoles.size(); ++i) {
                auto scope = ui.IdScope(keel::as<keel::u64>(i));
                if (auto cell = grid.Cell6()) {
                    if (auto box = guikit::Box(ui, {
                        .gap = s_box.gap,
                        .width = guikit::SizeMode::Stretch,
                        .minHeight = 104.0f,
                        .padding = guikit::Padding::All(s_box.padding),
                        .border = {.width = 1.0f},
                        .surface = kRoles[i],
                        .clipChildren = s_box.clipChildren
                    })) {
                        guikit::Tag(ui, kBoxSurfaceNames[i], {
                            .variant = (i == 2u) ? guikit::UiVariant::Primary : ((i == 3u) ? guikit::UiVariant::Info : guikit::UiVariant::Secondary),
                            .style = guikit::TagStyle::Soft,
                            .size = guikit::UiSize::Sm
                        });
                        guikit::Text(ui, {kDescs[i], guikit::TextWrap::Word()}, {
                            .role = guikit::TextRole::BodySmall,
                            .variant = guikit::UiVariant::Secondary
                        });
                    }
                }
            }
        }
    }
}

keel::String GenerateBoxCode(const DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::SurfaceBox);
    if (variant == 1u) {
        return keel::format(
            "// Comparing all 4 SurfaceRole treatments (None, Flat, Raised, Sunken)\n"
            "for (guikit::SurfaceRole role : {{guikit::SurfaceRole::None, guikit::SurfaceRole::Flat,\n"
            "                                 guikit::SurfaceRole::Raised, guikit::SurfaceRole::Sunken}}) {{\n"
            "    if (auto box = guikit::Box(ui, {{\n"
            "        .gap = {:.1f}f,\n"
            "        .width = guikit::SizeMode::Stretch,\n"
            "        .minHeight = 104.0f,\n"
            "        .padding = guikit::Padding::All({:.1f}f),\n"
            "        .border = {{.width = 1.0f}},\n"
            "        .surface = role,\n"
            "        .clipChildren = {}\n"
            "    }})) {{\n"
            "        guikit::Text(ui, \"Surface Role Content\");\n"
            "    }}\n"
            "}}",
            s_box.gap,
            s_box.padding,
            s_box.clipChildren ? "true" : "false"
        );
    }

    const keel::String borderCode = (s_box.radius < 0.0f)
        ? keel::format("{{.width = {:.1f}f, .color = {}}}", s_box.borderWidth, BoxBorderCodeName())
        : keel::format("{{.width = {:.1f}f, .radius = {:.1f}f, .color = {}}}", s_box.borderWidth, s_box.radius, BoxBorderCodeName());
    const keel::String minWLine = s_box.fullWidth
        ? keel::String{}
        : keel::format("    .minWidth = {:.0f}.0f,\n", s_box.minWidth);
    return keel::format(
        "if (auto box = guikit::Box(ui, {{\n"
        "    .gap = {:.1f}f,\n"
        "    .width = guikit::SizeMode::{},\n"
        "{}"
        "    .padding = guikit::Padding::All({:.1f}f),\n"
        "    .background = {},\n"
        "    .border = {},\n"
        "    .surface = guikit::SurfaceRole::{},\n"
        "    .clipChildren = {}\n"
        "}})) {{\n"
        "    guikit::Text(ui, \"Box Container\", {{.role = guikit::TextRole::TitleSmall}});\n"
        "    guikit::Text(ui, \"Lightweight primitive surface container.\");\n"
        "}}",
        s_box.gap,
        s_box.fullWidth ? "Stretch" : "Fit",
        minWLine,
        s_box.padding,
        BgPresetToCode(s_box.bgIdx),
        borderCode,
        BoxRoleCodeName(s_box.surface),
        s_box.clipChildren ? "true" : "false"
    );
}

} // namespace guikit::demo
