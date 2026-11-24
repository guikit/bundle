#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct ModalDemoState {
    bool customModalOpen = false;
    bool confirmDeleteModalOpen = false;
    keel::f32 modalWidth = 440.0f;
    keel::f32 backdropOpacity = 0.55f;
    bool closeOnBackdrop = true;
    bool closeOnEscape = true;
    bool showCloseButton = true;
    bool showHeaderDivider = true;
    bool confineToStage = false;
    keel::FixedString<128> profileName = "Station-Alpha";
    bool autoReconnect = true;
    keel::FixedString<128> lastStatus = "Ready";
};

ModalDemoState s_modal;

} // namespace

void RenderModalInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Dialog Width"})) {
        if (guikit::Slider(ui, s_modal.modalWidth, 320.0f, 640.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Backdrop Dimming"})) {
        if (guikit::Slider(ui, s_modal.backdropOpacity, 0.0f, 0.90f, {.valueFormat = "%.2f", .step = 0.05f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Close on Backdrop Click"})) {
        if (guikit::Switch(ui, s_modal.closeOnBackdrop).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Close on Escape Key"})) {
        if (guikit::Switch(ui, s_modal.closeOnEscape).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Header [X] Button"})) {
        if (guikit::Switch(ui, s_modal.showCloseButton).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Header Divider"})) {
        if (guikit::Switch(ui, s_modal.showHeaderDivider).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Confine Modal to Stage"})) {
        if (guikit::Switch(ui, s_modal.confineToStage).changed) ui.InvalidateAll();
    }
}

void RenderModalStage(guikit::Context& ui, DemoContext&) {
    if (auto stageCol = guikit::Column(ui, {
        .gap = 14.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center
    })) {
        if (auto btnRow = guikit::Row(ui, {.gap = 10.0f, .align = guikit::Alignment::Center})) {
            if (guikit::Button(ui, {
                .text = "Open Modal Dialog...",
                .icon = guikit::Icon::Settings(),
                .variant = guikit::UiVariant::Primary
            }).clicked) {
                s_modal.customModalOpen = true;
            }

            if (guikit::Button(ui, {
                .text = "Confirm Delete Modal...",
                .icon = guikit::Icon::Trash(),
                .variant = guikit::UiVariant::Danger
            }).clicked) {
                s_modal.confirmDeleteModalOpen = true;
            }
        }
        guikit::Text(ui, s_modal.lastStatus.as_ref(), {
            .role = guikit::TextRole::BodySmall,
            .variant = guikit::UiVariant::Secondary
        });
    }

    if (auto modal = guikit::Modal(ui, s_modal.customModalOpen, {
        .backdropOpacity = s_modal.backdropOpacity,
        .closeOnBackdrop = s_modal.closeOnBackdrop,
        .closeOnEscape = s_modal.closeOnEscape,
        .confineToParent = s_modal.confineToStage,
        .padding = guikit::Padding::All(16.0f)
    })) {
        if (auto card = guikit::Card(ui, {
            .width = s_modal.modalWidth,
            .gap = 12.0f,
            .padding = guikit::Padding::All(18.0f),
            .surface = guikit::SurfaceRole::Raised
        })) {
            if (auto hdr = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Alignment::Center})) {
                guikit::Heading(ui, {
                    .label = "Configure Station Endpoint",
                    .description = "Edit connection parameters for the active station.",
                    .level = guikit::HeadingLevel::H4,
                    .separator = s_modal.showHeaderDivider ? guikit::HeadingSeparator::Bottom : guikit::HeadingSeparator::None
                });
                if (s_modal.showCloseButton) {
                    if (guikit::Button(ui, {
                        .icon = guikit::Icon::Close(),
                        .square = true,
                        .style = guikit::ButtonStyle::Ghost
                    }).clicked) {
                        s_modal.lastStatus = "Closed via [X]";
                        modal.Close();
                    }
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Profile Name", .layout = guikit::ElementLayout::Top})) {
                guikit::InputField(ui, s_modal.profileName, {.placeholder = "Enter profile name...", .clearable = true});
            }
            if (auto el = guikit::Element(ui, {.label = "Auto-Reconnect on Drop"})) {
                guikit::Switch(ui, s_modal.autoReconnect);
            }
            if (auto footer = guikit::Row(ui, {
                .gap = 10.0f,
                .align = guikit::Alignment::Center,
                .justify = guikit::Justify::End
            })) {
                if (guikit::Button(ui, {.text = "Cancel", .variant = guikit::UiVariant::Secondary}).clicked) {
                    s_modal.lastStatus = "Cancelled Modal";
                    modal.Close();
                }
                if (guikit::Button(ui, {
                    .text = "Save",
                    .icon = guikit::Icon::Check(),
                    .variant = guikit::UiVariant::Primary
                }).clicked) {
                    s_modal.lastStatus = keel::format_fixed<128>("Saved '{}'", s_modal.profileName.as_ref());
                    modal.Close();
                }
            }
        }
    } else if (modal.closedByUser) {
        s_modal.lastStatus = "Dismissed via Backdrop / Esc";
    }

    if (auto modal = guikit::Modal(ui, s_modal.confirmDeleteModalOpen, {
        .backdropOpacity = s_modal.backdropOpacity,
        .closeOnBackdrop = s_modal.closeOnBackdrop,
        .closeOnEscape = s_modal.closeOnEscape,
        .confineToParent = s_modal.confineToStage,
        .padding = guikit::Padding::All(16.0f)
    })) {
        if (auto card = guikit::Card(ui, {
            .width = 380.0f,
            .gap = 12.0f,
            .padding = guikit::Padding::All(18.0f),
            .surface = guikit::SurfaceRole::Raised
        })) {
            guikit::Heading(ui, {
                .label = "Delete Station Node?",
                .description = "This action permanently removes the node.",
                .icon = guikit::Icon::Warning(),
                .level = guikit::HeadingLevel::H4,
                .separator = s_modal.showHeaderDivider ? guikit::HeadingSeparator::Bottom : guikit::HeadingSeparator::None
            });
            if (auto footer = guikit::Row(ui, {
                .gap = 10.0f,
                .align = guikit::Alignment::Center,
                .justify = guikit::Justify::End
            })) {
                if (guikit::Button(ui, {.text = "Cancel", .variant = guikit::UiVariant::Secondary}).clicked) {
                    s_modal.lastStatus = "Cancelled Delete";
                    modal.Close();
                }
                if (guikit::Button(ui, {
                    .text = "Delete",
                    .icon = guikit::Icon::Trash(),
                    .variant = guikit::UiVariant::Danger
                }).clicked) {
                    s_modal.lastStatus = "Confirmed Delete";
                    modal.Close();
                }
            }
        }
    }
}

keel::String GenerateModalCode(const DemoContext&) {
    const keel::String closeBtnSnippet = s_modal.showCloseButton
        ? keel::String::from(
            "            if (guikit::Button(ui, {.icon = guikit::Icon::Close(), .square = true, .style = guikit::ButtonStyle::Ghost}).clicked) {\n"
            "                modal.Close();\n"
            "            }\n"
        )
        : keel::String{};
    return keel::format(
        "if (guikit::Button(ui, {{\n"
        "    .text = \"Open Modal Dialog...\",\n"
        "    .icon = guikit::Icon::Settings(),\n"
        "    .variant = guikit::UiVariant::Primary\n"
        "}}).clicked) {{\n"
        "    isModalOpen = true;\n"
        "}}\n\n"
        "if (auto modal = guikit::Modal(ui, isModalOpen, {{\n"
        "    .backdropOpacity = {:.2f}f,\n"
        "    .closeOnBackdrop = {},\n"
        "    .closeOnEscape = {},\n"
        "    .confineToParent = {},\n"
        "    .padding = guikit::Padding::All(16.0f)\n"
        "}})) {{\n"
        "    if (auto card = guikit::Card(ui, {{\n"
        "        .width = {:.1f}f,\n"
        "        .gap = 12.0f,\n"
        "        .padding = guikit::Padding::All(18.0f),\n"
        "        .surface = guikit::SurfaceRole::Raised\n"
        "    }})) {{\n"
        "        if (auto hdr = guikit::Row(ui, {{.gap = 8.0f, .align = guikit::Alignment::Center}})) {{\n"
        "            guikit::Heading(ui, {{\n"
        "                .label = \"Configure Station Endpoint\",\n"
        "                .description = \"Edit connection parameters for the active station.\",\n"
        "                .level = guikit::HeadingLevel::H4,\n"
        "                .separator = guikit::HeadingSeparator::{}\n"
        "            }});\n"
        "{}"
        "        }}\n"
        "        if (auto el = guikit::Element(ui, {{.label = \"Profile Name\", .layout = guikit::ElementLayout::Top}})) {{\n"
        "            guikit::InputField(ui, profileName, {{.placeholder = \"Enter profile name...\", .clearable = true}});\n"
        "        }}\n"
        "        if (auto el = guikit::Element(ui, {{.label = \"Auto-Reconnect on Drop\"}})) {{\n"
        "            guikit::Switch(ui, autoReconnect);\n"
        "        }}\n"
        "        if (auto footer = guikit::Row(ui, {{.gap = 10.0f, .justify = guikit::Justify::End}})) {{\n"
        "            if (guikit::Button(ui, {{.text = \"Cancel\", .variant = guikit::UiVariant::Secondary}}).clicked) {{\n"
        "                modal.Close();\n"
        "            }}\n"
        "            if (guikit::Button(ui, {{.text = \"Save\", .icon = guikit::Icon::Check(), .variant = guikit::UiVariant::Primary}}).clicked) {{\n"
        "                modal.Close();\n"
        "            }}\n"
        "        }}\n"
        "    }}\n"
        "}}",
        s_modal.backdropOpacity,
        s_modal.closeOnBackdrop ? "true" : "false",
        s_modal.closeOnEscape ? "true" : "false",
        s_modal.confineToStage ? "true" : "false",
        s_modal.modalWidth,
        s_modal.showHeaderDivider ? "Bottom" : "None",
        closeBtnSnippet
    );
}

} // namespace guikit::demo
