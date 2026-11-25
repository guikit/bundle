#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

inline constexpr keel::StaticArray<keel::StringRef, 8> kPlainItems = {
    "Dashboard",
    "Projects",
    "Documents",
    "Analytics",
    "Notifications",
    "Integrations",
    "Preferences",
    "Security"
};

struct SelectState {
    keel::usize modeIdx = 0; // 0 = Select, 1 = ComboBox, 2 = Autocomplete
    keel::usize idx = 0;
    keel::SmallString<128> customText{"Dashboard"};
    keel::SmallArray<keel::usize, 8> multiSelected = keel::SmallArray<keel::usize, 8>::from({0, 2, 5});
    keel::SmallString<128> placeholder{"Select or search..."};
    keel::u32 maxItems = 5;
    bool multiSearchable = true;
    bool clearable = true;
    bool richItems = true;
    bool hasPrefix = false;
};

SelectState s_state;

keel::StaticArray<guikit::SelectItem, 8> BuildRichSelectItems() {
    return {{
        {.text = "Dashboard",     .prefix = guikit::Icon::Play(),     .suffix = "Home"},
        {.text = "Projects",      .prefix = guikit::Icon::Folder(),   .suffix = "12"},
        {.text = "Documents",     .prefix = guikit::Icon::Edit(),     .suffix = "Docs"},
        {.text = "Analytics",     .prefix = guikit::Icon::Search(),   .suffix = "Live"},
        {.text = "Notifications", .prefix = guikit::Icon::Flash(),    .suffix = "3 new"},
        {.text = "Integrations",  .prefix = guikit::Icon::Refresh(),  .suffix = "API"},
        {.text = "Preferences",   .prefix = guikit::Icon::Settings(), .suffix = "App"},
        {.text = "Security",      .prefix = guikit::Icon::Check(),    .suffix = "2FA"}
    }};
}

