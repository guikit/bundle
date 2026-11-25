#pragma once

#include "guikit/guikit.hpp"
#include "keel/keel.h"
#include "style/theme_studio_state.hpp"

namespace guikit::demo {

enum class NavCategory : keel::u8 {
    // 1. STYLE (Design System)
    StyleThemeStudio = 0,

    // 2. LAYOUT (Geometry & Slot Framing)
    LayoutFlex,
    LayoutWrap,
    LayoutGrid,
    LayoutScrollArea,
    LayoutSplitView,
    LayoutElement,

    // 3. SURFACES (Containers & Application Shell)
    SurfaceCard,
    SurfaceMenuBar,
    SurfaceToolbar,
    SurfaceSidebar,

    // 4. OVERLAYS (Transient Floating Layers)
    OverlayModal,
    OverlayPopover,
    OverlayContextMenu,
    OverlayToast,
    OverlayCommandPalette,
    OverlayTooltip,

    // 5. ACTIONS (Buttons & Navigation Triggers)
    ActionButton,
    ActionMenuButton,
    ActionSplitButton,
    ActionSegmented,
    ActionTabBar,
    ActionBreadcrumb,
    ActionPagination,

    // 6. FIELDS (Unified Editable Fields on FieldFrame)
    FieldInputField,
    FieldSelect,
    FieldMultiSelect,
    FieldSpinBox,
    FieldDatePicker,
    FieldTagInput,
    FieldColorPicker,

    // 7. CONTROLS (Direct Value Controls)
    ControlCheckbox,
    ControlRadio,
    ControlSwitch,
    ControlSlider,
    ControlStepSlider,
    ControlRangeSlider,
    ControlKnob,

    // 8. DISPLAY (Typography, Indicators & Media)
    DisplayHeading,
    DisplayTag,
    DisplayProgressBar,
    DisplayLevelMeter,
    DisplayReadout,
    DisplayImage,

