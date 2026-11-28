#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct PaginationDemoState {
    keel::usize currentPage{4};
    keel::i32 totalPages{20};
    keel::i32 siblingCount{1};
    bool showFirstLast{true};
    bool showPrevNext{true};
    bool showPageInfo{true};
    bool interactiveEllipsis{false};
    bool showSurfaceBox{false};
};

PaginationDemoState s_pg;

} // namespace

void RenderPaginationInspector(guikit::Context& ui, DemoContext& ctx) {
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
        .label = "Pagination Options",
        .level = guikit::HeadingLevel::H6,
        .separator = guikit::HeadingSeparator::Bottom
    });

    if (auto el = guikit::Element(ui, {.label = "Total Pages"})) {
        if (guikit::Slider(ui, s_pg.totalPages, 3, 30, {.valueFormat = "%d pages"}).changed) {
            if (s_pg.currentPage >= keel::as<keel::usize>(s_pg.totalPages)) {
                s_pg.currentPage = keel::as<keel::usize>(s_pg.totalPages - 1);
            }
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Sibling Count",
        .description = "Neighbors shown on each side of active page"
    })) {
        if (guikit::Slider(ui, s_pg.siblingCount, 0, 3, {.valueFormat = "%d"}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {
        .label = "Fast-Jump Ellipsis (...)",
        .description = "Off = static separator (MUI/Shadcn); On = fast-jump button (Ant Design)"
    })) {
        if (guikit::Switch(ui, s_pg.interactiveEllipsis).changed) ui.InvalidateAll();
    }

    if (auto el = guikit::Element(ui, {.label = "First / Last Buttons (« »)"})) {
        if (guikit::Switch(ui, s_pg.showFirstLast).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Prev / Next Buttons (‹ ›)"})) {
        if (guikit::Switch(ui, s_pg.showPrevNext).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Page Info Summary"})) {
        if (guikit::Switch(ui, s_pg.showPageInfo).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Surface Well Frame"})) {
        if (guikit::Switch(ui, s_pg.showSurfaceBox).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Disabled State"})) {
        if (guikit::Switch(ui, ctx.elemDisabled).changed) ui.InvalidateAll();
    }
}

void RenderPaginationStage(guikit::Context& ui, DemoContext& ctx) {
    const keel::usize totalPages = keel::as<keel::usize>(keel::max(1, s_pg.totalPages));
    if (s_pg.currentPage >= totalPages) {
        s_pg.currentPage = totalPages - 1;
    }

    auto renderPager = [&]() {
        guikit::Pagination(ui, s_pg.currentPage, totalPages, {
            .siblingCount = keel::as<keel::u32>(keel::max(0, s_pg.siblingCount)),
            .showFirstLast = s_pg.showFirstLast,
            .showPrevNext = s_pg.showPrevNext,
            .showPageInfo = s_pg.showPageInfo,
            .interactiveEllipsis = s_pg.interactiveEllipsis,
            .disabled = ctx.elemDisabled,
            .size = ctx.controlsSize,
            .variant = ctx.btnVariant
        });
    };

    if (s_pg.showSurfaceBox) {
        if (auto well = guikit::Box(ui, {
            .width = guikit::SizeMode::Fit,
            .padding = guikit::Padding::Symmetric(10.0f, 6.0f),
            .border = {.width = 1.0f},
            .surface = guikit::SurfaceRole::Flat
        })) {
            renderPager();
        }
    } else {
        renderPager();
    }
}

keel::String GeneratePaginationCode(const DemoContext& ctx) {
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLineIndented = hasCustomSize
        ? keel::format_fixed<64>("        .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::FixedString<64> sizeLineTop = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::FixedString<64> variantCode = keel::format_fixed<64>("guikit::UiVariant::{}", VariantName(ctx.btnVariant));

    if (s_pg.showSurfaceBox) {
        return keel::format(
            "if (auto well = guikit::Box(ui, {{\n"
            "    .width = guikit::SizeMode::Fit,\n"
            "    .padding = guikit::Padding::Symmetric(10.0f, 6.0f),\n"
            "    .border = {{.width = 1.0f}},\n"
            "    .surface = guikit::SurfaceRole::Flat\n"
            "}})) {{\n"
            "    guikit::Pagination(ui, currentPage, {}, {{\n"
            "        .siblingCount = {},\n"
            "        .showFirstLast = {},\n"
            "        .showPrevNext = {},\n"
            "        .showPageInfo = {},\n"
            "        .interactiveEllipsis = {},\n"
            "        .disabled = {},\n"
            "{}"
            "        .variant = {}\n"
            "    }});\n"
            "}}",
            s_pg.totalPages,
            s_pg.siblingCount,
            s_pg.showFirstLast ? "true" : "false",
            s_pg.showPrevNext ? "true" : "false",
            s_pg.showPageInfo ? "true" : "false",
            s_pg.interactiveEllipsis ? "true" : "false",
            ctx.elemDisabled ? "true" : "false",
            sizeLineIndented,
            variantCode
        );
    }

    return keel::format(
        "guikit::Pagination(ui, currentPage, {}, {{\n"
        "    .siblingCount = {},\n"
        "    .showFirstLast = {},\n"
        "    .showPrevNext = {},\n"
        "    .showPageInfo = {},\n"
        "    .interactiveEllipsis = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .variant = {}\n"
        "}});",
        s_pg.totalPages,
        s_pg.siblingCount,
        s_pg.showFirstLast ? "true" : "false",
        s_pg.showPrevNext ? "true" : "false",
        s_pg.showPageInfo ? "true" : "false",
        s_pg.interactiveEllipsis ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLineTop,
        variantCode
    );
}

} // namespace guikit::demo
