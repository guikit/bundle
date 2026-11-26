#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct StatusBarDemoState {
    guikit::StatusBarStyle style = guikit::StatusBarStyle::Surface;
    guikit::UiSize size = guikit::UiSize::Md;
    bool showDots = true;
    bool showBadges = true;
    bool showSeparators = true;
    keel::f32 borderWidth = 1.0f;
    bool disabled = false;

    // Interactive state toggled from the StatusBar itself
    bool connected = true;
    keel::usize branchIdx = 0;
    keel::usize encodingIdx = 0;
    bool notificationsOpen = false;
    keel::FixedString<80> lastEvent{"Click any interactive segment on the StatusBar below"};
};

StatusBarDemoState s_sb;

constexpr keel::StaticArray<keel::StringRef, 3> kBranches = {"main", "feat/table-grid", "release/v2.0"};
constexpr keel::StaticArray<keel::StringRef, 2> kEncodings = {"UTF-8", "UTF-16 LE"};

constexpr keel::StringRef StatusBarStyleCodeName(guikit::StatusBarStyle s) noexcept {
    switch (s) {
        case guikit::StatusBarStyle::Header: return "Header";
        case guikit::StatusBarStyle::Accent: return "Accent";
        default:                             return "Surface";
    }
}

} // namespace

void RenderStatusBarInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    if (auto el = guikit::Element(ui, {.label = "Bar Style", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_sb.style, {"Surface", "Header", "Accent"}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Bar Size"})) {
        if (guikit::StepSlider(ui, s_sb.size, {.showTicks = true, .showLabels = true}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Top Border Width"})) {
        if (guikit::Slider(ui, s_sb.borderWidth, 0.0f, 2.0f, {.valueFormat = "%.1f px", .step = 0.5f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Status LED Dots"})) {
        if (guikit::Switch(ui, s_sb.showDots).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Badges"})) {
        if (guikit::Switch(ui, s_sb.showBadges).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Separators"})) {
        if (guikit::Switch(ui, s_sb.showSeparators).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Disabled"})) {
        if (guikit::Switch(ui, s_sb.disabled).changed) ui.InvalidateAll();
    }
}

void RenderStatusBarStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    const auto& theme = ui.GetTheme();
    const Color okDot = theme.GetVariant(guikit::UiVariant::Success).bg;
    const Color offDot = theme.GetVariant(guikit::UiVariant::Danger).bg;

    if (auto shell = guikit::Column(ui, {
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch
    })) {
        if (auto body = guikit::Column(ui, {
            .gap = 12.0f,
            .padding = guikit::Padding::All(20.0f),
            .align = guikit::Alignment::Start,
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch
        })) {
            guikit::Heading(ui, {
                .label = "Window Status Strip",
                .description = "Compact horizontal strip combining static Items, clickable Actions, LED dots, badges, and spacers.",
                .separator = guikit::HeadingSeparator::Bottom
            });

            if (auto info = guikit::Card(ui, {
                .width = guikit::SizeMode::Stretch,
                .gap = 6.0f,
                .padding = guikit::Padding::All(14.0f)
            })) {
                guikit::Text(ui, "Last StatusBar Interaction", {.role = guikit::TextRole::LabelMedium, .variant = guikit::UiVariant::Secondary});
                guikit::Text(ui, s_sb.lastEvent.as_ref(), {.role = guikit::TextRole::BodyMedium});
            }
        }

        if (auto sb = guikit::StatusBar(ui, {
            .style = s_sb.style,
            .border = {.width = s_sb.borderWidth},
            .size = s_sb.size,
            .disabled = s_sb.disabled
        })) {
            // Interactive connection toggle with status LED dot
            if (sb.Action({
                .text = s_sb.connected ? "Connected" : "Offline",
                .dotColor = s_sb.showDots ? (s_sb.connected ? okDot : offDot) : guikit::Color::Transparent(),
                .tooltip = "Click to toggle connection state",
                .active = !s_sb.connected
            }).clicked) {
                s_sb.connected = !s_sb.connected;
                s_sb.lastEvent = s_sb.connected ? "Connection restored (Online)" : "Switched to Offline mode";
            }

            if (s_sb.showSeparators) sb.Separator();

            // Interactive Git branch switcher
            if (sb.Action({
                .text = kBranches[s_sb.branchIdx],
                .icon = guikit::Icon::Refresh(),
                .tooltip = "Click to cycle active Git branch"
            }).clicked) {
                s_sb.branchIdx = (s_sb.branchIdx + 1u) % kBranches.size();
                s_sb.lastEvent = keel::format_fixed<80>("Switched branch -> {}", kBranches[s_sb.branchIdx]);
            }

            if (s_sb.showSeparators) sb.Separator();

            // Static diagnostics segment with badge
            sb.Item({
                .text = "Diagnostics",
                .icon = guikit::Icon::Check(),
                .badge = s_sb.showBadges ? "0 err" : "",
                .tooltip = "Build & lint diagnostics"
            });

            sb.Spacer();

            // Static cursor position readout
            sb.Item({
                .text = "Ln 142, Col 18",
                .tooltip = "Active editor cursor position"
            });

            if (s_sb.showSeparators) sb.Separator();

            // Interactive encoding toggle
            if (sb.Action({
                .text = kEncodings[s_sb.encodingIdx],
                .tooltip = "Click to switch file encoding"
            }).clicked) {
                s_sb.encodingIdx = (s_sb.encodingIdx + 1u) % kEncodings.size();
                s_sb.lastEvent = keel::format_fixed<80>("File encoding -> {}", kEncodings[s_sb.encodingIdx]);
            }

            if (s_sb.showSeparators) sb.Separator();

            // Interactive notifications action with badge
            if (sb.Action({
                .text = "Alerts",
                .icon = guikit::Icon::Info(),
                .badge = s_sb.showBadges ? "2" : "",
                .tooltip = "Toggle notifications panel",
                .active = s_sb.notificationsOpen
            }).clicked) {
                s_sb.notificationsOpen = !s_sb.notificationsOpen;
                s_sb.lastEvent = s_sb.notificationsOpen ? "Notifications drawer opened" : "Notifications drawer closed";
            }
        }
    }
}

keel::String GenerateStatusBarCode(const DemoContext& /*ctx*/) {
    const keel::String dotArg = s_sb.showDots ? keel::String(", .dotColor = okDot") : keel::String{};
    const keel::String sepLine = s_sb.showSeparators ? keel::String("    sb.Separator();\n") : keel::String{};
    const keel::String diagBadge = s_sb.showBadges ? keel::String(", .badge = \"0 err\"") : keel::String{};
    const keel::String alertBadge = s_sb.showBadges ? keel::String(", .badge = \"2\"") : keel::String{};
    const keel::String disabledLine = s_sb.disabled ? keel::String(",\n    .disabled = true") : keel::String{};

    return keel::format(
        "if (auto sb = guikit::StatusBar(ui, {{\n"
        "    .style = guikit::StatusBarStyle::{},\n"
        "    .border = {{.width = {:.1f}f}}{}{}\n"
        "}})) {{\n"
        "    if (sb.Action({{.text = \"Connected\"{}}}).clicked) {{ /* toggle */ }}\n"
        "{}"
        "    if (sb.Action({{.text = \"main\", .icon = guikit::Icon::Refresh()}}).clicked) {{ /* switch branch */ }}\n"
        "{}"
        "    sb.Item({{.text = \"Diagnostics\", .icon = guikit::Icon::Check(){}}});\n"
        "    sb.Spacer();\n"
        "    sb.Item({{.text = \"Ln 142, Col 18\"}});\n"
        "{}"
        "    sb.Action({{.text = \"UTF-8\"}});\n"
        "{}"
        "    sb.Action({{.text = \"Alerts\", .icon = guikit::Icon::Info(){}}});\n"
        "}}",
        StatusBarStyleCodeName(s_sb.style),
        s_sb.borderWidth,
        FormatUiSizePrefix(s_sb.size),
        disabledLine,
        dotArg,
        sepLine,
        sepLine,
        diagBadge,
        sepLine,
        sepLine,
        alertBadge
    );
}

} // namespace guikit::demo
