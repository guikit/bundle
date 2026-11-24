#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct InputState {
    keel::SmallString<128> text{"guikit.dev"};
    keel::SmallString<128> placeholder{"Enter domain..."};
    keel::SmallString<64> prefix{"https://"};
    keel::SmallString<64> suffix{":443"};
    guikit::FieldStyle fieldStyle = guikit::FieldStyle::Default;
    guikit::UiVariant status = guikit::UiVariant::Default;
    guikit::CursorBlinkMode cursorMode = guikit::CursorBlinkMode::Blinking;
    bool password = false;
    bool clearable = true;
    bool readOnly = false;
    bool companionButton = false;
    keel::i32 connectCount = 0;
};
InputState s_state;

constexpr keel::StringRef FieldStyleName(guikit::FieldStyle s) noexcept {
    switch (s) {
        case guikit::FieldStyle::Default:    return "Default";
        case guikit::FieldStyle::Filled:     return "Filled";
        case guikit::FieldStyle::Borderless: return "Borderless";
    }
    return "Default";
}
} // namespace

void RenderInputInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 3> kFieldStyleNames = {"Default", "Filled", "Borderless"};
    if (auto el = guikit::Element(ui, {.label = "Field Style"})) {
        if (guikit::Segmented(ui, s_state.fieldStyle, kFieldStyleNames).changed) {
            ui.InvalidateAll();
        }
    }
    static constexpr keel::StaticArray<keel::StringRef, 5> kStatusNames = {"Default", "Success", "Warning", "Danger", "Info"};
    static constexpr keel::StaticArray<guikit::UiVariant, 5> kStatusVals = {
        guikit::UiVariant::Default, guikit::UiVariant::Success, guikit::UiVariant::Warning,
        guikit::UiVariant::Danger, guikit::UiVariant::Info
    };
    keel::usize statusIdx = 0;
    for (keel::usize i = 0; i < kStatusVals.size(); ++i) {
        if (kStatusVals[i] == s_state.status) { statusIdx = i; break; }
    }
    if (auto el = guikit::Element(ui, {.label = "Validation Status"})) {
        if (guikit::Select(ui, statusIdx, kStatusNames).changed) {
            s_state.status = kStatusVals[statusIdx];
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {
        .label = "Multi-Control Slot (Input + Button)",
        .description = "Place InputField + Button inside a single ui.Element"
    })) {
        if (guikit::Switch(ui, s_state.companionButton).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Placeholder"})) {
        if (guikit::InputField(ui, s_state.placeholder, {
            .placeholder = "No placeholder...",
            .clearable = true
        }).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Prefix Text"})) {
        if (guikit::InputField(ui, s_state.prefix, {
            .placeholder = "e.g. https://",
            .clearable = true
        }).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Suffix Text"})) {
        if (guikit::InputField(ui, s_state.suffix, {
            .placeholder = "e.g. :443",
            .clearable = true
        }).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Clearable Button ('X')"})) {
        if (guikit::Switch(ui, s_state.clearable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Password Mask & Reveal"})) {
        if (guikit::Switch(ui, s_state.password).changed) ui.InvalidateAll();
    }

    static constexpr keel::StaticArray<keel::StringRef, 3> cursorModes = {"Blinking", "Solid", "Hidden"};
    if (auto el = guikit::Element(ui, {.label = "Cursor Blink Mode"})) {
        if (guikit::Segmented(ui, s_state.cursorMode, cursorModes).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Read Only"})) {
        if (guikit::Switch(ui, s_state.readOnly).changed) ui.InvalidateAll();
    }
}

void RenderInputStage(guikit::Context& ui, DemoContext& ctx) {
    ctx.RunElementStage(ui, [&]() {
        static constexpr keel::StaticArray<keel::StringRef, 2> kHostVariants = {{"guikit.dev", "api.guikit.dev"}};
        guikit::InputFieldOptions opts{};
        opts.placeholder = s_state.placeholder;
        opts.prefix = s_state.prefix;
        opts.suffix = s_state.suffix;
        opts.valueFormat = {kHostVariants};
        opts.cursorMode = s_state.cursorMode;
        opts.password = s_state.password;
        opts.clearable = s_state.clearable;
        opts.readOnly = s_state.readOnly;
        opts.disabled = ctx.elemDisabled;
        opts.size = ctx.controlsSize;
        opts.width = ctx.ResolveCtrlWidth();
        opts.height = ctx.ResolveCtrlHeight();
        opts.style = s_state.fieldStyle;
        opts.status = s_state.status;
        opts.background = ctx.GetControlBackground(NavCategory::ElemInputField);
        opts.border = ctx.GetCtrlBorder();
        guikit::InputField(ui, s_state.text, opts);

        if (s_state.companionButton) {
            const keel::StringRef btnLbl = (s_state.connectCount > 0)
                ? ui.Format("Connect ({})", s_state.connectCount)
                : keel::StringRef("Connect");
            if (guikit::Button(ui, {
                .text = btnLbl,
                .icon = guikit::Icon::Play(),
                .disabled = ctx.elemDisabled,
                .size = ctx.controlsSize,
                .height = ctx.ResolveCtrlHeight(),
                .style = (ctx.btnVariant == guikit::UiVariant::Ghost) ? guikit::ButtonStyle::Ghost : guikit::ButtonStyle::Default,
                .variant = (ctx.btnVariant == guikit::UiVariant::Ghost) ? guikit::UiVariant::Default : ctx.btnVariant
            })) {
                ++s_state.connectCount;
                ui.InvalidateAll();
            }
        }
    });
}

keel::String GenerateInputCode(const DemoContext& ctx) {
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};
    const keel::FixedString<64> sizeInline = hasCustomSize
        ? keel::format_fixed<64>(".size = guikit::UiSize::{}, ", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    keel::String ctrl = keel::format(
        "guikit::InputField(ui, serverEndpoint, {{\n"
        "    .placeholder = \"{}\",\n"
        "    .prefix = \"{}\",\n"
        "    .suffix = \"{}\",\n"
        "    .valueFormat = {{{{\"guikit.dev\", \"api.guikit.dev\"}}}},\n"
        "    .cursorMode = guikit::CursorBlinkMode::{},\n"
        "    .password = {},\n"
        "    .clearable = {},\n"
        "    .readOnly = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .style = guikit::FieldStyle::{},\n"
        "    .status = guikit::UiVariant::{},\n"
        "    .background = {},\n"
        "    .border = {}\n"
        "}});",
        s_state.placeholder,
        s_state.prefix,
        s_state.suffix,
        CursorModeName(s_state.cursorMode),
        s_state.password ? "true" : "false",
        s_state.clearable ? "true" : "false",
        s_state.readOnly ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        ctx.FormatCtrlHeightCode(),
        FieldStyleName(s_state.fieldStyle),
        VariantName(s_state.status),
        ctx.FormatControlBackgroundCode(NavCategory::ElemInputField),
        ctx.FormatCtrlBorderCode()
    );

    if (s_state.companionButton) {
        keel::format_into(
            ctrl,
            "\nif (guikit::Button(ui, {{.text = \"Connect\", .icon = guikit::Icon::Play(), {}.variant = guikit::UiVariant::{}}})) {{\n"
            "    // Trigger endpoint action\n"
            "}}",
            sizeInline,
            VariantName(ctx.btnVariant)
        );
    }

    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