    // 9. VIEWS (Rich Data & Vector Viewports)
    ViewTable,
    ViewTreeView,
    ViewMarkdown,
    ViewCodeEditor,
    ViewImageView,
    ViewSchematic2D
};

inline constexpr keel::usize kNavCategoryCount = keel::as<keel::usize>(NavCategory::ViewSchematic2D) + 1u;

constexpr bool IsControlCategory(NavCategory cat) noexcept {
    switch (cat) {
        case NavCategory::ActionButton:
        case NavCategory::ActionMenuButton:
        case NavCategory::ActionSplitButton:
        case NavCategory::ActionSegmented:
        case NavCategory::FieldInputField:
        case NavCategory::FieldSelect:
        case NavCategory::FieldMultiSelect:
        case NavCategory::FieldSpinBox:
        case NavCategory::FieldDatePicker:
        case NavCategory::FieldTagInput:
        case NavCategory::FieldColorPicker:
        case NavCategory::ControlCheckbox:
        case NavCategory::ControlRadio:
        case NavCategory::ControlSwitch:
        case NavCategory::ControlSlider:
        case NavCategory::ControlStepSlider:
        case NavCategory::ControlRangeSlider:
        case NavCategory::ControlKnob:
        case NavCategory::DisplayTag:
        case NavCategory::DisplayProgressBar:
        case NavCategory::DisplayLevelMeter:
        case NavCategory::DisplayReadout:
        case NavCategory::DisplayImage:
            return true;
        default:
            return false;
    }
}

constexpr bool IsFieldCategory(NavCategory cat) noexcept {
    switch (cat) {
        case NavCategory::FieldInputField:
        case NavCategory::FieldSelect:
        case NavCategory::FieldMultiSelect:
        case NavCategory::FieldSpinBox:
        case NavCategory::FieldDatePicker:
        case NavCategory::FieldTagInput:
        case NavCategory::FieldColorPicker:
            return true;
        default:
            return false;
    }
}

constexpr bool HasElementWrapperCategory(NavCategory cat) noexcept {
    switch (cat) {
        case NavCategory::FieldInputField:
        case NavCategory::FieldSelect:
        case NavCategory::FieldMultiSelect:
        case NavCategory::FieldSpinBox:
        case NavCategory::FieldDatePicker:
        case NavCategory::FieldTagInput:
        case NavCategory::FieldColorPicker:
        case NavCategory::ControlCheckbox:
        case NavCategory::ControlRadio:
        case NavCategory::ControlSwitch:
        case NavCategory::ControlSlider:
        case NavCategory::ControlStepSlider:
        case NavCategory::ControlRangeSlider:
        case NavCategory::ControlKnob:
            return true;
        default:
            return false;
    }
}

enum class ElementWrapperPreset : keel::u8 {
    None = 0,            ///< Standalone control without guikit::Element wrapper
    LabelOnly,           ///< Minimal form field: .label only
    LabelAndDescription, ///< Standard setting row: .label + .description
    Full,                ///< Rich field: .icon + .label + .badge + .description
    InlineLeading,       ///< Horizontal row: label leading, control trailing
    InlineTrailing       ///< Horizontal row: control leading, label trailing
};

inline constexpr keel::StaticArray<keel::StringRef, 6> kElementWrapperPresetNames = {
    "None (Standalone)",
    "Label Only",
    "Label + Description",
    "Full (Icon + Badge)",
    "Inline (Leading)",
    "Inline (Trailing)"
};

constexpr bool IsSandboxCategory(NavCategory cat) noexcept {
    return keel::as<keel::usize>(cat) < kNavCategoryCount;
}

constexpr bool IsFullStageCategory(NavCategory cat) noexcept {
    return cat == NavCategory::StyleThemeStudio ||
           cat == NavCategory::LayoutFlex ||
           cat == NavCategory::LayoutWrap ||
           cat == NavCategory::LayoutGrid ||
           cat == NavCategory::LayoutScrollArea ||
           cat == NavCategory::LayoutSplitView ||
           cat == NavCategory::SurfaceSidebar ||
           cat == NavCategory::SurfaceMenuBar ||
           cat == NavCategory::ViewTable ||
           cat == NavCategory::ViewTreeView ||
           cat == NavCategory::ViewMarkdown ||
           cat == NavCategory::ViewCodeEditor ||
           cat == NavCategory::ViewImageView ||
           cat == NavCategory::ViewSchematic2D;
}

enum class PreviewFormFactor : keel::u8 {
    Window = 0,      ///< Desktop OS Window with titlebar (traffic lights, title, W x H badge)
    TabletLandscape, ///< Tablet / Touch Panel bezel in Landscape orientation
    TabletPortrait,  ///< Tablet / Touch Panel bezel in Portrait orientation
    PhonePortrait,   ///< Smartphone / Handheld bezel in Portrait orientation
    PhoneLandscape,  ///< Smartphone / Handheld bezel in Landscape orientation
    Full             ///< Clean edge-to-edge artboard on the recessed backdrop
};

inline constexpr keel::StaticArray<keel::StringRef, 6> kPreviewFormFactorNames = {
    "Window",
    "Tablet (Landscape)",
    "Tablet (Portrait)",
    "Phone (Portrait)",
    "Phone (Landscape)",
    "Full"
};

enum class PreviewZoomMode : keel::u8 {
    Fit = 0,     ///< Auto-fit device to available canvas (or 100% in Window/Full)
    Real1To1,    ///< 1:1 physical millimeter match using monitor EDID / DPI
    Zoom75,      ///< 75%
    Zoom100,     ///< 100%
    Zoom125,     ///< 125%
    Zoom150,     ///< 150%
    Zoom175,     ///< 175%
    Zoom200,     ///< 200%
    Custom       ///< Custom zoom factor driven by Ctrl + MouseWheel
};

inline constexpr keel::StaticArray<keel::StringRef, 8> kPreviewZoomPresetNames = {
    "Fit",
    "1:1",
    "75%",
    "100%",
    "125%",
    "150%",
    "175%",
    "200%"
};

constexpr keel::StringRef CategoryDisplayTitle(NavCategory cat) noexcept {
    switch (cat) {
        // 1. STYLE
        case NavCategory::StyleThemeStudio:   return "Theme Studio — Palette × Chrome × Fill & Tokens";
        // 2. LAYOUT
        case NavCategory::LayoutFlex:         return "Row & Column — Flex Layout & Elastic Spacer";
        case NavCategory::LayoutWrap:         return "Wrap — Responsive Flow-Wrapping Layout";
        case NavCategory::LayoutGrid:         return "Grid — 12-Column Responsive Track Grid";
        case NavCategory::LayoutScrollArea:   return "ScrollArea — 1D & 2D Scrollable Viewport";
        case NavCategory::LayoutSplitView:    return "SplitView — Resizable Dual-Pane Sash";
        case NavCategory::LayoutElement:      return "Element — Form-Field Wrapper & Slot Anatomy";
        // 3. SURFACES
        case NavCategory::SurfaceCard:        return "Card — Surface Container & Elevation";
        case NavCategory::SurfaceMenuBar:     return "MenuBar — Desktop Menu Bar & Cascading Submenus";
        case NavCategory::SurfaceToolbar:     return "Toolbar — Command Strip & Tool Palettes";
        case NavCategory::SurfaceSidebar:     return "Sidebar — Application Workbench Shell";
        // 4. OVERLAYS
        case NavCategory::OverlayModal:          return "Modal — Dialogs, Backdrops & Focus Trap";
        case NavCategory::OverlayPopover:        return "Popover — Anchored Non-Modal Floating Card";
        case NavCategory::OverlayContextMenu:    return "ContextMenu — Right-Click & Action Menus";
        case NavCategory::OverlayToast:          return "Toast — Stacked Notifications & Timers";
        case NavCategory::OverlayCommandPalette: return "CommandPalette — Fuzzy Search Command Launcher";
        case NavCategory::OverlayTooltip:        return "Tooltip — Hover Hints, Rich Text & Cursor Tracking";
        // 5. ACTIONS
        case NavCategory::ActionButton:       return "Button — Interactive Push Control";
        case NavCategory::ActionMenuButton:   return "MenuButton — Autonomous Dropdown Action Button";
        case NavCategory::ActionSplitButton:  return "SplitButton — Primary Action + Dropdown Menu";
        case NavCategory::ActionSegmented:    return "Segmented — Mutually Exclusive Pill Bar";
        case NavCategory::ActionTabBar:       return "TabBar — Scrollable & Closable Tabs";
        case NavCategory::ActionBreadcrumb:   return "Breadcrumb — Hierarchical Path Trail";
        case NavCategory::ActionPagination:   return "Pagination — Page Navigation & Ellipsis";
        // 6. FIELDS
        case NavCategory::FieldInputField:    return "InputField — Text & Validated Input";
        case NavCategory::FieldSelect:        return "Select — Dropdown, ComboBox & Autocomplete";
        case NavCategory::FieldMultiSelect:   return "MultiSelect — Multi-Chip Tag Dropdown";
        case NavCategory::FieldSpinBox:       return "SpinBox — Precision Numeric Stepper";
        case NavCategory::FieldDatePicker:    return "DatePicker — Calendar Grid & Time Stepper";
        case NavCategory::FieldTagInput:      return "TagInput — Editable Multi-Tag Chips";
        case NavCategory::FieldColorPicker:   return "ColorPicker — 2D HSV Canvas & HEX/RGBA";
        // 7. CONTROLS
        case NavCategory::ControlCheckbox:    return "Checkbox — Tri-State Boolean Control";
        case NavCategory::ControlRadio:       return "Radio — Exclusive Option Group";
        case NavCategory::ControlSwitch:      return "Switch — Toggle & Symmetric Dual-Label";
        case NavCategory::ControlSlider:      return "Slider — Continuous Numeric Track";
        case NavCategory::ControlStepSlider:  return "StepSlider — Discrete Labeled Steps";
        case NavCategory::ControlRangeSlider: return "RangeSlider — Dual-Thumb Min/Max Interval";
        case NavCategory::ControlKnob:        return "Knob — Rotary Parameter Control";
        // 8. DISPLAY
        case NavCategory::DisplayHeading:     return "Heading — Section & Title Typography";
        case NavCategory::DisplayTag:         return "Tag — Status Badges & Clickable Chips";
        case NavCategory::DisplayProgressBar: return "ProgressBar — Smooth, Segmented & Indeterminate";
        case NavCategory::DisplayLevelMeter:  return "LevelMeter — Segmented Audio/DSP VU Meter";
        case NavCategory::DisplayReadout:     return "Readout — Interactive Telemetry Channel Display";
        case NavCategory::DisplayImage:       return "Image — Vector SVG & Raster Display";
        // 9. VIEWS
        case NavCategory::ViewTable:          return "Table — 100k Virtualized Data Grid";
        case NavCategory::ViewTreeView:       return "TreeView — Hierarchical Node Explorer";
        case NavCategory::ViewMarkdown:       return "Markdown — Rich Document Renderer";
        case NavCategory::ViewCodeEditor:     return "Code Editor (TextArea) — Syntax Highlighting, Minimap & Debugger";
        case NavCategory::ViewImageView:      return "Image View — 16-bit Scientific Inspector";
        case NavCategory::ViewSchematic2D:    return "Schematic 2D — Node Graph Canvas";
    }
    return "GuiKit Preview";
}

constexpr keel::StringRef CategoryControlName(NavCategory cat) noexcept {
    switch (cat) {
        case NavCategory::StyleThemeStudio:   return "Theme Studio";
        case NavCategory::LayoutElement:      return "Element";
        case NavCategory::ActionButton:       return "Button";
        case NavCategory::ActionSegmented:    return "Segmented";
        case NavCategory::FieldInputField:    return "InputField";
        case NavCategory::FieldSelect:        return "Select";
        case NavCategory::FieldMultiSelect:   return "MultiSelect";
        case NavCategory::FieldSpinBox:       return "SpinBox";
        case NavCategory::FieldDatePicker:    return "DatePicker";
        case NavCategory::FieldTagInput:      return "TagInput";
        case NavCategory::FieldColorPicker:   return "ColorPicker";
        case NavCategory::ViewCodeEditor:     return "Code Editor";
        case NavCategory::ControlCheckbox:    return "Checkbox";
        case NavCategory::ControlRadio:       return "Radio";
        case NavCategory::ControlSwitch:      return "Switch";
        case NavCategory::ControlSlider:      return "Slider";
        case NavCategory::ControlStepSlider:  return "StepSlider";
        case NavCategory::ControlRangeSlider: return "RangeSlider";
        case NavCategory::ControlKnob:        return "Knob";
        case NavCategory::DisplayHeading:     return "Heading";
        case NavCategory::DisplayTag:         return "Tag";
        case NavCategory::DisplayProgressBar: return "ProgressBar";
        case NavCategory::DisplayLevelMeter:  return "LevelMeter";
        case NavCategory::DisplayReadout:     return "Readout";
        case NavCategory::DisplayImage:       return "Image";
        default:                              return "Control";
    }
}

inline constexpr keel::StaticArray<keel::StringRef, 3> kLabelAlignNames = {"Start", "Center", "End"};

inline constexpr keel::StaticArray<keel::StringRef, 3> kFieldStyleNames = {
    "Default", "Filled", "Borderless"
};

inline constexpr keel::StaticArray<keel::StringRef, 6> kFieldStatusNames = {
    "Default", "Primary", "Success", "Warning", "Danger", "Info"
};

inline constexpr keel::StaticArray<guikit::UiVariant, 6> kFieldStatusValues = {
    guikit::UiVariant::Default,
    guikit::UiVariant::Primary,
    guikit::UiVariant::Success,
    guikit::UiVariant::Warning,
    guikit::UiVariant::Danger,
    guikit::UiVariant::Info
};

constexpr keel::StringRef FieldStyleEnumName(guikit::FieldStyle s) noexcept {
    switch (s) {
        case guikit::FieldStyle::Default:    return "Default";
        case guikit::FieldStyle::Filled:     return "Filled";
        case guikit::FieldStyle::Borderless: return "Borderless";
    }
    return "Default";
}

inline constexpr keel::StaticArray<keel::StringRef, 8> kUiVariantNames = {
    "Default", "Primary", "Secondary", "Success", "Warning", "Danger", "Info", "Ghost"
};

inline constexpr keel::StaticArray<keel::StringRef, 4> kAxisModeNames = {
    "Auto", "Fit", "Stretch", "Fixed"
};

inline constexpr keel::StaticArray<keel::StringRef, 6> kElemLayoutNames = {
    "Auto", "Stacked", "Top", "Bottom", "Leading", "Trailing"
};

constexpr keel::StringRef SizeModeEnumName(guikit::SizeMode m) noexcept {
    switch (m) {
        case guikit::SizeMode::Fit:     return "Fit";
        case guikit::SizeMode::Fixed:   return "Fixed";
        case guikit::SizeMode::Stretch: return "Stretch";
    }
    return "Stretch";
}

constexpr keel::StringRef ElementLayoutEnumName(guikit::ElementLayout l) noexcept {
    switch (l) {
        case guikit::ElementLayout::Stacked:  return "Stacked";
        case guikit::ElementLayout::Top:      return "Top";
        case guikit::ElementLayout::Bottom:   return "Bottom";
        case guikit::ElementLayout::Leading:  return "Leading";
        case guikit::ElementLayout::Trailing: return "Trailing";
    }
    return "Stacked";
}

constexpr keel::StringRef HeadingLayoutEnumName(guikit::HeadingLayout l) noexcept {
    switch (l) {
        case guikit::HeadingLayout::Stacked:  return "Stacked";
        case guikit::HeadingLayout::Overline: return "Overline";
        case guikit::HeadingLayout::Inline:   return "Inline";
    }
    return "Stacked";
}

constexpr keel::StringRef LabelAlignEnumName(guikit::Align a) noexcept {
    switch (a) {
        case guikit::Align::Start:   return "Start";
        case guikit::Align::Center:  return "Center";
        case guikit::Align::End:     return "End";
        case guikit::Align::Stretch: return "Stretch";
    }
    return "Start";
}

constexpr keel::StringRef AlignName(guikit::Align a) noexcept {
    switch (a) {
        case guikit::Align::Start:   return "Start";
        case guikit::Align::Center:  return "Center";
        case guikit::Align::End:     return "End";
        case guikit::Align::Stretch: return "Stretch";
    }
    return "Center";
}

constexpr keel::StringRef VariantName(guikit::UiVariant v) noexcept {
    switch (v) {
        case guikit::UiVariant::Primary:   return "Primary";
        case guikit::UiVariant::Secondary: return "Secondary";
        case guikit::UiVariant::Success:   return "Success";
        case guikit::UiVariant::Warning:   return "Warning";
        case guikit::UiVariant::Danger:    return "Danger";
        case guikit::UiVariant::Info:      return "Info";
        case guikit::UiVariant::Ghost:     return "Ghost";
        case guikit::UiVariant::Muted:     return "Muted";
        default:                           return "Default";
    }
}

constexpr keel::StringRef ButtonStyleEnumName(guikit::ButtonStyle s) noexcept {
    switch (s) {
        case guikit::ButtonStyle::Default: return "Default";
        case guikit::ButtonStyle::Solid:   return "Solid";
        case guikit::ButtonStyle::Soft:    return "Soft";
        case guikit::ButtonStyle::Outline: return "Outline";
        case guikit::ButtonStyle::Ghost:   return "Ghost";
    }
    return "Default";
}

constexpr keel::StringRef SizeName(guikit::UiSize s) noexcept {
    switch (s.GetPreset()) {
        case guikit::UiSize::Preset::Xs: return "Xs";
        case guikit::UiSize::Preset::Sm: return "Sm";
        case guikit::UiSize::Preset::Md: return "Md";
        case guikit::UiSize::Preset::Lg: return "Lg";
        case guikit::UiSize::Preset::Xl: return "Xl";
        default:                         return "Md";
    }
}

inline keel::FixedString<48> FormatUiSizePrefix(guikit::UiSize s) noexcept {
    if (s == guikit::UiSize::None || s == guikit::UiSize::Md) {
        return {};
    }
    return keel::format_fixed<48>(",\n    .size = guikit::UiSize::{}", SizeName(s));
}

constexpr keel::StringRef PlacementName(guikit::Placement p) noexcept {
    switch (p) {
        case guikit::Placement::Leading:  return "Leading";
        case guikit::Placement::Trailing: return "Trailing";
        case guikit::Placement::Top:      return "Top";
        case guikit::Placement::Bottom:   return "Bottom";
        case guikit::Placement::Thumb:    return "Thumb";
    }
    return "Trailing";
}

constexpr keel::StringRef CursorModeName(guikit::CursorBlinkMode m) noexcept {
    switch (m) {
        case guikit::CursorBlinkMode::Blinking: return "Blinking";
        case guikit::CursorBlinkMode::Solid:    return "Solid";
        case guikit::CursorBlinkMode::Hidden:   return "Hidden";
    }
    return "Blinking";
}

inline keel::String FormatDesc(bool show, keel::StringRef text, guikit::UiVariant v) {
    if (!show) return keel::String::from("\"\"");
    if (v == guikit::UiVariant::Muted) {
        return keel::format("\"{}\"", text);
    }
    return keel::format("{{\"{}\", guikit::StateColor::{}()}}", text, VariantName(v));
}

inline void AppendIndentedLines(keel::String& out, keel::StringRef block, keel::StringRef indent) {
    keel::usize start = 0;
    while (start <= block.size()) {
        keel::usize end = start;
        while (end < block.size() && block[end] != '\n') {
            ++end;
        }
        const keel::StringRef line = block.substr(start, end - start);
        if (!line.empty()) {
            out.append(indent);
            out.append(line);
        }
        out.push_back('\n');
        if (end == block.size()) {
            break;
        }
        start = end + 1;
    }
}

inline constexpr keel::StaticArray<keel::StringRef, 10> kBgPresetNames = {
    "Default (Theme)", "None / Transparent", "Surface", "Primary", "Secondary", "Success", "Warning", "Danger", "Info", "Ghost"
};

constexpr guikit::StateColor BgPresetToStateColor(
    keel::usize idx,
    keel::f32 tint = 1.0f,
    keel::f32 alpha = 1.0f
) noexcept {
    guikit::StateColor res{};
    switch (idx) {
        case 0:  return guikit::StateColor{};
        case 1:  return guikit::StateColor::None();
        case 2:  res = guikit::StateColor::Surface(); break;
        case 3:  res = guikit::StateColor::Primary(); break;
        case 4:  res = guikit::StateColor::Secondary(); break;
        case 5:  res = guikit::StateColor::Success(); break;
        case 6:  res = guikit::StateColor::Warning(); break;
        case 7:  res = guikit::StateColor::Danger(); break;
        case 8:  res = guikit::StateColor::Info(); break;
        case 9:  res = guikit::StateColor::Ghost(); break;
        default: return guikit::StateColor{};
    }
    if (idx >= 3 && idx <= 8 && tint < 0.995f) {
        res = res.Tinted(tint);
    }
    if (alpha < 0.995f) {
        res = res.WithAlphaF(alpha);
    }
    return res;
}

inline keel::String BgPresetToCode(
    keel::usize idx,
    keel::f32 tint = 1.0f,
    keel::f32 alpha = 1.0f
) {
    keel::String code;
    switch (idx) {
        case 0:  return keel::String::from("guikit::StateColor{}");
        case 1:  return keel::String::from("guikit::StateColor::None()");
        case 2:  code = keel::String::from("guikit::StateColor::Surface()"); break;
        case 3:  code = keel::String::from("guikit::StateColor::Primary()"); break;
        case 4:  code = keel::String::from("guikit::StateColor::Secondary()"); break;
        case 5:  code = keel::String::from("guikit::StateColor::Success()"); break;
        case 6:  code = keel::String::from("guikit::StateColor::Warning()"); break;
        case 7:  code = keel::String::from("guikit::StateColor::Danger()"); break;
        case 8:  code = keel::String::from("guikit::StateColor::Info()"); break;
        case 9:  code = keel::String::from("guikit::StateColor::Ghost()"); break;
        default: return keel::String::from("guikit::StateColor{}");
    }
    if (idx >= 3 && idx <= 8 && tint < 0.995f) {
        keel::format_into(code, ".Tinted({:.2f}f)", tint);
    }
    if (alpha < 0.995f) {
        keel::format_into(code, ".WithAlphaF({:.2f}f)", alpha);
    }
    return code;
}

struct DemoContext {
    bool shellLightMode = false;
    bool shellFloating = false;
    bool previewDarkMode = true;
    bool previewLightMode = false;
    bool previewBounds = false;
    bool previewInspect = false;
    bool previewShowCode = true;
    keel::usize previewFormFactorIdx = 0; // PreviewFormFactor index (0 = Window .. 5 = Full)
    keel::usize previewZoomModeIdx = 3;   // 0=Fit, 1=Real 1:1, 2=75%, 3=100%, 4=125%, 5=150%, 6=175%, 7=200%, 8=Custom
    keel::f32 previewCustomZoom = 1.0f;       // Custom zoom factor set via Ctrl + MouseWheel
    keel::f32 previewEffectiveZoom = 1.0f;    // Resolved effective zoom factor for current frame
    keel::f32 displayPxPerMm = 96.0f / 25.4f; // Physical monitor density (px/mm) for Real 1:1 mode
    bool previewWindowCustomSize = false; // false = Maximized (Stretch), true = user-resized WxH (dp)
    keel::f32 previewWindowW = 760.0f;        // User-resized window width in logical dp
    keel::f32 previewWindowH = 500.0f;        // User-resized window height in logical dp
    keel::u8 previewWindowResizeEdge = 0; // Active resize edge bitmask (1=Left, 2=Right, 4=Bottom)
    keel::u8 previewWindowHoverEdge = 0;  // Hovered resize edge bitmask (1=Left, 2=Right, 4=Bottom)
    guikit::Vec2 previewWindowDragStartMouse{0.0f, 0.0f};
    keel::f32 previewWindowDragStartW = 0.0f;
    keel::f32 previewWindowDragStartH = 0.0f;

