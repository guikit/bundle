#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct ThemeStudioState {
    // Base Preset & Orthogonal Design-System Axes (Palette x Chrome x Fill)
    guikit::ThemePreset preset = guikit::ThemePreset::Dark;
    guikit::ChromeStyle chrome = guikit::ChromeStyle::Modern;
    guikit::FillStyle fillStyle = guikit::FillStyle::Solid;
    guikit::SyntaxPreset syntaxPreset = guikit::SyntaxPreset::Auto;

    // Scrollbar Customization
    guikit::ScrollbarPreset scrollbarPreset = guikit::ScrollbarPreset::Auto;
    bool overrideScrollbarWidth = false;
    keel::f32 scrollbarWidth = 10.0f;
    bool customizeScrollbarTokens = false;
    guikit::ScrollbarStyleTokens customScrollbarTokens = guikit::ScrollbarStyleTokens::Modern();

    // Seed & Direct Color Overrides
    bool overrideAccent = false;
    guikit::Color accentColor = guikit::Color::Hex(0x8B5CF6FF);
    bool overrideNeutral = false;
    guikit::Color neutralColor = guikit::Color::Hex(0x141824FF);

    bool overrideBackground = false;
    guikit::Color backgroundColor = guikit::Color::Hex(0x0B0F19FF);
    bool overrideSurface = false;
    guikit::Color surfaceColor = guikit::Color::Hex(0x1E293BFF);
    bool overrideBorder = false;
    guikit::Color borderColor = guikit::Color::Hex(0x334155FF);
    bool overrideText = false;
    guikit::Color textColor = guikit::Color::Hex(0xF8FAFCFF);

    // Semantic Color Overrides
    bool overrideSuccess = false;
    guikit::Color successColor = guikit::Color::Hex(0x10B981FF);
    bool overrideWarning = false;
    guikit::Color warningColor = guikit::Color::Hex(0xF59E0BFF);
    bool overrideDanger = false;
    guikit::Color dangerColor = guikit::Color::Hex(0xEF4444FF);
    bool overrideInfo = false;
    guikit::Color infoColor = guikit::Color::Hex(0x06B6D4FF);

    // Geometry, Contrast, Elevation & Accessibility
    keel::f32 surfaceContrast = 1.0f;
    keel::f32 radiusScale = 1.0f;
    keel::f32 spacingScale = 1.0f;
    keel::f32 fontScale = 1.0f;
    keel::f32 lineHeightMultiplier = guikit::kDefaultLineHeightMultiplier;
    keel::f32 borderScale = 1.0f;
    keel::f32 shadowIntensity = 1.0f;
    keel::f32 focusRingWidth = 2.0f;
    keel::f32 disabledOpacity = 0.50f;
    keel::f32 animationScale = 1.0f;
    guikit::ShellStyle shellStyle = guikit::ShellStyle::Docked;

    // Theme Morphing (UiTheme::Lerp)
    bool enableLerp = false;
    guikit::ThemePreset lerpTarget = guikit::ThemePreset::CatppuccinLatte;
    keel::f32 lerpT = 0.0f;

    // JSON Import & Code Export
    keel::i32 codeFormatIdx = 0; // 0 = C++20, 1 = JSON
    keel::String jsonDraft{};
    keel::i32 jsonImportStatus = 0; // 0 = none, 1 = loaded, -1 = error
    keel::u32 randomSeed = 1337u;

    // Stage Navigation & Live Preview Widget State
    keel::usize activeStageTab = 0u; // 0 = Showcase, 1 = Swatches, 2 = Controls, 3 = Zones & Code
    keel::SmallString<64> sampleInput{"Click to test Focus Ring"};
    keel::SmallString<64> disabledInput{"Disabled input preview"};
    keel::usize sampleSelectIdx = 1u;
    bool sampleSwitch = true;
    bool sampleSwitchOff = false;
    bool sampleCheckbox = true;
    keel::f32 sampleSlider = 68.0f;
    keel::f32 sampleKnob = 42.0f;
    bool indeterminateBar = true;
};

ThemeStudioState s_studio;

void ResetToDefaults() noexcept {
    s_studio.preset = guikit::ThemePreset::Dark;
    s_studio.chrome = guikit::ChromeStyle::Modern;
    s_studio.fillStyle = guikit::FillStyle::Solid;
    s_studio.syntaxPreset = guikit::SyntaxPreset::Auto;
    s_studio.scrollbarPreset = guikit::ScrollbarPreset::Auto;
    s_studio.overrideScrollbarWidth = false;
    s_studio.scrollbarWidth = 10.0f;
    s_studio.customizeScrollbarTokens = false;
    s_studio.customScrollbarTokens = guikit::ScrollbarStyleTokens::Modern();

    s_studio.overrideAccent = false;
    s_studio.overrideNeutral = false;
    s_studio.overrideBackground = false;
    s_studio.overrideSurface = false;
    s_studio.overrideBorder = false;
    s_studio.overrideText = false;

    s_studio.overrideSuccess = false;
    s_studio.overrideWarning = false;
    s_studio.overrideDanger = false;
    s_studio.overrideInfo = false;

    s_studio.surfaceContrast = 1.0f;
    s_studio.radiusScale = 1.0f;
    s_studio.spacingScale = 1.0f;
    s_studio.fontScale = 1.0f;
    s_studio.lineHeightMultiplier = guikit::kDefaultLineHeightMultiplier;
    s_studio.borderScale = 1.0f;
    s_studio.shadowIntensity = 1.0f;
    s_studio.focusRingWidth = 2.0f;
    s_studio.disabledOpacity = 0.50f;
    s_studio.animationScale = 1.0f;
    s_studio.shellStyle = guikit::ShellStyle::Docked;

    s_studio.enableLerp = false;
    s_studio.lerpT = 0.0f;
}

void SelectBasePreset(guikit::ThemePreset preset) noexcept {
    s_studio.preset = preset;
    const guikit::UiTheme def = guikit::UiTheme::FromPreset(preset);
    s_studio.chrome = def.chromeStyle;
    s_studio.fillStyle = def.fillStyle;
    s_studio.overrideAccent = false;
    s_studio.overrideNeutral = false;
    s_studio.overrideBackground = false;
    s_studio.overrideSurface = false;
    s_studio.overrideBorder = false;
    s_studio.overrideText = false;
    s_studio.overrideSuccess = false;
    s_studio.overrideWarning = false;
    s_studio.overrideDanger = false;
    s_studio.overrideInfo = false;
}

void ApplySpecToStudioState(const guikit::ThemeSpec& spec) noexcept {
    ResetToDefaults();
    s_studio.preset = spec.base;
    const guikit::UiTheme def = guikit::UiTheme::FromPreset(spec.base);
    s_studio.chrome = spec.chrome.value_or(def.chromeStyle);
    s_studio.fillStyle = spec.fillStyle.value_or(def.fillStyle);
    s_studio.syntaxPreset = spec.syntaxPreset;
    s_studio.scrollbarPreset = spec.scrollbarPreset.value_or(guikit::ScrollbarPreset::Auto);
    if (spec.scrollbarWidth.has_value()) {
        s_studio.overrideScrollbarWidth = true;
        s_studio.scrollbarWidth = *spec.scrollbarWidth;
    }
    if (spec.accent.has_value()) { s_studio.overrideAccent = true; s_studio.accentColor = *spec.accent; }
    if (spec.neutral.has_value()) { s_studio.overrideNeutral = true; s_studio.neutralColor = *spec.neutral; }
    if (spec.background.has_value()) { s_studio.overrideBackground = true; s_studio.backgroundColor = *spec.background; }
    if (spec.surface.has_value()) { s_studio.overrideSurface = true; s_studio.surfaceColor = *spec.surface; }
    if (spec.border.has_value()) { s_studio.overrideBorder = true; s_studio.borderColor = *spec.border; }
    if (spec.textPrimary.has_value()) { s_studio.overrideText = true; s_studio.textColor = *spec.textPrimary; }
    if (spec.success.has_value()) { s_studio.overrideSuccess = true; s_studio.successColor = *spec.success; }
    if (spec.warning.has_value()) { s_studio.overrideWarning = true; s_studio.warningColor = *spec.warning; }
    if (spec.danger.has_value()) { s_studio.overrideDanger = true; s_studio.dangerColor = *spec.danger; }
    if (spec.info.has_value()) { s_studio.overrideInfo = true; s_studio.infoColor = *spec.info; }
    s_studio.surfaceContrast = spec.surfaceContrast;
    s_studio.radiusScale = spec.radiusScale;
    s_studio.spacingScale = spec.spacingScale;
    s_studio.fontScale = spec.fontScale;
    s_studio.lineHeightMultiplier = spec.lineHeightMultiplier;
    s_studio.borderScale = spec.borderScale;
    s_studio.shadowIntensity = spec.shadowIntensity;
    s_studio.focusRingWidth = spec.focusRingWidth;
    s_studio.disabledOpacity = spec.disabledOpacity;
    s_studio.animationScale = spec.animationScale;
    s_studio.shellStyle = spec.shellStyle;
}

