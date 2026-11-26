#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct CardDemoState {
    guikit::SurfaceRole surface = guikit::SurfaceRole::Flat;
    bool fullWidth = false;
    keel::f32 minWidth = 280.0f;
    keel::usize bgIdx = 0;     // 0=Default(Theme), 1=None/Transparent, 2=Surface, 3..9=Semantic
    keel::usize borderIdx = 0; // 0=Default, 1=Primary, 2=Secondary, 3=Success, 4=Warning, 5=Danger, 6=Info, 7=Ghost
    bool showHeader = true;
    bool showDivider = true;
};

CardDemoState s_card;

constexpr keel::StaticArray<keel::StringRef, 4> kSurfaceRoleNames = {
    "Flat", "Raised", "Sunken", "None"
};

guikit::SurfaceRole SurfaceRoleFromIdx(keel::usize idx) noexcept {
    switch (idx) {
        case 1:  return guikit::SurfaceRole::Raised;
        case 2:  return guikit::SurfaceRole::Sunken;
        case 3:  return guikit::SurfaceRole::None;
        default: return guikit::SurfaceRole::Flat;
    }
}

keel::usize SurfaceRoleToIdx(guikit::SurfaceRole r) noexcept {
    switch (r) {
        case guikit::SurfaceRole::Raised: return 1u;
        case guikit::SurfaceRole::Sunken: return 2u;
        case guikit::SurfaceRole::None:   return 3u;
        default:                          return 0u;
    }
}

constexpr keel::StringRef SurfaceRoleCodeName(guikit::SurfaceRole r) noexcept {
    switch (r) {
        case guikit::SurfaceRole::Raised: return "Raised";
        case guikit::SurfaceRole::Sunken: return "Sunken";
        case guikit::SurfaceRole::None:   return "None";
        default:                          return "Flat";
    }
}

guikit::StateColor ResolveCardBg() noexcept {
    return BgPresetToStateColor(s_card.bgIdx);
}

keel::String CardBgCodeName() {
    return BgPresetToCode(s_card.bgIdx);
}

guikit::StateColor ResolveCardBorderColor() noexcept {
    return BorderPresetToStateColor(s_card.borderIdx);
}

keel::StringRef CardBorderCodeName() noexcept {
    return BorderPresetToCode(s_card.borderIdx);
}

} // namespace