    ThemeStudioState studio{};
    guikit::UiTheme shellTheme = guikit::UiTheme::Dark();
    guikit::UiTheme previewTheme = guikit::UiTheme::Dark();
    keel::Optional<guikit::UiTheme> pendingShellTheme = keel::kNullOpt;
    keel::f32 previewDensity = 1.0f;

    void RebuildPreviewTheme() noexcept {
        previewTheme = studio.BuildEffectiveTheme();
        previewLightMode = (previewTheme.mode == guikit::ThemeMode::Light);
        previewDarkMode = !previewLightMode;
    }

    void SelectPreviewPreset(guikit::ThemePreset preset) noexcept {
        studio.SelectBasePreset(preset);
        RebuildPreviewTheme();
    }

    void SelectPreviewChrome(guikit::ChromeStyle chrome) noexcept {
        studio.chrome = chrome;
        RebuildPreviewTheme();
    }

    void SelectPreviewFillStyle(guikit::FillStyle fill) noexcept {
        studio.fillStyle = fill;
        RebuildPreviewTheme();
    }

    void TogglePreviewDarkLight() noexcept {
        const bool nextLight = (previewTheme.mode == guikit::ThemeMode::Dark);
        SelectPreviewPreset(nextLight ? guikit::ThemePreset::Light : guikit::ThemePreset::Dark);
    }

