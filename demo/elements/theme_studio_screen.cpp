#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct ThemeStudioState {
    // Base & Mode
    guikit::ThemePreset preset = guikit::ThemePreset::Dark;
    keel::i32 modeOverrideIdx = 0; // 0 = Auto, 1 = Dark, 2 = Light
    guikit::SurfaceStyle surfaceStyle = guikit::SurfaceStyle::Solid;
    guikit::SyntaxPreset syntaxPreset = guikit::SyntaxPreset::Auto;

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
    s_studio.modeOverrideIdx = 0;
    s_studio.surfaceStyle = guikit::SurfaceStyle::Solid;
    s_studio.syntaxPreset = guikit::SyntaxPreset::Auto;

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

void ApplySpecToStudioState(const guikit::ThemeSpec& spec) noexcept {
    ResetToDefaults();
    s_studio.preset = spec.base;
    if (spec.mode.has_value()) {
        s_studio.modeOverrideIdx = (*spec.mode == guikit::ThemeMode::Light) ? 2 : 1;
    }
    s_studio.surfaceStyle = spec.surfaceStyle;
    s_studio.syntaxPreset = spec.syntaxPreset;
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
    if (s_studio.modeOverrideIdx == 1) spec.mode = guikit::ThemeMode::Dark;
    else if (s_studio.modeOverrideIdx == 2) spec.mode = guikit::ThemeMode::Light;
    spec.surfaceStyle = s_studio.surfaceStyle;
    spec.syntaxPreset = s_studio.syntaxPreset;
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
    const guikit::UiTheme primary = guikit::UiTheme::Build(BuildCurrentSpec());
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
    s_studio.surfaceStyle = keel::from_underlying<guikit::SurfaceStyle>(styleIdx);
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

[[nodiscard]] constexpr keel::StringRef SurfaceStyleEnumIdentifier(guikit::SurfaceStyle s) noexcept {
    switch (s) {
        case guikit::SurfaceStyle::Solid:     return "Solid";
        case guikit::SurfaceStyle::Soft:      return "Soft";
        case guikit::SurfaceStyle::Outline:   return "Outline";
        case guikit::SurfaceStyle::Brutalist: return "Brutalist";
        case guikit::SurfaceStyle::Bevel:     return "Bevel";
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
    if (auto tile = ui.Box({
        .width = 128.0f,
        .padding = guikit::Padding::Symmetric(10.0f, 8.0f),
        .background = guikit::StateColor{color},
        .border = {.width = 1.0f, .color = guikit::StateColor{borderCol}}
    })) {
        if (auto col = ui.Column({.gap = 2.0f})) {
            ui.Text(guikit::UiText{name, guikit::StateColor{ink}}, {.role = guikit::TextRole::LabelSmall});
            ui.Text(guikit::UiText{FormatHexLabel(color).as_ref(), guikit::StateColor{ink.WithAlphaF(0.85f)}}, {.role = guikit::TextRole::CodeSmall});
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
    if (auto el = ui.Element({.label = label, .layout = guikit::ElementLayout::Leading})) {
        if (ui.Switch(enabled, {.size = guikit::UiSize::Sm}).changed) {
            if (enabled) {
                color = defaultSeed;
            }
            ui.InvalidateAll();
        }
    }
    if (enabled) {
        if (ui.ColorPicker(color, {.showHex = true, .showAlpha = false, .showSwatches = true, .size = guikit::UiSize::Sm}).changed) {
            ui.InvalidateAll();
        }
    }
}

} // namespace

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

    static constexpr keel::StaticArray<keel::StringRef, guikit::kSurfaceStyleCount> kSurfaceNames = {{
        guikit::SurfaceStyleName(guikit::SurfaceStyle::Solid),
        guikit::SurfaceStyleName(guikit::SurfaceStyle::Soft),
        guikit::SurfaceStyleName(guikit::SurfaceStyle::Outline),
        guikit::SurfaceStyleName(guikit::SurfaceStyle::Brutalist),
        guikit::SurfaceStyleName(guikit::SurfaceStyle::Bevel)
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
    if (auto topActions = ui.Row({.gap = 6.0f})) {
        if (ui.Button({
            .text = "Apply to App",
            .icon = guikit::Icon::Check(),
            .tooltip = "Apply the crafted ThemeSpec live to the entire Demo application",
            .size = guikit::UiSize::Sm,
            .width = guikit::SizeMode::Stretch,
            .color = guikit::StateColor::Primary()
        }).clicked) {
            ctx.pendingShellTheme = BuildEffectiveTheme();
            ui.InvalidateAll();
        }
        if (ui.Button({
            .icon = guikit::Icon::Flash(),
            .square = true,
            .tooltip = "Generate a harmonic WCAG-compliant theme",
            .size = guikit::UiSize::Sm,
            .color = guikit::StateColor::Info()
        }).clicked) {
            RandomizeHarmonicTheme();
            ui.InvalidateAll();
        }
        if (ui.Button({
            .icon = guikit::Icon::Refresh(),
            .square = true,
            .tooltip = "Reset all tokens to default",
            .size = guikit::UiSize::Sm,
            .color = guikit::StateColor::Secondary()
        }).clicked) {
            ResetToDefaults();
            ui.InvalidateAll();
        }
    }

    // 1. PRESETS & RECIPES
    if (auto acc = ui.Accordion({.label = "PRESETS & RECIPES", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize presetIdx = keel::as<keel::usize>(s_studio.preset);
        if (auto el = ui.Element({.label = "Base Preset"})) {
            if (ui.Select(presetIdx, kPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.preset = keel::from_underlying<guikit::ThemePreset>(presetIdx);
                s_studio.surfaceStyle = guikit::UiTheme::FromPreset(s_studio.preset).surfaceStyle;
                ui.InvalidateAll();
            }
        }

        static constexpr keel::StaticArray<keel::StringRef, 3> kModeNames = {"Auto", "Dark", "Light"};
        if (auto el = ui.Element({.label = "Mode Override"})) {
            if (ui.Segmented(s_studio.modeOverrideIdx, kModeNames, {.size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }

        if (auto el = ui.Element({.label = "Designer Recipes", .description = "1-click design system archetypes"})) {
            if (auto w = ui.Wrap({.gap = 6.0f})) {
                if (ui.Button({.text = "Linear", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) {
                    guikit::ThemeSpec r{};
                    r.base = guikit::ThemePreset::Dark;
                    r.surfaceStyle = guikit::SurfaceStyle::Soft;
                    r.accent = guikit::Color::Hex(0x818CF8FF);
                    r.neutral = guikit::Color::Hex(0x11121BFF);
                    r.surfaceContrast = 0.80f;
                    r.radiusScale = 0.85f;
                    r.shadowIntensity = 0.70f;
                    ApplySpecToStudioState(r);
                    ui.InvalidateAll();
                }
                if (ui.Button({.text = "Material 3", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) {
                    guikit::ThemeSpec r{};
                    r.base = guikit::ThemePreset::Light;
                    r.surfaceStyle = guikit::SurfaceStyle::Soft;
                    r.accent = guikit::Color::Hex(0x4F46E5FF);
                    r.neutral = guikit::Color::Hex(0xEEF2FFFF);
                    r.radiusScale = 1.55f;
                    r.shadowIntensity = 1.15f;
                    r.shellStyle = guikit::ShellStyle::Floating;
                    ApplySpecToStudioState(r);
                    ui.InvalidateAll();
                }
                if (ui.Button({.text = "Vercel", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) {
                    guikit::ThemeSpec r{};
                    r.base = guikit::ThemePreset::Dark;
                    r.surfaceStyle = guikit::SurfaceStyle::Outline;
                    r.background = guikit::Color::Hex(0x09090BFF);
                    r.surface = guikit::Color::Hex(0x111114FF);
                    r.border = guikit::Color::Hex(0x27272AFF);
                    r.accent = guikit::Color::Hex(0xFAFAFAFF);
                    r.radiusScale = 0.75f;
                    r.shadowIntensity = 0.0f;
                    ApplySpecToStudioState(r);
                    ui.InvalidateAll();
                }
                if (ui.Button({.text = "Brutalist", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) {
                    guikit::ThemeSpec r{};
                    r.base = guikit::ThemePreset::Light;
                    r.surfaceStyle = guikit::SurfaceStyle::Brutalist;
                    r.background = guikit::Color::Hex(0xFAF7F2FF);
                    r.surface = guikit::Color::Hex(0xFFFFFFFF);
                    r.accent = guikit::Color::Hex(0xE11D48FF);
                    r.radiusScale = 0.0f;
                    r.borderScale = 1.5f;
                    r.shadowIntensity = 0.0f;
                    ApplySpecToStudioState(r);
                    ui.InvalidateAll();
                }
                if (ui.Button({.text = "SCADA", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) {
                    guikit::ThemeSpec r{};
                    r.base = guikit::ThemePreset::Industrial;
                    r.surfaceStyle = guikit::SurfaceStyle::Solid;
                    r.syntaxPreset = guikit::SyntaxPreset::Monokai;
                    r.accent = guikit::Color::Hex(0x06B6D4FF);
                    r.surfaceContrast = 1.35f;
                    r.radiusScale = 0.30f;
                    r.borderScale = 1.25f;
                    r.shadowIntensity = 0.50f;
                    ApplySpecToStudioState(r);
                    ui.InvalidateAll();
                }
                if (ui.Button({.text = "Win98", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) {
                    guikit::ThemeSpec r{};
                    r.base = guikit::ThemePreset::Windows98;
                    r.surfaceStyle = guikit::SurfaceStyle::Bevel;
                    r.shadowIntensity = 0.0f;
                    ApplySpecToStudioState(r);
                    ui.InvalidateAll();
                }
            }
        }
    }

    // 2. APPEARANCE & PALETTE
    const guikit::UiTheme baseRef = guikit::UiTheme::FromPreset(s_studio.preset);
    if (auto acc = ui.Accordion({.label = "APPEARANCE & PALETTE", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize surfIdx = keel::as<keel::usize>(s_studio.surfaceStyle);
        if (auto el = ui.Element({.label = "Surface Style", .description = "Global button & control treatment"})) {
            if (ui.Select(surfIdx, kSurfaceNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.surfaceStyle = keel::from_underlying<guikit::SurfaceStyle>(surfIdx);
                ui.InvalidateAll();
            }
        }

        if (auto el = ui.Element({.label = "Layer Contrast", .description = "Separation between bg, cards & borders"})) {
            if (ui.Slider(s_studio.surfaceContrast, 0.50f, 1.80f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }

        RenderColorOverrideRow(ui, "Brand Accent", s_studio.overrideAccent, s_studio.accentColor, baseRef.palette.accent);
        RenderColorOverrideRow(ui, "Neutral Tint", s_studio.overrideNeutral, s_studio.neutralColor, baseRef.palette.background);

        keel::usize synIdx = keel::as<keel::usize>(s_studio.syntaxPreset);
        if (auto el = ui.Element({.label = "Syntax Scheme"})) {
            if (ui.Select(synIdx, kSyntaxNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.syntaxPreset = keel::from_underlying<guikit::SyntaxPreset>(synIdx);
                ui.InvalidateAll();
            }
        }
    }

    // 3. DIRECT SURFACE & SEMANTIC COLORS
    if (auto acc = ui.Accordion({.label = "SURFACE & SEMANTIC TOKENS", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        RenderColorOverrideRow(ui, "Background",  s_studio.overrideBackground, s_studio.backgroundColor, baseRef.palette.background);
        RenderColorOverrideRow(ui, "Card Surface", s_studio.overrideSurface, s_studio.surfaceColor, baseRef.palette.surface);
        RenderColorOverrideRow(ui, "Border Color", s_studio.overrideBorder, s_studio.borderColor, baseRef.palette.border);
        RenderColorOverrideRow(ui, "Primary Text", s_studio.overrideText, s_studio.textColor, baseRef.palette.textPrimary);
        ui.Separator();
        RenderColorOverrideRow(ui, "Success", s_studio.overrideSuccess, s_studio.successColor, baseRef.GetSemanticAccent(guikit::UiVariant::Success));
        RenderColorOverrideRow(ui, "Warning", s_studio.overrideWarning, s_studio.warningColor, baseRef.GetSemanticAccent(guikit::UiVariant::Warning));
        RenderColorOverrideRow(ui, "Danger",  s_studio.overrideDanger,  s_studio.dangerColor,  baseRef.GetSemanticAccent(guikit::UiVariant::Danger));
        RenderColorOverrideRow(ui, "Info",    s_studio.overrideInfo,    s_studio.infoColor,    baseRef.GetSemanticAccent(guikit::UiVariant::Info));
    }

    // 4. GEOMETRY & ELEVATION
    if (auto acc = ui.Accordion({.label = "GEOMETRY & ELEVATION", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = ui.Element({.label = "Radius Scale"})) {
            if (ui.Slider(s_studio.radiusScale, 0.0f, 2.0f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto rRow = ui.Row({.gap = 4.0f, .equalWidths = true})) {
            if (ui.Button({.text = "Sharp", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) { s_studio.radiusScale = 0.0f; ui.InvalidateAll(); }
            if (ui.Button({.text = "Crisp", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) { s_studio.radiusScale = 0.5f; ui.InvalidateAll(); }
            if (ui.Button({.text = "Std",   .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) { s_studio.radiusScale = 1.0f; ui.InvalidateAll(); }
            if (ui.Button({.text = "Soft",  .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) { s_studio.radiusScale = 1.6f; ui.InvalidateAll(); }
        }
        if (auto el = ui.Element({.label = "Spacing Scale"})) {
            if (ui.Slider(s_studio.spacingScale, 0.70f, 1.40f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = ui.Element({.label = "Font Scale"})) {
            if (ui.Slider(s_studio.fontScale, 0.85f, 1.35f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = ui.Element({.label = "Line Height"})) {
            if (ui.Slider(s_studio.lineHeightMultiplier, 1.10f, 1.75f, {.valueFormat = "%.2f", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = ui.Element({.label = "Border Width Scale"})) {
            if (ui.Slider(s_studio.borderScale, 0.0f, 2.5f, {.valueFormat = "%.1fx", .step = 0.1f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = ui.Element({.label = "Shadow Intensity", .description = "0.00x = Flat / No shadows"})) {
            if (ui.Slider(s_studio.shadowIntensity, 0.0f, 1.6f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        keel::i32 shellIdx = (s_studio.shellStyle == guikit::ShellStyle::Floating) ? 1 : 0;
        static constexpr keel::StaticArray<keel::StringRef, 2> kShellNames = {"Docked", "Floating"};
        if (auto el = ui.Element({.label = "Shell Chrome"})) {
            if (ui.Segmented(shellIdx, kShellNames, {.size = guikit::UiSize::Sm}).changed) {
                s_studio.shellStyle = (shellIdx == 1) ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked;
                ui.InvalidateAll();
            }
        }
    }

    // 5. ACCESSIBILITY & MOTION
    if (auto acc = ui.Accordion({.label = "ACCESSIBILITY & MOTION", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = ui.Element({.label = "Focus Ring Width", .description = "Keyboard focus stroke in dp"})) {
            if (ui.Slider(s_studio.focusRingWidth, 1.0f, 4.5f, {.valueFormat = "%.1f dp", .step = 0.5f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = ui.Element({.label = "Disabled Opacity"})) {
            if (ui.Slider(s_studio.disabledOpacity, 0.20f, 0.80f, {.valueFormat = "%.2f", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (auto el = ui.Element({.label = "Animation Scale", .description = "0.00x enables Reduced Motion"})) {
            if (ui.Slider(s_studio.animationScale, 0.0f, 2.0f, {.valueFormat = "%.2fx", .step = 0.1f, .size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
    }

    // 6. THEME MORPHING (UiTheme::Lerp)
    if (auto acc = ui.Accordion({.label = "THEME MORPHING (LERP)", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = ui.Element({.label = "Enable Blend", .layout = guikit::ElementLayout::Leading})) {
            if (ui.Switch(s_studio.enableLerp, {.size = guikit::UiSize::Sm}).changed) {
                if (s_studio.enableLerp && s_studio.lerpT < 0.01f) s_studio.lerpT = 0.5f;
                ui.InvalidateAll();
            }
        }
        if (s_studio.enableLerp) {
            keel::usize targetIdx = keel::as<keel::usize>(s_studio.lerpTarget);
            if (auto el = ui.Element({.label = "Target Theme B"})) {
                if (ui.Select(targetIdx, kPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                    s_studio.lerpTarget = keel::from_underlying<guikit::ThemePreset>(targetIdx);
                    ui.InvalidateAll();
                }
            }
            if (auto el = ui.Element({.label = "Blend Factor t"})) {
                if (ui.Slider(s_studio.lerpT, 0.0f, 1.0f, {.valueFormat = "%.0f%%", .step = 0.02f, .size = guikit::UiSize::Sm}).changed) {
                    ui.InvalidateAll();
                }
            }
        }
    }

    // 7. JSON IMPORT / EXPORT
    if (auto acc = ui.Accordion({.label = "JSON IMPORT & CODE", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        static constexpr keel::StaticArray<keel::StringRef, 2> kCodeFormats = {"C++20", "JSON"};
        if (auto el = ui.Element({.label = "Bottom Code Panel"})) {
            if (ui.Segmented(s_studio.codeFormatIdx, kCodeFormats, {.size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (s_studio.jsonDraft.empty()) {
            s_studio.jsonDraft = BuildCurrentSpec().ToJson();
        }
        if (auto btnRow = ui.Row({.gap = 6.0f, .equalWidths = true})) {
            if (ui.Button({.text = "Snapshot", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Secondary()}).clicked) {
                s_studio.jsonDraft = BuildCurrentSpec().ToJson();
                s_studio.jsonImportStatus = 0;
                ui.InvalidateAll();
            }
            if (ui.Button({.text = "Load JSON", .size = guikit::UiSize::Xs, .color = guikit::StateColor::Primary()}).clicked) {
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
            ui.Tag("JSON Applied", {.variant = guikit::UiVariant::Success, .size = guikit::UiSize::Xs});
        } else if (s_studio.jsonImportStatus == -1) {
            ui.Tag("Invalid JSON syntax", {.variant = guikit::UiVariant::Danger, .size = guikit::UiSize::Xs});
        }
        ui.TextArea(s_studio.jsonDraft, {
            .autoHeight = false,
            .showLineNumbers = false,
            .monospace = true,
            .height = 132.0f
        });
    }
}

void RenderThemeStudioStage(guikit::Context& ui, DemoContext&) {
    const guikit::UiTheme built = BuildEffectiveTheme();

    if (auto zone = ui.Zone({.theme = built})) {
        if (auto bg = ui.Box({
            .gap = 0.0f,
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch,
            .background = guikit::StateColor{built.palette.background}
        })) {
            // Live Application Chrome Preview (MenuBar + Toolbar under the active theme)
            guikit::ContextMenuOptions dropOpts{};
            dropOpts.size = guikit::UiSize::Sm;
            if (auto mb = ui.MenuBar({.size = guikit::UiSize::Sm})) {
                if (auto file = mb.Menu("File", dropOpts)) {
                    if (file.Item("Snapshot JSON", {.icon = guikit::Icon::Save()})) {
                        s_studio.jsonDraft = BuildCurrentSpec().ToJson();
                        s_studio.jsonImportStatus = 0;
                        ui.InvalidateAll();
                    }
                    if (file.Item("Copy Theme JSON", {.icon = guikit::Icon::Copy()})) {
                        const keel::String json = BuildCurrentSpec().ToJson();
                        ui.SetClipboardText(json.as_ref());
                        ui.ShowToast({
                            .message = "Copied ThemeSpec JSON to clipboard",
                            .variant = guikit::UiVariant::Success
                        });
                    }
                    file.Separator();
                    if (file.Item("Reset to Defaults", {.icon = guikit::Icon::Refresh(), .variant = guikit::UiVariant::Danger})) {
                        ResetToDefaults();
                        ui.InvalidateAll();
                    }
                }

                if (auto themeMenu = mb.Menu("Theme", dropOpts)) {
                    if (auto presetsSub = themeMenu.SubMenu("Base Preset", {.icon = guikit::Icon::Eye()})) {
                        keel::i32 presetIdx = keel::as<keel::i32>(s_studio.preset);
                        for (keel::usize pi = 0; pi < guikit::kThemePresetCount; ++pi) {
                            const auto p = keel::from_underlying<guikit::ThemePreset>(pi);
                            if (presetsSub.Radio(guikit::ThemePresetName(p), presetIdx, keel::as<keel::i32>(pi))) {
                                s_studio.preset = p;
                                s_studio.surfaceStyle = guikit::UiTheme::FromPreset(p).surfaceStyle;
                                ui.InvalidateAll();
                            }
                        }
                    }
                    themeMenu.Separator();
                    if (themeMenu.Item("Harmonic Randomize", {.icon = guikit::Icon::Flash()})) {
                        RandomizeHarmonicTheme();
                        ui.InvalidateAll();
                    }
                    if (themeMenu.Checkbox("Theme Morph (Lerp)", s_studio.enableLerp, {.keepOpen = true})) {
                        ui.InvalidateAll();
                    }
                }

                if (auto surfMenu = mb.Menu("Surface", dropOpts)) {
                    surfMenu.Section("SURFACE STYLE");
                    keel::i32 surfIdx = keel::as<keel::i32>(s_studio.surfaceStyle);
                    for (keel::i32 si = 0; si < keel::as<keel::i32>(guikit::kSurfaceStyleCount); ++si) {
                        const auto ss = keel::from_underlying<guikit::SurfaceStyle>(si);
                        if (surfMenu.Radio(guikit::SurfaceStyleName(ss), surfIdx, si)) {
                            s_studio.surfaceStyle = ss;
                            ui.InvalidateAll();
                        }
                    }
                    surfMenu.Separator();
                    surfMenu.Section("SHELL STYLE");
                    keel::i32 shellIdx = (s_studio.shellStyle == guikit::ShellStyle::Floating) ? 1 : 0;
                    if (surfMenu.Radio("Docked", shellIdx, 0)) {
                        s_studio.shellStyle = guikit::ShellStyle::Docked;
                        ui.InvalidateAll();
                    }
                    if (surfMenu.Radio("Floating", shellIdx, 1)) {
                        s_studio.shellStyle = guikit::ShellStyle::Floating;
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
                ui.Tag(guikit::ThemePresetName(built.preset), {
                    .variant = guikit::UiVariant::Primary,
                    .size = guikit::UiSize::Xs
                });
            }

            if (auto tb = ui.Toolbar({.size = guikit::UiSize::Sm})) {
                if (tb.Action({
                    .text = "Randomize",
                    .tooltip = "Generate a harmonic color scheme",
                    .icon = guikit::Icon::Refresh(),
                    .color = guikit::StateColor::Primary()
                }).clicked) {
                    RandomizeHarmonicTheme();
                    ui.InvalidateAll();
                }
                if (tb.Action({
                    .text = "Reset",
                    .tooltip = "Reset all theme studio overrides",
                    .icon = guikit::Icon::Close(),
                    .color = guikit::StateColor::Secondary()
                }).clicked) {
                    ResetToDefaults();
                    ui.InvalidateAll();
                }

                tb.Separator();

                keel::i32 surfTool = keel::as<keel::i32>(s_studio.surfaceStyle);
                bool surfChanged = false;
                surfChanged |= tb.Item(surfTool, 0, {.text = "Solid"}).changed;
                surfChanged |= tb.Item(surfTool, 1, {.text = "Soft"}).changed;
                surfChanged |= tb.Item(surfTool, 2, {.text = "Outline"}).changed;
                surfChanged |= tb.Item(surfTool, 3, {.text = "Brutalist"}).changed;
                surfChanged |= tb.Item(surfTool, 4, {.text = "Classic 3D"}).changed;
                if (surfChanged) {
                    s_studio.surfaceStyle = keel::from_underlying<guikit::SurfaceStyle>(surfTool);
                    ui.InvalidateAll();
                }

                tb.Separator();

                bool isFloating = (s_studio.shellStyle == guikit::ShellStyle::Floating);
                if (tb.Toggle(isFloating, {
                    .text = "Floating",
                    .tooltip = "Toggle Docked vs Floating shell panels",
                    .color = guikit::StateColor::Info()
                }).changed) {
                    s_studio.shellStyle = isFloating ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked;
                    ui.InvalidateAll();
                }
                if (tb.Toggle(s_studio.enableLerp, {
                    .text = "Morph",
                    .tooltip = "Toggle UiTheme::Lerp morphing",
                    .icon = guikit::Icon::Flash(),
                    .color = guikit::StateColor::Warning()
                }).changed) {
                    ui.InvalidateAll();
                }

                tb.Spacer();
                const keel::f32 txtBgRatio = guikit::Color::ContrastRatio(built.palette.textPrimary, built.palette.background);
                ui.Tag(ui.Format("WCAG {:.1f}:1", txtBgRatio), {
                    .variant = ContrastBadgeVariant(txtBgRatio),
                    .size = guikit::UiSize::Xs
                });
            }

            if (auto scroll = ui.ScrollArea({
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch
            })) {
                if (auto root = ui.Column({
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

                    ui.Heading({
                        .label = titleText,
                        .description = ui.Format("{} · {} · Focus {:.1f}dp · Motion {:.1f}x",
                            built.mode == guikit::ThemeMode::Dark ? "Dark" : "Light",
                            guikit::SurfaceStyleName(built.surfaceStyle),
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
                    ui.TabBar(s_studio.activeStageTab, kStageTabs, {.size = guikit::UiSize::Sm});

                    const bool showAll = (s_studio.activeStageTab == 0u);

                    // WCAG 2.1 Audit Cards (visible on Full Showcase & Token Swatches)
                    if (showAll || s_studio.activeStageTab == 1u) {
                        if (auto wcagRow = ui.Row({.gap = 10.0f, .equalWidths = true})) {
                            auto auditCard = [&](keel::StringRef label, keel::f32 ratio) {
                                if (auto c = ui.Card({.gap = 4.0f, .padding = guikit::Padding::All(10.0f)})) {
                                    ui.Text(label, {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
                                    if (auto r = ui.Row({.gap = 8.0f, .align = guikit::Align::Center})) {
                                        ui.Text(ui.Format("{:.1f}:1", ratio), {.role = guikit::TextRole::TitleMedium});
                                        ui.Tag(ContrastGradeLabel(ratio), {.variant = ContrastBadgeVariant(ratio), .size = guikit::UiSize::Xs});
                                    }
                                }
                            };
                            auditCard("Text / Background", textBgRatio);
                            auditCard("Text / Surface",    surfaceRatio);
                            auditCard("Accent / Ink",      accentRatio);
                            auditCard("Primary Button",    primBtnRatio);
                        }

                        // Color Swatch Token Map
                        if (auto swCard = ui.Card({.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
                            ui.Heading({
                                .label = "Design Token Palette",
                                .description = "Resolved surface, ink, border & semantic hue swatches",
                                .level = guikit::HeadingLevel::H6
                            });
                            if (auto w = ui.Wrap({.gap = 8.0f})) {
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
                        if (auto cmpRow = ui.Row({.gap = 12.0f, .equalWidths = true})) {
                            if (auto flatCard = ui.Card({.gap = 6.0f, .padding = guikit::Padding::All(14.0f), .elevated = false})) {
                                ui.Heading({
                                    .label = "Flat Surface Card",
                                    .description = "Tests surfaceContrast & borderScale separation",
                                    .level = guikit::HeadingLevel::H6
                                });
                                if (auto tagWrap = ui.Wrap({.gap = 6.0f})) {
                                    ui.Tag("Soft Primary",    {.variant = guikit::UiVariant::Primary, .style = guikit::TagStyle::Soft,    .size = guikit::UiSize::Sm});
                                    ui.Tag("Outline Success", {.variant = guikit::UiVariant::Success, .style = guikit::TagStyle::Outline, .size = guikit::UiSize::Sm});
                                    ui.Tag("Solid Danger",    {.variant = guikit::UiVariant::Danger,  .style = guikit::TagStyle::Solid,   .size = guikit::UiSize::Sm});
                                    ui.Tag("Disabled",        {.variant = guikit::UiVariant::Warning, .style = guikit::TagStyle::Soft,    .size = guikit::UiSize::Sm, .disabled = true});
                                }
                            }
                            if (auto elevCard = ui.Card({.gap = 6.0f, .padding = guikit::Padding::All(14.0f), .elevated = true})) {
                                ui.Heading({
                                    .label = "Elevated Surface Card",
                                    .description = "Tests Elevation::Md shadowIntensity & surfaceElevated",
                                    .level = guikit::HeadingLevel::H6
                                });
                                ui.Text("Shadow blur, vertical offset, and alpha react immediately to Shadow Intensity and Brutalist / HighContrast presets.", {
                                    .role = guikit::TextRole::BodySmall,
                                    .variant = guikit::UiVariant::Secondary
                                });
                            }
                        }

                        // Buttons (Active + Disabled rows) & Interactive Controls
                        if (auto card = ui.Card({
                            .gap = 12.0f,
                            .padding = guikit::Padding::All(16.0f),
                            .elevated = true
                        })) {
                            ui.Heading({
                                .label = ui.Format("Controls & States — {}", guikit::SurfaceStyleName(built.surfaceStyle)),
                                .description = "Active vs Disabled rows verify SurfaceStyle, Disabled Opacity & Focus Ring",
                                .level = guikit::HeadingLevel::H5
                            });

                            if (auto btnWrap = ui.Wrap({.gap = 8.0f})) {
                                ui.Button({.text = "Default"});
                                ui.Button({.text = "Primary", .color = guikit::StateColor::Primary()});
                                ui.Button({.text = "Success", .color = guikit::StateColor::Success()});
                                ui.Button({.text = "Warning", .color = guikit::StateColor::Warning()});
                                ui.Button({.text = "Danger",  .color = guikit::StateColor::Danger()});
                                ui.Button({.text = "Info",    .color = guikit::StateColor::Info()});
                                ui.Button({.text = "Ghost",   .color = guikit::StateColor::Ghost()});
                            }

                            if (auto disWrap = ui.Wrap({.gap = 8.0f})) {
                                ui.Button({.text = "Default (Dis)", .disabled = true});
                                ui.Button({.text = "Primary (Dis)", .disabled = true, .color = guikit::StateColor::Primary()});
                                ui.Button({.text = "Success (Dis)", .disabled = true, .color = guikit::StateColor::Success()});
                                ui.Button({.text = "Danger (Dis)",  .disabled = true, .color = guikit::StateColor::Danger()});
                            }

                            if (auto controlsRow = ui.Row({.gap = 14.0f, .equalWidths = true})) {
                                if (auto leftCol = ui.Column({.gap = 8.0f})) {
                                    if (auto el = ui.Element({.label = "Focusable Input (Focus Ring)"})) {
                                        ui.InputField(s_studio.sampleInput, {.clearable = true});
                                    }
                                    if (auto el = ui.Element({.label = "Disabled Input", .disabled = true})) {
                                        ui.InputField(s_studio.disabledInput, {.disabled = true});
                                    }
                                    static constexpr keel::StaticArray<keel::StringRef, 3> kSampleItems = {
                                        "Low Latency", "Balanced", "High Precision"
                                    };
                                    if (auto el = ui.Element({.label = "Dropdown Select"})) {
                                        ui.Select(s_studio.sampleSelectIdx, kSampleItems);
                                    }
                                }
                                if (auto rightCol = ui.Column({.gap = 8.0f})) {
                                    if (auto el = ui.Element({.label = "Parameter Level"})) {
                                        ui.Slider(s_studio.sampleSlider, 0.0f, 100.0f, {.valueFormat = "%.0f %%"});
                                    }
                                    if (auto el = ui.Element({.label = "Progress & Motion (Animation Scale)"})) {
                                        ui.ProgressBar(s_studio.sampleSlider / 100.0f, {
                                            .indeterminate = s_studio.indeterminateBar,
                                            .color = guikit::StateColor::Primary()
                                        });
                                    }
                                    if (auto toggleRow = ui.Row({.gap = 14.0f, .align = guikit::Align::Center})) {
                                        ui.Switch(s_studio.sampleSwitch, {.textRight = "Active"});
                                        ui.Switch(s_studio.sampleSwitchOff, {.textRight = "Dis", .disabled = true});
                                        if (auto chkEl = ui.Element({.label = "Motion Bar", .layout = guikit::ElementLayout::Trailing})) {
                                            ui.Checkbox(s_studio.indeterminateBar);
                                        }
                                    }
                                }
                            }

                            // Instrumentation Strip: Knob + LevelMeter
                            if (auto instRow = ui.Row({.gap = 16.0f, .align = guikit::Align::Center})) {
                                ui.Knob(s_studio.sampleKnob, 0.0f, 100.0f, {.valueFormat = "%.0f%%", .size = guikit::UiSize::Sm});
                                if (auto meterCol = ui.Column({.gap = 4.0f, .width = guikit::SizeMode::Stretch})) {
                                    ui.Text("Multi-Zone DSP LevelMeter (tracks Success / Warning / Danger tokens)", {
                                        .role = guikit::TextRole::LabelSmall,
                                        .variant = guikit::UiVariant::Muted
                                    });
                                    const keel::f64 dbVal = -60.0 + keel::as<keel::f64>(s_studio.sampleSlider) * 0.58;
                                    ui.LevelMeter(dbVal, -60.0, 0.0, {.segments = 24u, .multiZoneColor = true, .size = guikit::UiSize::Sm});
                                }
                            }
                        }
                    }

                    // Scoped Subtree Overrides (ui.Zone) + Syntax Highlighting Preview
                    if (showAll || s_studio.activeStageTab == 3u) {
                        if (auto zoneRow = ui.Row({.gap = 12.0f, .equalWidths = true})) {
                            if (auto z1 = ui.Zone({
                                .surfaceStyle = guikit::SurfaceStyle::Soft,
                                .accent = guikit::Color::Hex(0xF59E0BFF),
                                .radiusScale = 1.4f
                            })) {
                                if (auto zCard = ui.Card({.gap = 8.0f, .padding = guikit::Padding::All(12.0f)})) {
                                    ui.Heading({
                                        .label = "Zone: Soft Amber",
                                        .description = ".surfaceStyle = Soft, .accent = Amber",
                                        .level = guikit::HeadingLevel::H6
                                    });
                                    if (auto r = ui.Row({.gap = 6.0f})) {
                                        ui.Button({.text = "Tonal CTA", .size = guikit::UiSize::Sm, .color = guikit::StateColor::Primary()});
                                        ui.Tag("Zero-Alloc", {.variant = guikit::UiVariant::Warning, .size = guikit::UiSize::Sm});
                                    }
                                }
                            }

                            if (auto z2 = ui.Zone({
                                .surfaceStyle = guikit::SurfaceStyle::Outline,
                                .accent = guikit::Color::Hex(0x10B981FF),
                                .radiusScale = 0.0f
                            })) {
                                if (auto zCard = ui.Card({.gap = 8.0f, .padding = guikit::Padding::All(12.0f)})) {
                                    ui.Heading({
                                        .label = "Zone: Sharp Outline",
                                        .description = ".surfaceStyle = Outline, .radiusScale = 0",
                                        .level = guikit::HeadingLevel::H6
                                    });
                                    if (auto r = ui.Row({.gap = 6.0f})) {
                                        ui.Button({.text = "Outlined", .size = guikit::UiSize::Sm, .color = guikit::StateColor::Primary()});
                                        ui.Tag("Emerald", {.variant = guikit::UiVariant::Success, .size = guikit::UiSize::Sm});
                                    }
                                }
                            }

                            const guikit::ThemePreset hcPreset = (built.mode == guikit::ThemeMode::Dark)
                                ? guikit::ThemePreset::HighContrastDark
                                : guikit::ThemePreset::HighContrastLight;
                            if (auto z3 = ui.Zone({.preset = hcPreset})) {
                                if (auto zCard = ui.Card({.gap = 8.0f, .padding = guikit::Padding::All(12.0f)})) {
                                    ui.Heading({
                                        .label = "Zone: High Contrast",
                                        .description = ".preset = HighContrast",
                                        .level = guikit::HeadingLevel::H6
                                    });
                                    if (auto r = ui.Row({.gap = 6.0f})) {
                                        ui.Button({.text = "WCAG AAA", .size = guikit::UiSize::Sm, .color = guikit::StateColor::Primary()});
                                        ui.Tag("3dp Focus", {.variant = guikit::UiVariant::Info, .size = guikit::UiSize::Sm});
                                    }
                                }
                            }
                        }

                        // Syntax Palette Preview
                        static constexpr keel::StringRef kSyntaxSnippet =
                            "// Live SyntaxPalette (synced with ThemePreset or custom SyntaxPreset)\n"
                            "guikit::ThemeSpec spec{\n"
                            "    .base = guikit::ThemePreset::Nord,\n"
                            "    .surfaceStyle = guikit::SurfaceStyle::Soft,\n"
                            "    .surfaceContrast = 1.15f,\n"
                            "    .radiusScale = 0.85f\n"
                            "};\n"
                            "const guikit::UiTheme theme = guikit::UiTheme::Build(spec);";
                        keel::String previewCode = keel::String::from(kSyntaxSnippet);
                        ui.TextArea(previewCode, {
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
    if (spec.surfaceStyle != guikit::SurfaceStyle::Solid) {
        keel::format_into(out, ",\n    .surfaceStyle = guikit::SurfaceStyle::{}",
            SurfaceStyleEnumIdentifier(spec.surfaceStyle));
    }
    if (spec.syntaxPreset != guikit::SyntaxPreset::Auto) {
        keel::format_into(out, ",\n    .syntaxPreset = guikit::SyntaxPreset::{}",
            SyntaxPresetEnumIdentifier(spec.syntaxPreset));
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
