#include "../demo_context.hpp"
#include "guikit/widgets/tag.hpp"

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
    ctx.RunElementStage(ui, [&]() {
        const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Sm : ctx.controlsSize;
        if (auto wrap = ui.Wrap({.gap = 8.0f})) {
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
            const auto r = ui.Tag(s_state.label.as_ref(), opts);
            if (r.clicked) {
                ui.ShowToast({.title = "Tag clicked", .message = s_state.label.as_ref(), .variant = s_state.variant});
            }
            if (r.closed) {
                ui.ShowToast({.title = "Tag closed", .message = s_state.label.as_ref(), .variant = guikit::UiVariant::Warning});
            }

            // Companion semantic chips mirroring current style/size/pill settings
            struct SampleChip { keel::StringRef text; guikit::UiVariant var; };
            static constexpr keel::StaticArray<SampleChip, 4> kSamples = {{
                {"# cpp20",   guikit::UiVariant::Default},
                {"Stable",    guikit::UiVariant::Info},
                {"Beta",      guikit::UiVariant::Warning},
                {"Breaking",  guikit::UiVariant::Danger}
            }};
            for (const auto& chip : kSamples) {
                guikit::TagOptions sampleOpts = opts;
                sampleOpts.variant = chip.var;
                sampleOpts.icon = {};
                (void)ui.Tag(chip.text, sampleOpts);
            }
        }
    });
}

keel::String GenerateTagCode(const DemoContext& ctx) {
    const guikit::UiSize effSize = (ctx.controlsSize == guikit::UiSize::None) ? guikit::UiSize::Sm : ctx.controlsSize;
    const keel::String ctrl = keel::format(
        "if (auto r = ui.Tag(\"{}\", {{\n"
        "    .variant = guikit::UiVariant::{},\n"
        "    .style = guikit::TagStyle::{},\n"
        "    .size = guikit::UiSize::{},\n"
        "    .pill = {},\n"
        "    .clickable = {},\n"
        "    .closable = {}\n"
        "}}); r.clicked) {{\n"
        "    // Handle click\n"
        "}}",
        s_state.label,
        VariantName(s_state.variant),
        TagStyleName(s_state.style),
        SizeName(effSize),
        s_state.pill ? "true" : "false",
        s_state.clickable ? "true" : "false",
        s_state.closable ? "true" : "false"
    );
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
