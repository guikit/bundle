#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct ToastDemoState {
    keel::usize placementIdx = 0; // BottomRight, TopRight, BottomLeft, TopLeft, TopCenter, BottomCenter
    keel::f32 duration = 4.0f;
    keel::f32 width = 320.0f;
    keel::i32 maxVisible = 5;
    bool pauseOnHover = true;
    bool showProgress = true;
    bool closable = true;
    bool confineToStage = false;
    keel::i32 counter = 0;
};

ToastDemoState s_toast;

constexpr keel::StaticArray<keel::StringRef, 6> kToastPlacementNames = {
    "BottomRight", "TopRight", "BottomLeft", "TopLeft", "TopCenter", "BottomCenter"
};

} // namespace

void RenderToastInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Stack Placement"})) {
        if (guikit::Select(ui, s_toast.placementIdx, kToastPlacementNames).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Auto-Dismiss Timer"})) {
        static constexpr keel::StaticArray<keel::StringRef, 2> kTimerVariants = {{"Sticky", "%.1f s"}};
        if (guikit::Slider(ui, s_toast.duration, 0.0f, 10.0f, {
            .valueFormat = (s_toast.duration <= 0.0f)
                ? guikit::UiText{"Sticky", kTimerVariants}
                : guikit::UiText{"%.1f s", kTimerVariants},
            .step = 0.5f
        }).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Toast Width"})) {
        if (guikit::Slider(ui, s_toast.width, 240.0f, 420.0f, {.valueFormat = "%.0f px", .step = 10.0f}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Max Visible Stack"})) {
        if (guikit::SpinBox(ui, s_toast.maxVisible, {.min = 1.0, .max = 8.0, .step = 1.0}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Pause Timer on Hover"})) {
        if (guikit::Switch(ui, s_toast.pauseOnHover).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Countdown Bar"})) {
        if (guikit::Switch(ui, s_toast.showProgress).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Closable [X] Button"})) {
        if (guikit::Switch(ui, s_toast.closable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Confine Toasts to Stage"})) {
        if (guikit::Switch(ui, s_toast.confineToStage).changed) ui.InvalidateAll();
    }
}

void RenderToastStage(guikit::Context& ui, DemoContext&) {
    if (auto stageCol = guikit::Column(ui, {
        .gap = 14.0f,
        .align = guikit::Alignment::Center,
        .justify = guikit::Justify::Center
    })) {
        if (auto btnWrap = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Alignment::Center})) {
            if (guikit::Button(ui, {
                .text = "Info",
                .icon = guikit::Icon::Info(),
                .variant = guikit::UiVariant::Info
            }).clicked) {
                ++s_toast.counter;
                guikit::ShowToast(ui, {
                    .title = ui.Format("Telemetry Sync #{}", s_toast.counter),
                    .message = "Connected to primary stream at 48.0 kHz.",
                    .variant = guikit::UiVariant::Info,
                    .duration = s_toast.duration,
                    .showProgress = s_toast.showProgress,
                    .closable = s_toast.closable
                });
            }

            if (guikit::Button(ui, {
                .text = "Success",
                .icon = guikit::Icon::Check(),
                .variant = guikit::UiVariant::Success
            }).clicked) {
                ++s_toast.counter;
                guikit::ShowToast(ui, {
                    .title = "Configuration Saved",
                    .message = "Profile committed to flash.",
                    .variant = guikit::UiVariant::Success,
                    .duration = s_toast.duration,
                    .showProgress = s_toast.showProgress,
                    .closable = s_toast.closable
                });
            }

            if (guikit::Button(ui, {
                .text = "Warning",
                .icon = guikit::Icon::Warning(),
                .variant = guikit::UiVariant::Warning
            }).clicked) {
                ++s_toast.counter;
                guikit::ShowToast(ui, {
                    .title = "High Buffer Latency",
                    .message = "Jitter buffer exceeded 85% threshold.",
                    .variant = guikit::UiVariant::Warning,
                    .duration = s_toast.duration,
                    .showProgress = s_toast.showProgress,
                    .closable = s_toast.closable
                });
            }

            if (guikit::Button(ui, {
                .text = "Danger",
                .icon = guikit::Icon::Error(),
                .variant = guikit::UiVariant::Danger
            }).clicked) {
                ++s_toast.counter;
                guikit::ShowToast(ui, {
                    .title = "Carrier Lock Lost",
                    .message = "Uplink timeout; retrying in 2.0s...",
                    .variant = guikit::UiVariant::Danger,
                    .duration = s_toast.duration,
                    .showProgress = s_toast.showProgress,
                    .closable = s_toast.closable
                });
            }

            if (guikit::Button(ui, {
                .text = "Clear",
                .icon = guikit::Icon::Trash(),
                .disabled = (guikit::GetActiveToastCount(ui) == 0),
                .variant = guikit::UiVariant::Secondary
            }).clicked) {
                guikit::ClearToasts(ui);
            }
        }
        guikit::Text(ui, ui.Format("Active Toasts: {}", guikit::GetActiveToastCount(ui)), {
            .role = guikit::TextRole::BodySmall,
            .variant = guikit::UiVariant::Secondary
        });
    }

    guikit::ToastHost(ui, {
        .placement = keel::from_underlying<guikit::ToastPlacement>(s_toast.placementIdx),
        .width = s_toast.width,
        .maxVisible = keel::as<keel::usize>(keel::max(1, s_toast.maxVisible)),
        .pauseOnHover = s_toast.pauseOnHover,
        .confineToParent = s_toast.confineToStage
    });
}

keel::String GenerateToastCode(const DemoContext&) {
    const keel::StringRef placementName = kToastPlacementNames[s_toast.placementIdx];
    return keel::format(
        "if (guikit::Button(ui, {{.text = \"Success\", .icon = guikit::Icon::Check(), .variant = guikit::UiVariant::Success}}).clicked) {{\n"
        "    guikit::ShowToast(ui, {{\n"
        "        .title = \"Configuration Saved\",\n"
        "        .message = \"Profile committed to flash.\",\n"
        "        .variant = guikit::UiVariant::Success,\n"
        "        .duration = {:.1f}f,\n"
        "        .showProgress = {},\n"
        "        .closable = {}\n"
        "    }});\n"
        "}}\n\n"
        "guikit::ToastHost(ui, {{\n"
        "    .placement = guikit::ToastPlacement::{},\n"
        "    .width = {:.1f}f,\n"
        "    .maxVisible = {},\n"
        "    .pauseOnHover = {},\n"
        "    .confineToParent = {}\n"
        "}});",
        s_toast.duration,
        s_toast.showProgress ? "true" : "false",
        s_toast.closable ? "true" : "false",
        placementName,
        s_toast.width,
        s_toast.maxVisible,
        s_toast.pauseOnHover ? "true" : "false",
        s_toast.confineToStage ? "true" : "false"
    );
}

} // namespace guikit::demo