    // Semantic Element form-field wrapper state (shared across ELEMENTS screens, excluding ElemHeading)
    // Shared Element wrapper preset (Fields & Controls) + Control disabled state
    ElementWrapperPreset elemPreset = ElementWrapperPreset::LabelOnly;
    bool elemDisabled = false;

    // Shared Control state
    bool ctrlLayoutOpen = false;
    guikit::UiSize controlsSize = guikit::UiSize::Md;
    guikit::UiVariant btnVariant = guikit::UiVariant::Primary;
    guikit::FieldStyle ctrlFieldStyle = guikit::FieldStyle::Default;
    guikit::UiVariant ctrlFieldStatus = guikit::UiVariant::Default;
    keel::usize ctrlWidthModeIdx = 0;   // 0 = Auto, 1 = Fit, 2 = Stretch, 3 = Fixed
    keel::f32 ctrlWidthDp = 220.0f;         // 80..480 dp
    keel::usize ctrlHeightModeIdx = 0;  // 0 = Auto, 1 = Fit, 2 = Stretch, 3 = Fixed
    keel::f32 ctrlHeightDp = 44.0f;         // 24..320 dp
    bool knobCustomDiameter = false;    // Optional explicit diameter override for Knob
    keel::f32 knobDiameterDp = 64.0f;       // 28..140 dp
    guikit::Align ctrlAlign = guikit::Align::Center;      // Capsule content alignment for Button
    guikit::Align ctrlSelectAlign = guikit::Align::Start; // Content alignment for Select
    keel::StaticArray<keel::usize, kNavCategoryCount> ctrlBgIdxByCat{}; // 0 = Default (Theme) for all controls
    keel::StaticArray<keel::usize, kNavCategoryCount> demoVariantIdx{}; // Per-screen Demo Variant index (0 = default)
    keel::f32 ctrlBgTint = 1.0f;
    keel::f32 ctrlBgAlpha = 1.0f;
    keel::f32 ctrlBorderWidth = 0.0f;
    guikit::UiVariant ctrlBorderVariant = guikit::UiVariant::Default;
    keel::f32 ctrlBorderRadius = -1.0f; // < 0 = Auto (inherit active theme & chrome radius)

    // Shared assembly & container layout tokens
    keel::f32 cardRadius = -1.0f; // < 0 = Auto (inherit active theme & chrome radius)
    keel::f32 cardPadding = 16.0f;
    keel::f32 cardGap = 8.0f;
    keel::f32 cardBorderWidth = 1.0f;
    keel::f32 gridColGap = 12.0f;
    keel::f32 gridRowGap = 10.0f;
    keel::f32 colGap = 12.0f;
    keel::f32 rowGap = 8.0f;

    static keel::ArrayRef<keel::StringRef> DemoVariantsFor(NavCategory cat) noexcept {
        static constexpr keel::StaticArray<keel::StringRef, 3> kFlex = {"Row", "Column", "Row + Spacer"};
        static constexpr keel::StaticArray<keel::StringRef, 2> kGrid = {"12-Col Bento", "Equal Columns"};
        static constexpr keel::StaticArray<keel::StringRef, 2> kSplitView = {"3-Pane Nested", "2-Pane Single"};
        static constexpr keel::StaticArray<keel::StringRef, 4> kElement = {
            "InputField (Stretch)", "Switch (Compact)", "Composite Row", "Standalone (No Slot)"
        };
        static constexpr keel::StaticArray<keel::StringRef, 3> kTooltip = {
            "Widget (.tooltip)", "Anchored (Rect)", "Follow Cursor"
        };
        static constexpr keel::StaticArray<keel::StringRef, 2> kLevelMeter = {
            "Stereo (L/R)", "Mono (1ch)"
        };
        static constexpr keel::StaticArray<keel::StringRef, 2> kTable = {
            "Rich DataGrid", "100k Virtual Log"
        };
        static constexpr keel::StaticArray<keel::StringRef, 2> kImageView = {
            "16-bit Monochrome", "RGB Multi-Channel"
        };
        switch (cat) {
            case NavCategory::LayoutFlex:        return kFlex;
            case NavCategory::LayoutGrid:        return kGrid;
            case NavCategory::LayoutSplitView:   return kSplitView;
            case NavCategory::LayoutElement:     return kElement;
            case NavCategory::OverlayTooltip:    return kTooltip;
            case NavCategory::DisplayLevelMeter: return kLevelMeter;
            case NavCategory::ViewTable:         return kTable;
            case NavCategory::ViewImageView:     return kImageView;
            default:                             return {};
        }
    }

    keel::usize GetDemoVariant(NavCategory cat) const noexcept {
        const keel::usize ci = keel::as<keel::usize>(cat);
        if (ci >= demoVariantIdx.size()) return 0u;
        const auto variants = DemoVariantsFor(cat);
        if (variants.empty()) return 0u;
        return keel::min(demoVariantIdx[ci], variants.size() - 1u);
    }

    bool RenderDemoVariantSelector(guikit::Context& ui, NavCategory cat) {
        const auto variants = DemoVariantsFor(cat);
        if (variants.size() < 2u) {
            return false;
        }
        const keel::usize ci = keel::as<keel::usize>(cat);
        if (demoVariantIdx[ci] >= variants.size()) {
            demoVariantIdx[ci] = 0u;
        }
        bool changed = false;
        if (auto el = guikit::Element(ui, {
            .label = "Demo Variant",
            .layout = guikit::ElementLayout::Top
        })) {
            if (guikit::Select(ui, demoVariantIdx[ci], variants).changed) {
                changed = true;
                ui.InvalidateAll();
            }
        }
        guikit::Separator(ui);
        return changed;
    }

