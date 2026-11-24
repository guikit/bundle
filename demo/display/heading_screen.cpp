#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct HeadingStudioState {
    guikit::HeadingLevel level = guikit::HeadingLevel::H3;
    guikit::HeadingLayout layout = guikit::HeadingLayout::Stacked;
    guikit::HeadingSeparator separator = guikit::HeadingSeparator::Bottom;
    guikit::Align align = guikit::Align::Start;
    keel::usize widthModeIdx = 0; // 0 = Auto (Stretch), 1 = Fit, 2 = Stretch, 3 = Fixed
    keel::f32 widthDp = 360.0f;
    bool showSubtitle = true;
    bool showIconAndBadge = true;
    bool disabled = false;
};

HeadingStudioState s_state;

constexpr keel::StringRef kSampleTitle = "Signal Processing Pipeline";
constexpr keel::StringRef kSampleSubtitle =
    "Real-time polyphase filterbank & quadrature demodulator";
constexpr keel::StringRef kSampleBadge = "v3.2";

constexpr keel::StringRef LevelCodeName(guikit::HeadingLevel lvl) noexcept {
    switch (lvl) {
        case guikit::HeadingLevel::H1: return "guikit::HeadingLevel::H1";
        case guikit::HeadingLevel::H2: return "guikit::HeadingLevel::H2";
        case guikit::HeadingLevel::H3: return "guikit::HeadingLevel::H3";
        case guikit::HeadingLevel::H4: return "guikit::HeadingLevel::H4";
        case guikit::HeadingLevel::H5: return "guikit::HeadingLevel::H5";
        case guikit::HeadingLevel::H6: return "guikit::HeadingLevel::H6";
    }
    return "guikit::HeadingLevel::H3";
}

guikit::AxisSize ActiveHeadingWidth() noexcept {
    return DemoContext::ResolveAxisSize(s_state.widthModeIdx, s_state.widthDp);
}

guikit::HeadingOptions BuildHeadingOptions() noexcept {
    return guikit::HeadingOptions{
        .label = kSampleTitle,
        .description = s_state.showSubtitle
            ? guikit::UiText{kSampleSubtitle, guikit::UiVariant::Muted, guikit::TextWrap::Word()}
            : guikit::UiText{},
        .icon = s_state.showIconAndBadge ? guikit::Icon::Flash() : guikit::Icon{},
        .badge = s_state.showIconAndBadge
            ? guikit::UiText{kSampleBadge, guikit::UiVariant::Primary}
            : guikit::UiText{},
        .level = s_state.level,
        .layout = s_state.layout,
        .align = s_state.align,
        .width = ActiveHeadingWidth(),
        .separator = s_state.separator,
        .disabled = s_state.disabled
    };
}

} // namespace

void RenderHeadingInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 6> kLevelNames = {
        "H1", "H2", "H3", "H4", "H5", "H6"
    };
    if (auto el = guikit::Element(ui, {.label = "Level"})) {
        if (guikit::StepSlider(ui, s_state.level, kLevelNames, {
            .showTicks = true,
            .showLabels = true
        }).changed) {
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 3> kHeadingLayouts = {
        "Stacked", "Overline", "Inline"
    };
    if (auto el = guikit::Element(ui, {.label = "Layout"})) {
        if (guikit::Segmented(ui, s_state.layout, kHeadingLayouts).changed) {
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 3> kSepStyles = {
        "None", "Bottom", "Trailing"
    };
    if (auto el = guikit::Element(ui, {.label = "Separator"})) {
        if (guikit::Segmented(ui, s_state.separator, kSepStyles).changed) {
            ui.InvalidateAll();
        }
    }

    keel::usize alignIdx = keel::as<keel::usize>(s_state.align);
    if (auto el = guikit::Element(ui, {.label = "Alignment"})) {
        if (guikit::Segmented(ui, alignIdx, kLabelAlignNames).changed) {
            s_state.align = keel::from_underlying<guikit::Align>(alignIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Width",
        .badge = {"Stretch", guikit::UiVariant::Secondary}
    })) {
        if (guikit::Segmented(ui, s_state.widthModeIdx, kAxisModeNames).changed) {
            ui.InvalidateAll();
        }
    }

    const bool fixedWidth = (s_state.widthModeIdx == 3);
    if (auto el = guikit::Element(ui, {.label = "Width (dp)", .disabled = !fixedWidth})) {
        if (guikit::Slider(ui, s_state.widthDp, 140.0f, 560.0f, {
            .valueFormat = "%.0f dp",
            .step = 10.0f,
            .disabled = !fixedWidth
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Subtitle"})) {
        if (guikit::Switch(ui, s_state.showSubtitle).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Icon & Badge"})) {
        if (guikit::Switch(ui, s_state.showIconAndBadge).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Disabled"})) {
        if (guikit::Switch(ui, s_state.disabled).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderHeadingStage(guikit::Context& ui, DemoContext&) {
    guikit::Heading(ui, BuildHeadingOptions());
}

keel::String GenerateHeadingCode(const DemoContext&) {
    keel::String stmt = keel::format(
        "guikit::Heading(ui, {{\n"
        "    .label = \"{}\"",
        kSampleTitle
    );

    if (s_state.showSubtitle) {
        keel::format_into(stmt, ",\n    .description = \"{}\"", kSampleSubtitle);
    }
    if (s_state.showIconAndBadge) {
        stmt.append(",\n    .icon = guikit::Icon::Flash()");
        keel::format_into(
            stmt,
            ",\n    .badge = {{\"{}\", guikit::StateColor::Primary()}}",
            kSampleBadge
        );
    }
    if (s_state.level != guikit::HeadingLevel::H3) {
        keel::format_into(stmt, ",\n    .level = {}", LevelCodeName(s_state.level));
    }
    if (s_state.layout != guikit::HeadingLayout::Stacked) {
        keel::format_into(
            stmt,
            ",\n    .layout = guikit::HeadingLayout::{}",
            HeadingLayoutEnumName(s_state.layout)
        );
    }
    if (s_state.align != guikit::Align::Start) {
        keel::format_into(stmt, ",\n    .align = guikit::Align::{}", LabelAlignEnumName(s_state.align));
    }
    if (const guikit::AxisSize headingW = ActiveHeadingWidth(); !headingW.IsAuto()) {
        keel::format_into(stmt, ",\n    .width = {}", DemoContext::FormatAxisSizeCode(headingW));
    }
    if (s_state.separator == guikit::HeadingSeparator::Bottom) {
        stmt.append(",\n    .separator = guikit::HeadingSeparator::Bottom");
    } else if (s_state.separator == guikit::HeadingSeparator::Trailing) {
        stmt.append(",\n    .separator = guikit::HeadingSeparator::Trailing");
    }
    if (s_state.disabled) {
        stmt.append(",\n    .disabled = true");
    }
    stmt.append("\n});");
    return stmt;
}

} // namespace guikit::demo
