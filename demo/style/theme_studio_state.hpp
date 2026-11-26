#pragma once

#include "guikit/guikit.hpp"
#include "keel/keel.h"

namespace guikit::demo {

/// Single source of truth for the live Preview Zone design system (Palette x Chrome x Fill + tokens).
struct ThemeStudioState {
    // Base Preset & Orthogonal Design-System Axes (Palette x Chrome x Fill)
    guikit::ThemePreset preset = guikit::ThemePreset::Dark;
    guikit::ChromeStyle chrome = guikit::ChromeStyle::Modern;
    guikit::FillStyle fillStyle = guikit::FillStyle::Solid;
    guikit::SyntaxPreset syntaxPreset = guikit::SyntaxPreset::Auto;

    // Scrollbar & Slider Customization
    keel::Optional<guikit::ScrollbarChromeStyle> scrollbarChrome = keel::kNullOpt;
    bool overrideScrollbarWidth = false;
    keel::f32 scrollbarWidth = 10.0f;
    bool customizeScrollbarTokens = false;
    guikit::ScrollbarStyleTokens customScrollbarTokens = guikit::ScrollbarStyleTokens::Modern();
    keel::Optional<guikit::SliderThumbShape> sliderThumb = keel::kNullOpt;

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
    keel::usize activeStageTab = 0u; // 0=Tokens, 1=Typography, 2=Surfaces, 3=Actions, 4=Fields, 5=Controls, 6=Display

    // Actions preview state
    keel::usize sampleSegmentedIdx = 1u;
    keel::usize sampleSubTabIdx = 0u;
    keel::usize sampleBreadcrumbIdx = 2u;
    keel::usize samplePage = 2u;

    // Fields preview state
    keel::SmallString<64> sampleInput{"Click to test Focus Ring"};
    keel::SmallString<64> sampleFilledInput{"Filled field style"};
    keel::SmallString<64> disabledInput{"Disabled input preview"};
    keel::SmallString<64> sampleValidInput{"Validated value"};
    keel::SmallString<64> sampleWarnInput{"Needs review"};
    keel::SmallString<64> sampleErrorInput{"Invalid syntax"};
    keel::usize sampleSelectIdx = 1u;
    keel::SmallArray<keel::usize, 6> sampleMultiSelect =
        keel::SmallArray<keel::usize, 6>::from({0u, 2u});
    keel::SmallArray<keel::FixedString<64>, 8> sampleTags =
        keel::SmallArray<keel::FixedString<64>, 8>::from({"C++20", "GuiKit", "Zero-STL"});
    keel::f64 sampleSpin = 42.5;
    keel::i32 sampleSpinSplit = 12;
    guikit::DateTime sampleDate{2026, 11, 26, 14, 30, 0};
    guikit::Color sampleColor = guikit::Color::Hex(0x38BDF8FF);

    // Controls preview state
    bool sampleCheckbox = true;
    bool sampleCheckboxOff = false;
    keel::i32 sampleRadio = 1;
    bool sampleSwitch = true;
    bool sampleSwitchOff = false;
    keel::f32 sampleSlider = 68.0f;
    keel::usize sampleStepIdx = 2u;
    keel::f32 sampleRangeLow = 22.0f;
    keel::f32 sampleRangeHigh = 74.0f;
    keel::f32 sampleKnob = 42.0f;
    keel::f32 sampleKnobAux = 76.0f;

    // Display preview state
    bool indeterminateBar = false;
    guikit::ReadoutState sampleReadoutA{};
    guikit::ReadoutState sampleReadoutB{};
    guikit::ReadoutState sampleReadoutC{};

    void ResetToDefaults() noexcept {
        preset = guikit::ThemePreset::Dark;
        chrome = guikit::ChromeStyle::Modern;
        fillStyle = guikit::FillStyle::Solid;
        syntaxPreset = guikit::SyntaxPreset::Auto;
        scrollbarChrome = keel::kNullOpt;
        overrideScrollbarWidth = false;
        scrollbarWidth = 10.0f;
        customizeScrollbarTokens = false;
        customScrollbarTokens = guikit::ScrollbarStyleTokens::Modern();
        sliderThumb = keel::kNullOpt;

        overrideAccent = false;
        overrideNeutral = false;
        overrideBackground = false;
        overrideSurface = false;
        overrideBorder = false;
        overrideText = false;

        overrideSuccess = false;
        overrideWarning = false;
        overrideDanger = false;
        overrideInfo = false;

        surfaceContrast = 1.0f;
        radiusScale = 1.0f;
        spacingScale = 1.0f;
        fontScale = 1.0f;
        lineHeightMultiplier = guikit::kDefaultLineHeightMultiplier;
        borderScale = 1.0f;
        shadowIntensity = 1.0f;
        focusRingWidth = 2.0f;
        disabledOpacity = 0.50f;
        animationScale = 1.0f;
        shellStyle = guikit::ShellStyle::Docked;

        enableLerp = false;
        lerpT = 0.0f;
    }

    void SelectBasePreset(guikit::ThemePreset p) noexcept {
        preset = p;
        const guikit::UiTheme def = guikit::UiTheme::FromPreset(p);
        chrome = def.chromeStyle;
        fillStyle = def.fillStyle;
        overrideAccent = false;
        overrideNeutral = false;
        overrideBackground = false;
        overrideSurface = false;
        overrideBorder = false;
        overrideText = false;
        overrideSuccess = false;
        overrideWarning = false;
        overrideDanger = false;
        overrideInfo = false;
    }

