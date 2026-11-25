#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

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

[[nodiscard]] constexpr keel::StringRef ScrollbarChromeEnumIdentifier(guikit::ScrollbarChromeStyle c) noexcept {
    switch (c) {
        case guikit::ScrollbarChromeStyle::Modern:       return "Modern";
        case guikit::ScrollbarChromeStyle::Flat:         return "Flat";
        case guikit::ScrollbarChromeStyle::ClassicBevel: return "ClassicBevel";
        case guikit::ScrollbarChromeStyle::Framed:       return "Framed";
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

bool RenderColorOverrideRow(
    guikit::Context& ui,
    keel::StringRef label,
    bool& enabled,
    guikit::Color& color,
    guikit::Color defaultSeed
) {
    bool changed = false;
    if (auto el = guikit::Element(ui, {.label = label, .layout = guikit::ElementLayout::Leading})) {
        if (guikit::Switch(ui, enabled, {.size = guikit::UiSize::Sm}).changed) {
            if (enabled) {
                color = defaultSeed;
            }
            changed = true;
        }
    }
    if (enabled) {
        if (guikit::ColorPicker(ui, color, {.showHex = true, .showAlpha = false, .showSwatches = true, .size = guikit::UiSize::Sm}).changed) {
            changed = true;
        }
    }
    return changed;
}

} // namespace

void RenderThemeStudioInspector(guikit::Context& ui, DemoContext& ctx) {
    auto& st = ctx.studio;
    bool dirty = false;
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
            .tooltip = "Also apply the Preview Theme to the outer Demo application shell",
            .size = guikit::UiSize::Sm,
            .width = guikit::SizeMode::Stretch,
            .variant = guikit::UiVariant::Primary
        }).clicked) {
            ctx.pendingShellTheme = ctx.previewTheme;
            ui.InvalidateAll();
        }
        if (guikit::Button(ui, {
            .icon = guikit::Icon::Flash(),
            .square = true,
            .tooltip = "Generate a harmonic WCAG-compliant theme",
            .size = guikit::UiSize::Sm,
            .variant = guikit::UiVariant::Info
        }).clicked) {
            st.RandomizeHarmonic();
            dirty = true;
        }
        if (guikit::Button(ui, {
            .icon = guikit::Icon::Refresh(),
            .square = true,
            .tooltip = "Reset all tokens to default",
            .size = guikit::UiSize::Sm,
            .variant = guikit::UiVariant::Secondary
        }).clicked) {
            st.ResetToDefaults();
            dirty = true;
        }
    }

    // 1. THEME PRESET
    if (auto acc = guikit::Accordion(ui, {.label = "THEME PRESET", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize presetIdx = keel::as<keel::usize>(st.preset);
        if (auto el = guikit::Element(ui, {.label = "Base Preset"})) {
            if (guikit::Select(ui, presetIdx, kPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                st.SelectBasePreset(keel::from_underlying<guikit::ThemePreset>(presetIdx));
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {
            .label = "Orthogonal Cross-Combos",
            .description = "Any color palette combined with any ChromeStyle & FillStyle"
        })) {
            auto applyCombo = [&](guikit::ThemePreset p, guikit::ChromeStyle c, guikit::FillStyle f) {
                st.SelectBasePreset(p);
                st.chrome = c;
                st.fillStyle = f;
                st.scrollbarChrome = keel::kNullOpt;
                const auto tok = guikit::ChromeTokens::FromStyle(c);
                st.customScrollbarTokens = guikit::ScrollbarStyleTokens::ForChrome(tok.scrollbar);
                dirty = true;
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
    const guikit::UiTheme baseRef = guikit::UiTheme::FromPreset(st.preset);
    if (auto acc = guikit::Accordion(ui, {.label = "APPEARANCE & PALETTE", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize chromeIdx = keel::as<keel::usize>(st.chrome);
        if (auto el = guikit::Element(ui, {.label = "Chrome Style", .description = "Structural look-and-feel (color-independent)"})) {
            if (guikit::Select(ui, chromeIdx, kChromeNames, {.size = guikit::UiSize::Sm}).changed) {
                st.chrome = keel::from_underlying<guikit::ChromeStyle>(chromeIdx);
                dirty = true;
            }
        }

        keel::usize fillIdx = keel::as<keel::usize>(st.fillStyle);
        if (auto el = guikit::Element(ui, {.label = "Fill Style", .description = "Semantic emphasis fill (Solid / Soft / Outline)"})) {
            if (guikit::Select(ui, fillIdx, kFillNames, {.size = guikit::UiSize::Sm}).changed) {
                st.fillStyle = keel::from_underlying<guikit::FillStyle>(fillIdx);
                dirty = true;
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Layer Contrast", .description = "Separation between bg, cards & borders"})) {
            if (guikit::Slider(ui, st.surfaceContrast, 0.50f, 1.80f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }

        dirty |= RenderColorOverrideRow(ui, "Brand Accent", st.overrideAccent, st.accentColor, baseRef.palette.accent);
        dirty |= RenderColorOverrideRow(ui, "Neutral Tint", st.overrideNeutral, st.neutralColor, baseRef.palette.background);

        keel::usize synIdx = keel::as<keel::usize>(st.syntaxPreset);
        if (auto el = guikit::Element(ui, {.label = "Syntax Scheme"})) {
            if (guikit::Select(ui, synIdx, kSyntaxNames, {.size = guikit::UiSize::Sm}).changed) {
                st.syntaxPreset = keel::from_underlying<guikit::SyntaxPreset>(synIdx);
                dirty = true;
            }
        }
    }

    // 2b. SCROLLBAR CUSTOMIZATION
    static constexpr keel::StaticArray<keel::StringRef, 5> kSbOverrideNames = {
        "Auto (Match Chrome)", "Modern", "Flat", "Classic 3D", "Framed"
    };
    static constexpr keel::StaticArray<keel::StringRef, guikit::kScrollbarChromeStyleCount> kSbChromeNames = {
        guikit::ScrollbarChromeStyleName(guikit::ScrollbarChromeStyle::Modern),
        guikit::ScrollbarChromeStyleName(guikit::ScrollbarChromeStyle::Flat),
        guikit::ScrollbarChromeStyleName(guikit::ScrollbarChromeStyle::ClassicBevel),
        guikit::ScrollbarChromeStyleName(guikit::ScrollbarChromeStyle::Framed)
    };
    static constexpr keel::StaticArray<keel::StringRef, 3> kSbArrowNames = {
        "None", "Triangle", "Chevron"
    };
    if (auto acc = guikit::Accordion(ui, {.label = "SCROLLBAR", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
        keel::usize sbOverrideIdx = st.scrollbarChrome.has_value()
            ? (keel::as<keel::usize>(*st.scrollbarChrome) + 1u)
            : 0u;
        if (auto el = guikit::Element(ui, {.label = "Scrollbar Style", .description = "Modern pill, Flat (Win10/IDE), Classic 3D, or Framed"})) {
            if (guikit::Select(ui, sbOverrideIdx, kSbOverrideNames, {.size = guikit::UiSize::Sm}).changed) {
                if (sbOverrideIdx == 0u) {
                    st.scrollbarChrome = keel::kNullOpt;
                    const auto tok = guikit::ChromeTokens::FromStyle(st.chrome);
                    st.customScrollbarTokens = guikit::ScrollbarStyleTokens::ForChrome(tok.scrollbar);
                } else {
                    const auto sc = keel::from_underlying<guikit::ScrollbarChromeStyle>(sbOverrideIdx - 1u);
                    st.scrollbarChrome = sc;
                    st.customScrollbarTokens = guikit::ScrollbarStyleTokens::ForChrome(sc);
                }
                st.customizeScrollbarTokens = false;
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Custom Width", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, st.overrideScrollbarWidth, {.size = guikit::UiSize::Sm}).changed) {
                if (st.overrideScrollbarWidth) {
                    st.scrollbarWidth = ctx.previewTheme.GetScrollbar(guikit::UiSize::Md).trackWidth;
                }
                dirty = true;
            }
        }
        if (st.overrideScrollbarWidth) {
            if (guikit::Slider(ui, st.scrollbarWidth, 4.0f, 24.0f, {.valueFormat = "%.0f dp", .step = 1.0f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }

        if (auto el = guikit::Element(ui, {.label = "Fine-Tune Tokens", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, st.customizeScrollbarTokens, {.size = guikit::UiSize::Sm}).changed) {
                if (st.customizeScrollbarTokens) {
                    st.customScrollbarTokens = ctx.previewTheme.scrollbarStyle;
                }
                dirty = true;
            }
        }
        if (st.customizeScrollbarTokens) {
            auto& sbTok = st.customScrollbarTokens;
            keel::usize chromeIdx = keel::as<keel::usize>(sbTok.chrome);
            if (auto el = guikit::Element(ui, {.label = "Chrome Style"})) {
                if (guikit::Select(ui, chromeIdx, kSbChromeNames, {.size = guikit::UiSize::Sm}).changed) {
                    sbTok = guikit::ScrollbarStyleTokens::ForChrome(
                        keel::from_underlying<guikit::ScrollbarChromeStyle>(chromeIdx)
                    );
                    dirty = true;
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Arrow Buttons", .layout = guikit::ElementLayout::Leading})) {
                if (guikit::Switch(ui, sbTok.showButtons, {.size = guikit::UiSize::Sm}).changed) {
                    if (sbTok.showButtons && sbTok.arrowStyle == guikit::ScrollbarArrowStyle::None) {
                        sbTok.arrowStyle = guikit::ScrollbarArrowStyle::Triangle;
                    }
                    dirty = true;
                }
            }
            if (sbTok.showButtons) {
                keel::usize arrowIdx = keel::as<keel::usize>(sbTok.arrowStyle);
                if (auto el = guikit::Element(ui, {.label = "Arrow Glyph"})) {
                    if (guikit::Select(ui, arrowIdx, kSbArrowNames, {.size = guikit::UiSize::Sm}).changed) {
                        sbTok.arrowStyle = keel::from_underlying<guikit::ScrollbarArrowStyle>(arrowIdx);
                        dirty = true;
                    }
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Thumb Inset"})) {
                if (guikit::Slider(ui, sbTok.thumbInset, 0.0f, 4.0f, {.valueFormat = "%.1f dp", .step = 0.5f, .size = guikit::UiSize::Sm}).changed) {
                    dirty = true;
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Always Show Track", .layout = guikit::ElementLayout::Leading})) {
                if (guikit::Switch(ui, sbTok.alwaysShowTrack, {.size = guikit::UiSize::Sm}).changed) {
                    dirty = true;
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Corner Box", .layout = guikit::ElementLayout::Leading})) {
                if (guikit::Switch(ui, sbTok.showCornerBox, {.size = guikit::UiSize::Sm}).changed) {
                    dirty = true;
                }
            }
        }
    }

    // 3. DIRECT SURFACE & SEMANTIC COLORS
    if (auto acc = guikit::Accordion(ui, {.label = "SURFACE & SEMANTIC TOKENS", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        dirty |= RenderColorOverrideRow(ui, "Background",  st.overrideBackground, st.backgroundColor, baseRef.palette.background);
        dirty |= RenderColorOverrideRow(ui, "Card Surface", st.overrideSurface, st.surfaceColor, baseRef.palette.surface);
        dirty |= RenderColorOverrideRow(ui, "Border Color", st.overrideBorder, st.borderColor, baseRef.palette.border);
        dirty |= RenderColorOverrideRow(ui, "Primary Text", st.overrideText, st.textColor, baseRef.palette.textPrimary);
        guikit::Separator(ui);
        dirty |= RenderColorOverrideRow(ui, "Success", st.overrideSuccess, st.successColor, baseRef.GetSemanticAccent(guikit::UiVariant::Success));
        dirty |= RenderColorOverrideRow(ui, "Warning", st.overrideWarning, st.warningColor, baseRef.GetSemanticAccent(guikit::UiVariant::Warning));
        dirty |= RenderColorOverrideRow(ui, "Danger",  st.overrideDanger,  st.dangerColor,  baseRef.GetSemanticAccent(guikit::UiVariant::Danger));
        dirty |= RenderColorOverrideRow(ui, "Info",    st.overrideInfo,    st.infoColor,    baseRef.GetSemanticAccent(guikit::UiVariant::Info));
    }

    // 4. GEOMETRY & ELEVATION
    if (auto acc = guikit::Accordion(ui, {.label = "GEOMETRY & ELEVATION", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = guikit::Element(ui, {.label = "Radius Scale"})) {
            if (guikit::Slider(ui, st.radiusScale, 0.0f, 2.0f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        if (auto rRow = guikit::Row(ui, {.gap = 4.0f, .equalWidths = true})) {
            if (guikit::Button(ui, {.text = "Sharp", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { st.radiusScale = 0.0f; dirty = true; }
            if (guikit::Button(ui, {.text = "Crisp", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { st.radiusScale = 0.5f; dirty = true; }
            if (guikit::Button(ui, {.text = "Std",   .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { st.radiusScale = 1.0f; dirty = true; }
            if (guikit::Button(ui, {.text = "Soft",  .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) { st.radiusScale = 1.6f; dirty = true; }
        }
        if (auto el = guikit::Element(ui, {.label = "Spacing Scale"})) {
            if (guikit::Slider(ui, st.spacingScale, 0.70f, 1.40f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Font Scale"})) {
            if (guikit::Slider(ui, st.fontScale, 0.85f, 1.35f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Line Height"})) {
            if (guikit::Slider(ui, st.lineHeightMultiplier, 1.10f, 1.75f, {.valueFormat = "%.2f", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Border Width Scale"})) {
            if (guikit::Slider(ui, st.borderScale, 0.0f, 2.5f, {.valueFormat = "%.1fx", .step = 0.1f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Shadow Intensity", .description = "0.00x = Flat / No shadows"})) {
            if (guikit::Slider(ui, st.shadowIntensity, 0.0f, 1.6f, {.valueFormat = "%.2fx", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        keel::i32 shellIdx = (st.shellStyle == guikit::ShellStyle::Floating) ? 1 : 0;
        static constexpr keel::StaticArray<keel::StringRef, 2> kShellNames = {"Docked", "Floating"};
        if (auto el = guikit::Element(ui, {.label = "Shell Chrome"})) {
            if (guikit::Segmented(ui, shellIdx, kShellNames, {.size = guikit::UiSize::Sm}).changed) {
                st.shellStyle = (shellIdx == 1) ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked;
                dirty = true;
            }
        }
    }

    // 5. ACCESSIBILITY & MOTION
    if (auto acc = guikit::Accordion(ui, {.label = "ACCESSIBILITY & MOTION", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = guikit::Element(ui, {.label = "Focus Ring Width", .description = "Keyboard focus stroke in dp"})) {
            if (guikit::Slider(ui, st.focusRingWidth, 1.0f, 4.5f, {.valueFormat = "%.1f dp", .step = 0.5f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Disabled Opacity"})) {
            if (guikit::Slider(ui, st.disabledOpacity, 0.20f, 0.80f, {.valueFormat = "%.2f", .step = 0.05f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Animation Scale", .description = "0.00x enables Reduced Motion"})) {
            if (guikit::Slider(ui, st.animationScale, 0.0f, 2.0f, {.valueFormat = "%.2fx", .step = 0.1f, .size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
        }
    }

    // 6. THEME MORPHING (UiTheme::Lerp)
    if (auto acc = guikit::Accordion(ui, {.label = "THEME MORPHING (LERP)", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        if (auto el = guikit::Element(ui, {.label = "Enable Blend", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, st.enableLerp, {.size = guikit::UiSize::Sm}).changed) {
                if (st.enableLerp && st.lerpT < 0.01f) st.lerpT = 0.5f;
                dirty = true;
            }
        }
        if (st.enableLerp) {
            keel::usize targetIdx = keel::as<keel::usize>(st.lerpTarget);
            if (auto el = guikit::Element(ui, {.label = "Target Theme B"})) {
                if (guikit::Select(ui, targetIdx, kPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                    st.lerpTarget = keel::from_underlying<guikit::ThemePreset>(targetIdx);
                    dirty = true;
                }
            }
            if (auto el = guikit::Element(ui, {.label = "Blend Factor t"})) {
                if (guikit::Slider(ui, st.lerpT, 0.0f, 1.0f, {.valueFormat = "%.0f%%", .step = 0.02f, .size = guikit::UiSize::Sm}).changed) {
                    dirty = true;
                }
            }
        }
    }

    // 7. JSON IMPORT / EXPORT
    if (auto acc = guikit::Accordion(ui, {.label = "JSON IMPORT & CODE", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        static constexpr keel::StaticArray<keel::StringRef, 2> kCodeFormats = {"C++20", "JSON"};
        if (auto el = guikit::Element(ui, {.label = "Bottom Code Panel"})) {
            if (guikit::Segmented(ui, st.codeFormatIdx, kCodeFormats, {.size = guikit::UiSize::Sm}).changed) {
                ui.InvalidateAll();
            }
        }
        if (st.jsonDraft.empty()) {
            st.jsonDraft = st.BuildSpec().ToJson();
        }
        if (auto btnRow = guikit::Row(ui, {.gap = 6.0f, .equalWidths = true})) {
            if (guikit::Button(ui, {.text = "Snapshot", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Secondary}).clicked) {
                st.jsonDraft = st.BuildSpec().ToJson();
                st.jsonImportStatus = 0;
                ui.InvalidateAll();
            }
            if (guikit::Button(ui, {.text = "Load JSON", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Primary}).clicked) {
                const auto parsed = guikit::ThemeSpec::FromJson(st.jsonDraft);
                if (parsed.has_value()) {
                    st.ApplySpec(*parsed);
                    st.jsonImportStatus = 1;
                    dirty = true;
                } else {
                    st.jsonImportStatus = -1;
                    ui.InvalidateAll();
                }
            }
        }
        if (st.jsonImportStatus == 1) {
            guikit::Tag(ui, "JSON Applied", {.variant = guikit::UiVariant::Success, .size = guikit::UiSize::Xs});
        } else if (st.jsonImportStatus == -1) {
            guikit::Tag(ui, "Invalid JSON syntax", {.variant = guikit::UiVariant::Danger, .size = guikit::UiSize::Xs});
        }
        guikit::TextArea(ui, st.jsonDraft, {
            .autoHeight = false,
            .showLineNumbers = false,
            .monospace = true,
            .height = 132.0f
        });
    }

    if (dirty) {
        ctx.RebuildPreviewTheme();
        ui.InvalidateAll();
    }
}

void RenderThemeStudioStage(guikit::Context& ui, DemoContext& ctx) {
    auto& st = ctx.studio;
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
                        st.jsonDraft = st.BuildSpec().ToJson();
                        st.jsonImportStatus = 0;
                        ui.InvalidateAll();
                    }
                    if (file.Item("Copy Theme JSON", {.icon = guikit::Icon::Copy()})) {
                        const keel::String json = st.BuildSpec().ToJson();
                        ui.SetClipboardText(json.as_ref());
                        guikit::ShowToast(ui, {
                            .message = "Copied ThemeSpec JSON to clipboard",
                            .variant = guikit::UiVariant::Success
                        });
                    }
                    file.Separator();
                    if (file.Item("Reset to Defaults", {.icon = guikit::Icon::Refresh(), .variant = guikit::UiVariant::Danger})) {
                        st.ResetToDefaults();
                        ctx.RebuildPreviewTheme();
                        ui.InvalidateAll();
                    }
                }

                if (auto themeMenu = mb.Menu("Theme", dropOpts)) {
                    if (auto presetsSub = themeMenu.SubMenu("Base Preset", {.icon = guikit::Icon::Eye()})) {
                        keel::i32 presetIdx = keel::as<keel::i32>(st.preset);
                        for (keel::usize pi = 0; pi < guikit::kThemePresetCount; ++pi) {
                            const auto p = keel::from_underlying<guikit::ThemePreset>(pi);
                            if (presetsSub.Radio(guikit::ThemePresetName(p), presetIdx, keel::as<keel::i32>(pi))) {
                                ctx.SelectPreviewPreset(p);
                                ui.InvalidateAll();
                            }
                        }
                    }
                    themeMenu.Separator();
                    if (themeMenu.Item("Harmonic Randomize", {.icon = guikit::Icon::Flash()})) {
                        st.RandomizeHarmonic();
                        ctx.RebuildPreviewTheme();
                        ui.InvalidateAll();
                    }
                    if (themeMenu.Checkbox("Theme Morph (Lerp)", st.enableLerp, {.keepOpen = true})) {
                        ctx.RebuildPreviewTheme();
                        ui.InvalidateAll();
                    }
                }

                if (auto surfMenu = mb.Menu("Chrome", dropOpts)) {
                    surfMenu.Section("CHROME STYLE");
                    keel::i32 chrIdx = keel::as<keel::i32>(st.chrome);
                    for (keel::i32 ci = 0; ci < keel::as<keel::i32>(guikit::kChromeStyleCount); ++ci) {
                        const auto cs = keel::from_underlying<guikit::ChromeStyle>(ci);
                        if (surfMenu.Radio(guikit::ChromeStyleName(cs), chrIdx, ci)) {
                            ctx.SelectPreviewChrome(cs);
                            ui.InvalidateAll();
                        }
                    }
                    surfMenu.Separator();
                    surfMenu.Section("FILL STYLE");
                    keel::i32 fIdx = keel::as<keel::i32>(st.fillStyle);
                    for (keel::i32 fi = 0; fi < keel::as<keel::i32>(guikit::kFillStyleCount); ++fi) {
                        const auto fs = keel::from_underlying<guikit::FillStyle>(fi);
                        if (surfMenu.Radio(guikit::FillStyleName(fs), fIdx, fi)) {
                            ctx.SelectPreviewFillStyle(fs);
                            ui.InvalidateAll();
                        }
                    }
                    surfMenu.Separator();
                    surfMenu.Section("SHELL STYLE");
                    keel::i32 shellIdx = (st.shellStyle == guikit::ShellStyle::Floating) ? 1 : 0;
                    if (surfMenu.Radio("Docked", shellIdx, 0)) {
                        st.shellStyle = guikit::ShellStyle::Docked;
                        ctx.RebuildPreviewTheme();
                        ui.InvalidateAll();
                    }
                    if (surfMenu.Radio("Floating", shellIdx, 1)) {
                        st.shellStyle = guikit::ShellStyle::Floating;
                        ctx.RebuildPreviewTheme();
                        ui.InvalidateAll();
                    }
                }

                if (auto viewMenu = mb.Menu("View", dropOpts)) {
                    viewMenu.Section("STAGE SECTION");
                    keel::i32 tabIdx = keel::as<keel::i32>(st.activeStageTab);
                    if (viewMenu.Radio("Full Showcase", tabIdx, 0))     { st.activeStageTab = 0u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Token Swatches", tabIdx, 1))    { st.activeStageTab = 1u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Controls & States", tabIdx, 2)) { st.activeStageTab = 2u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Zones & Syntax", tabIdx, 3))    { st.activeStageTab = 3u; ui.InvalidateAll(); }
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
                    st.RandomizeHarmonic();
                    ctx.RebuildPreviewTheme();
                    ui.InvalidateAll();
                }
                if (tb.Action({
                    .text = "Reset",
                    .tooltip = "Reset all theme studio overrides",
                    .icon = guikit::Icon::Close(),
                    .style = guikit::ButtonStyle::Default,
                    .variant = guikit::UiVariant::Secondary
                }).clicked) {
                    st.ResetToDefaults();
                    ctx.RebuildPreviewTheme();
                    ui.InvalidateAll();
                }

                tb.Separator();

                keel::i32 chrTool = keel::as<keel::i32>(st.chrome);
                bool chrChanged = false;
                chrChanged |= tb.Item(chrTool, 0, {.text = "Modern"}).changed;
                chrChanged |= tb.Item(chrTool, 1, {.text = "Fluent"}).changed;
                chrChanged |= tb.Item(chrTool, 2, {.text = "Classic 3D"}).changed;
                chrChanged |= tb.Item(chrTool, 3, {.text = "Brutalist"}).changed;
                if (chrChanged) {
                    ctx.SelectPreviewChrome(keel::from_underlying<guikit::ChromeStyle>(chrTool));
                    ui.InvalidateAll();
                }

                tb.Separator();

                keel::i32 fillTool = keel::as<keel::i32>(st.fillStyle);
                bool fillChanged = false;
                fillChanged |= tb.Item(fillTool, 0, {.text = "Solid"}).changed;
                fillChanged |= tb.Item(fillTool, 1, {.text = "Soft"}).changed;
                fillChanged |= tb.Item(fillTool, 2, {.text = "Outline"}).changed;
                if (fillChanged) {
                    ctx.SelectPreviewFillStyle(keel::from_underlying<guikit::FillStyle>(fillTool));
                    ui.InvalidateAll();
                }

                tb.Separator();

                bool isFloating = (st.shellStyle == guikit::ShellStyle::Floating);
                if (tb.Toggle(isFloating, {
                    .text = "Floating",
                    .tooltip = "Toggle Docked vs Floating shell panels",
                    .variant = guikit::UiVariant::Info
                }).changed) {
                    st.shellStyle = isFloating ? guikit::ShellStyle::Floating : guikit::ShellStyle::Docked;
                    ctx.RebuildPreviewTheme();
                    ui.InvalidateAll();
                }
                if (tb.Toggle(st.enableLerp, {
                    .text = "Morph",
                    .tooltip = "Toggle UiTheme::Lerp morphing",
                    .icon = guikit::Icon::Flash(),
                    .variant = guikit::UiVariant::Warning
                }).changed) {
                    ctx.RebuildPreviewTheme();
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

                    const keel::StringRef titleText = st.enableLerp && st.lerpT > 0.001f
                        ? ui.Format("Theme Studio — {} <-> {} ({:.0f}%)",
                            guikit::ThemePresetName(st.preset),
                            guikit::ThemePresetName(st.lerpTarget),
                            st.lerpT * 100.0f)
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
                    guikit::TabBar(ui, st.activeStageTab, kStageTabs, {.size = guikit::UiSize::Sm});

                    const bool showAll = (st.activeStageTab == 0u);

                    // WCAG 2.1 Audit Cards (visible on Full Showcase & Token Swatches)
                    if (showAll || st.activeStageTab == 1u) {
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
                    if (showAll || st.activeStageTab == 2u) {
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
                                        guikit::InputField(ui, st.sampleInput, {.clearable = true});
                                    }
                                    if (auto el = guikit::Element(ui, {.label = "Disabled Input", .disabled = true})) {
                                        guikit::InputField(ui, st.disabledInput, {.disabled = true});
                                    }
                                    static constexpr keel::StaticArray<keel::StringRef, 3> kSampleItems = {
                                        "Low Latency", "Balanced", "High Precision"
                                    };
                                    if (auto el = guikit::Element(ui, {.label = "Dropdown Select"})) {
                                        guikit::Select(ui, st.sampleSelectIdx, kSampleItems);
                                    }
                                }
                                if (auto rightCol = guikit::Column(ui, {.gap = 8.0f})) {
                                    if (auto el = guikit::Element(ui, {.label = "Parameter Level"})) {
                                        guikit::Slider(ui, st.sampleSlider, 0.0f, 100.0f, {.valueFormat = "%.0f %%"});
                                    }
                                    if (auto el = guikit::Element(ui, {.label = "Progress & Motion (Animation Scale)"})) {
                                        guikit::ProgressBar(ui, st.sampleSlider / 100.0f, {
                                            .indeterminate = st.indeterminateBar,
                                            .color = guikit::StateColor::Primary()
                                        });
                                    }
                                    if (auto toggleRow = guikit::Row(ui, {.gap = 14.0f, .align = guikit::Align::Center})) {
                                        guikit::Switch(ui, st.sampleSwitch, {.textRight = "Active"});
                                        guikit::Switch(ui, st.sampleSwitchOff, {.textRight = "Dis", .disabled = true});
                                        if (auto chkEl = guikit::Element(ui, {.label = "Motion Bar", .layout = guikit::ElementLayout::Trailing})) {
                                            guikit::Checkbox(ui, st.indeterminateBar);
                                        }
                                    }
                                }
                            }

                            // Instrumentation Strip: Knob + LevelMeter
                            if (auto instRow = guikit::Row(ui, {.gap = 16.0f, .align = guikit::Align::Center})) {
                                guikit::Knob(ui, st.sampleKnob, 0.0f, 100.0f, {.valueFormat = "%.0f%%", .size = guikit::UiSize::Sm});
                                if (auto meterCol = guikit::Column(ui, {.gap = 4.0f, .width = guikit::SizeMode::Stretch})) {
                                    guikit::Text(ui, "Multi-Zone DSP LevelMeter (tracks Success / Warning / Danger tokens)", {
                                        .role = guikit::TextRole::LabelSmall,
                                        .variant = guikit::UiVariant::Muted
                                    });
                                    const keel::f64 dbVal = -60.0 + keel::as<keel::f64>(st.sampleSlider) * 0.58;
                                    guikit::LevelMeter(ui, dbVal, -60.0, 0.0, {.segments = 24u, .multiZoneColor = true, .size = guikit::UiSize::Sm});
                                }
                            }

                            // Interactive 2D Scrollbar Preview (Vertical + Horizontal + Corner Box)
                            if (auto sbEl = guikit::Element(ui, {
                                .label = ui.Format("Scrollbar Preview — {} ({:.0f}dp)",
                                    guikit::ScrollbarChromeStyleName(built.scrollbarStyle.chrome),
                                    built.GetScrollbar(guikit::UiSize::Md).trackWidth),
                                .description = "Test vertical/horizontal stepper arrows, thumb drag & corner box"
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
                                            "Row {:02d}  |  Theme: {}  |  Scrollbar: {}  |  Horizontal overflow sample column A · column B · column C · column D",
                                            row,
                                            guikit::ChromeStyleName(built.chromeStyle),
                                            guikit::ScrollbarChromeStyleName(built.scrollbarStyle.chrome)
                                        ), {.role = guikit::TextRole::CodeSmall});
                                    }
                                }
                            }
                        }
                    }

                    // Scoped Subtree Overrides (ui.Zone) + Syntax Highlighting Preview
                    if (showAll || st.activeStageTab == 3u) {
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

keel::String GenerateThemeStudioCode(const DemoContext& ctx) {
    const auto& st = ctx.studio;
    const guikit::ThemeSpec spec = st.BuildSpec();
    if (st.codeFormatIdx == 1) {
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
    if (spec.scrollbarChrome.has_value()) {
        keel::format_into(out, ",\n    .scrollbarChrome = guikit::ScrollbarChromeStyle::{}",
            ScrollbarChromeEnumIdentifier(*spec.scrollbarChrome));
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
    if (st.enableLerp && st.lerpT > 0.001f) {
        keel::format_into(out,
            "const guikit::UiTheme morphed = guikit::UiTheme::Lerp(\n"
            "    theme,\n"
            "    guikit::UiTheme::FromPreset(guikit::ThemePreset::{}),\n"
            "    {:.2f}f\n"
            ");\n",
            PresetEnumIdentifier(st.lerpTarget),
            st.lerpT
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