    bool RenderElementPresetSelector(guikit::Context& ui, NavCategory cat) {
        if (!HasElementWrapperCategory(cat)) {
            return false;
        }
        bool changed = false;
        if (auto el = guikit::Element(ui, {
            .label = "Element",
            .layout = guikit::ElementLayout::Top
        })) {
            if (guikit::Select(ui, elemPreset, kElementWrapperPresetNames).changed) {
                changed = true;
                ui.InvalidateAll();
            }
        }
        guikit::Separator(ui);
        return changed;
    }

    static constexpr guikit::SizeMode DefaultCtrlWidthMode(NavCategory cat) noexcept {
        switch (cat) {
            case NavCategory::ActionButton:
            case NavCategory::ControlKnob:
            case NavCategory::ControlSwitch:
            case NavCategory::ControlCheckbox:
            case NavCategory::ControlRadio:
            case NavCategory::DisplayImage:
                return guikit::SizeMode::Fit;
            default:
                return guikit::SizeMode::Stretch;
        }
    }

    static constexpr bool HasCtrlLayoutCategory(NavCategory cat) noexcept {
        return cat != NavCategory::ControlSwitch &&
               cat != NavCategory::ControlCheckbox &&
               cat != NavCategory::ControlRadio;
    }

    static constexpr bool IsCtrlBoxedCategory(NavCategory cat) noexcept {
        return cat == NavCategory::ActionButton ||
               cat == NavCategory::ActionSegmented ||
               cat == NavCategory::FieldSpinBox ||
               cat == NavCategory::FieldInputField ||
               cat == NavCategory::FieldTagInput ||
               cat == NavCategory::FieldSelect ||
               cat == NavCategory::FieldMultiSelect ||
               cat == NavCategory::FieldColorPicker ||
               cat == NavCategory::FieldDatePicker ||
               cat == NavCategory::DisplayImage;
    }

    static constexpr guikit::ElementLayout DefaultElemLayout(NavCategory cat) noexcept {
        switch (cat) {
            case NavCategory::ControlSwitch:
            case NavCategory::ControlCheckbox:
            case NavCategory::ControlRadio:
                return guikit::ElementLayout::Trailing;
            case NavCategory::ControlKnob:
                return guikit::ElementLayout::Leading;
            default:
                return guikit::ElementLayout::Stacked;
        }
    }

    constexpr bool HasElementWrapper() const noexcept {
        return elemPreset != ElementWrapperPreset::None;
    }

    constexpr guikit::StateColor ResolveControlColor() const noexcept {
        return guikit::StateColor::FromVariant(btnVariant);
    }

    constexpr guikit::StateColor GetControlColor() const noexcept {
        return guikit::StateColor::FromVariant(btnVariant);
    }

    constexpr guikit::Align GetCtrlAlign(NavCategory cat) const noexcept {
        return (cat == NavCategory::FieldSelect || cat == NavCategory::FieldMultiSelect)
            ? ctrlSelectAlign
            : ctrlAlign;
    }

    static constexpr guikit::AxisSize ResolveAxisSize(keel::usize modeIdx, keel::f32 dp) noexcept {
        switch (modeIdx) {
            case 1:  return guikit::AxisSize::Fit();
            case 2:  return guikit::AxisSize::Stretch();
            case 3:  return guikit::AxisSize::Fixed(dp);
            default: return guikit::AxisSize::Auto();
        }
    }

    static keel::FixedString<48> FormatAxisSizeCode(guikit::AxisSize sz) noexcept {
        if (sz.IsFit()) {
            return "guikit::SizeMode::Fit";
        }
        if (sz.IsStretch()) {
            if (sz.HasCustomStretchWeight()) {
                return keel::format_fixed<48>("guikit::AxisSize::Stretch({:.1f}f)", sz.StretchWeight());
            }
            return "guikit::SizeMode::Stretch";
        }
        if (sz.IsFixed()) {
            return keel::format_fixed<48>("{:.1f}f", sz.Dp());
        }
        return "guikit::AxisSize::Auto()";
    }

    constexpr guikit::AxisSize ResolveCtrlWidth() const noexcept {
        return ResolveAxisSize(ctrlWidthModeIdx, ctrlWidthDp);
    }

    constexpr guikit::AxisSize ResolveCtrlHeight() const noexcept {
        return ResolveAxisSize(ctrlHeightModeIdx, ctrlHeightDp);
    }

    constexpr keel::Optional<keel::f32> ResolveKnobDiameter() const noexcept {
        return knobCustomDiameter ? keel::Optional<keel::f32>(knobDiameterDp) : keel::kNullOpt;
    }

    keel::FixedString<48> FormatKnobDiameterCode() const noexcept {
        if (knobCustomDiameter) {
            return keel::format_fixed<48>("{:.1f}f", knobDiameterDp);
        }
        return "keel::kNullOpt";
    }

    guikit::StateColor GetControlBackground(NavCategory cat) const noexcept {
        const keel::usize idx = keel::as<keel::usize>(cat);
        const keel::usize bgIdx = (idx < ctrlBgIdxByCat.size()) ? ctrlBgIdxByCat[idx] : 0u;
        return BgPresetToStateColor(bgIdx, ctrlBgTint, ctrlBgAlpha);
    }

    keel::String FormatControlBackgroundCode(NavCategory cat) const {
        const keel::usize idx = keel::as<keel::usize>(cat);
        const keel::usize bgIdx = (idx < ctrlBgIdxByCat.size()) ? ctrlBgIdxByCat[idx] : 0u;
        return BgPresetToCode(bgIdx, ctrlBgTint, ctrlBgAlpha);
    }

    guikit::StateColor GetCtrlBorderColor() const noexcept {
        return guikit::StateColor::FromVariant(ctrlBorderVariant);
    }

    guikit::Border GetCtrlBorder() const noexcept {
        return guikit::Border{
            .width = ctrlBorderWidth,
            .radius = (ctrlBorderRadius >= 0.0f) ? keel::Optional<keel::f32>(ctrlBorderRadius) : keel::kNullOpt,
            .color = GetCtrlBorderColor()
        };
    }

    static constexpr keel::StringRef kDefaultElementLabel = "Element Label";
    static constexpr keel::StringRef kDefaultElementDesc  = "Helper description text";
    static constexpr keel::StringRef kDefaultElementBadge = "Badge";

    guikit::ElementOptions MakeElementOptions(guikit::UiText customLabel) const noexcept {
        guikit::ElementOptions opts{};
        opts.label = customLabel;
        opts.disabled = elemDisabled;
        switch (elemPreset) {
            case ElementWrapperPreset::LabelAndDescription:
                opts.description = guikit::UiText{kDefaultElementDesc, guikit::UiVariant::Muted, guikit::TextWrap::Word()};
                break;
            case ElementWrapperPreset::Full:
                opts.description = guikit::UiText{kDefaultElementDesc, guikit::UiVariant::Muted, guikit::TextWrap::Word()};
                opts.icon = guikit::Icon::Settings();
                opts.badge = guikit::UiText{kDefaultElementBadge, guikit::UiVariant::Primary};
                break;
            case ElementWrapperPreset::InlineLeading:
                opts.layout = guikit::ElementLayout::Leading;
                break;
            case ElementWrapperPreset::InlineTrailing:
                opts.layout = guikit::ElementLayout::Trailing;
                break;
            case ElementWrapperPreset::None:
            case ElementWrapperPreset::LabelOnly:
            default:
                break;
        }
        return opts;
    }

    guikit::ElementOptions MakeElementOptions() const noexcept {
        return MakeElementOptions(guikit::UiText{kDefaultElementLabel});
    }

    template <typename Fn>
    void RunElementStage(guikit::Context& ui, guikit::UiText customLabel, Fn&& renderControls) const {
        if (!HasElementWrapper()) {
            renderControls();
            return;
        }
        if (auto el = guikit::Element(ui, MakeElementOptions(customLabel))) {
            renderControls();
        }
    }

    template <typename Fn>
    void RunElementStage(guikit::Context& ui, Fn&& renderControls) const {
        RunElementStage(ui, guikit::UiText{kDefaultElementLabel}, keel::forward<Fn>(renderControls));
    }