void RenderCardInspector(guikit::Context& ui, DemoContext& ctx) {
    keel::usize roleIdx = SurfaceRoleToIdx(s_card.surface);
    if (auto el = guikit::Element(ui, {.label = "Surface Role"})) {
        if (guikit::Segmented(ui, roleIdx, kSurfaceRoleNames).changed) {
            s_card.surface = SurfaceRoleFromIdx(roleIdx);
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Corner Radius"})) {
        static constexpr keel::StaticArray<keel::StringRef, 2> kRadiusVariants = {{"Auto", "%.0f px"}};
        if (guikit::Slider(ui, ctx.cardRadius, -1.0f, 24.0f, {
            .valueFormat = (ctx.cardRadius < 0.0f)
                ? guikit::UiText{"Auto", kRadiusVariants}
                : guikit::UiText{"%.0f px", kRadiusVariants},
            .step = 1.0f
        }).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Internal Padding"})) {
        if (guikit::Slider(ui, ctx.cardPadding, 0.0f, 32.0f, {.valueFormat = "%.0f px"}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Content Spacing (Gap)"})) {
        if (guikit::Slider(ui, ctx.cardGap, 0.0f, 24.0f, {.valueFormat = "%.0f px"}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Background"})) {
        if (guikit::Select(ui, s_card.bgIdx, kBgPresetNames).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Border Width"})) {
        if (guikit::Slider(ui, ctx.cardBorderWidth, 0.0f, 4.0f, {.valueFormat = "%.1f px", .step = 0.5f}).changed) {
            ui.InvalidateAll();
        }
    }
    const bool hasBorder = (ctx.cardBorderWidth > 0.0f);
    if (auto el = guikit::Element(ui, {.label = "Border Color", .disabled = !hasBorder})) {
        if (guikit::Select(ui, s_card.borderIdx, kUiVariantNames, {.disabled = !hasBorder}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Stretch Full Width"})) {
        if (guikit::Switch(ui, s_card.fullWidth).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Minimum Width", .disabled = s_card.fullWidth})) {
        if (guikit::Slider(ui, s_card.minWidth, 200.0f, 560.0f, {.valueFormat = "%.0f px", .step = 10.0f, .disabled = s_card.fullWidth}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Show Header"})) {
        if (guikit::Switch(ui, s_card.showHeader).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Header Divider", .disabled = !s_card.showHeader})) {
        if (guikit::Switch(ui, s_card.showDivider, {.disabled = !s_card.showHeader}).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderCardStage(guikit::Context& ui, DemoContext& ctx) {
    if (auto card = guikit::Card(ui, {
        .minWidth = s_card.fullWidth ? keel::Optional<keel::f32>{keel::kNullOpt} : keel::Optional<keel::f32>{s_card.minWidth},
        .width = s_card.fullWidth ? guikit::SizeMode::Stretch : guikit::SizeMode::Fit,
        .gap = ctx.cardGap,
        .padding = guikit::Padding::All(ctx.cardPadding),
        .background = ResolveCardBg(),
        .border = {
            .width = ctx.cardBorderWidth,
            .radius = (ctx.cardRadius >= 0.0f) ? keel::Optional<keel::f32>(ctx.cardRadius) : keel::kNullOpt,
            .color = ResolveCardBorderColor()
        },
        .surface = s_card.surface
    })) {
        if (s_card.showHeader) {
            guikit::Heading(ui, {
                .label = "Card Container",
                .description = "Elevated surface with padding, gap, and border",
                .icon = guikit::Icon::Folder(),
                .badge = {"Surface", guikit::StateColor::Primary()},
                .separator = s_card.showDivider ? guikit::HeadingSeparator::Bottom : guikit::HeadingSeparator::None
            });
        }

        guikit::Text(ui, "First content row inside the card container.", {
            .role = guikit::TextRole::BodyMedium
        });

        guikit::Text(ui, "Second content row spaced by the card's gap property.", {
            .role = guikit::TextRole::BodySmall,
            .variant = guikit::UiVariant::Secondary
        });
    }
}

keel::String GenerateCardCode(const DemoContext& ctx) {
    const keel::String borderCode = (ctx.cardRadius < 0.0f)
        ? keel::format("{{.width = {:.1f}f, .color = {}}}", ctx.cardBorderWidth, CardBorderCodeName())
        : keel::format("{{.width = {:.1f}f, .radius = {:.1f}f, .color = {}}}", ctx.cardBorderWidth, ctx.cardRadius, CardBorderCodeName());
    const keel::String minWLine = s_card.fullWidth
        ? keel::String{}
        : keel::format("    .minWidth = {:.1f}f,\n", s_card.minWidth);
    const keel::String headerBlock = s_card.showHeader
        ? keel::format(
            "    guikit::Heading(ui, {{\n"
            "        .label = \"Card Container\",\n"
            "        .description = \"Elevated surface with padding, gap, and border\",\n"
            "        .separator = {}\n"
            "    }});\n",
            s_card.showDivider ? "guikit::HeadingSeparator::Bottom" : "guikit::HeadingSeparator::None"
        )
        : keel::String{};
    return keel::format(
        "if (auto card = guikit::Card(ui, {{\n"
        "{}"
        "    .width = guikit::SizeMode::{},\n"
        "    .gap = {:.1f}f,\n"
        "    .padding = guikit::Padding::All({:.1f}f),\n"
        "    .background = {},\n"
        "    .border = {},\n"
        "    .surface = guikit::SurfaceRole::{}\n"
        "}})) {{\n"
        "{}"
        "    guikit::Text(ui, \"First content row inside the card container.\");\n"
        "    guikit::Text(ui, \"Second content row spaced by the card's gap property.\");\n"
        "}}",
        minWLine,
        s_card.fullWidth ? "Stretch" : "Fit",
        ctx.cardGap,
        ctx.cardPadding,
        CardBgCodeName(),
        borderCode,
        SurfaceRoleCodeName(s_card.surface),
        headerBlock
    );
}

} // namespace guikit::demo