[[nodiscard]] guikit::ThemeSpec BuildCurrentSpec() noexcept {
    guikit::ThemeSpec spec{};
    spec.base = s_studio.preset;
    spec.chrome = s_studio.chrome;
    spec.fillStyle = s_studio.fillStyle;
    spec.syntaxPreset = s_studio.syntaxPreset;
    if (s_studio.scrollbarPreset != guikit::ScrollbarPreset::Auto) {
        spec.scrollbarPreset = s_studio.scrollbarPreset;
    }
    if (s_studio.overrideScrollbarWidth) {
        spec.scrollbarWidth = s_studio.scrollbarWidth;
    }
    if (s_studio.overrideAccent)     spec.accent = s_studio.accentColor;
    if (s_studio.overrideNeutral)    spec.neutral = s_studio.neutralColor;
    if (s_studio.overrideBackground) spec.background = s_studio.backgroundColor;
    if (s_studio.overrideSurface)    spec.surface = s_studio.surfaceColor;
    if (s_studio.overrideBorder)     spec.border = s_studio.borderColor;
    if (s_studio.overrideText)       spec.textPrimary = s_studio.textColor;
    if (s_studio.overrideSuccess)    spec.success = s_studio.successColor;
    if (s_studio.overrideWarning)    spec.warning = s_studio.warningColor;
    if (s_studio.overrideDanger)     spec.danger = s_studio.dangerColor;
    if (s_studio.overrideInfo)       spec.info = s_studio.infoColor;
    spec.surfaceContrast = s_studio.surfaceContrast;
    spec.radiusScale = s_studio.radiusScale;
    spec.spacingScale = s_studio.spacingScale;
    spec.fontScale = s_studio.fontScale;
    spec.lineHeightMultiplier = s_studio.lineHeightMultiplier;
    spec.borderScale = s_studio.borderScale;
    spec.shadowIntensity = s_studio.shadowIntensity;
    spec.focusRingWidth = s_studio.focusRingWidth;
    spec.disabledOpacity = s_studio.disabledOpacity;
    spec.animationScale = s_studio.animationScale;
    spec.shellStyle = s_studio.shellStyle;
    return spec;
}

[[nodiscard]] guikit::UiTheme BuildEffectiveTheme() noexcept {
    guikit::UiTheme primary = guikit::UiTheme::Build(BuildCurrentSpec());
    if (s_studio.customizeScrollbarTokens) {
        primary.SetScrollbarStyle(s_studio.customScrollbarTokens);
        if (s_studio.overrideScrollbarWidth) {
            primary.SetScrollbarWidth(s_studio.scrollbarWidth);
        }
    }
    if (s_studio.enableLerp && s_studio.lerpT > 0.001f) {
        const guikit::UiTheme target = guikit::UiTheme::FromPreset(s_studio.lerpTarget);
        return guikit::UiTheme::Lerp(primary, target, s_studio.lerpT);
    }
    return primary;
}

void RandomizeHarmonicTheme() noexcept {
    s_studio.randomSeed = s_studio.randomSeed * 1664525u + 1013904223u;
    const keel::f32 hue = keel::as<keel::f32>(s_studio.randomSeed % 360u);
    const bool isLight = ((s_studio.randomSeed >> 9) & 1u) != 0u;
    const keel::u32 styleIdx = (s_studio.randomSeed >> 10) % 3u; // Solid, Soft, Outline

    ResetToDefaults();
    s_studio.preset = isLight ? guikit::ThemePreset::Light : guikit::ThemePreset::Dark;
    s_studio.chrome = guikit::ChromeStyle::Modern;
    s_studio.fillStyle = keel::from_underlying<guikit::FillStyle>(styleIdx);
    s_studio.overrideAccent = true;
    s_studio.accentColor = guikit::Color::FromHsl(hue, isLight ? 0.72f : 0.78f, isLight ? 0.42f : 0.58f);
    s_studio.overrideNeutral = true;
    s_studio.neutralColor = guikit::Color::FromHsl(hue, 0.22f, isLight ? 0.94f : 0.14f);
    s_studio.radiusScale = 0.65f + keel::as<keel::f32>((s_studio.randomSeed >> 13) % 10u) * 0.10f;
}

[[nodiscard]] constexpr keel::StringRef PresetEnumIdentifier(guikit::ThemePreset p) noexcept {
    switch (p) {
        case guikit::ThemePreset::Dark:              return "Dark";
        case guikit::ThemePreset::Light:             return "Light";
        case guikit::ThemePreset::Nord:              return "Nord";
        case guikit::ThemePreset::Dracula:           return "Dracula";
        case guikit::ThemePreset::CatppuccinMocha:   return "CatppuccinMocha";
        case guikit::ThemePreset::CatppuccinLatte:   return "CatppuccinLatte";
        case guikit::ThemePreset::Industrial:        return "Industrial";
        case guikit::ThemePreset::IndustrialLight:   return "IndustrialLight";
        case guikit::ThemePreset::HighContrastDark:  return "HighContrastDark";
        case guikit::ThemePreset::HighContrastLight: return "HighContrastLight";
        case guikit::ThemePreset::Windows98:         return "Windows98";
    }
    return "Dark";
}

[[nodiscard]] constexpr keel::StringRef ChromeStyleEnumIdentifier(guikit::ChromeStyle s) noexcept {
    switch (s) {
        case guikit::ChromeStyle::Modern:    return "Modern";
        case guikit::ChromeStyle::Fluent:    return "Fluent";
        case guikit::ChromeStyle::Classic3D: return "Classic3D";
        case guikit::ChromeStyle::Brutalist: return "Brutalist";
    }
    return "Modern";
}

[[nodiscard]] constexpr keel::StringRef FillStyleEnumIdentifier(guikit::FillStyle s) noexcept {
    switch (s) {
        case guikit::FillStyle::Solid:   return "Solid";
        case guikit::FillStyle::Soft:    return "Soft";
        case guikit::FillStyle::Outline: return "Outline";
    }
    return "Solid";
}

[[nodiscard]] constexpr keel::StringRef SyntaxPresetEnumIdentifier(guikit::SyntaxPreset s) noexcept {
    switch (s) {
        case guikit::SyntaxPreset::Auto:          return "Auto";
        case guikit::SyntaxPreset::VsCodeDark:    return "VsCodeDark";
        case guikit::SyntaxPreset::Monokai:       return "Monokai";
        case guikit::SyntaxPreset::Dracula:       return "Dracula";
        case guikit::SyntaxPreset::OneDark:       return "OneDark";
        case guikit::SyntaxPreset::Solarized:     return "Solarized";
        case guikit::SyntaxPreset::GitHubLight:   return "GitHubLight";
        case guikit::SyntaxPreset::MatlabClassic: return "MatlabClassic";
    }
    return "Auto";
}

[[nodiscard]] constexpr keel::StringRef ScrollbarPresetEnumIdentifier(guikit::ScrollbarPreset p) noexcept {
    switch (p) {
        case guikit::ScrollbarPreset::Auto:         return "Auto";
        case guikit::ScrollbarPreset::Modern:       return "Modern";
        case guikit::ScrollbarPreset::Classic1988:  return "Classic1988";
        case guikit::ScrollbarPreset::Windows98:    return "Windows98";
        case guikit::ScrollbarPreset::WindowsXp:    return "WindowsXp";
        case guikit::ScrollbarPreset::WindowsVista: return "WindowsVista";
        case guikit::ScrollbarPreset::Windows7:     return "Windows7";
        case guikit::ScrollbarPreset::WindowsFlat:  return "WindowsFlat";
    }
    return "Auto";
}

[[nodiscard]] constexpr keel::StringRef ScrollbarChromeLabel(guikit::ScrollbarChromeStyle c) noexcept {
    switch (c) {
        case guikit::ScrollbarChromeStyle::Modern:       return "Modern";
        case guikit::ScrollbarChromeStyle::ClassicBevel: return "Classic 3D";
        case guikit::ScrollbarChromeStyle::Outlined:     return "Outlined";
        case guikit::ScrollbarChromeStyle::Flat:         return "Flat";
    }
    return "Modern";
}

[[nodiscard]] keel::FixedString<16> FormatHexLiteral(guikit::Color c) noexcept {
    return keel::format_fixed<16>("0x{:02X}{:02X}{:02X}{:02X}", c.r, c.g, c.b, c.a);
}

[[nodiscard]] keel::FixedString<10> FormatHexLabel(guikit::Color c) noexcept {
    return keel::format_fixed<10>("#{:02X}{:02X}{:02X}", c.r, c.g, c.b);
}

[[nodiscard]] guikit::UiVariant ContrastBadgeVariant(keel::f32 ratio) noexcept {
    if (ratio >= 7.0f) return guikit::UiVariant::Success;
    if (ratio >= 4.5f) return guikit::UiVariant::Info;
    if (ratio >= 3.0f) return guikit::UiVariant::Warning;
    return guikit::UiVariant::Danger;
}

[[nodiscard]] keel::StringRef ContrastGradeLabel(keel::f32 ratio) noexcept {
    if (ratio >= 7.0f) return "AAA";
    if (ratio >= 4.5f) return "AA";
    if (ratio >= 3.0f) return "AA Lg";
    return "Fail";
}

void RenderColorSwatchCard(guikit::Context& ui, keel::StringRef name, guikit::Color color, guikit::Color borderCol) {
    const guikit::Color ink = guikit::Color::BestTextColor(color, guikit::Color::Hex(0x0F172AFF), guikit::Color::Hex(0xFFFFFFFF));
    if (auto tile = guikit::Box(ui, {
        .width = 128.0f,
        .padding = guikit::Padding::Symmetric(10.0f, 8.0f),
        .background = guikit::StateColor{color},
        .border = {.width = 1.0f, .color = guikit::StateColor{borderCol}}
    })) {
        if (auto col = guikit::Column(ui, {.gap = 2.0f})) {
            guikit::Text(ui, guikit::UiText{name, guikit::StateColor{ink}}, {.role = guikit::TextRole::LabelSmall});
            guikit::Text(ui, guikit::UiText{FormatHexLabel(color).as_ref(), guikit::StateColor{ink.WithAlphaF(0.85f)}}, {.role = guikit::TextRole::CodeSmall});
        }
    }
}