    keel::FixedString<48> FormatCtrlWidthCode() const noexcept {
        return FormatAxisSizeCode(ResolveCtrlWidth());
    }

    keel::FixedString<48> FormatControlWidthCode() const noexcept {
        return FormatCtrlWidthCode();
    }

    keel::FixedString<48> FormatCtrlHeightCode() const noexcept {
        return FormatAxisSizeCode(ResolveCtrlHeight());
    }

    keel::FixedString<48> FormatControlHeightCode() const noexcept {
        return FormatCtrlHeightCode();
    }

    keel::FixedString<64> FormatControlColorCode() const noexcept {
        return keel::format_fixed<64>("guikit::StateColor::{}()", VariantName(btnVariant));
    }

    keel::FixedString<64> FormatCtrlBorderColorCode() const noexcept {
        return keel::format_fixed<64>("guikit::StateColor::{}()", VariantName(ctrlBorderVariant));
    }

    keel::String FormatCtrlBorderCode() const {
        if (ctrlBorderRadius < 0.0f) {
            return keel::format(
                "{{.width = {:.1f}f, .color = {}}}",
                ctrlBorderWidth,
                FormatCtrlBorderColorCode()
            );
        }
        return keel::format(
            "{{.width = {:.1f}f, .radius = {:.1f}f, .color = {}}}",
            ctrlBorderWidth,
            ctrlBorderRadius,
            FormatCtrlBorderColorCode()
        );
    }

    keel::String FormatElementOpenCode(keel::Optional<keel::StringRef> customLabelOverride = keel::kNullOpt) const {
        const keel::StringRef lbl = customLabelOverride.has_value() ? *customLabelOverride : kDefaultElementLabel;
        keel::String out = keel::format(
            "if (auto el = guikit::Element(ui, {{\n"
            "    .label = \"{}\"",
            lbl
        );
        if (elemPreset == ElementWrapperPreset::LabelAndDescription ||
            elemPreset == ElementWrapperPreset::Full) {
            keel::format_into(out, ",\n    .description = \"{}\"", kDefaultElementDesc);
        }
        if (elemPreset == ElementWrapperPreset::Full) {
            out.append(",\n    .icon = guikit::Icon::Settings()");
            keel::format_into(out, ",\n    .badge = {{\"{}\", guikit::StateColor::Primary()}}", kDefaultElementBadge);
        }
        if (elemPreset == ElementWrapperPreset::InlineLeading) {
            out.append(",\n    .layout = guikit::ElementLayout::Leading");
        } else if (elemPreset == ElementWrapperPreset::InlineTrailing) {
            out.append(",\n    .layout = guikit::ElementLayout::Trailing");
        }
        if (elemDisabled) {
            out.append(",\n    .disabled = true");
        }
        out.append("\n})) {");
        return out;
    }

    keel::String WrapInElementCode(
        keel::StringRef controlCode,
        keel::Optional<keel::StringRef> customLabelOverride = keel::kNullOpt
    ) const {
        if (!HasElementWrapper()) {
            return keel::String::from(controlCode);
        }
        keel::String out = FormatElementOpenCode(customLabelOverride);
        out.push_back('\n');

        keel::usize start = 0;
        while (start <= controlCode.size()) {
            keel::usize end = start;
            while (end < controlCode.size() && controlCode[end] != '\n') {
                ++end;
            }
            const keel::StringRef line = controlCode.substr(start, end - start);
            if (!line.empty()) {
                out.append("    ");
                out.append(line);
            }
            out.push_back('\n');
            if (end == controlCode.size()) {
                break;
            }
            start = end + 1;
        }
        out.push_back('}');
        return out;
    }

    guikit::CardOptions MakeCard() const noexcept {
        return guikit::CardOptions{
            .gap = cardGap,
            .padding = guikit::Padding(cardPadding),
            .border = {.width = cardBorderWidth, .radius = cardRadius}
        };
    }

    guikit::GridOptions MakeGrid() const noexcept {
        return guikit::GridOptions{
            .gap = gridColGap,
            .runGap = gridRowGap
        };
    }

    void RenderVariantPicker(guikit::Context& ui) {
        if (auto el = guikit::Element(ui, {.label = "Semantic Variant"})) {
            if (guikit::Select(ui, btnVariant, kUiVariantNames).changed) {
                ui.InvalidateAll();
            }
        }
    }