    void ApplySpec(const guikit::ThemeSpec& spec) noexcept {
        ResetToDefaults();
        preset = spec.base;
        const guikit::UiTheme def = guikit::UiTheme::FromPreset(spec.base);
        chrome = spec.chrome.value_or(def.chromeStyle);
        fillStyle = spec.fillStyle.value_or(def.fillStyle);
        syntaxPreset = spec.syntaxPreset;
        scrollbarChrome = spec.scrollbarChrome;
        sliderThumb = spec.sliderThumb;
        if (spec.scrollbarWidth.has_value()) {
            overrideScrollbarWidth = true;
            scrollbarWidth = *spec.scrollbarWidth;
        }
        if (spec.accent.has_value())      { overrideAccent = true;     accentColor = *spec.accent; }
        if (spec.neutral.has_value())     { overrideNeutral = true;    neutralColor = *spec.neutral; }
        if (spec.background.has_value())  { overrideBackground = true; backgroundColor = *spec.background; }
        if (spec.surface.has_value())     { overrideSurface = true;    surfaceColor = *spec.surface; }
        if (spec.border.has_value())      { overrideBorder = true;     borderColor = *spec.border; }
        if (spec.textPrimary.has_value()) { overrideText = true;       textColor = *spec.textPrimary; }
        if (spec.success.has_value())     { overrideSuccess = true;    successColor = *spec.success; }
        if (spec.warning.has_value())     { overrideWarning = true;    warningColor = *spec.warning; }
        if (spec.danger.has_value())      { overrideDanger = true;     dangerColor = *spec.danger; }
        if (spec.info.has_value())        { overrideInfo = true;       infoColor = *spec.info; }
        surfaceContrast = spec.surfaceContrast;
        radiusScale = spec.radiusScale;
        spacingScale = spec.spacingScale;
        fontScale = spec.fontScale;
        lineHeightMultiplier = spec.lineHeightMultiplier;
        borderScale = spec.borderScale;
        shadowIntensity = spec.shadowIntensity;
        focusRingWidth = spec.focusRingWidth;
        disabledOpacity = spec.disabledOpacity;
        animationScale = spec.animationScale;
        shellStyle = spec.shellStyle;
    }

    void RandomizeHarmonic() noexcept {
        randomSeed = randomSeed * 1664525u + 1013904223u;
        const keel::f32 hue = keel::as<keel::f32>(randomSeed % 360u);
        const bool isLight = ((randomSeed >> 9) & 1u) != 0u;
        const keel::u32 styleIdx = (randomSeed >> 10) % 3u;

        ResetToDefaults();
        preset = isLight ? guikit::ThemePreset::Light : guikit::ThemePreset::Dark;
        chrome = guikit::ChromeStyle::Modern;
        fillStyle = keel::from_underlying<guikit::FillStyle>(styleIdx);
        overrideAccent = true;
        accentColor = guikit::Color::FromHsl(hue, isLight ? 0.72f : 0.78f, isLight ? 0.42f : 0.58f);
        overrideNeutral = true;
        neutralColor = guikit::Color::FromHsl(hue, 0.22f, isLight ? 0.94f : 0.14f);
        radiusScale = 0.65f + keel::as<keel::f32>((randomSeed >> 13) % 10u) * 0.10f;
    }

    [[nodiscard]] guikit::ThemeSpec BuildSpec() const noexcept {
        guikit::ThemeSpec spec{};
        spec.base = preset;
        spec.chrome = chrome;
        spec.fillStyle = fillStyle;
        spec.syntaxPreset = syntaxPreset;
        if (scrollbarChrome.has_value()) {
            spec.scrollbarChrome = scrollbarChrome;
        }
        if (overrideScrollbarWidth) {
            spec.scrollbarWidth = scrollbarWidth;
        }
        if (sliderThumb.has_value()) {
            spec.sliderThumb = sliderThumb;
        }
        if (overrideAccent)     spec.accent = accentColor;
        if (overrideNeutral)    spec.neutral = neutralColor;
        if (overrideBackground) spec.background = backgroundColor;
        if (overrideSurface)    spec.surface = surfaceColor;
        if (overrideBorder)     spec.border = borderColor;
        if (overrideText)       spec.textPrimary = textColor;
        if (overrideSuccess)    spec.success = successColor;
        if (overrideWarning)    spec.warning = warningColor;
        if (overrideDanger)     spec.danger = dangerColor;
        if (overrideInfo)       spec.info = infoColor;
        spec.surfaceContrast = surfaceContrast;
        spec.radiusScale = radiusScale;
        spec.spacingScale = spacingScale;
        spec.fontScale = fontScale;
        spec.lineHeightMultiplier = lineHeightMultiplier;
        spec.borderScale = borderScale;
        spec.shadowIntensity = shadowIntensity;
        spec.focusRingWidth = focusRingWidth;
        spec.disabledOpacity = disabledOpacity;
        spec.animationScale = animationScale;
        spec.shellStyle = shellStyle;
        return spec;
    }

    [[nodiscard]] guikit::UiTheme BuildEffectiveTheme() const noexcept {
        guikit::UiTheme primary = guikit::UiTheme::Build(BuildSpec());
        if (customizeScrollbarTokens) {
            primary.SetScrollbarStyle(customScrollbarTokens);
            if (overrideScrollbarWidth) {
                primary.SetScrollbarWidth(scrollbarWidth);
            }
        }
        if (enableLerp && lerpT > 0.001f) {
            const guikit::UiTheme target = guikit::UiTheme::FromPreset(lerpTarget);
            return guikit::UiTheme::Lerp(primary, target, lerpT);
        }
        return primary;
    }
};

} // namespace guikit::demo
