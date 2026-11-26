#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct AccordionDemoState {
    bool openSections[3] = {true, false, false};
    bool exclusive = false;
    guikit::Placement chevron = guikit::Placement::Leading;
    guikit::UiSize size = guikit::UiSize::Md;
    bool showIcons = true;
    bool showDescriptions = true;
    bool showBadges = true;
    keel::f32 borderWidth = 1.0f;
    keel::usize borderIdx = 0; // 0=Default
    keel::f32 radius = -1.0f;  // <0 = Auto
    bool disableThird = false;

    // Sample interactive controls inside expanded accordion bodies
    bool darkScrollbars = true;
    bool compactTables = false;
    keel::f32 uiOpacity = 100.0f;
    bool emailAlerts = true;
    bool desktopPush = false;
    keel::usize digestFreq = 1;
    bool requireMfa = true;
};

AccordionDemoState s_acc;

constexpr keel::StaticArray<keel::StringRef, 3> kDigestOptions = {
    "Real-time", "Hourly Summary", "Daily Digest"
};

} // namespace

void RenderAccordionInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    if (auto el = guikit::Element(ui, {.label = "Header Size"})) {
        if (guikit::StepSlider(ui, s_acc.size, {.showTicks = true, .showLabels = true}).changed) {
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 2> kChevronNames = {"Leading", "Trailing"};
    keel::usize chevIdx = (s_acc.chevron == guikit::Placement::Trailing) ? 1u : 0u;
    if (auto el = guikit::Element(ui, {.label = "Chevron Placement"})) {
        if (guikit::Segmented(ui, chevIdx, kChevronNames).changed) {
            s_acc.chevron = (chevIdx == 1u) ? guikit::Placement::Trailing : guikit::Placement::Leading;
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Single-Expand (Exclusive)",
        .description = "Opening one section collapses the others"
    })) {
        if (guikit::Switch(ui, s_acc.exclusive).changed) {
            if (s_acc.exclusive) {
                bool foundFirst = false;
                for (keel::usize i = 0; i < 3; ++i) {
                    if (s_acc.openSections[i] && !foundFirst) {
                        foundFirst = true;
                    } else {
                        s_acc.openSections[i] = false;
                    }
                }
            }
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Show Icons"})) {
        if (guikit::Switch(ui, s_acc.showIcons).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Descriptions"})) {
        if (guikit::Switch(ui, s_acc.showDescriptions).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Badges"})) {
        if (guikit::Switch(ui, s_acc.showBadges).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Border Width"})) {
        if (guikit::Slider(ui, s_acc.borderWidth, 0.0f, 4.0f, {.valueFormat = "%.1f px", .step = 0.5f}).changed) {
            ui.InvalidateAll();
        }
    }
    const bool hasBorder = (s_acc.borderWidth > 0.0f);
    if (auto el = guikit::Element(ui, {.label = "Border Color", .disabled = !hasBorder})) {
        if (guikit::Select(ui, s_acc.borderIdx, kUiVariantNames, {.disabled = !hasBorder}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Corner Radius"})) {
        static constexpr keel::StaticArray<keel::StringRef, 2> kRadiusVariants = {{"Auto", "%.0f px"}};
        if (guikit::Slider(ui, s_acc.radius, -1.0f, 24.0f, {
            .valueFormat = (s_acc.radius < 0.0f)
                ? guikit::UiText{"Auto", kRadiusVariants}
                : guikit::UiText{"%.0f px", kRadiusVariants},
            .step = 1.0f
        }).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Disable Third Section"})) {
        if (guikit::Switch(ui, s_acc.disableThird).changed) ui.InvalidateAll();
    }

    if (auto row = guikit::Row(ui, {.gap = 8.0f})) {
        if (guikit::Button(ui, {
            .text = "Expand All",
            .disabled = s_acc.exclusive,
            .size = guikit::UiSize::Sm,
            .width = guikit::SizeMode::Stretch,
            .variant = guikit::UiVariant::Secondary
        }).clicked) {
            s_acc.openSections[0] = true;
            s_acc.openSections[1] = true;
            if (!s_acc.disableThird) s_acc.openSections[2] = true;
            ui.InvalidateAll();
        }
        if (guikit::Button(ui, {
            .text = "Collapse All",
            .size = guikit::UiSize::Sm,
            .width = guikit::SizeMode::Stretch,
            .variant = guikit::UiVariant::Secondary
        }).clicked) {
            s_acc.openSections[0] = false;
            s_acc.openSections[1] = false;
            s_acc.openSections[2] = false;
            ui.InvalidateAll();
        }
    }
}

void RenderAccordionStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    auto handleExclusive = [](keel::usize openedIdx) {
        if (!s_acc.exclusive || !s_acc.openSections[openedIdx]) return;
        for (keel::usize i = 0; i < 3; ++i) {
            if (i != openedIdx) s_acc.openSections[i] = false;
        }
    };

    const guikit::Border hdrBorder{
        .width = s_acc.borderWidth,
        .radius = (s_acc.radius >= 0.0f) ? keel::Optional<keel::f32>(s_acc.radius) : keel::kNullOpt,
        .color = BorderPresetToStateColor(s_acc.borderIdx)
    };

    if (auto col = guikit::Column(ui, {
        .gap = 8.0f,
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Fit,
        .maxWidth = 520.0f
    })) {
        // Section 1: Appearance
        const bool wasOpen0 = s_acc.openSections[0];
        if (auto acc = guikit::Accordion(ui, s_acc.openSections[0], {
            .label = "Appearance & Layout",
            .description = s_acc.showDescriptions ? "Visual density, scrollbars, and surface opacity" : "",
            .icon = s_acc.showIcons ? guikit::Icon::Eye() : guikit::Icon{},
            .badge = s_acc.showBadges ? guikit::UiText{"3 options", guikit::StateColor::Primary()} : guikit::UiText{},
            .chevronPlacement = s_acc.chevron,
            .gap = 8.0f,
            .padding = guikit::Padding::Symmetric(12.0f, 10.0f),
            .border = hdrBorder,
            .size = s_acc.size
        })) {
            if (auto el = guikit::Element(ui, {.label = "Compact Table Rows"})) {
                guikit::Switch(ui, s_acc.compactTables);
            }
            if (auto el = guikit::Element(ui, {.label = "Themed Scrollbars"})) {
                guikit::Switch(ui, s_acc.darkScrollbars);
            }
            if (auto el = guikit::Element(ui, {.label = "Panel Opacity"})) {
                guikit::Slider(ui, s_acc.uiOpacity, 60.0f, 100.0f, {.valueFormat = "%.0f%%", .step = 5.0f, .size = guikit::UiSize::Sm});
            }
        }
        if (!wasOpen0 && s_acc.openSections[0]) handleExclusive(0);

        // Section 2: Notifications
        const bool wasOpen1 = s_acc.openSections[1];
        if (auto acc = guikit::Accordion(ui, s_acc.openSections[1], {
            .label = "Notifications & Digests",
            .description = s_acc.showDescriptions ? "Alert delivery channels and summary cadence" : "",
            .icon = s_acc.showIcons ? guikit::Icon::Info() : guikit::Icon{},
            .badge = s_acc.showBadges ? guikit::UiText{"Active", guikit::StateColor::Success()} : guikit::UiText{},
            .chevronPlacement = s_acc.chevron,
            .gap = 8.0f,
            .padding = guikit::Padding::Symmetric(12.0f, 10.0f),
            .border = hdrBorder,
            .size = s_acc.size
        })) {
            if (auto el = guikit::Element(ui, {.label = "Email Alerts"})) {
                guikit::Switch(ui, s_acc.emailAlerts);
            }
            if (auto el = guikit::Element(ui, {.label = "Desktop Push"})) {
                guikit::Switch(ui, s_acc.desktopPush);
            }
            if (auto el = guikit::Element(ui, {.label = "Digest Cadence"})) {
                guikit::Select(ui, s_acc.digestFreq, kDigestOptions, {.size = guikit::UiSize::Sm});
            }
        }
        if (!wasOpen1 && s_acc.openSections[1]) handleExclusive(1);

        // Section 3: Security
        const bool wasOpen2 = s_acc.openSections[2];
        if (auto acc = guikit::Accordion(ui, s_acc.openSections[2], {
            .label = "Privacy & Security",
            .description = s_acc.showDescriptions ? "Session verification and hardware keys" : "",
            .icon = s_acc.showIcons ? guikit::Icon::Settings() : guikit::Icon{},
            .badge = s_acc.showBadges ? guikit::UiText{s_acc.disableThird ? "Locked" : "Protected", guikit::StateColor::Warning()} : guikit::UiText{},
            .chevronPlacement = s_acc.chevron,
            .gap = 8.0f,
            .padding = guikit::Padding::Symmetric(12.0f, 10.0f),
            .border = hdrBorder,
            .size = s_acc.size,
            .disabled = s_acc.disableThird
        })) {
            if (auto el = guikit::Element(ui, {
                .label = "Require Multi-Factor Auth",
                .description = "Prompt for security key on new sign-ins"
            })) {
                guikit::Switch(ui, s_acc.requireMfa);
            }
        }
        if (!wasOpen2 && s_acc.openSections[2]) handleExclusive(2);
    }
}

keel::String GenerateAccordionCode(const DemoContext& /*ctx*/) {
    keel::String optFields;
    if (s_acc.showDescriptions) {
        optFields.append("    .description = \"Visual density, scrollbars, and surface opacity\",\n");
    }
    if (s_acc.showIcons) {
        optFields.append("    .icon = guikit::Icon::Eye(),\n");
    }
    if (s_acc.showBadges) {
        optFields.append("    .badge = {\"3 options\", guikit::StateColor::Primary()},\n");
    }

    const keel::StringRef chevStr = (s_acc.chevron == guikit::Placement::Trailing) ? "Trailing" : "Leading";
    const keel::String borderCode = (s_acc.radius < 0.0f)
        ? keel::format("{{.width = {:.1f}f, .color = {}}}", s_acc.borderWidth, BorderPresetToCode(s_acc.borderIdx))
        : keel::format("{{.width = {:.1f}f, .radius = {:.1f}f, .color = {}}}", s_acc.borderWidth, s_acc.radius, BorderPresetToCode(s_acc.borderIdx));
    const auto szSuffix = FormatUiSizePrefix(s_acc.size);

    if (s_acc.exclusive) {
        return keel::format(
            "// Exclusive (single-expand) accordion group\n"
            "keel::i32 activeSection = 0;\n"
            "bool isOpen = (activeSection == 0);\n"
            "if (auto acc = guikit::Accordion(ui, isOpen, {{\n"
            "    .label = \"Appearance & Layout\",\n"
            "{}"
            "    .chevronPlacement = guikit::Placement::{},\n"
            "    .gap = 8.0f,\n"
            "    .padding = guikit::Padding::Symmetric(12.0f, 10.0f),\n"
            "    .border = {}{}\n"
            "}})) {{\n"
            "    if (auto el = guikit::Element(ui, {{.label = \"Compact Table Rows\"}})) {{\n"
            "        guikit::Switch(ui, compactTables);\n"
            "    }}\n"
            "}}\n"
            "if (isOpen) {{ activeSection = 0; }}\n"
            "else if (activeSection == 0) {{ activeSection = -1; }}",
            optFields,
            chevStr,
            borderCode,
            szSuffix
        );
    }

    return keel::format(
        "if (auto acc = guikit::Accordion(ui, sectionOpen, {{\n"
        "    .label = \"Appearance & Layout\",\n"
        "{}"
        "    .chevronPlacement = guikit::Placement::{},\n"
        "    .gap = 8.0f,\n"
        "    .padding = guikit::Padding::Symmetric(12.0f, 10.0f),\n"
        "    .border = {}{}{}\n"
        "}})) {{\n"
        "    // Skipped completely when collapsed (O(0) body work)\n"
        "    if (auto el = guikit::Element(ui, {{.label = \"Compact Table Rows\"}})) {{\n"
        "        guikit::Switch(ui, compactTables);\n"
        "    }}\n"
        "}}",
        optFields,
        chevStr,
        borderCode,
        szSuffix,
        s_acc.disableThird ? ",\n    .disabled = true" : ""
    );
}

} // namespace guikit::demo