    void RenderDescVariantPicker(guikit::Context& ui, guikit::UiVariant& descVar, bool enabled) {
        static constexpr keel::StaticArray<keel::StringRef, 7> descVarNames = {
            "Muted", "Default", "Primary", "Success", "Warning", "Danger", "Info"
        };
        static constexpr keel::StaticArray<guikit::UiVariant, 7> descVars = {
            guikit::UiVariant::Muted, guikit::UiVariant::Default, guikit::UiVariant::Primary,
            guikit::UiVariant::Success, guikit::UiVariant::Warning, guikit::UiVariant::Danger, guikit::UiVariant::Info
        };
        keel::usize descVarIdx = 0;
        for (keel::usize i = 0; i < descVars.size(); ++i) {
            if (descVars[i] == descVar) {
                descVarIdx = i;
                break;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Description Color", .disabled = !enabled})) {
            if (guikit::Select(ui, descVarIdx, descVarNames, {
                .disabled = !enabled
            }).changed) {
                descVar = descVars[descVarIdx];
                ui.InvalidateAll();
            }
        }
    }

    void RenderControlLayoutSection(guikit::Context& ui, NavCategory cat) {
        if (!HasCtrlLayoutCategory(cat)) {
            return;
        }

        if (cat == NavCategory::ControlKnob) {
            if (auto el = guikit::Element(ui, {.label = "Custom Diameter"})) {
                if (guikit::Switch(ui, knobCustomDiameter).changed) {
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Diameter (dp)", .disabled = !knobCustomDiameter})) {
                if (guikit::Slider(ui, knobDiameterDp, 28.0f, 140.0f, {
                    .valueFormat = "%.0f dp",
                    .step = 2.0f,
                    .disabled = !knobCustomDiameter
                }).changed) {
                    ui.InvalidateAll();
                }
            }
            return;
        }

        if (auto el = guikit::Element(ui, {.label = "Width"})) {
            if (guikit::Segmented(ui, ctrlWidthModeIdx, kAxisModeNames).changed) {
                ui.InvalidateAll();
            }
        }

        const bool ctrlFixedWidth = (ctrlWidthModeIdx == 3);
        if (auto el = guikit::Element(ui, {.label = "Width (dp)", .disabled = !ctrlFixedWidth})) {
            if (guikit::Slider(ui, ctrlWidthDp, 80.0f, 480.0f, {
                .valueFormat = "%.0f dp",
                .step = 10.0f,
                .disabled = !ctrlFixedWidth
            }).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Height"})) {
            if (guikit::Segmented(ui, ctrlHeightModeIdx, kAxisModeNames).changed) {
                ui.InvalidateAll();
            }
        }

        const bool ctrlFixedHeight = (ctrlHeightModeIdx == 3);
        if (auto el = guikit::Element(ui, {.label = "Height (dp)", .disabled = !ctrlFixedHeight})) {
            if (guikit::Slider(ui, ctrlHeightDp, 24.0f, 320.0f, {
                .valueFormat = "%.0f dp",
                .step = 4.0f,
                .disabled = !ctrlFixedHeight
            }).changed) {
                ui.InvalidateAll();
            }
        }

        if (cat == NavCategory::ActionButton ||
            cat == NavCategory::ActionMenuButton ||
            cat == NavCategory::ActionSplitButton ||
            cat == NavCategory::FieldSelect ||
            cat == NavCategory::FieldMultiSelect) {
            guikit::Align& targetAlign = (cat == NavCategory::FieldSelect || cat == NavCategory::FieldMultiSelect)
                ? ctrlSelectAlign
                : ctrlAlign;
            keel::usize alignIdx = (targetAlign == guikit::Align::Start)
                ? 0
                : ((targetAlign == guikit::Align::End) ? 2 : 1);
            if (auto el = guikit::Element(ui, {.label = "Alignment"})) {
                if (guikit::Segmented(ui, alignIdx, kLabelAlignNames).changed) {
                    targetAlign = (alignIdx == 0)
                        ? guikit::Align::Start
                        : ((alignIdx == 2) ? guikit::Align::End : guikit::Align::Center);
                    ui.InvalidateAll();
                }
            }
        }
    }

    void RenderFieldStyleControls(guikit::Context& ui) {
        if (auto el = guikit::Element(ui, {.label = "Field Style"})) {
            if (guikit::Segmented(ui, ctrlFieldStyle, kFieldStyleNames).changed) {
                ui.InvalidateAll();
            }
        }
        keel::usize statusIdx = 0u;
        for (keel::usize i = 0u; i < kFieldStatusValues.size(); ++i) {
            if (kFieldStatusValues[i] == ctrlFieldStatus) {
                statusIdx = i;
                break;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Field Status"})) {
            if (guikit::Select(ui, statusIdx, kFieldStatusNames).changed) {
                ctrlFieldStatus = kFieldStatusValues[statusIdx];
                ui.InvalidateAll();
            }
        }
    }

    void RenderControlStyleSection(guikit::Context& ui, NavCategory cat) {
        if (auto el = guikit::Element(ui, {.label = "Size"})) {
            if (guikit::StepSlider(ui, controlsSize, {.showTicks = true, .showLabels = true}).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Disabled"})) {
            if (guikit::Switch(ui, elemDisabled).changed) {
                ui.InvalidateAll();
            }
        }

        if (IsFieldCategory(cat)) {
            RenderFieldStyleControls(ui);
        }

        const bool hasControlVariant = (
            cat == NavCategory::ActionButton ||
            cat == NavCategory::ActionSegmented ||
            cat == NavCategory::ControlSlider ||
            cat == NavCategory::ControlStepSlider ||
            cat == NavCategory::ControlRangeSlider ||
            cat == NavCategory::ControlKnob ||
            cat == NavCategory::ControlSwitch ||
            cat == NavCategory::ControlCheckbox ||
            cat == NavCategory::ControlRadio ||
            cat == NavCategory::DisplayProgressBar ||
            cat == NavCategory::DisplayLevelMeter
        );
        if (hasControlVariant) {
            if (auto el = guikit::Element(ui, {.label = "Color"})) {
                if (guikit::Select(ui, btnVariant, kUiVariantNames).changed) {
                    ui.InvalidateAll();
                }
            }
        }

        if (IsCtrlBoxedCategory(cat)) {
            keel::usize& ctrlBgIdx = ctrlBgIdxByCat[keel::as<keel::usize>(cat)];
            if (auto el = guikit::Element(ui, {.label = "Background"})) {
                if (guikit::Select(ui, ctrlBgIdx, kBgPresetNames).changed) {
                    ui.InvalidateAll();
                }
            }
            const bool hasCtrlBgTint = (ctrlBgIdx >= 3 && ctrlBgIdx <= 8);
            if (auto el = guikit::Element(ui, {.label = "Background Tint", .disabled = !hasCtrlBgTint})) {
                if (guikit::Slider(ui, ctrlBgTint, 0.0f, 1.0f, {
                    .valueFormat = "%.2f",
                    .step = 0.05f,
                    .disabled = !hasCtrlBgTint
                }).changed) {
                    ui.InvalidateAll();
                }
            }
            const bool hasCtrlBgAlpha = (ctrlBgIdx >= 2);
            if (auto el = guikit::Element(ui, {.label = "Background Opacity", .disabled = !hasCtrlBgAlpha})) {
                if (guikit::Slider(ui, ctrlBgAlpha, 0.0f, 1.0f, {
                    .valueFormat = "%.2f",
                    .step = 0.05f,
                    .disabled = !hasCtrlBgAlpha
                }).changed) {
                    ui.InvalidateAll();
                }
            }

            const bool hasIntrinsicBorder = (cat != NavCategory::DisplayImage && cat != NavCategory::FieldTagInput);
            if (auto el = guikit::Element(ui, {.label = "Border Width"})) {
                static constexpr keel::StaticArray<keel::StringRef, 2> kBorderVariants = {{"Default", "%.1f dp"}};
                if (guikit::Slider(ui, ctrlBorderWidth, 0.0f, 4.0f, {
                    .valueFormat = (hasIntrinsicBorder && ctrlBorderWidth <= 0.0f)
                        ? guikit::UiText{"Default", kBorderVariants}
                        : guikit::UiText{"%.1f dp", kBorderVariants},
                    .step = 0.5f
                }).changed) {
                    ui.InvalidateAll();
                }
            }

            const bool hasVisibleCtrlBorder =
                (ctrlBorderWidth > 0.0f) ||
                (cat == NavCategory::FieldSpinBox) ||
                (cat == NavCategory::FieldInputField) ||
                (cat == NavCategory::FieldTagInput) ||
                (cat == NavCategory::FieldSelect) ||
                (cat == NavCategory::FieldMultiSelect) ||
                (cat == NavCategory::ActionSegmented) ||
                (cat == NavCategory::FieldColorPicker) ||
                (cat == NavCategory::FieldDatePicker) ||
                (cat == NavCategory::ActionButton &&
                    (btnVariant == guikit::UiVariant::Default || btnVariant == guikit::UiVariant::Secondary));
            if (auto el = guikit::Element(ui, {.label = "Border Color", .disabled = !hasVisibleCtrlBorder})) {
                if (guikit::Select(ui, ctrlBorderVariant, kUiVariantNames, {
                    .disabled = !hasVisibleCtrlBorder
                }).changed) {
                    ui.InvalidateAll();
                }
            }

            if (auto el = guikit::Element(ui, {.label = "Corner Radius"})) {
                static constexpr keel::StaticArray<keel::StringRef, 2> kRadiusVariants = {{"Auto", "%.0f dp"}};
                if (guikit::Slider(ui, ctrlBorderRadius, -1.0f, 24.0f, {
                    .valueFormat = (ctrlBorderRadius < 0.0f)
                        ? guikit::UiText{"Auto", kRadiusVariants}
                        : guikit::UiText{"%.0f dp", kRadiusVariants},
                    .step = 1.0f
                }).changed) {
                    ui.InvalidateAll();
                }
            }
        }
    }

    void RenderAssemblyCommonInspector(guikit::Context& ui) {
        if (auto el = guikit::Element(ui, {
            .label = "Preview Dark Mode",
            .description = "Switch preview between Dark and Light palette"
        })) {
            if (guikit::Switch(ui, previewDarkMode).changed) {
                previewLightMode = !previewDarkMode;
                const auto defTheme = previewDarkMode
                    ? guikit::UiTheme::Dark()
                    : guikit::UiTheme::Light();
                previewTheme.palette = defTheme.palette;
                previewTheme.variants = defTheme.variants;
                previewTheme.syntax = defTheme.syntax;
                previewTheme.mode = previewDarkMode ? guikit::ThemeMode::Dark : guikit::ThemeMode::Light;
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Layout Density"})) {
            if (guikit::Slider(ui, previewDensity, 0.5f, 2.0f, {
                .valueFormat = "%.2fx",
                .step = 0.05
            }).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Horizontal Gap (Columns)"})) {
            if (guikit::Slider(ui, gridColGap, 0.0f, 32.0f, {.valueFormat = "%.0f dp"}).changed) ui.InvalidateAll();
        }
        if (auto el = guikit::Element(ui, {.label = "Vertical Gap (Rows)"})) {
            if (guikit::Slider(ui, gridRowGap, 0.0f, 32.0f, {.valueFormat = "%.0f dp"}).changed) ui.InvalidateAll();
        }

        if (auto el = guikit::Element(ui, {.label = "Card Corner Radius"})) {
            if (guikit::Slider(ui, cardRadius, 0.0f, 24.0f, {.valueFormat = "%.0f dp"}).changed) ui.InvalidateAll();
        }
        if (auto el = guikit::Element(ui, {.label = "Card Padding"})) {
            if (guikit::Slider(ui, cardPadding, 4.0f, 32.0f, {.valueFormat = "%.0f dp"}).changed) ui.InvalidateAll();
        }
        if (auto el = guikit::Element(ui, {.label = "Card Border Width"})) {
            if (guikit::Slider(ui, cardBorderWidth, 0.0f, 4.0f, {.valueFormat = "%.1f dp"}).changed) ui.InvalidateAll();
        }
    }
};

// ============================================================================
// 1. ELEMENTS (Atomic Controls)
// ============================================================================
void RenderHeadingInspector(guikit::Context& ui, DemoContext& ctx);
void RenderHeadingStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateHeadingCode(const DemoContext& ctx);

void RenderElementInspector(guikit::Context& ui, DemoContext& ctx);
void RenderElementStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateElementCode(const DemoContext& ctx);

void RenderThemeStudioInspector(guikit::Context& ui, DemoContext& ctx);
void RenderThemeStudioStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateThemeStudioCode(const DemoContext& ctx);

void RenderButtonInspector(guikit::Context& ui, DemoContext& ctx);
void RenderButtonStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateButtonCode(const DemoContext& ctx);

void RenderMenuButtonInspector(guikit::Context& ui, DemoContext& ctx);
void RenderMenuButtonStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateMenuButtonCode(const DemoContext& ctx);

void RenderSplitButtonInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSplitButtonStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSplitButtonCode(const DemoContext& ctx);

void RenderSliderInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSliderStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSliderCode(const DemoContext& ctx);

void RenderStepSliderInspector(guikit::Context& ui, DemoContext& ctx);
void RenderStepSliderStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateStepSliderCode(const DemoContext& ctx);

void RenderRangeSliderInspector(guikit::Context& ui, DemoContext& ctx);
void RenderRangeSliderStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateRangeSliderCode(const DemoContext& ctx);

void RenderProgressBarInspector(guikit::Context& ui, DemoContext& ctx);
void RenderProgressBarStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateProgressBarCode(const DemoContext& ctx);

void RenderLevelMeterInspector(guikit::Context& ui, DemoContext& ctx);
void RenderLevelMeterStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateLevelMeterCode(const DemoContext& ctx);

void RenderKnobInspector(guikit::Context& ui, DemoContext& ctx);
void RenderKnobStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateKnobCode(const DemoContext& ctx);

void RenderSpinBoxInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSpinBoxStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSpinBoxCode(const DemoContext& ctx);

void RenderInputInspector(guikit::Context& ui, DemoContext& ctx);
void RenderInputStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateInputCode(const DemoContext& ctx);

void RenderReadoutInspector(guikit::Context& ui, DemoContext& ctx);
void RenderReadoutStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateReadoutCode(const DemoContext& ctx);

void RenderTagInspector(guikit::Context& ui, DemoContext& ctx);
void RenderTagStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateTagCode(const DemoContext& ctx);

void RenderTagInputInspector(guikit::Context& ui, DemoContext& ctx);
void RenderTagInputStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateTagInputCode(const DemoContext& ctx);

void RenderSelectInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSelectStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSelectCode(const DemoContext& ctx);

void RenderMultiSelectInspector(guikit::Context& ui, DemoContext& ctx);
void RenderMultiSelectStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateMultiSelectCode(const DemoContext& ctx);

void RenderSegmentedInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSegmentedStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSegmentedCode(const DemoContext& ctx);

void RenderColorPickerInspector(guikit::Context& ui, DemoContext& ctx);
void RenderColorPickerStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateColorPickerCode(const DemoContext& ctx);

void RenderDatePickerInspector(guikit::Context& ui, DemoContext& ctx);
void RenderDatePickerStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateDatePickerCode(const DemoContext& ctx);

void RenderSwitchInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSwitchStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSwitchCode(const DemoContext& ctx);

void RenderCheckboxInspector(guikit::Context& ui, DemoContext& ctx);
void RenderCheckboxStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateCheckboxCode(const DemoContext& ctx);

void RenderRadioInspector(guikit::Context& ui, DemoContext& ctx);
void RenderRadioStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateRadioCode(const DemoContext& ctx);

// ============================================================================
// 2. PANELS & NAV (Containers & Navigation)
// ============================================================================
void RenderCardInspector(guikit::Context& ui, DemoContext& ctx);
void RenderCardStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateCardCode(const DemoContext& ctx);

void RenderFlexInspector(guikit::Context& ui, DemoContext& ctx);
void RenderFlexStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateFlexCode(const DemoContext& ctx);

void RenderWrapInspector(guikit::Context& ui, DemoContext& ctx);
void RenderWrapStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateWrapCode(const DemoContext& ctx);

void RenderGridInspector(guikit::Context& ui, DemoContext& ctx);
void RenderGridStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateGridCode(const DemoContext& ctx);

void RenderScrollAreaInspector(guikit::Context& ui, DemoContext& ctx);
void RenderScrollAreaStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateScrollAreaCode(const DemoContext& ctx);

void RenderSplitViewInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSplitViewStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSplitViewCode(const DemoContext& ctx);

void RenderTabBarInspector(guikit::Context& ui, DemoContext& ctx);
void RenderTabBarStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateTabBarCode(const DemoContext& ctx);

void RenderBreadcrumbInspector(guikit::Context& ui, DemoContext& ctx);
void RenderBreadcrumbStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateBreadcrumbCode(const DemoContext& ctx);

void RenderPaginationInspector(guikit::Context& ui, DemoContext& ctx);
void RenderPaginationStage(guikit::Context& ui, DemoContext& ctx);
keel::String GeneratePaginationCode(const DemoContext& ctx);

void RenderSidebarInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSidebarStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSidebarCode(const DemoContext& ctx);

void RenderTreeViewInspector(guikit::Context& ui, DemoContext& ctx);
void RenderTreeViewStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateTreeViewCode(const DemoContext& ctx);

void RenderTableInspector(guikit::Context& ui, DemoContext& ctx);
void RenderTableStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateTableCode(const DemoContext& ctx);

void RenderModalInspector(guikit::Context& ui, DemoContext& ctx);
void RenderModalStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateModalCode(const DemoContext& ctx);

void RenderPopoverInspector(guikit::Context& ui, DemoContext& ctx);
void RenderPopoverStage(guikit::Context& ui, DemoContext& ctx);
keel::String GeneratePopoverCode(const DemoContext& ctx);

void RenderContextMenuInspector(guikit::Context& ui, DemoContext& ctx);
void RenderContextMenuStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateContextMenuCode(const DemoContext& ctx);

void RenderToastInspector(guikit::Context& ui, DemoContext& ctx);
void RenderToastStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateToastCode(const DemoContext& ctx);

void RenderCommandPaletteInspector(guikit::Context& ui, DemoContext& ctx);
void RenderCommandPaletteStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateCommandPaletteCode(const DemoContext& ctx);

void RenderTooltipInspector(guikit::Context& ui, DemoContext& ctx);
void RenderTooltipStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateTooltipCode(const DemoContext& ctx);

void RenderMenuBarInspector(guikit::Context& ui, DemoContext& ctx);
void RenderMenuBarStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateMenuBarCode(const DemoContext& ctx);

void RenderToolbarInspector(guikit::Context& ui, DemoContext& ctx);
void RenderToolbarStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateToolbarCode(const DemoContext& ctx);

// ============================================================================
// 3. CONTENT & MEDIA (Documents, Editors, Graphics)
// ============================================================================
void RenderImageInspector(guikit::Context& ui, DemoContext& ctx);
void RenderImageStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateImageCode(const DemoContext& ctx);

void RenderMarkdownInspector(guikit::Context& ui, DemoContext& ctx);
void RenderMarkdownStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateMarkdownCode(const DemoContext& ctx);

void RenderCodeEditorInspector(guikit::Context& ui, DemoContext& ctx);
void RenderCodeEditorStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateCodeEditorCode(const DemoContext& ctx);

// ============================================================================
// 4. 2D SCENES (Interactive Vector Viewports)
// ============================================================================
void RenderSchematicInspector(guikit::Context& ui, DemoContext& ctx);
void RenderSchematicStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateSchematicCode(const DemoContext& ctx);

void RenderImageViewInspector(guikit::Context& ui, DemoContext& ctx);
void RenderImageViewStage(guikit::Context& ui, DemoContext& ctx);
keel::String GenerateImageViewCode(const DemoContext& ctx);

void ReleaseImageViewDemoBuffers() noexcept;

} // namespace guikit::demo
