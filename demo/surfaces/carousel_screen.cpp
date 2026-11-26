#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct CarouselDemoState {
    guikit::UiSize size = guikit::UiSize::Md;
    keel::f32 gap = 12.0f;
    keel::f32 padding = 8.0f;
    keel::usize alignIdx = 1; // 0=Start, 1=Center, 2=End, 3=Stretch
    bool scrollWheel = true;
    keel::u32 itemCount = 8;
    keel::usize selectedCard = 0;
    keel::u32 activeChipMask = 0x05u;
};

CarouselDemoState s_carousel;

constexpr keel::StaticArray<keel::StringRef, 4> kCarouselAlignNames = {
    "Start", "Center", "End", "Stretch"
};

} // namespace

void RenderCarouselInspector(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::SurfaceCarousel);

    if (auto el = guikit::Element(ui, {.label = "Arrow & Control Size"})) {
        if (guikit::StepSlider(ui, s_carousel.size, {.showTicks = true, .showLabels = true}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Vertical Align", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_carousel.alignIdx, kCarouselAlignNames).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Item Gap"})) {
        if (guikit::Slider(ui, s_carousel.gap, 2.0f, 24.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Viewport Padding"})) {
        if (guikit::Slider(ui, s_carousel.padding, 0.0f, 20.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (variant == 0u) {
        if (auto el = guikit::Element(ui, {.label = "Card Count"})) {
            if (guikit::Slider(ui, s_carousel.itemCount, 4u, 12u, {.valueFormat = "%u cards", .step = 1.0}).changed) {
                if (s_carousel.selectedCard >= s_carousel.itemCount) {
                    s_carousel.selectedCard = 0;
                }
                ui.InvalidateAll();
            }
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Mouse Wheel Scroll",
        .description = "Scroll horizontally with mouse wheel over the ribbon"
    })) {
        if (guikit::Switch(ui, s_carousel.scrollWheel).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderCarouselStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::SurfaceCarousel);
    const guikit::Alignment align = keel::from_underlying<guikit::Alignment>(s_carousel.alignIdx);
    const auto& pal = ui.GetThemePalette();

    if (auto wrapper = guikit::Column(ui, {
        .gap = 12.0f,
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Fit
    })) {
        if (variant == 0u) {
            static constexpr keel::StaticArray<keel::StringRef, 12> kTitles = {
                "Design Tokens", "Layout Solver", "Typography", "Vector Icons",
                "Focus Ring", "Overlay Portal", "Data Grid", "Tree Explorer",
                "Code Editor", "Split Panes", "Command Bar", "Theme Studio"
            };
            static constexpr keel::StaticArray<keel::StringRef, 12> kTags = {
                "Style", "Layout", "Text", "Graphics",
                "A11y", "Overlay", "Views", "Views",
                "Views", "Layout", "Overlay", "Style"
            };
            static constexpr keel::StaticArray<keel::f32, 12> kCardHeights = {
                122.0f, 138.0f, 118.0f, 132.0f, 124.0f, 140.0f,
                120.0f, 136.0f, 126.0f, 134.0f, 122.0f, 130.0f
            };

            if (auto frame = guikit::Box(ui, {
                .width = guikit::SizeMode::Stretch,
                .padding = guikit::Padding::All(6.0f),
                .background = {pal.background.WithAlphaF(0.45f)},
                .border = {.width = 1.0f, .radius = ui.GetTheme().GetRadius(guikit::UiSize::Md), .color = {pal.borderSubtle}},
                .clipChildren = true
            })) {
                if (auto carousel = guikit::Carousel(ui, {
                    .gap = s_carousel.gap,
                    .padding = guikit::Padding::All(s_carousel.padding),
                    .align = align,
                    .scrollWheel = s_carousel.scrollWheel,
                    .size = s_carousel.size
                })) {
                    for (keel::u32 i = 0; i < s_carousel.itemCount; ++i) {
                        auto scope = ui.IdScope(keel::as<keel::u64>(i));
                        const bool isSelected = (s_carousel.selectedCard == keel::as<keel::usize>(i));
                        const guikit::AxisSize cardH = (align == guikit::Alignment::Stretch)
                            ? guikit::AxisSize(guikit::SizeMode::Stretch)
                            : guikit::AxisSize::Fixed(kCardHeights[i % kCardHeights.size()]);

                        if (auto card = guikit::Card(ui, {
                            .width = 164.0f,
                            .height = cardH,
                            .gap = 8.0f,
                            .padding = guikit::Padding::All(12.0f),
                            .border = {
                                .width = isSelected ? 2.0f : 1.0f,
                                .color = isSelected ? guikit::StateColor::Primary() : guikit::StateColor{}
                            }
                        })) {
                            guikit::Tag(ui, kTags[i % kTags.size()], {
                                .variant = isSelected ? guikit::UiVariant::Primary : guikit::UiVariant::Secondary,
                                .style = guikit::TagStyle::Soft,
                                .size = guikit::UiSize::Xs
                            });
                            guikit::Text(ui, kTitles[i % kTitles.size()], {.role = guikit::TextRole::LabelLarge});
                            guikit::Spacer(ui);
                            if (guikit::Button(ui, {
                                .text = isSelected ? "Selected" : "Select",
                                .size = guikit::UiSize::Xs,
                                .width = guikit::SizeMode::Stretch,
                                .style = isSelected ? guikit::ButtonStyle::Solid : guikit::ButtonStyle::Outline,
                                .variant = isSelected ? guikit::UiVariant::Primary : guikit::UiVariant::Secondary
                            }).clicked) {
                                s_carousel.selectedCard = keel::as<keel::usize>(i);
                            }
                        }
                    }
                }
            }

            guikit::Text(ui, ui.Format(
                "Active card: #{} ({}) — use arrow buttons or mouse wheel to scroll the ribbon.",
                s_carousel.selectedCard + 1u,
                kTitles[s_carousel.selectedCard % kTitles.size()]
            ), {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary});
        } else {
            static constexpr keel::StaticArray<keel::StringRef, 14> kChips = {
                "All Items", "UI Components", "Layout", "Typography", "Classic 3D",
                "Dark Mode", "High Contrast", "WebAssembly", "Zero-STL", "C++20",
                "Data Grid", "Tree View", "Overlays", "Accessibility"
            };

            if (auto frame = guikit::Box(ui, {
                .width = guikit::SizeMode::Stretch,
                .padding = guikit::Padding::Symmetric(6.0f, 4.0f),
                .background = {pal.background.WithAlphaF(0.45f)},
                .border = {.width = 1.0f, .radius = ui.GetTheme().GetRadius(guikit::UiSize::Md), .color = {pal.borderSubtle}},
                .clipChildren = true
            })) {
                if (auto carousel = guikit::Carousel(ui, {
                    .gap = s_carousel.gap,
                    .padding = guikit::Padding::All(s_carousel.padding),
                    .align = align,
                    .scrollWheel = s_carousel.scrollWheel,
                    .size = s_carousel.size
                })) {
                    for (keel::usize i = 0; i < kChips.size(); ++i) {
                        auto scope = ui.IdScope(keel::as<keel::u64>(i));
                        const keel::u32 bit = (1u << i);
                        const bool active = (s_carousel.activeChipMask & bit) != 0u;
                        if (guikit::Button(ui, {
                            .text = kChips[i],
                            .size = guikit::UiSize::Sm,
                            .style = active ? guikit::ButtonStyle::Solid : guikit::ButtonStyle::Outline,
                            .variant = active ? guikit::UiVariant::Primary : guikit::UiVariant::Secondary
                        }).clicked) {
                            s_carousel.activeChipMask ^= bit;
                        }
                    }
                }
            }

            guikit::Text(ui, "Click filter chips to toggle them; overflowing chips scroll smoothly with arrow buttons.", {
                .role = guikit::TextRole::BodySmall,
                .variant = guikit::UiVariant::Secondary
            });
        }
    }
}

keel::String GenerateCarouselCode(const DemoContext& ctx) {
    const keel::usize variant = ctx.GetDemoVariant(NavCategory::SurfaceCarousel);
    if (variant == 1u) {
        return keel::format(
            "if (auto carousel = guikit::Carousel(ui, {{\n"
            "    .gap = {:.1f}f,\n"
            "    .padding = guikit::Padding::All({:.1f}f),\n"
            "    .align = guikit::Alignment::{},\n"
            "    .scrollWheel = {}{}\n"
            "}})) {{\n"
            "    for (keel::usize i = 0; i < chips.size(); ++i) {{\n"
            "        auto scope = ui.IdScope(i);\n"
            "        const bool active = (activeMask & (1u << i)) != 0u;\n"
            "        if (guikit::Button(ui, {{\n"
            "            .text = chips[i],\n"
            "            .size = guikit::UiSize::Sm,\n"
            "            .style = active ? guikit::ButtonStyle::Solid : guikit::ButtonStyle::Outline\n"
            "        }}).clicked) {{\n"
            "            activeMask ^= (1u << i);\n"
            "        }}\n"
            "    }}\n"
            "}}",
            s_carousel.gap,
            s_carousel.padding,
            kCarouselAlignNames[s_carousel.alignIdx],
            s_carousel.scrollWheel ? "true" : "false",
            FormatUiSizePrefix(s_carousel.size)
        );
    }

    return keel::format(
        "if (auto carousel = guikit::Carousel(ui, {{\n"
        "    .gap = {:.1f}f,\n"
        "    .padding = guikit::Padding::All({:.1f}f),\n"
        "    .align = guikit::Alignment::{},\n"
        "    .scrollWheel = {}{}\n"
        "}})) {{\n"
        "    for (keel::u32 i = 0; i < {}; ++i) {{\n"
        "        auto scope = ui.IdScope(i);\n"
        "        if (auto card = guikit::Card(ui, {{.width = 164.0f}})) {{\n"
        "            guikit::Text(ui, ui.Format(\"Card {{}}\", i + 1));\n"
        "        }}\n"
        "    }}\n"
        "}}",
        s_carousel.gap,
        s_carousel.padding,
        kCarouselAlignNames[s_carousel.alignIdx],
        s_carousel.scrollWheel ? "true" : "false",
        FormatUiSizePrefix(s_carousel.size),
        s_carousel.itemCount
    );
}

} // namespace guikit::demo