void RenderSharedSelectInspector(guikit::Context& ui) {
    if (auto el = guikit::Element(ui, {.label = "Placeholder"})) {
        if (guikit::InputField(ui, s_state.placeholder, {
            .placeholder = "Select...",
            .clearable = true
        }).changed) ui.InvalidateAll();
    }
    keel::f32 maxItemsFloat = keel::as<keel::f32>(s_state.maxItems);
    if (auto el = guikit::Element(ui, {.label = "Max Visible Menu Items"})) {
        if (guikit::Slider(ui, maxItemsFloat, 2.0f, 8.0f, {.valueFormat = "%.0f items", .step = 1.0f}).changed) {
            s_state.maxItems = keel::as<keel::u32>(maxItemsFloat);
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Clearable (X Button)"})) {
        if (guikit::Switch(ui, s_state.clearable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Rich Items (Icons & Hints)"})) {
        if (guikit::Switch(ui, s_state.richItems).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Default Prefix Icon"})) {
        if (guikit::Switch(ui, s_state.hasPrefix).changed) ui.InvalidateAll();
    }
}

guikit::SelectOptions BuildBaseSelectOptions(const DemoContext& ctx, NavCategory cat) noexcept {
    guikit::SelectOptions opts{};
    opts.placeholder = s_state.placeholder;
    opts.prefix = s_state.hasPrefix ? guikit::Icon::Search() : guikit::Icon{};
    opts.maxVisibleItems = s_state.maxItems;
    opts.align = ctx.GetCtrlAlign(cat);
    opts.clearable = s_state.clearable;
    opts.disabled = ctx.elemDisabled;
    opts.size = ctx.controlsSize;
    opts.width = ctx.ResolveCtrlWidth();
    opts.height = ctx.ResolveCtrlHeight();
    opts.style = ctx.ctrlFieldStyle;
    opts.status = ctx.ctrlFieldStatus;
    opts.background = ctx.GetControlBackground(cat);
    opts.border = ctx.GetCtrlBorder();
    return opts;
}

} // namespace

// ============================================================================
// 1. Select / ComboBox / Autocomplete
// ============================================================================

void RenderSelectInspector(guikit::Context& ui, DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 3> kModeNames = {
        "Select", "ComboBox", "Autocomplete"
    };
    if (auto el = guikit::Element(ui, {.label = "Mode"})) {
        if (guikit::Segmented(ui, s_state.modeIdx, kModeNames).changed) {
            ui.InvalidateAll();
        }
    }
    RenderSharedSelectInspector(ui);
}

void RenderSelectStage(guikit::Context& ui, DemoContext& ctx) {
    ctx.RunElementStage(ui, [&]() {
        const guikit::SelectOptions opts = BuildBaseSelectOptions(ctx, NavCategory::FieldSelect);
        const keel::usize mode = s_state.modeIdx;
        if (mode == 2u && s_state.customText.empty() && s_state.idx < kPlainItems.size()) {
            s_state.customText.assign(kPlainItems[s_state.idx]);
        }
        const auto richList = BuildRichSelectItems();
        if (mode == 0u) {
            if (s_state.richItems) {
                guikit::Select(ui, s_state.idx, richList, opts);
            } else {
                guikit::Select(ui, s_state.idx, kPlainItems, opts);
            }
        } else if (mode == 1u) {
            if (s_state.richItems) {
                guikit::ComboBox(ui, s_state.idx, richList, opts);
            } else {
                guikit::ComboBox(ui, s_state.idx, kPlainItems, opts);
            }
        } else {
            if (s_state.richItems) {
                guikit::Autocomplete(ui, s_state.customText, richList, opts);
            } else {
                guikit::Autocomplete(ui, s_state.customText, kPlainItems, opts);
            }
        }
    });
}

keel::String GenerateSelectCode(const DemoContext& ctx) {
    const keel::usize mode = s_state.modeIdx;
    const keel::StringRef widgetCall = (mode == 0u)
        ? "Select(ui, presetIndex"
        : ((mode == 1u) ? "ComboBox(ui, presetIndex" : "Autocomplete(ui, customText");
    const guikit::Align ctrlAlign = ctx.GetCtrlAlign(NavCategory::FieldSelect);
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    const keel::String ctrl = keel::format(
        "guikit::{}, itemsList, {{\n"
        "    .placeholder = \"{}\",\n"
        "    .prefix = {},\n"
        "    .maxVisibleItems = {},\n"
        "    .align = guikit::Align::{},\n"
        "    .clearable = {},\n"
        "    .disabled = {},\n"
        "{}"
        "    .width = {},\n"
        "    .height = {},\n"
        "    .style = guikit::FieldStyle::{},\n"
        "    .status = guikit::UiVariant::{},\n"
        "    .background = {},\n"
        "    .border = {}\n"
        "}});",
        widgetCall,
        s_state.placeholder,
        s_state.hasPrefix ? "guikit::Icon::Search()" : "{}",
        s_state.maxItems,
        AlignName(ctrlAlign),
        s_state.clearable ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        ctx.FormatCtrlHeightCode(),
        FieldStyleEnumName(ctx.ctrlFieldStyle),
        VariantName(ctx.ctrlFieldStatus),
        ctx.FormatControlBackgroundCode(NavCategory::FieldSelect),
        ctx.FormatCtrlBorderCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

// ============================================================================
// 2. MultiSelect
// ============================================================================

void RenderMultiSelectInspector(guikit::Context& ui, DemoContext&) {
    if (auto el = guikit::Element(ui, {.label = "Searchable Filter"})) {
        if (guikit::Switch(ui, s_state.multiSearchable).changed) ui.InvalidateAll();
    }
    RenderSharedSelectInspector(ui);
}

void RenderMultiSelectStage(guikit::Context& ui, DemoContext& ctx) {
    ctx.RunElementStage(ui, [&]() {
        guikit::SelectOptions opts = BuildBaseSelectOptions(ctx, NavCategory::FieldMultiSelect);
        opts.searchable = s_state.multiSearchable;
        const auto richList = BuildRichSelectItems();
        if (s_state.richItems) {
            guikit::MultiSelect(ui, s_state.multiSelected, richList, opts);
        } else {
            guikit::MultiSelect(ui, s_state.multiSelected, kPlainItems, opts);
        }
    });
}

keel::String GenerateMultiSelectCode(const DemoContext& ctx) {
    const guikit::Align ctrlAlign = ctx.GetCtrlAlign(NavCategory::FieldMultiSelect);
    const bool hasCustomSize = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md);
    const keel::FixedString<64> sizeLine = hasCustomSize
        ? keel::format_fixed<64>("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::FixedString<64>{};

    const keel::String ctrl = keel::format(
        "guikit::MultiSelect(ui, selectedIndices, itemsList, {{\n"
        "    .placeholder = \"{}\",\n"
        "    .prefix = {},\n"
        "    .maxVisibleItems = {},\n"
        "    .align = guikit::Align::{},\n"
        "    .searchable = {},\n"
        "    .clearable = {},\n"
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
        s_state.hasPrefix ? "guikit::Icon::Search()" : "{}",
        s_state.maxItems,
        AlignName(ctrlAlign),
        s_state.multiSearchable ? "true" : "false",
        s_state.clearable ? "true" : "false",
        ctx.elemDisabled ? "true" : "false",
        sizeLine,
        ctx.FormatCtrlWidthCode(),
        ctx.FormatCtrlHeightCode(),
        FieldStyleEnumName(ctx.ctrlFieldStyle),
        VariantName(ctx.ctrlFieldStatus),
        ctx.FormatControlBackgroundCode(NavCategory::FieldMultiSelect),
        ctx.FormatCtrlBorderCode()
    );
    return ctx.WrapInElementCode(ctrl);
}

} // namespace guikit::demo