void RenderColorOverrideRow(
    guikit::Context& ui,
    keel::StringRef label,
    bool& enabled,
    guikit::Color& color,
    guikit::Color defaultSeed
) {
    if (auto el = guikit::Element(ui, {.label = label, .layout = guikit::ElementLayout::Leading})) {
        if (guikit::Switch(ui, enabled, {.size = guikit::UiSize::Sm}).changed) {
            if (enabled) {
                color = defaultSeed;
            }
            ui.InvalidateAll();
        }
    }
    if (enabled) {
        if (guikit::ColorPicker(ui, color, {.showHex = true, .showAlpha = false, .showSwatches = true, .size = guikit::UiSize::Sm}).changed) {
            ui.InvalidateAll();
        }
    }
}

} // namespace

void SyncPreviewThemeFromStudio(DemoContext& ctx) noexcept {
    ctx.previewTheme = BuildEffectiveTheme();
    ctx.previewLightMode = (ctx.previewTheme.mode == guikit::ThemeMode::Light);
    ctx.previewDarkMode = !ctx.previewLightMode;
}

void SelectStudioPreset(DemoContext& ctx, guikit::ThemePreset preset) noexcept {
    SelectBasePreset(preset);
    SyncPreviewThemeFromStudio(ctx);
}

void SelectStudioChrome(DemoContext& ctx, guikit::ChromeStyle chrome) noexcept {
    s_studio.chrome = chrome;
    SyncPreviewThemeFromStudio(ctx);
}

void SelectStudioFillStyle(DemoContext& ctx, guikit::FillStyle fill) noexcept {
    s_studio.fillStyle = fill;
    SyncPreviewThemeFromStudio(ctx);
}

void ToggleStudioDarkLight(DemoContext& ctx) noexcept {
    const bool nextLight = (ctx.previewTheme.mode == guikit::ThemeMode::Dark);
    SelectBasePreset(nextLight ? guikit::ThemePreset::Light : guikit::ThemePreset::Dark);
    SyncPreviewThemeFromStudio(ctx);
}

guikit::ThemePreset GetStudioPreset() noexcept {
    return s_studio.preset;
}

guikit::ChromeStyle GetStudioChrome() noexcept {
    return s_studio.chrome;
}

guikit::FillStyle GetStudioFillStyle() noexcept {
    return s_studio.fillStyle;
}

