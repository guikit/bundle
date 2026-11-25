#include "../demo_context.hpp"
#include "guikit/display/tag.hpp"

namespace guikit::demo {

namespace {

struct TagDemoState {
    keel::SmallString<64> label{"v2.4.0"};
    guikit::UiVariant variant = guikit::UiVariant::Success;
    guikit::TagStyle style = guikit::TagStyle::Soft;
    bool pill = false;
    bool clickable = true;
    bool closable = false;
    bool showIcon = false;
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

} // namespace

void RenderTagInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Tag Label"})) {
        if (guikit::InputField(ui, s_state.label, {.placeholder = "Tag text...", .clearable = true}).changed) {
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
    if (auto el = guikit::Element(ui, {.label = "Semantic Variant"})) {
        if (guikit::Select(ui, varIdx, kVariantLabels).changed) {
            s_state.variant = kVariants[varIdx];
            ui.InvalidateAll();
        }
    }

    keel::i32 styleIdx = keel::as<keel::i32>(s_state.style);
    if (auto el = guikit::Element(ui, {.label = "Tag Style"})) {
        if (guikit::Segmented(ui, styleIdx, kStyleLabels).changed) {
            s_state.style = keel::from_underlying<guikit::TagStyle>(styleIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Pill Capsule"})) {
        if (guikit::Switch(ui, s_state.pill).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Leading Icon"})) {
        if (guikit::Switch(ui, s_state.showIcon).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Clickable"})) {
        if (guikit::Switch(ui, s_state.clickable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Closable ('x')"})) {
        if (guikit::Switch(ui, s_state.closable).changed) ui.InvalidateAll();
    }
}

void RenderTagStage(guikit::Context& ui, DemoContext& ctx) {
    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Sm : ctx.controlsSize;
    guikit::TagOptions opts{};
    opts.variant = s_state.variant;
    opts.style = s_state.style;
    opts.size = effSize;
    opts.pill = s_state.pill;
    opts.clickable = s_state.clickable;
    opts.closable = s_state.closable;
    opts.disabled = ctx.elemDisabled;
    if (s_state.showIcon) {
        opts.icon = guikit::Icon::Check();
    }
    const auto r = guikit::Tag(ui, s_state.label.as_ref(), opts);
    if (r.clicked) {
        guikit::ShowToast(ui, {.title = "Tag clicked", .message = s_state.label.as_ref(), .variant = s_state.variant});
    }
    if (r.closed) {
        guikit::ShowToast(ui, {.title = "Tag closed", .message = s_state.label.as_ref(), .variant = guikit::UiVariant::Warning});
    }
}

keel::String GenerateTagCode(const DemoContext& ctx) {
    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Sm : ctx.controlsSize;
    return keel::format(
        "if (auto r = guikit::Tag(ui, \"{}\", {{\n"
        "    .variant = guikit::UiVariant::{},\n"
        "    .style = guikit::TagStyle::{},\n"
        "    .size = guikit::UiSize::{},\n"
        "    .pill = {},\n"
        "    .clickable = {},\n"
        "    .closable = {},\n"
        "    .disabled = {}\n"
        "}}); r.clicked) {{\n"
        "    // Handle click\n"
        "}}",
        s_state.label,
        VariantName(s_state.variant),
        TagStyleName(s_state.style),
        SizeName(effSize),
        s_state.pill ? "true" : "false",
        s_state.clickable ? "true" : "false",
        s_state.closable ? "true" : "false",
        ctx.elemDisabled ? "true" : "false"
    );
}

} // namespace guikit::demo
