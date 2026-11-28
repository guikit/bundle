#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct TooltipDemoState {
    keel::usize contentPresetIdx = 0; // 0 = Short, 1 = Multi-Line, 2 = Rich Text, 3 = Live Coordinates
    keel::FixedString<192> customText = "Save changes (Ctrl+S)";
    bool pinnedPreview = false;
};

TooltipDemoState s_tip;

constexpr keel::StaticArray<keel::StringRef, 4> kContentPresets = {
    "Short", "Multi-Line", "Rich Text", "Live Cursor"
};

guikit::UiText BuildTooltipText(guikit::Context& ui) {
    switch (s_tip.contentPresetIdx) {
        case 1:
            return guikit::UiText{
                "Automatically wraps long explanatory tooltips up to 280 dp and flips below the anchor near the top edge.",
                guikit::TextWrap::Word()
            };
        case 2:
            return guikit::UiText{
                "**Quick Save** — Press `Ctrl+S` to save all *unsaved* changes.",
                guikit::TextWrap::Rich()
            };
        case 3: {
            const guikit::Vec2 pos = ui.GetMousePos();
            return guikit::UiText{
                ui.Format("Cursor: ({:.0f}, {:.0f}) dp", pos.x, pos.y),
                guikit::TextRole::CodeSmall
            };
        }
        default:
            return guikit::UiText{s_tip.customText.as_ref()};
    }
}

} // namespace

void RenderTooltipInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Content Preset"})) {
        if (guikit::Select(ui, s_tip.contentPresetIdx, kContentPresets).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {
        .label = "Custom Text",
        .layout = guikit::ElementLayout::Top,
        .disabled = (s_tip.contentPresetIdx != 0)
    })) {
        if (guikit::InputField(ui, s_tip.customText, {
            .placeholder = "Enter tooltip text...",
            .clearable = true,
            .disabled = (s_tip.contentPresetIdx != 0)
        }).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {
        .label = "Pin Tooltip Visible",
        .description = "Keep tooltip rendered over target without hovering"
    })) {
        if (guikit::Switch(ui, s_tip.pinnedPreview).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderTooltipStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize anchorModeIdx = ctx.GetDemoVariant(NavCategory::OverlayTooltip);
    const guikit::UiText tip = BuildTooltipText(ui);

    if (auto stageCol = guikit::Column(ui, {
        .gap = 16.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center
    })) {
        if (anchorModeIdx == 0u) {
            const auto saveBtn = guikit::Button(ui, {
                .text = "Save Changes",
                .icon = guikit::Icon::Save(),
                .tooltip = tip,
                .variant = guikit::UiVariant::Primary
            });
            if (s_tip.pinnedPreview) {
                ui.SetTooltip(tip, saveBtn.rect);
            }
            guikit::Text(ui, "Hover the button above to trigger its declarative .tooltip", {
                .role = guikit::TextRole::BodySmall,
                .variant = guikit::UiVariant::Secondary
            });
        } else if (anchorModeIdx == 1u) {
            if (auto zone = guikit::Card(ui, {
                .width = guikit::SizeMode::Fit,
                .gap = 4.0f,
                .padding = guikit::Padding::Symmetric(32.0f, 22.0f),
                .surface = guikit::SurfaceRole::Sunken
            })) {
                guikit::Text(ui, "Hover this anchored target box", {.role = guikit::TextRole::LabelMedium});
                const auto resp = zone.GetResponse();
                if (resp.hovered || s_tip.pinnedPreview) {
                    ui.SetTooltip(tip, resp.rect);
                }
            }
            guikit::Text(ui, "Anchored via ui.SetTooltip(text, rect) — centered above, auto-flips near top edge", {
                .role = guikit::TextRole::BodySmall,
                .variant = guikit::UiVariant::Secondary
            });
        } else {
            if (auto zone = guikit::Card(ui, {
                .width = guikit::SizeMode::Fit,
                .gap = 4.0f,
                .padding = guikit::Padding::Symmetric(40.0f, 28.0f),
                .surface = guikit::SurfaceRole::Sunken
            })) {
                guikit::Text(ui, "Move pointer inside this canvas zone", {.role = guikit::TextRole::LabelMedium});
                const auto resp = zone.GetResponse();
                if (resp.hovered) {
                    ui.SetTooltip(tip, {});
                } else if (s_tip.pinnedPreview) {
                    ui.SetTooltip(tip, resp.rect);
                }
            }
            guikit::Text(ui, "Cursor-tracking via ui.SetTooltip(text, {}) with empty anchor rect", {
                .role = guikit::TextRole::BodySmall,
                .variant = guikit::UiVariant::Secondary
            });
        }
    }
}

keel::String GenerateTooltipCode(const DemoContext& ctx) {
    const keel::usize anchorModeIdx = ctx.GetDemoVariant(NavCategory::OverlayTooltip);
    keel::String tipExpr;
    switch (s_tip.contentPresetIdx) {
        case 1:
            tipExpr = keel::String::from(
                "guikit::UiText{\"Automatically wraps long explanatory tooltips up to 280 dp.\", guikit::TextWrap::Word()}"
            );
            break;
        case 2:
            tipExpr = keel::String::from(
                "guikit::UiText{\"**Quick Save** — Press `Ctrl+S` to save all *unsaved* changes.\", guikit::TextWrap::Rich()}"
            );
            break;
        case 3:
            tipExpr = keel::String::from(
                "guikit::UiText{ui.Format(\"Cursor: ({:.0f}, {:.0f}) dp\", pos.x, pos.y), guikit::TextRole::CodeSmall}"
            );
            break;
        default:
            tipExpr = keel::format("\"{}\"", s_tip.customText);
            break;
    }

    if (anchorModeIdx == 0u) {
        return keel::format(
            "guikit::Button(ui, {{\n"
            "    .text = \"Save Changes\",\n"
            "    .icon = guikit::Icon::Save(),\n"
            "    .tooltip = {},\n"
            "    .variant = guikit::UiVariant::Primary\n"
            "}});",
            tipExpr
        );
    }

    if (anchorModeIdx == 1u) {
        return keel::format(
            "if (auto zone = guikit::Card(ui, {{\n"
            "    .width = guikit::SizeMode::Fit,\n"
            "    .padding = guikit::Padding::Symmetric(32.0f, 22.0f),\n"
            "    .surface = guikit::SurfaceRole::Sunken\n"
            "}})) {{\n"
            "    guikit::Text(ui, \"Hover this anchored target box\");\n"
            "    const auto resp = zone.GetResponse();\n"
            "    if (resp.hovered) {{\n"
            "        ui.SetTooltip({}, resp.rect);\n"
            "    }}\n"
            "}}",
            tipExpr
        );
    }

    return keel::format(
        "if (auto zone = guikit::Card(ui, {{\n"
        "    .width = guikit::SizeMode::Fit,\n"
        "    .padding = guikit::Padding::Symmetric(40.0f, 28.0f),\n"
        "    .surface = guikit::SurfaceRole::Sunken\n"
        "}})) {{\n"
        "    guikit::Text(ui, \"Move pointer inside this canvas zone\");\n"
        "    if (zone.GetResponse().hovered) {{\n"
        "        const guikit::Vec2 pos = ui.GetMousePos();\n"
        "        ui.SetTooltip({}, {{}});\n"
        "    }}\n"
        "}}",
        tipExpr
    );
}

} // namespace guikit::demo
