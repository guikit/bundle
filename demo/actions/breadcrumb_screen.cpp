#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct BreadcrumbDemoState {
    keel::usize crumbIdx{4};
    keel::usize separatorIdx{0};
    guikit::BreadcrumbSeparator separator{guikit::BreadcrumbSeparator::Chevron};
    keel::i32 maxVisibleItems{0}; // 0 = show all, 2..5 = collapse middle items into "..."
    bool showCrumbIcons{true};
    bool showCrumbBadges{true};
    bool showSurfaceBox{false};
};

BreadcrumbDemoState s_bc;

constexpr keel::StringRef SeparatorEnumName(guikit::BreadcrumbSeparator s) noexcept {
    switch (s) {
        case guikit::BreadcrumbSeparator::Chevron: return "Chevron";
        case guikit::BreadcrumbSeparator::Slash:   return "Slash";
        case guikit::BreadcrumbSeparator::Arrow:   return "Arrow";
        case guikit::BreadcrumbSeparator::Dot:     return "Dot";
    }
    return "Chevron";
}

} // namespace

void RenderBreadcrumbInspector(guikit::Context& ui, DemoContext& ctx) {
    if (auto el = guikit::Element(ui, {.label = "Control Size"})) {
        if (guikit::StepSlider(ui, ctx.controlsSize, {.showTicks = true, .showLabels = true, .includeNone = true}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Semantic Variant"})) {
        if (guikit::Select(ui, ctx.btnVariant, kUiVariantNames).changed) {
            ui.InvalidateAll();
        }
    }

    guikit::Heading(ui, {
        .label = "Breadcrumb Options",
        .level = guikit::HeadingLevel::H6,
        .separator = guikit::HeadingSeparator::Bottom
    });

    static constexpr keel::StaticArray<keel::StringRef, 4> kSepNames = {
        "Chevron", "Slash", "Arrow", "Dot"
    };
    if (auto el = guikit::Element(ui, {.label = "Separator Style"})) {
        if (guikit::Segmented(ui, s_bc.separatorIdx, kSepNames).changed) {
            s_bc.separator = keel::from_underlying<guikit::BreadcrumbSeparator>(s_bc.separatorIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Max Visible Items",
        .description = "0 = show all; >= 2 collapses middle items into '...'"
    })) {
        if (guikit::Slider(ui, s_bc.maxVisibleItems, 0, 5, {
            .valueFormat = (s_bc.maxVisibleItems < 2) ? "All" : "%d items"
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Item Icons"})) {
        if (guikit::Switch(ui, s_bc.showCrumbIcons).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Item Badges"})) {
        if (guikit::Switch(ui, s_bc.showCrumbBadges).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Surface Well Frame"})) {
        if (guikit::Switch(ui, s_bc.showSurfaceBox).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Disabled State"})) {
        if (guikit::Switch(ui, ctx.elemDisabled).changed) ui.InvalidateAll();
    }
}

void RenderBreadcrumbStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::u32 maxVis = (s_bc.maxVisibleItems >= 2) ? keel::as<keel::u32>(s_bc.maxVisibleItems) : 0u;

    const keel::StaticArray<guikit::BreadcrumbItem, 5> crumbs = {{
        {.label = "Workspace", .icon = s_bc.showCrumbIcons ? guikit::Icon::Folder()   : guikit::Icon{}},
        {.label = "Projects",  .icon = s_bc.showCrumbIcons ? guikit::Icon::Settings() : guikit::Icon{}, .badge = s_bc.showCrumbBadges ? "Team" : ""},
        {.label = "WebApp",    .icon = s_bc.showCrumbIcons ? guikit::Icon::Flash()    : guikit::Icon{}},
        {.label = "Releases",  .icon = s_bc.showCrumbIcons ? guikit::Icon::Search()   : guikit::Icon{}, .badge = s_bc.showCrumbBadges ? "v2.4" : ""},
        {.label = "Overview",  .icon = s_bc.showCrumbIcons ? guikit::Icon::Play()     : guikit::Icon{}}
    }};

    auto renderTrail = [&]() {
        guikit::Breadcrumb(ui, s_bc.crumbIdx, crumbs, {
            .separator = s_bc.separator,
            .maxVisibleItems = maxVis,
            .disabled = ctx.elemDisabled,
            .size = ctx.controlsSize,
            .variant = ctx.btnVariant
        });
    };

    if (s_bc.showSurfaceBox) {
        if (auto well = guikit::Box(ui, {
            .width = guikit::SizeMode::Fit,
            .padding = guikit::Padding::Symmetric(10.0f, 6.0f),
            .background = guikit::StateColor::Surface(),
            .border = {.width = 1.0f, .radius = 8.0f}
        })) {
            renderTrail();
        }
    } else {
        renderTrail();
    }
}

keel::String GenerateBreadcrumbCode(const DemoContext& ctx) {
    const keel::u32 maxVis = (s_bc.maxVisibleItems >= 2) ? keel::as<keel::u32>(s_bc.maxVisibleItems) : 0u;
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLineIndented = hasCustomSize
        ? keel::format_fixed<64>("        .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::FixedString<64> sizeLineTop = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::FixedString<64> variantCode = keel::format_fixed<64>("guikit::UiVariant::{}", VariantName(ctx.btnVariant));

    const keel::String crumbsDecl = keel::format(
        "static const keel::StaticArray<guikit::BreadcrumbItem, 5> crumbs = {{{{\n"
        "    {{.label = \"Workspace\"{}}},\n"
        "    {{.label = \"Projects\"{}{}}},\n"
        "    {{.label = \"WebApp\"{}}},\n"
        "    {{.label = \"Releases\"{}{}}},\n"
        "    {{.label = \"Overview\"{}}}\n"
        "}}}};\n",
        s_bc.showCrumbIcons ? ", .icon = guikit::Icon::Folder()" : "",
        s_bc.showCrumbIcons ? ", .icon = guikit::Icon::Settings()" : "",
        s_bc.showCrumbBadges ? ", .badge = \"Team\"" : "",
        s_bc.showCrumbIcons ? ", .icon = guikit::Icon::Flash()" : "",
        s_bc.showCrumbIcons ? ", .icon = guikit::Icon::Search()" : "",
        s_bc.showCrumbBadges ? ", .badge = \"v2.4\"" : "",
        s_bc.showCrumbIcons ? ", .icon = guikit::Icon::Play()" : ""
    );

    if (s_bc.showSurfaceBox) {
        return keel::format(
            "{}"
            "if (auto well = guikit::Box(ui, {{\n"
            "    .width = guikit::SizeMode::Fit,\n"
            "    .padding = guikit::Padding::Symmetric(10.0f, 6.0f),\n"
            "    .background = guikit::StateColor::Surface(),\n"
            "    .border = {{.width = 1.0f, .radius = 8.0f}}\n"
            "}})) {{\n"
            "    guikit::Breadcrumb(ui, activeCrumb, crumbs, {{\n"
            "        .separator = guikit::BreadcrumbSeparator::{},\n"
            "        .maxVisibleItems = {},\n"
            "        .disabled = {},\n"
            "{}"
            "        .variant = {}\n"
            "    }});\n"
            "}}",
            crumbsDecl,
            SeparatorEnumName(s_bc.separator),
            maxVis,
            ctx.elemDisabled ? "true" : "false",
            sizeLineIndented,
            variantCode
        );
    }

    return keel::format(
        "{}"
        "guikit::Breadcrumb(ui, activeCrumb, crumbs, {{\n"
        "    .separator = guikit::BreadcrumbSeparator::{},\n"
        "    .maxVisibleItems = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .variant = {}\n"
        "}});",
        crumbsDecl,
        SeparatorEnumName(s_bc.separator),
        maxVis,
        ctx.elemDisabled ? "true" : "false",
        sizeLineTop,
        variantCode
    );
}

} // namespace guikit::demo