void RenderThemeStudioInspector(guikit::Context& ui, DemoContext& ctx) {
    static constexpr keel::StaticArray<keel::StringRef, guikit::kThemePresetCount> kPresetNames = {{
        guikit::ThemePresetName(guikit::ThemePreset::Dark),
        guikit::ThemePresetName(guikit::ThemePreset::Light),
        guikit::ThemePresetName(guikit::ThemePreset::Nord),
        guikit::ThemePresetName(guikit::ThemePreset::Dracula),
        guikit::ThemePresetName(guikit::ThemePreset::CatppuccinMocha),
        guikit::ThemePresetName(guikit::ThemePreset::CatppuccinLatte),
        guikit::ThemePresetName(guikit::ThemePreset::Industrial),
        guikit::ThemePresetName(guikit::ThemePreset::IndustrialLight),
        guikit::ThemePresetName(guikit::ThemePreset::HighContrastDark),
        guikit::ThemePresetName(guikit::ThemePreset::HighContrastLight),
        guikit::ThemePresetName(guikit::ThemePreset::Windows98)
    }};

    static constexpr keel::StaticArray<keel::StringRef, guikit::kChromeStyleCount> kChromeNames = {{
        guikit::ChromeStyleName(guikit::ChromeStyle::Modern),
        guikit::ChromeStyleName(guikit::ChromeStyle::Fluent),
        guikit::ChromeStyleName(guikit::ChromeStyle::Classic3D),
        guikit::ChromeStyleName(guikit::ChromeStyle::Brutalist)
    }};

    static constexpr keel::StaticArray<keel::StringRef, guikit::kFillStyleCount> kFillNames = {{
        guikit::FillStyleName(guikit::FillStyle::Solid),
        guikit::FillStyleName(guikit::FillStyle::Soft),
        guikit::FillStyleName(guikit::FillStyle::Outline)
    }};

    static constexpr keel::StaticArray<keel::StringRef, guikit::kSyntaxPresetCount> kSyntaxNames = {{
        guikit::SyntaxPresetName(guikit::SyntaxPreset::Auto),
        guikit::SyntaxPresetName(guikit::SyntaxPreset::VsCodeDark),
        guikit::SyntaxPresetName(guikit::SyntaxPreset::Monokai),
        guikit::SyntaxPresetName(guikit::SyntaxPreset::Dracula),
        guikit::SyntaxPresetName(guikit::SyntaxPreset::OneDark),
        guikit::SyntaxPresetName(guikit::SyntaxPreset::Solarized),
        guikit::SyntaxPresetName(guikit::SyntaxPreset::GitHubLight),
        guikit::SyntaxPresetName(guikit::SyntaxPreset::MatlabClassic)
    }};

    // Top Quick Action Strip (always accessible)
    if (auto topActions = guikit::Row(ui, {.gap = 6.0f})) {
        if (guikit::Button(ui, {
            .text = "Apply to App",
            .icon = guikit::Icon::Check(),
            .tooltip = "Apply the crafted ThemeSpec live to the entire Demo application",
            .size = guikit::UiSize::Sm,
            .width = guikit::SizeMode::Stretch,
            .variant = guikit::UiVariant::Primary
        }).clicked) {
            ctx.pendingShellTheme = BuildEffectiveTheme();
            ui.InvalidateAll();
        }
        if (guikit::Button(ui, {
            .icon = guikit::Icon::Flash(),
            .square = true,
            .tooltip = "Generate a harmonic WCAG-compliant theme",
            .size = guikit::UiSize::Sm,
            .variant = guikit::UiVariant::Info
        }).clicked) {
            RandomizeHarmonicTheme();
            ui.InvalidateAll();
        }
        if (guikit::Button(ui, {
            .icon = guikit::Icon::Refresh(),
            .square = true,
            .tooltip = "Reset all tokens to default",
            .size = guikit::UiSize::Sm,
            .variant = guikit::UiVariant::Secondary
        }).clicked) {
            ResetToDefaults();
            ui.InvalidateAll();
        }
    }

    // 1. THEME PRESET
    if (auto acc = guikit::Accordion(ui, {.label = "THEME PRESET", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize presetIdx = keel::as<keel::usize>(s_studio.preset);
        if (auto el = guikit::Element(ui, {.label = "Base Preset"})) {
            if (guikit::Select(ui, presetIdx, kPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                SelectBasePreset(keel::from_underlying<guikit::ThemePreset>(presetIdx));
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {
            .label = "Orthogonal Cross-Combos",
            .description = "Any color palette combined with any ChromeStyle & FillStyle"
        })) {
            auto applyCombo = [&](guikit::ThemePreset p, guikit::ChromeStyle c, guikit::FillStyle f) {
                SelectBasePreset(p);
                s_studio.chrome = c;
                s_studio.fillStyle = f;
                s_studio.scrollbarPreset = guikit::ScrollbarPreset::Auto;
                const auto tok = guikit::ChromeTokens::FromStyle(c);
                s_studio.customScrollbarTokens = guikit::ScrollbarStyleTokens::ForChrome(tok.scrollbar);
                ui.InvalidateAll();
            };
            if (auto w = guikit::Wrap(ui, {.gap = 4.0f, .runGap = 4.0f})) {
                if (guikit::Button(ui, {.text = "Dracula x 3D", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Primary}).clicked) {
                    applyCombo(guikit::ThemePreset::Dracula, guikit::ChromeStyle::Classic3D, guikit::FillStyle::Soft);
                }
                if (guikit::Button(ui, {.text = "Dark x Fluent", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Info}).clicked) {
                    applyCombo(guikit::ThemePreset::Dark, guikit::ChromeStyle::Fluent, guikit::FillStyle::Solid);
                }
                if (guikit::Button(ui, {.text = "Win98 x Modern", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Success}).clicked) {
                    applyCombo(guikit::ThemePreset::Windows98, guikit::ChromeStyle::Modern, guikit::FillStyle::Solid);
                }
                if (guikit::Button(ui, {.text = "Industrial x Brutal", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Warning}).clicked) {
                    applyCombo(guikit::ThemePreset::Industrial, guikit::ChromeStyle::Brutalist, guikit::FillStyle::Solid);
                }
            }
        }
    }

    // 2. APPEARANCE & PALETTE
    const guikit::UiTheme baseRef = guikit::UiTheme::FromPreset(s_studio.preset);
    if (auto acc = guikit::Accordion(ui, {.label = "APPEARANCE & PALETTE", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize chromeIdx = keel::as<keel::usize>(s_studio.chrome);
        if (auto el = guikit::Element(ui, {.label = "Chrome Style", .description = "Structural look-and-feel (color-independent)"})) {
            if (guikit::Select(ui, chromeIdx, kChromeNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.chrome = keel::from_underlying<guikit::ChromeStyle>(chromeIdx);
                ui.InvalidateAll();
            }
        }

        keel::usize fillIdx = keel::as<keel::usize>(s_studio.fillStyle);
        if (auto el = guikit::Element(ui, {.label = "Fill Style", .description = "Semantic emphasis fill (Solid / Soft / Outline)"})) {
            if (guikit::Select(ui, fillIdx, kFillNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.fillStyle = keel::from_underlying<guikit::FillStyle>(fillIdx);
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Layer Contrast", .description = "Separation between bg, cards & borders"})) {
            if (guikit::Slider(ui, s_studio.surfaceContrast, 0.50f, 1.80f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }

        RenderColorOverrideRow(ui, "Brand Accent", s_studio.overrideAccent, s_studio.accentColor, baseRef.palette.accent);
        RenderColorOverrideRow(ui, "Neutral Tint", s_studio.overrideNeutral, s_studio.neutralColor, baseRef.palette.background);

        keel::usize synIdx = keel::as<keel::usize>(s_studio.syntaxPreset);
        if (auto el = guikit::Element(ui, {.label = "Syntax Scheme"})) {
            if (guikit::Select(ui, synIdx, kSyntaxNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.syntaxPreset = keel::from_underlying<guikit::SyntaxPreset>(synIdx);
                ui.InvalidateAll();
            }
        }
    }

    // 2b. SCROLLBAR CUSTOMIZATION
    static constexpr keel::StaticArray<keel::StringRef, guikit::kScrollbarPresetCount> kScrollbarPresetNames = {{
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::Auto),
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::Modern),
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::Classic1988),
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::Windows98),
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::WindowsXp),
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::WindowsVista),
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::Windows7),
        guikit::ScrollbarPresetName(guikit::ScrollbarPreset::WindowsFlat)
    }};
    static constexpr keel::StaticArray<keel::StringRef, 4> kSbChromeNames = {
        "Modern", "Classic 3D", "Outlined", "Flat"
    };
    static constexpr keel::StaticArray<keel::StringRef, 4> kSbArrowNames = {
        "None", "Triangle", "Chevron", "Arrow"
    };
    static constexpr keel::StaticArray<keel::StringRef, 3> kSbGripNames = {
        "None", "Lines", "Dots"
    };
    if (auto acc = guikit::Accordion(ui, {.label = "SCROLLBAR", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize sbPresetIdx = keel::as<keel::usize>(s_studio.scrollbarPreset);
        if (auto el = guikit::Element(ui, {.label = "Scrollbar Preset", .description = "Windows 1988–2012 or Modern"})) {
            if (guikit::Select(ui, sbPresetIdx, kScrollbarPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.scrollbarPreset = keel::from_underlying<guikit::ScrollbarPreset>(sbPresetIdx);
                if (s_studio.scrollbarPreset == guikit::ScrollbarPreset::Auto) {
                    const auto chromeTok = guikit::ChromeTokens::FromStyle(s_studio.chrome);
                    s_studio.customScrollbarTokens = guikit::ScrollbarStyleTokens::ForChrome(chromeTok.scrollbar);
                } else {
                    s_studio.customScrollbarTokens = guikit::ScrollbarStyleTokens::FromPreset(s_studio.scrollbarPreset);
                }
                s_studio.customizeScrollbarTokens = false;
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Custom Width", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, s_studio.overrideScrollbarWidth, {.size = guikit::UiSize::Sm}).changed) {
                if (s_studio.overrideScrollbarWidth) {
                    s_studio.scrollbarWidth = BuildEffectiveTheme().GetScrollbar(guikit::UiSize::Md).trackWidth;
                }
                ui.InvalidateAll();
            }
        }
        if (s_studio.overrideScrollbarWidth) {
            if (guikit::Slider(ui, s_studio.scrollbarWidth, 4.0f, 24.0f, {.valueFormat = "%.0f dp", .step = 1.0f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Fine-Tune Tokens", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, s_studio.customizeScrollbarTokens, {.size = guikit::UiSize::Sm}).changed) {
                if (s_studio.customizeScrollbarTokens) {
                    s_studio.customScrollbarTokens = BuildEffectiveTheme().scrollbarStyle;
                }
                ui.InvalidateAll();
            }
        }
        if (s_studio.customizeScrollbarTokens) {
            auto& st = s_studio.customScrollbarTokens;
            keel::usize chromeIdx = keel::as<keel::usize>(st.chrome);
            if (auto el = guikit::Element(ui, {.label = "Chrome Style"})) {
                if (guikit::Select(ui, chromeIdx, kSbChromeNames, {.size = guikit::UiSize::Sm}).changed) {
                    st.chrome = keel::from_underlying<guikit::ScrollbarChromeStyle>(chromeIdx);
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Arrow Buttons", .layout = guikit::ElementLayout::Leading})) {
                if (guikit::Switch(ui, st.showButtons, {.size = guikit::UiSize::Sm}).changed) {
                    if (st.showButtons && st.arrowStyle == guikit::ScrollbarArrowStyle::None) {
                        st.arrowStyle = guikit::ScrollbarArrowStyle::Triangle;
                    }
                    ui.InvalidateAll();
                }
            }
            if (st.showButtons) {
                keel::usize arrowIdx = keel::as<keel::usize>(st.arrowStyle);
                if (auto el = guikit::Element(ui, {.label = "Arrow Glyph"})) {
                    if (guikit::Select(ui, arrowIdx, kSbArrowNames, {.size = guikit::UiSize::Sm}).changed) {
                        st.arrowStyle = keel::from_underlying<guikit::ScrollbarArrowStyle>(arrowIdx);
                        ui.InvalidateAll();
                    }
                }
            }
            keel::usize gripIdx = keel::as<keel::usize>(st.gripStyle);
            if (auto el = guikit::Element(ui, {.label = "Thumb Grip"})) {
                if (guikit::Select(ui, gripIdx, kSbGripNames, {.size = guikit::UiSize::Sm}).changed) {
                    st.gripStyle = keel::from_underlying<guikit::ScrollbarGripStyle>(gripIdx);
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Thumb Inset"})) {
                if (guikit::Slider(ui, st.thumbInset, 0.0f, 4.0f, {.valueFormat = "%.1f dp", .step = 0.5f, .size = guikit::UiSize::Sm}).changed) {
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Border Width"})) {
                if (guikit::Slider(ui, st.borderWidth, 0.0f, 3.0f, {.valueFormat = "%.1f dp", .step = 0.5f, .size = guikit::UiSize::Sm}).changed) {
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Always Show Track", .layout = guikit::ElementLayout::Leading})) {
                if (guikit::Switch(ui, st.alwaysShowTrack, {.size = guikit::UiSize::Sm}).changed) {
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Track Hatch", .layout = guikit::ElementLayout::Leading})) {
                if (guikit::Switch(ui, st.trackHatch, {.size = guikit::UiSize::Sm}).changed) {
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Corner Box", .layout = guikit::ElementLayout::Leading})) {
                if (guikit::Switch(ui, st.showCornerBox, {.size = guikit::UiSize::Sm}).changed) {
                    ui.InvalidateAll();
                }
            }
        }
    }

    // 3. DIRECT SURFACE & SEMANTIC COLORS
    if (auto acc = guikit::Accordion(ui, {.label = "SURFACE & SEMANTIC TOKENS", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        RenderColorOverrideRow(ui, "Background",  s_studio.overrideBackground, s_studio.backgroundColor, baseRef.palette.background);
        RenderColorOverrideRow(ui, "Card Surface", s_studio.overrideSurface, s_studio.surfaceColor, baseRef.palette.surface);
        RenderColorOverrideRow(ui, "Border Color", s_studio.overrideBorder, s_studio.borderColor, baseRef.palette.border);
        RenderColorOverrideRow(ui, "Primary Text", s_studio.overrideText, s_studio.textColor, baseRef.palette.textPrimary);
        guikit::Separator(ui);
        RenderColorOverrideRow(ui, "Success", s_studio.overrideSuccess, s_studio.successColor, baseRef.GetSemanticAccent(guikit::UiVariant::Success));
        RenderColorOverrideRow(ui, "Warning", s_studio.overrideWarning, s_studio.warningColor, baseRef.GetSemanticAccent(guikit::UiVariant::Warning));
        RenderColorOverrideRow(ui, "Danger",  s_studio.overrideDanger,  s_studio.dangerColor,  baseRef.GetSemanticAccent(guikit::UiVariant::Danger));
        RenderColorOverrideRow(ui, "Info",    s_studio.overrideInfo,    s_studio.infoColor,    baseRef.GetSemanticAccent(guikit::UiVariant::Info));
    }

    // 4. GEOMETRY & ELEVATION
    if (auto acc = guikit::Accordion(ui, {.label = "GEOMETRY & ELEVATION", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = guikit::Element(ui, {.label = "Radius Scale"})) {
            if (guikit::Slider(ui, s_studio.radiusScale, 0.0f, 2.0f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto rRow = guikit::Row(ui, {.gap = 4.0f, .equalWidths = true})) {
            if (guikit::Button(ui, {.text = "Sharp", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { s_studio.radiusScale = 0.0f; ui.InvalidateAll(); }
            if (guikit::Button(ui, {.text = "Crisp", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { s_studio.radiusScale = 0.5f; ui.InvalidateAll(); }
            if (guikit::Button(ui, {.text = "Std",   .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { s_studio.radiusScale = 1.0f; ui.InvalidateAll(); }
            if (guikit::Button(ui, {.text = "Soft",  .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { s_studio.radiusScale = 1.6f; ui.InvalidateAll(); }
        }
        if (auto el = guikit::Element(ui, {.label = "Spacing Scale"})) {
            if (guikit::Slider(ui, s_studio.spacingScale, 0.70f, 1.40f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Font Scale"})) {
            if (guikit::Slider(ui, s_studio.fontScale, 0.85f, 1.35f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Line Height"})) {
            if (guikit::Slider(ui, s_studio.lineHeightMultiplier, 1.10f, 1.75f, {.valueFormat = "%.2f", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Border Width Scale"})) {
            if (guikit::Slider(ui, s_studio.borderScale, 0.0f, 2.5f, {.valueFormat = "%.1fx", .step = 0.1f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Shadow Intensity", .description = "0.00x = Flat / No shadows"})) {
            if (guikit::Slider(ui, s_studio.shadowIntensity, 0.0f, 1.6f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        keel::i32 shellIdx = (s_studio.shellStyle == guikit::ShellStyle::Floating) ? 1 : 0;
        static constexpr keel::StaticArray<keel::StringRef, 2> kShellNames = {"Docked", "Floating"};
        if (auto el = guikit::Element(ui, {.label = "Shell Chrome"})) {
            if (guikit::Segmented(ui, shellIdx, kShellNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.shellStyle = (shellIdx == 1) ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked;
                ui.InvalidateAll();
            }
        }
    }

    // 5. ACCESSIBILITY & MOTION
    if (auto acc = guikit::Accordion(ui, {.label = "ACCESSIBILITY & MOTION", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = guikit::Element(ui, {.label = "Focus Ring Width", .description = "Keyboard focus stroke in dp"})) {
            if (guikit::Slider(ui, s_studio.focusRingWidth, 1.0f, 4.5f, {.valueFormat = "%.1f dp", .step = 0.5f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Disabled Opacity"})) {
            if (guikit::Slider(ui, s_studio.disabledOpacity, 0.20f, 0.80f, {.valueFormat = "%.2f", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Animation Scale", .description = "0.00x enables Reduced Motion"})) {
            if (guikit::Slider(ui, s_studio.animationScale, 0.0f, 2.0f, {.valueFormat = "%.2fx", .step = 0.1f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
    }

    // 6. THEME MORPHING (UiTheme::Lerp)
    if (auto acc = guikit::Accordion(ui, {.label = "THEME MORPHING (LERP)", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = guikit::Element(ui, {.label = "Enable Blend", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, s_studio.enableLerp, {.size = guikit::UiSize::Sm}).changed) {
                if (s_studio.enableLerp && s_studio.lerpT < 0.01f) s_studio.lerpT = 0.5f;
                ui.InvalidateAll();
            }
        }
        if (s_studio.enableLerp) {
            keel::usize targetIdx = keel::as<keel::usize>(s_studio.lerpTarget);
            if (auto el = guikit::Element(ui, {.label = "Target Theme B"})) {
                if (guikit::Select(ui, targetIdx, kPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                    s_studio.lerpTarget = keel::from_underlying<guikit::ThemePreset>(targetIdx);
                    ui.InvalidateAll();
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Blend Factor t"})) {
                if (guikit::Slider(ui, s_studio.lerpT, 0.0f, 1.0f, {.valueFormat = "%.0f%%", .step = 0.02f, .size = guikit::UiSize::Sm}).changed) {
                    ui.InvalidateAll();
                }
            }
        }
    }

    // 7. JSON IMPORT / EXPORT
    if (auto acc = guikit::Accordion(ui, {.label = "JSON IMPORT & CODE", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        static constexpr keel::StaticArray<keel::StringRef, 2> kCodeFormats = {"C++20", "JSON"};
        if (auto el = guikit::Element(ui, {.label = "Bottom Code Panel"})) {
            if (guikit::Segmented(ui, s_studio.codeFormatIdx, kCodeFormats, {.size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (s_studio.jsonDraft.empty()) {
            s_studio.jsonDraft = BuildCurrentSpec().ToJson();
        }
        if (auto btnRow = guikit::Row(ui, {.gap = 6.0f, .equalWidths = true})) {
            if (guikit::Button(ui, {.text = "Snapshot", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) {
                s_studio.jsonDraft = BuildCurrentSpec().ToJson();
                s_studio.jsonImportStatus = 0;
                ui.InvalidateAll();
            }
            if (guikit::Button(ui, {.text = "Load JSON", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Primary}).clicked) {
                const auto parsed = guikit::ThemeSpec::FromJson(s_studio.jsonDraft);
                if (parsed.has_value()) {
                    ApplySpecToStudioState(*parsed);
                    s_studio.jsonImportStatus = 1;
                } else {
                    s_studio.jsonImportStatus = -1;
                }
                ui.InvalidateAll();
            }
        }
        if (s_studio.jsonImportStatus == 1) {
            guikit::Tag(ui, "JSON Applied", {.variant = guikit::UiVariant::Success, .size = guikit::UiSize::Xs});
        } else if (s_studio.jsonImportStatus == -1) {
            guikit::Tag(ui, "Invalid JSON syntax", {.variant = guikit::UiVariant::Danger, .size = guikit::UiSize::Xs});
        }
        guikit::TextArea(ui, s_studio.jsonDraft, {
            .autoHeight = false,
            .showLineNumbers = false,
            .monospace = true,
            .height = 132.0f
        });
    }

    // Keep global Preview Zone theme continuously synchronized with Theme Studio
    SyncPreviewThemeFromStudio(ctx);
}

void RenderThemeStudioStage(guikit::Context& ui, DemoContext& ctx) {
    const guikit::UiTheme& built = ctx.previewTheme;

    if (auto bg = guikit::Box(ui, {
            .gap = 0.0f,
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch,
            .background = guikit::StateColor{built.palette.background}
        })) {
            // Live Application Chrome Preview (MenuBar + Toolbar under the active theme)
            guikit::ContextMenuOptions dropOpts{};
            dropOpts.size = guikit::UiSize::Sm;
            if (auto mb = guikit::MenuBar(ui, {.size = guikit::UiSize::Sm})) {
                if (auto file = mb.Menu("File", dropOpts)) {
                    if (file.Item("Snapshot JSON", {.icon = guikit::Icon::Save()})) {
                        s_studio.jsonDraft = BuildCurrentSpec().ToJson();
                        s_studio.jsonImportStatus = 0;
                        ui.InvalidateAll();
                    }
                    if (file.Item("Copy Theme JSON", {.icon = guikit::Icon::Copy()})) {
                        const keel::String json = BuildCurrentSpec().ToJson();
                        ui.SetClipboardText(json.as_ref());
                        guikit::ShowToast(ui, {
                            .message = "Copied ThemeSpec JSON to clipboard",
                            .variant = guikit::UiVariant::Success
                        });
                    }
                    file.Separator();
                    if (file.Item("Reset to Defaults", {.icon = guikit::Icon::Refresh(), .variant = guikit::UiVariant::Danger})) {
                        ResetToDefaults();
                        SyncPreviewThemeFromStudio(ctx);
                        ui.InvalidateAll();
                    }
                }

                if (auto themeMenu = mb.Menu("Theme", dropOpts)) {
                    if (auto presetsSub = themeMenu.SubMenu("Base Preset", {.icon = guikit::Icon::Eye()})) {
                        keel::i32 presetIdx = keel::as<keel::i32>(s_studio.preset);
                        for (keel::usize pi = 0; pi < guikit::kThemePresetCount; ++pi) {
                            const auto p = keel::from_underlying<guikit::ThemePreset>(pi);
                            if (presetsSub.Radio(guikit::ThemePresetName(p), presetIdx, keel::as<keel::i32>(pi))) {
                                SelectStudioPreset(ctx, p);
                                ui.InvalidateAll();
                            }
                        }
                    }
                    themeMenu.Separator();
                    if (themeMenu.Item("Harmonic Randomize", {.icon = guikit::Icon::Flash()})) {
                        RandomizeHarmonicTheme();
                        SyncPreviewThemeFromStudio(ctx);
                        ui.InvalidateAll();
                    }
                    if (themeMenu.Checkbox("Theme Morph (Lerp)", s_studio.enableLerp, {.keepOpen = true})) {
                        SyncPreviewThemeFromStudio(ctx);
                        ui.InvalidateAll();
                    }
                }

                if (auto surfMenu = mb.Menu("Chrome", dropOpts)) {
                    surfMenu.Section("CHROME STYLE");
                    keel::i32 chrIdx = keel::as<keel::i32>(s_studio.chrome);
                    for (keel::i32 ci = 0; ci < keel::as<keel::i32>(guikit::kChromeStyleCount); ++ci) {
                        const auto cs = keel::from_underlying<guikit::ChromeStyle>(ci);
                        if (surfMenu.Radio(guikit::ChromeStyleName(cs), chrIdx, ci)) {
                            SelectStudioChrome(ctx, cs);
                            ui.InvalidateAll();
                        }
                    }
                    surfMenu.Separator();
                    surfMenu.Section("FILL STYLE");
                    keel::i32 fIdx = keel::as<keel::i32>(s_studio.fillStyle);
                    for (keel::i32 fi = 0; fi < keel::as<keel::i32>(guikit::kFillStyleCount); ++fi) {
                        const auto fs = keel::from_underlying<guikit::FillStyle>(fi);
                        if (surfMenu.Radio(guikit::FillStyleName(fs), fIdx, fi)) {
                            SelectStudioFillStyle(ctx, fs);
                            ui.InvalidateAll();
                        }
                    }
                    surfMenu.Separator();
                    surfMenu.Section("SHELL STYLE");
                    keel::i32 shellIdx = (s_studio.shellStyle == guikit::ShellStyle::Floating) ? 1 : 0;
                    if (surfMenu.Radio("Docked", shellIdx, 0)) {
                        s_studio.shellStyle = guikit::ShellStyle::Docked;
                        SyncPreviewThemeFromStudio(ctx);
                        ui.InvalidateAll();
                    }
                    if (surfMenu.Radio("Floating", shellIdx, 1)) {
                        s_studio.shellStyle = guikit::ShellStyle::Floating;
                        SyncPreviewThemeFromStudio(ctx);
                        ui.InvalidateAll();
                    }
                }

                if (auto viewMenu = mb.Menu("View", dropOpts)) {
                    viewMenu.Section("STAGE SECTION");
                    keel::i32 tabIdx = keel::as<keel::i32>(s_studio.activeStageTab);
                    if (viewMenu.Radio("Full Showcase", tabIdx, 0))     { s_studio.activeStageTab = 0u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Token Swatches", tabIdx, 1))    { s_studio.activeStageTab = 1u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Controls & States", tabIdx, 2)) { s_studio.activeStageTab = 2u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Zones & Syntax", tabIdx, 3))    { s_studio.activeStageTab = 3u; ui.InvalidateAll(); }
                }

                mb.Spacer();
                guikit::Tag(ui, guikit::ThemePresetName(built.preset), {
                    .variant = guikit::UiVariant::Primary,
                    .size = guikit::UiSize::Xs
                });
            }

            if (auto tb = guikit::Toolbar(ui, {.size = guikit::UiSize::Sm})) {
                if (tb.Action({
                    .text = "Randomize",
                    .tooltip = "Generate a harmonic color scheme",
                    .icon = guikit::Icon::Refresh(),
                    .style = guikit::ButtonStyle::Solid,
                    .variant = guikit::UiVariant::Primary
                }).clicked) {
                    RandomizeHarmonicTheme();
                    ui.InvalidateAll();
                }
                if (tb.Action({
                    .text = "Reset",
                    .tooltip = "Reset all theme studio overrides",
                    .icon = guikit::Icon::Close(),
                    .style = guikit::ButtonStyle::Default,
                    .variant = guikit::UiVariant::Secondary
                }).clicked) {
                    ResetToDefaults();
                    ui.InvalidateAll();
                }

                tb.Separator();

                keel::i32 chrTool = keel::as<keel::i32>(s_studio.chrome);
                bool chrChanged = false;
                chrChanged |= tb.Item(chrTool, 0, {.text = "Modern"}).changed;
                chrChanged |= tb.Item(chrTool, 1, {.text = "Fluent"}).changed;
                chrChanged |= tb.Item(chrTool, 2, {.text = "Classic 3D"}).changed;
                chrChanged |= tb.Item(chrTool, 3, {.text = "Brutalist"}).changed;
                if (chrChanged) {
                    s_studio.chrome = keel::from_underlying<guikit::ChromeStyle>(chrTool);
                    ui.InvalidateAll();
                }

                tb.Separator();

                keel::i32 fillTool = keel::as<keel::i32>(s_studio.fillStyle);
                bool fillChanged = false;
                fillChanged |= tb.Item(fillTool, 0, {.text = "Solid"}).changed;
                fillChanged |= tb.Item(fillTool, 1, {.text = "Soft"}).changed;
                fillChanged |= tb.Item(fillTool, 2, {.text = "Outline"}).changed;
                if (fillChanged) {
                    s_studio.fillStyle = keel::from_underlying<guikit::FillStyle>(fillTool);
                    ui.InvalidateAll();
                }

                tb.Separator();

                bool isFloating = (s_studio.shellStyle == guikit::ShellStyle::Floating);
                if (tb.Toggle(isFloating, {
                    .text = "Floating",
                    .tooltip = "Toggle Docked vs Floating shell panels",
                    .variant = guikit::UiVariant::Info
                }).changed) {
                    s_studio.shellStyle = isFloating ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked;
                    ui.InvalidateAll();
                }
                if (tb.Toggle(s_studio.enableLerp, {
                    .text = "Morph",
                    .tooltip = "Toggle UiTheme::Lerp morphing",
                    .icon = guikit::Icon::Flash(),
                    .variant = guikit::UiVariant::Warning
                }).changed) {
                    ui.InvalidateAll();
                }

                tb.Spacer();
                const keel::f32 txtBgRatio = guikit::Color::ContrastRatio(built.palette.textPrimary, built.palette.background);
                guikit::Tag(ui, ui.Format("WCAG {:.1f}:1", txtBgRatio), {
                    .variant = ContrastBadgeVariant(txtBgRatio),
                    .size = guikit::UiSize::Xs
                });
            }

            if (auto scroll = guikit::ScrollArea(ui, {
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                if (auto root = guikit::Column(ui, {
                    .gap = 14.0f,
                    .padding = guikit::Padding::All(18.0f),
                    .width = guikit::SizeMode::Stretch
                })) {
                    // 1. Header & Live Audit Strip
                    const keel::f32 textBgRatio   = guikit::Color::ContrastRatio(built.palette.textPrimary, built.palette.background);
                    const keel::f32 accentRatio   = guikit::Color::ContrastRatio(built.palette.accentText, built.palette.accent);
                    const keel::f32 surfaceRatio  = guikit::Color::ContrastRatio(built.palette.textPrimary, built.palette.surface);
                    const auto& primVc = built.GetVariant(guikit::UiVariant::Primary);
                    const guikit::Color primBgRef = (primVc.bg.a > 0u) ? primVc.bg : built.palette.surface;
                    const keel::f32 primBtnRatio  = guikit::Color::ContrastRatio(primVc.text, primBgRef);

                    const keel::StringRef titleText = s_studio.enableLerp && s_studio.lerpT > 0.001f
                        ? ui.Format("Theme Studio — {} <-> {} ({:.0f}%)",
                            guikit::ThemePresetName(s_studio.preset),
                            guikit::ThemePresetName(s_studio.lerpTarget),
                            s_studio.lerpT * 100.0f)
                        : ui.Format("Theme Studio — {}", guikit::ThemePresetName(built.preset));

                    guikit::Heading(ui, {
                        .label = titleText,
                        .description = ui.Format("{} · {} · {} · Focus {:.1f}dp · Motion {:.1f}x",
                            built.mode == guikit::ThemeMode::Dark ? "Dark" : "Light",
                            guikit::ChromeStyleName(built.chromeStyle),
                            guikit::FillStyleName(built.fillStyle),
                            built.focusRingWidth,
                            built.animationScale),
                        .badge = {
                            built.shadowsEnabled ? "Elevated" : "Flat",
                            built.shadowsEnabled ? guikit::UiVariant::Primary : guikit::UiVariant::Secondary
                        },
                        .level = guikit::HeadingLevel::H3,
                        .separator = guikit::HeadingSeparator::Bottom
                    });

                    // Stage section filter tabs
                    static const keel::StaticArray<guikit::TabItem, 4> kStageTabs = {{
                        {.label = "Full Showcase"},
                        {.label = "Token Swatches"},
                        {.label = "Controls & States"},
                        {.label = "Zones & Syntax"}
                    }};
                    guikit::TabBar(ui, s_studio.activeStageTab, kStageTabs, {.size = guikit::UiSize::Sm});

                    const bool showAll = (s_studio.activeStageTab == 0u);

                    // WCAG 2.1 Audit Cards (visible on Full Showcase & Token Swatches)
                    if (showAll || s_studio.activeStageTab == 1u) {
                        if (auto wcagRow = guikit::Row(ui, {.gap = 10.0f, .equalWidths = true})) {
                            auto auditCard = [&](keel::StringRef label, keel::f32 ratio) {
                                if (auto c = guikit::Card(ui, {.gap = 4.0f, .padding = guikit::Padding::All(10.0f)})) {
                                    guikit::Text(ui, label, {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
                                    if (auto r = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Align::Center})) {
                                        guikit::Text(ui, ui.Format("{:.1f}:1", ratio), {.role = guikit::TextRole::TitleMedium});
                                        guikit::Tag(ui, ContrastGradeLabel(ratio), {.variant = ContrastBadgeVariant(ratio), .size = guikit::UiSize::Xs});
                                    }
                                }
                            };
                            auditCard("Text / Background", textBgRatio);
                            auditCard("Text / Surface",    surfaceRatio);
                            auditCard("Accent / Ink",      accentRatio);
                            auditCard("Primary Button",    primBtnRatio);
                        }

                        // Color Swatch Token Map
                        if (auto swCard = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
                            guikit::Heading(ui, {
                                .label = "Design Token Palette",
                                .description = "Resolved surface, ink, border & semantic hue swatches",
                                .level = guikit::HeadingLevel::H6
                            });
                            if (auto w = guikit::Wrap(ui, {.gap = 8.0f})) {
                                RenderColorSwatchCard(ui, "Background",  built.palette.background,      built.palette.border);
                                RenderColorSwatchCard(ui, "Surface",     built.palette.surface,         built.palette.border);
                                RenderColorSwatchCard(ui, "Elevated",    built.palette.surfaceElevated, built.palette.border);
                                RenderColorSwatchCard(ui, "Input Bg",    built.palette.inputBackground, built.palette.border);
                                RenderColorSwatchCard(ui, "Border",      built.palette.border,          built.palette.borderHover);
                                RenderColorSwatchCard(ui, "Focus Ring",  built.palette.focusRing,       built.palette.border);
                                RenderColorSwatchCard(ui, "Text Ink",    built.palette.textPrimary,     built.palette.border);
                                RenderColorSwatchCard(ui, "Accent",      built.palette.accent,          built.palette.border);
                                RenderColorSwatchCard(ui, "Success",     built.GetSemanticAccent(guikit::UiVariant::Success), built.palette.border);
                                RenderColorSwatchCard(ui, "Warning",     built.GetSemanticAccent(guikit::UiVariant::Warning), built.palette.border);
                                RenderColorSwatchCard(ui, "Danger",      built.GetSemanticAccent(guikit::UiVariant::Danger),  built.palette.border);
                                RenderColorSwatchCard(ui, "Info",        built.GetSemanticAccent(guikit::UiVariant::Info),    built.palette.border);
                            }
                        }
                    }

                    // Controls, SurfaceStyle Matrix, Elevation Comparison & Accessibility Testbench
                    if (showAll || s_studio.activeStageTab == 2u) {
                        // Flat vs Elevated card comparison
                        if (auto cmpRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
                            if (auto flatCard = guikit::Card(ui, {.gap = 6.0f, .padding = guikit::Padding::All(14.0f), .surface = guikit::SurfaceRole::Flat})) {
                                guikit::Heading(ui, {
                                    .label = "Flat Surface Card",
                                    .description = "Tests surfaceContrast & borderScale separation",
                                    .level = guikit::HeadingLevel::H6
                                });
                                if (auto tagWrap = guikit::Wrap(ui, {.gap = 6.0f})) {
                                    guikit::Tag(ui, "Soft Primary",    {.variant = guikit::UiVariant::Primary, .style = guikit::TagStyle::Soft,    .size = guikit::UiSize::Sm});
                                    guikit::Tag(ui, "Outline Success", {.variant = guikit::UiVariant::Success, .style = guikit::TagStyle::Outline, .size = guikit::UiSize::Sm});
                                    guikit::Tag(ui, "Solid Danger",    {.variant = guikit::UiVariant::Danger,  .style = guikit::TagStyle::Solid,   .size = guikit::UiSize::Sm});
                                    guikit::Tag(ui, "Disabled",        {.variant = guikit::UiVariant::Warning, .style = guikit::TagStyle::Soft,    .size = guikit::UiSize::Sm, .disabled = true});
                                }
                            }
                            if (auto elevCard = guikit::Card(ui, {.gap = 6.0f, .padding = guikit::Padding::All(14.0f), .surface = guikit::SurfaceRole::Raised})) {
                                guikit::Heading(ui, {
                                    .label = "Elevated Surface Card",
                                    .description = "Tests Elevation::Md shadowIntensity & surfaceElevated",
                                    .level = guikit::HeadingLevel::H6
                                });
                                guikit::Text(ui, "Shadow blur, vertical offset, and alpha react immediately to Shadow Intensity and Brutalist / HighContrast presets.", {
                                    .role = guikit::TextRole::BodySmall,
                                    .variant = guikit::UiVariant::Secondary
                                });
                            }
                        }

                        // Buttons (Active + Disabled rows) & Interactive Controls
                        if (auto card = guikit::Card(ui, {
                            .gap = 12.0f,
                            .padding = guikit::Padding::All(16.0f),
                            .surface = guikit::SurfaceRole::Raised
                        })) {
                            guikit::Heading(ui, {
                                .label = ui.Format("Controls & States — {} / {}",
                                    guikit::ChromeStyleName(built.chromeStyle),
                                    guikit::FillStyleName(built.fillStyle)),
                                .description = "Active vs Disabled rows verify ChromeStyle, FillStyle, Disabled Opacity & Focus Ring",
                                .level = guikit::HeadingLevel::H5
                            });

                            if (auto btnWrap = guikit::Wrap(ui, {.gap = 8.0f})) {
                                guikit::Button(ui, {.text = "Default"});
                                guikit::Button(ui, {.text = "Primary", .variant = guikit::UiVariant::Primary});
                                guikit::Button(ui, {.text = "Soft",    .style = guikit::ButtonStyle::Soft,    .variant = guikit::UiVariant::Primary});
                                guikit::Button(ui, {.text = "Outline", .style = guikit::ButtonStyle::Outline, .variant = guikit::UiVariant::Info});
                                guikit::Button(ui, {.text = "Success", .variant = guikit::UiVariant::Success});
                                guikit::Button(ui, {.text = "Warning", .variant = guikit::UiVariant::Warning});
                                guikit::Button(ui, {.text = "Danger",  .variant = guikit::UiVariant::Danger});
                                guikit::Button(ui, {.text = "Ghost",   .style = guikit::ButtonStyle::Ghost});
                            }

                            if (auto disWrap = guikit::Wrap(ui, {.gap = 8.0f})) {
                                guikit::Button(ui, {.text = "Default (Dis)", .disabled = true});
                                guikit::Button(ui, {.text = "Primary (Dis)", .disabled = true, .variant = guikit::UiVariant::Primary});
                                guikit::Button(ui, {.text = "Success (Dis)", .disabled = true, .variant = guikit::UiVariant::Success});
                                guikit::Button(ui, {.text = "Danger (Dis)",  .disabled = true, .variant = guikit::UiVariant::Danger});
                            }

                            if (auto controlsRow = guikit::Row(ui, {.gap = 14.0f, .equalWidths = true})) {
                                if (auto leftCol = guikit::Column(ui, {.gap = 8.0f})) {
                                    if (auto el = guikit::Element(ui, {.label = "Focusable Input (Focus Ring)"})) {
                                        guikit::InputField(ui, s_studio.sampleInput, {.clearable = true});
                                    }
                                    if (auto el = guikit::Element(ui, {.label = "Disabled Input", .disabled = true})) {
                                        guikit::InputField(ui, s_studio.disabledInput, {.disabled = true});
                                    }
                                    static constexpr keel::StaticArray<keel::StringRef, 3> kSampleItems = {
                                        "Low Latency", "Balanced", "High Precision"
                                    };
                                    if (auto el = guikit::Element(ui, {.label = "Dropdown Select"})) {
                                        guikit::Select(ui, s_studio.sampleSelectIdx, kSampleItems);
                                    }
                                }
                                if (auto rightCol = guikit::Column(ui, {.gap = 8.0f})) {
                                    if (auto el = guikit::Element(ui, {.label = "Parameter Level"})) {
                                        guikit::Slider(ui, s_studio.sampleSlider, 0.0f, 100.0f, {.valueFormat = "%.0f %%"});
                                    }
                                    if (auto el = guikit::Element(ui, {.label = "Progress & Motion (Animation Scale)"})) {
                                        guikit::ProgressBar(ui, s_studio.sampleSlider / 100.0f, {
                                            .indeterminate = s_studio.indeterminateBar,
                                            .color = guikit::StateColor::Primary()
                                        });
                                    }
                                    if (auto toggleRow = guikit::Row(ui, {.gap = 14.0f, .align = guikit::Align::Center})) {
                                        guikit::Switch(ui, s_studio.sampleSwitch, {.textRight = "Active"});
                                        guikit::Switch(ui, s_studio.sampleSwitchOff, {.textRight = "Dis", .disabled = true});
                                        if (auto chkEl = guikit::Element(ui, {.label = "Motion Bar", .layout = guikit::ElementLayout::Trailing})) {
                                            guikit::Checkbox(ui, s_studio.indeterminateBar);
                                        }
                                    }
                                }
                            }

                            // Instrumentation Strip: Knob + LevelMeter
                            if (auto instRow = guikit::Row(ui, {.gap = 16.0f, .align = guikit::Align::Center})) {
                                guikit::Knob(ui, s_studio.sampleKnob, 0.0f, 100.0f, {.valueFormat = "%.0f%%", .size = guikit::UiSize::Sm});
                                if (auto meterCol = guikit::Column(ui, {.gap = 4.0f, .width = guikit::SizeMode::Stretch})) {
                                    guikit::Text(ui, "Multi-Zone DSP LevelMeter (tracks Success / Warning / Danger tokens)", {
                                        .role = guikit::TextRole::LabelSmall,
                                        .variant = guikit::UiVariant::Muted
                                    });
                                    const keel::f64 dbVal = -60.0 + keel::as<keel::f64>(s_studio.sampleSlider) * 0.58;
                                    guikit::LevelMeter(ui, dbVal, -60.0, 0.0, {.segments = 24u, .multiZoneColor = true, .size = guikit::UiSize::Sm});
                                }
                            }

                            // Interactive 2D Scrollbar Preview (Vertical + Horizontal + Corner Box)
                            if (auto sbEl = guikit::Element(ui, {
                                .label = ui.Format("Scrollbar Preview — {} ({:.0f}dp)",
                                    guikit::ScrollbarPresetName(built.scrollbarPreset),
                                    built.GetScrollbar(guikit::UiSize::Md).trackWidth),
                                .description = "Test vertical/horizontal stepper arrows, thumb drag, grip & corner box"
                            })) {
                                if (auto sbBox = guikit::ScrollArea(ui, {
                                    .orientation = guikit::Orientation::Both,
                                    .scrollbar = guikit::VisibilityPolicy::Always,
                                    .gap = 4.0f,
                                    .padding = guikit::Padding::All(10.0f),
                                    .width = guikit::SizeMode::Stretch,
                                    .height = 136.0f,
                                    .background = guikit::StateColor{built.palette.inputBackground},
                                    .border = {.width = 1.0f, .color = guikit::StateColor{built.palette.border}}
                                })) {
                                    for (keel::i32 row = 1; row <= 14; ++row) {
                                        guikit::Text(ui, ui.Format(
                                            "Row {:02d}  |  Scrollbar Preset: {}  |  Chrome: {}  |  Horizontal overflow sample column A · column B · column C · column D",
                                            row,
                                            guikit::ScrollbarPresetName(built.scrollbarPreset),
                                            ScrollbarChromeLabel(built.scrollbarStyle.chrome)
                                        ), {.role = guikit::TextRole::CodeSmall});
                                    }
                                }
                            }
                        }
                    }

                    // Scoped Subtree Overrides (ui.Zone) + Syntax Highlighting Preview
                    if (showAll || s_studio.activeStageTab == 3u) {
                        if (auto zoneRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
                            if (auto z1 = ui.Zone({
                                .chrome = guikit::ChromeStyle::Fluent,
                                .fillStyle = guikit::FillStyle::Soft,
                                .accent = guikit::Color::Hex(0xF59E0BFF)
                            })) {
                                if (auto zCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(12.0f)})) {
                                    guikit::Heading(ui, {
                                        .label = "Zone: Fluent Soft Amber",
                                        .description = ".chrome = Fluent, .fillStyle = Soft",
                                        .level = guikit::HeadingLevel::H6
                                    });
                                    if (auto r = guikit::Row(ui, {.gap = 6.0f})) {
                                        guikit::Button(ui, {.text = "Tonal CTA", .size = guikit::UiSize::Sm, .variant = guikit::UiVariant::Primary});
                                        guikit::Tag(ui, "Zero-Alloc", {.variant = guikit::UiVariant::Warning, .size = guikit::UiSize::Sm});
                                    }
                                }
                            }

                            if (auto z2 = ui.Zone({
                                .chrome = guikit::ChromeStyle::Classic3D,
                                .fillStyle = guikit::FillStyle::Outline,
                                .accent = guikit::Color::Hex(0x10B981FF)
                            })) {
                                if (auto zCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(12.0f)})) {
                                    guikit::Heading(ui, {
                                        .label = "Zone: Classic 3D Outline",
                                        .description = ".chrome = Classic3D, .fillStyle = Outline",
                                        .level = guikit::HeadingLevel::H6
                                    });
                                    if (auto r = guikit::Row(ui, {.gap = 6.0f})) {
                                        guikit::Button(ui, {.text = "Outlined", .size = guikit::UiSize::Sm, .variant = guikit::UiVariant::Primary});
                                        guikit::Tag(ui, "Emerald", {.variant = guikit::UiVariant::Success, .size = guikit::UiSize::Sm});
                                    }
                                }
                            }

                            const guikit::ThemePreset hcPreset = (built.mode == guikit::ThemeMode::Dark)
                                ? guikit::ThemePreset::HighContrastDark
                                : guikit::ThemePreset::HighContrastLight;
                            if (auto z3 = ui.Zone({.preset = hcPreset})) {
                                if (auto zCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(12.0f)})) {
                                    guikit::Heading(ui, {
                                        .label = "Zone: High Contrast",
                                        .description = ".preset = HighContrast",
                                        .level = guikit::HeadingLevel::H6
                                    });
                                    if (auto r = guikit::Row(ui, {.gap = 6.0f})) {
                                        guikit::Button(ui, {.text = "WCAG AAA", .size = guikit::UiSize::Sm, .variant = guikit::UiVariant::Primary});
                                        guikit::Tag(ui, "3dp Focus", {.variant = guikit::UiVariant::Info, .size = guikit::UiSize::Sm});
                                    }
                                }
                            }
                        }

                        // Syntax Palette Preview
                        static constexpr keel::StringRef kSyntaxSnippet =
                            "// Live SyntaxPalette (synced with ThemePreset or custom SyntaxPreset)\n"
                            "guikit::ThemeSpec spec{\n"
                            "    .base = guikit::ThemePreset::Nord,\n"
                            "    .chrome = guikit::ChromeStyle::Classic3D,\n"
                            "    .fillStyle = guikit::FillStyle::Soft,\n"
                            "    .surfaceContrast = 1.15f,\n"
                            "    .radiusScale = 0.85f\n"
                            "};\n"
                            "const guikit::UiTheme theme = guikit::UiTheme::Build(spec);";
                        keel::String previewCode = keel::String::from(kSyntaxSnippet);
                        guikit::TextArea(ui, previewCode, {
                            .autoHeight = true,
                            .readOnly = true,
                            .showLineNumbers = true,
                            .monospace = true,
                            .syntax = guikit::SyntaxConfig::Cpp()
                        });
                    }
                }
            }
        }
}

keel::String GenerateThemeStudioCode(const DemoContext&) {
    const guikit::ThemeSpec spec = BuildCurrentSpec();
    if (s_studio.codeFormatIdx == 1) {
        return spec.ToJson();
    }

    keel::String out;
    keel::format_into(out,
        "guikit::ThemeSpec spec{{\n"
        "    .base = guikit::ThemePreset::{}",
        PresetEnumIdentifier(spec.base)
    );
    if (spec.mode.has_value()) {
        keel::format_into(out, ",\n    .mode = guikit::ThemeMode::{}",
            (*spec.mode == guikit::ThemeMode::Light) ? "Light" : "Dark");
    }
    const guikit::UiTheme presetDefault = guikit::UiTheme::FromPreset(spec.base);
    if (spec.chrome.has_value() && *spec.chrome != presetDefault.chromeStyle) {
        keel::format_into(out, ",\n    .chrome = guikit::ChromeStyle::{}",
            ChromeStyleEnumIdentifier(*spec.chrome));
    }
    if (spec.fillStyle.has_value() && *spec.fillStyle != presetDefault.fillStyle) {
        keel::format_into(out, ",\n    .fillStyle = guikit::FillStyle::{}",
            FillStyleEnumIdentifier(*spec.fillStyle));
    }
    if (spec.syntaxPreset != guikit::SyntaxPreset::Auto) {
        keel::format_into(out, ",\n    .syntaxPreset = guikit::SyntaxPreset::{}",
            SyntaxPresetEnumIdentifier(spec.syntaxPreset));
    }
    if (spec.scrollbarPreset.has_value() && *spec.scrollbarPreset != guikit::ScrollbarPreset::Auto) {
        keel::format_into(out, ",\n    .scrollbarPreset = guikit::ScrollbarPreset::{}",
            ScrollbarPresetEnumIdentifier(*spec.scrollbarPreset));
    }
    if (spec.scrollbarWidth.has_value()) {
        keel::format_into(out, ",\n    .scrollbarWidth = {:.1f}f", *spec.scrollbarWidth);
    }
    if (spec.accent.has_value())      keel::format_into(out, ",\n    .accent = guikit::Color::Hex({})", FormatHexLiteral(*spec.accent));
    if (spec.neutral.has_value())     keel::format_into(out, ",\n    .neutral = guikit::Color::Hex({})", FormatHexLiteral(*spec.neutral));
    if (spec.background.has_value())  keel::format_into(out, ",\n    .background = guikit::Color::Hex({})", FormatHexLiteral(*spec.background));
    if (spec.surface.has_value())     keel::format_into(out, ",\n    .surface = guikit::Color::Hex({})", FormatHexLiteral(*spec.surface));
    if (spec.border.has_value())      keel::format_into(out, ",\n    .border = guikit::Color::Hex({})", FormatHexLiteral(*spec.border));
    if (spec.textPrimary.has_value()) keel::format_into(out, ",\n    .textPrimary = guikit::Color::Hex({})", FormatHexLiteral(*spec.textPrimary));
    if (keel::abs(spec.surfaceContrast - 1.0f) > 0.005f) keel::format_into(out, ",\n    .surfaceContrast = {:.2f}f", spec.surfaceContrast);
    if (keel::abs(spec.radiusScale - 1.0f) > 0.005f)     keel::format_into(out, ",\n    .radiusScale = {:.2f}f", spec.radiusScale);
    if (keel::abs(spec.spacingScale - 1.0f) > 0.005f)    keel::format_into(out, ",\n    .spacingScale = {:.2f}f", spec.spacingScale);
    if (keel::abs(spec.fontScale - 1.0f) > 0.005f)       keel::format_into(out, ",\n    .fontScale = {:.2f}f", spec.fontScale);
    if (keel::abs(spec.lineHeightMultiplier - guikit::kDefaultLineHeightMultiplier) > 0.005f) {
        keel::format_into(out, ",\n    .lineHeightMultiplier = {:.2f}f", spec.lineHeightMultiplier);
    }
    if (keel::abs(spec.borderScale - 1.0f) > 0.005f)     keel::format_into(out, ",\n    .borderScale = {:.2f}f", spec.borderScale);
    if (keel::abs(spec.shadowIntensity - 1.0f) > 0.005f) keel::format_into(out, ",\n    .shadowIntensity = {:.2f}f", spec.shadowIntensity);
    if (keel::abs(spec.focusRingWidth - 2.0f) > 0.05f)   keel::format_into(out, ",\n    .focusRingWidth = {:.1f}f", spec.focusRingWidth);
    if (keel::abs(spec.disabledOpacity - 0.50f) > 0.005f) keel::format_into(out, ",\n    .disabledOpacity = {:.2f}f", spec.disabledOpacity);
    if (keel::abs(spec.animationScale - 1.0f) > 0.005f)  keel::format_into(out, ",\n    .animationScale = {:.2f}f", spec.animationScale);
    if (spec.shellStyle == guikit::ShellStyle::Floating) out.append(",\n    .shellStyle = guikit::ShellStyle::Floating");
    if (spec.success.has_value()) keel::format_into(out, ",\n    .success = guikit::Color::Hex({})", FormatHexLiteral(*spec.success));
    if (spec.warning.has_value()) keel::format_into(out, ",\n    .warning = guikit::Color::Hex({})", FormatHexLiteral(*spec.warning));
    if (spec.danger.has_value())  keel::format_into(out, ",\n    .danger = guikit::Color::Hex({})", FormatHexLiteral(*spec.danger));
    if (spec.info.has_value())    keel::format_into(out, ",\n    .info = guikit::Color::Hex({})", FormatHexLiteral(*spec.info));

    out.append("\n};\nconst guikit::UiTheme theme = guikit::UiTheme::Build(spec);\n");
    if (s_studio.enableLerp && s_studio.lerpT > 0.001f) {
        keel::format_into(out,
            "const guikit::UiTheme morphed = guikit::UiTheme::Lerp(\n"
            "    theme,\n"
            "    guikit::UiTheme::FromPreset(guikit::ThemePreset::{}),\n"
            "    {:.2f}f\n"
            ");\n",
            PresetEnumIdentifier(s_studio.lerpTarget),
            s_studio.lerpT
        );
    }
    out.append(
        "\n// Apply to MainWindow or scope locally with zero heap allocations:\n"
        "// SetShellTheme(theme);\n"
        "if (auto zone = ui.Zone({.theme = theme})) {\n"
        "    // All widgets inside inherit the custom design system\n"
        "}\n"
    );
    return out;
}

} // namespace guikit::demo
