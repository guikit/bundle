#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct PopoverDemoState {
    bool open = false;
    keel::usize placementIdx = 0; // Auto, BottomStart, BottomEnd, TopStart, TopEnd
    keel::f32 width = 280.0f;
    keel::f32 offset = 6.0f;
    bool closeOutside = true;
    bool closeEscape = true;
    bool showGrid = true;
    bool autoFit = true;
    keel::f32 opacityPct = 85.0f;
    keel::FixedString<128> lastStatus = "Click \"Display Options\" to open Popover";
};

PopoverDemoState s_pop;

constexpr keel::StaticArray<keel::StringRef, 5> kPopoverPlacementNames = {
    "Auto", "BottomStart", "BottomEnd", "TopStart", "TopEnd"
};

} // namespace

void RenderPopoverInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Placement"})) {
        if (guikit::Select(ui, s_pop.placementIdx, kPopoverPlacementNames).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Card Width"})) {
        if (guikit::Slider(ui, s_pop.width, 220.0f, 360.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Anchor Offset"})) {
        if (guikit::Slider(ui, s_pop.offset, 0.0f, 16.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Close on Click Outside"})) {
        if (guikit::Switch(ui, s_pop.closeOutside).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Close on Escape Key"})) {
        if (guikit::Switch(ui, s_pop.closeEscape).changed) ui.InvalidateAll();
    }
}

void RenderPopoverStage(guikit::Context& ui, DemoContext&) {
    if (auto stageCol = guikit::Column(ui, {
        .gap = 14.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center
    })) {
        const auto popBtn = guikit::Button(ui, {
            .text = "Display Options",
            .icon = guikit::Icon::ChevronDown(),
            .iconPlacement = guikit::Placement::Trailing,
            .active = s_pop.open,
            .variant = guikit::UiVariant::Secondary
        });
        if (popBtn.clicked) {
            s_pop.open = !s_pop.open;
        }

        if (auto pop = guikit::Popover(ui, s_pop.open, {
            .anchorRect = popBtn.rect,
            .placement = keel::from_underlying<guikit::PopoverPlacement>(s_pop.placementIdx),
            .offset = s_pop.offset,
            .closeOnClickOutside = s_pop.closeOutside,
            .closeOnEscape = s_pop.closeEscape
        })) {
            if (auto card = guikit::Card(ui, {
                .width = s_pop.width,
                .gap = 10.0f,
                .padding = guikit::Padding::All(14.0f),
                .surface = guikit::SurfaceRole::Raised
            })) {
                guikit::Heading(ui, {
                    .label = "Display Options",
                    .level = guikit::HeadingLevel::H5,
                    .separator = guikit::HeadingSeparator::Bottom
                });
                if (auto el = guikit::Element(ui, {.label = "Show Grid Lines"})) {
                    guikit::Switch(ui, s_pop.showGrid, {.size = guikit::UiSize::Sm});
                }
                if (auto el = guikit::Element(ui, {.label = "Auto-Fit Content"})) {
                    guikit::Switch(ui, s_pop.autoFit, {.size = guikit::UiSize::Sm});
                }
                if (auto el = guikit::Element(ui, {.label = "Overlay Opacity"})) {
                    guikit::Slider(ui, s_pop.opacityPct, 0.0f, 100.0f, {
                        .valueFormat = "%.0f%%",
                        .step = 5.0f,
                        .size = guikit::UiSize::Sm
                    });
                }
                if (auto footer = guikit::Row(ui, {.gap = 8.0f, .justify = guikit::Justify::End})) {
                    if (guikit::Button(ui, {
                        .text = "Reset",
                        .size = guikit::UiSize::Sm,
                        .style = guikit::ButtonStyle::Ghost
                    }).clicked) {
                        s_pop.showGrid = true;
                        s_pop.autoFit = true;
                        s_pop.opacityPct = 85.0f;
                    }
                    if (guikit::Button(ui, {
                        .text = "Done",
                        .size = guikit::UiSize::Sm,
                        .variant = guikit::UiVariant::Primary
                    }).clicked) {
                        s_pop.lastStatus = keel::format_fixed<128>(
                            "Grid={} | AutoFit={} | Opacity={:.0f}%",
                            s_pop.showGrid ? "On" : "Off",
                            s_pop.autoFit ? "On" : "Off",
                            s_pop.opacityPct
                        );
                        pop.Close();
                    }
                }
            }
        } else if (pop.closedByUser) {
            s_pop.lastStatus = "Dismissed via Outside Click / Esc";
        }

        guikit::Text(ui, s_pop.lastStatus.as_ref(), {
            .role = guikit::TextRole::BodySmall,
            .variant = guikit::UiVariant::Secondary
        });
    }
}

keel::String GeneratePopoverCode(const DemoContext&) {
    const keel::StringRef placeName = kPopoverPlacementNames[s_pop.placementIdx];
    return keel::format(
        "const auto popBtn = guikit::Button(ui, {{\n"
        "    .text = \"Display Options\",\n"
        "    .icon = guikit::Icon::ChevronDown(),\n"
        "    .iconPlacement = guikit::Placement::Trailing,\n"
        "    .active = popoverOpen,\n"
        "    .variant = guikit::UiVariant::Secondary\n"
        "}});\n"
        "if (popBtn.clicked) {{\n"
        "    popoverOpen = !popoverOpen;\n"
        "}}\n\n"
        "if (auto pop = guikit::Popover(ui, popoverOpen, {{\n"
        "    .anchorRect = popBtn.rect,\n"
        "    .placement = guikit::PopoverPlacement::{},\n"
        "    .offset = {:.1f}f,\n"
        "    .closeOnClickOutside = {},\n"
        "    .closeOnEscape = {}\n"
        "}})) {{\n"
        "    if (auto card = guikit::Card(ui, {{\n"
        "        .width = {:.1f}f,\n"
        "        .gap = 10.0f,\n"
        "        .padding = guikit::Padding::All(14.0f),\n"
        "        .surface = guikit::SurfaceRole::Raised\n"
        "    }})) {{\n"
        "        guikit::Heading(ui, {{.label = \"Display Options\", .level = guikit::HeadingLevel::H5, .separator = guikit::HeadingSeparator::Bottom}});\n"
        "        if (auto el = guikit::Element(ui, {{.label = \"Show Grid Lines\"}})) {{ guikit::Switch(ui, showGrid, {{.size = guikit::UiSize::Sm}}); }}\n"
        "        if (auto el = guikit::Element(ui, {{.label = \"Auto-Fit Content\"}})) {{ guikit::Switch(ui, autoFit, {{.size = guikit::UiSize::Sm}}); }}\n"
        "        if (auto el = guikit::Element(ui, {{.label = \"Overlay Opacity\"}})) {{ guikit::Slider(ui, opacityPct, 0.0f, 100.0f, {{.valueFormat = \"%.0f%%\", .size = guikit::UiSize::Sm}}); }}\n"
        "        if (guikit::Button(ui, {{.text = \"Done\", .size = guikit::UiSize::Sm, .variant = guikit::UiVariant::Primary}}).clicked) {{\n"
        "            pop.Close();\n"
        "        }}\n"
        "    }}\n"
        "}}",
        placeName,
        s_pop.offset,
        s_pop.closeOutside ? "true" : "false",
        s_pop.closeEscape ? "true" : "false",
        s_pop.width
    );
}

} // namespace guikit::demo
