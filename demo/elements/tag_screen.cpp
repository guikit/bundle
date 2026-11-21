#include "../demo_context.hpp"
#include "guikit/widgets/tag.hpp"

namespace guikit::demo {

namespace {

struct TagDemoState {
    keel::SmallString<64> label{"In Method"};
    guikit::UiVariant variant = guikit::UiVariant::Success;
    guikit::TagStyle style = guikit::TagStyle::Soft;
    bool pill = false;
    bool clickable = true;
    bool closable = false;
    bool showIcon = false;
    keel::i32 clickCount = 0;
    keel::i32 closeCount = 0;
    keel::SmallString<64> catalogFilter{};
    keel::i32 selectedAlgo = -1;
};

TagDemoState s_state;

constexpr keel::StaticArray<keel::StringRef, 8> kVariantLabels = {
    "Default", "Primary", "Secondary", "Success", "Warning", "Danger", "Info", "Muted"
};

constexpr keel::StaticArray<guikit::UiVariant, 8> kVariants = {
    guikit::UiVariant::Default,
    guikit::UiVariant::Primary,
    guikit::UiVariant::Secondary,
    guikit::UiVariant::Success,
    guikit::UiVariant::Warning,
    guikit::UiVariant::Danger,
    guikit::UiVariant::Info,
    guikit::UiVariant::Muted
};

constexpr keel::StaticArray<keel::StringRef, 3> kStyleLabels = {
    "Soft", "Outline", "Solid"
};

keel::StringRef TagStyleName(guikit::TagStyle st) noexcept {
    switch (st) {
        case guikit::TagStyle::Outline: return "Outline";
        case guikit::TagStyle::Solid:   return "Solid";
        default:                        return "Soft";
    }
}

struct AlgoItem {
    keel::StringRef title;
    keel::StringRef symbol;
    keel::StringRef unit;
    keel::StringRef desc;
    bool inMethod;
    keel::StaticArray<keel::StringRef, 4> tags;
};

constexpr keel::StaticArray<AlgoItem, 2> kCatalogAlgos = {{
    {
        "Peak Tensile Force",
        "(P_max)",
        "[kN]",
        "Absolute peak load on the stress-strain curve and corresponding crosshead displacement.",
        true,
        {{"# strength", "# tensile", "# metals", "# iso-6892"}}
    },
    {
        "Upper Yield Strength",
        "(ReH)",
        "[MPa]",
        "Stress at the first force maximum prior to load drop at the yield plateau.",
        true,
        {{"# strength", "# yield", "# metals", "# iso-6892"}}
    }
}};

bool MatchesFilter(const AlgoItem& item, keel::StringRef filter) noexcept {
    if (filter.empty()) return true;
    auto containsI = [](keel::StringRef hay, keel::StringRef needle) noexcept -> bool {
        if (needle.empty()) return true;
        if (needle.size() > hay.size()) return false;
        for (keel::usize i = 0; i + needle.size() <= hay.size(); ++i) {
            if (hay.substr(i, needle.size()).equals_ignore_ascii_case(needle)) return true;
        }
        return false;
    };
    if (containsI(item.title, filter) || containsI(item.symbol, filter) || containsI(item.desc, filter)) {
        return true;
    }
    for (const auto& t : item.tags) {
        if (containsI(t, filter)) return true;
    }
    return false;
}

} // namespace

void RenderTagInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = ui.Element({.label = "Tag Label"})) {
        if (ui.InputField(s_state.label, {.placeholder = "Tag text...", .clearable = true}).changed) {
            ui.InvalidateAll();
        }
    }

    keel::usize varIdx = 0;
    for (keel::usize i = 0; i < kVariants.size(); ++i) {
        if (kVariants[i] == s_state.variant) {
            varIdx = i;
            break;
        }
    }
    if (auto el = ui.Element({.label = "Semantic Variant"})) {
        if (ui.Select(varIdx, kVariantLabels).changed) {
            s_state.variant = kVariants[varIdx];
            ui.InvalidateAll();
        }
    }

    keel::i32 styleIdx = static_cast<keel::i32>(s_state.style);
    if (auto el = ui.Element({.label = "Tag Style"})) {
        if (ui.Segmented(styleIdx, kStyleLabels).changed) {
            s_state.style = static_cast<guikit::TagStyle>(styleIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = ui.Element({.label = "Pill Capsule"})) {
        if (ui.Switch(s_state.pill).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Leading Icon"})) {
        if (ui.Switch(s_state.showIcon).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Clickable"})) {
        if (ui.Switch(s_state.clickable).changed) ui.InvalidateAll();
    }
    if (auto el = ui.Element({.label = "Closable ('x')"})) {
        if (ui.Switch(s_state.closable).changed) ui.InvalidateAll();
    }
}

void RenderTagStage(guikit::Context& ui, DemoContext& ctx) {
    if (auto col = ui.Column({.gap = 16.0f, .width = guikit::SizeMode::Stretch})) {
        ctx.RunElementStage(ui, [&]() {
            guikit::TagOptions opts{};
            opts.variant = s_state.variant;
            opts.style = s_state.style;
            opts.size = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Sm : ctx.controlsSize;
            opts.pill = s_state.pill;
            opts.clickable = s_state.clickable;
            opts.closable = s_state.closable;
            opts.disabled = ctx.elemDisabled;
            if (s_state.showIcon) {
                opts.icon = guikit::Icon::Check();
            }
            const auto r = ui.Tag(s_state.label.as_ref(), opts);
            if (r.clicked) {
                ++s_state.clickCount;
                ui.ShowToast({.title = "Tag clicked", .message = s_state.label.as_ref(), .variant = s_state.variant});
            }
            if (r.closed) {
                ++s_state.closeCount;
                ui.ShowToast({.title = "Tag closed", .message = s_state.label.as_ref(), .variant = guikit::UiVariant::Warning});
            }
        });

        // Composite Pattern Showcase: Algorithm Catalog Card (Card + Tag + Wrap + Button)
        if (auto catalog = ui.Card({
            .gap = 10.0f,
            .padding = guikit::Padding::All(14.0f)
        })) {
            ui.Heading({
                .label = "Algorithm & Formula Catalog",
                .description = "Composite pattern: Card + hover border + clickable Tag chips + status Tag + action Button",
                .level = guikit::HeadingLevel::H4
            });

            ui.InputField(s_state.catalogFilter, {
                .placeholder = "Filter by name, symbol, or click any #tag below...",
                .clearable = true
            });

            const auto& pal = ui.GetTheme().palette;
            for (keel::usize i = 0; i < kCatalogAlgos.size(); ++i) {
                const auto& item = kCatalogAlgos[i];
                if (!MatchesFilter(item, s_state.catalogFilter.as_ref())) {
                    continue;
                }
                const bool isSel = (s_state.selectedAlgo == static_cast<keel::i32>(i));
                if (auto itemCard = ui.Card({
                    .padding = guikit::Padding::Symmetric(10.0f, 12.0f),
                    .border = {
                        .width = 1.0f,
                        .radius = 8.0f,
                        .color = {
                            .normal = isSel ? pal.accent.WithAlphaF(0.75f) : pal.border,
                            .hover = pal.accent.WithAlphaF(0.65f)
                        }
                    }
                })) {
                    if (auto row = ui.Row({.gap = 12.0f, .align = guikit::Alignment::Center, .width = guikit::SizeMode::Stretch})) {
                        if (auto left = ui.Column({.gap = 4.0f, .width = guikit::SizeMode::Stretch})) {
                            if (auto titleRow = ui.Row({.gap = 6.0f, .align = guikit::Alignment::Center})) {
                                ui.Text(item.title, {.role = guikit::TextRole::TitleSmall});
                                ui.Text(item.symbol, {.role = guikit::TextRole::TitleSmall, .variant = guikit::UiVariant::Info});
                                ui.Text(item.unit, {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Muted});
                            }
                            ui.Text(item.desc, {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Muted});
                            if (auto tagWrap = ui.Wrap({.gap = 6.0f})) {
                                for (const auto& t : item.tags) {
                                    if (ui.Tag(t, {
                                        .variant = guikit::UiVariant::Default,
                                        .style = guikit::TagStyle::Soft,
                                        .size = guikit::UiSize::Xs,
                                        .clickable = true
                                    }).clicked) {
                                        s_state.catalogFilter.assign(t);
                                        ui.InvalidateAll();
                                    }
                                }
                            }
                        }

                        if (item.inMethod) {
                            ui.Tag("In Method", {
                                .variant = guikit::UiVariant::Success,
                                .style = guikit::TagStyle::Soft,
                                .size = guikit::UiSize::Sm
                            });
                        }

                        if (ui.Button({
                            .icon = guikit::Icon::ChevronRight(),
                            .size = guikit::UiSize::Sm,
                            .background = guikit::StateColor::Secondary()
                        }).clicked) {
                            s_state.selectedAlgo = static_cast<keel::i32>(i);
                            ui.ShowToast({
                                .title = item.title,
                                .message = "Opened formula configuration",
                                .variant = guikit::UiVariant::Info
                            });
                        }
                    }
                }
            }
        }
    }
}

keel::String GenerateTagCode(const DemoContext& ctx) {
    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Sm : ctx.controlsSize;
    return keel::format(
        "if (auto r = ui.Tag(\"{}\", {{\n"
        "    .variant = guikit::UiVariant::{},\n"
        "    .style = guikit::TagStyle::{},\n"
        "    .size = guikit::UiSize::{},\n"
        "    .pill = {},\n"
        "    .clickable = {},\n"
        "    .closable = {}\n"
        "}}); r.clicked) {{\n"
        "    // Handle tag click\n"
        "}}",
        s_state.label,
        VariantName(s_state.variant),
        TagStyleName(s_state.style),
        SizeName(effSize),
        s_state.pill ? "true" : "false",
        s_state.clickable ? "true" : "false",
        s_state.closable ? "true" : "false"
    );
}

} // namespace guikit::demo
