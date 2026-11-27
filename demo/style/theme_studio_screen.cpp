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
        case guikit::ThemePreset::Pulse:             return "Pulse";
        case guikit::ThemePreset::PulseLight:        return "PulseLight";
    }
    return "Dark";
}

[[nodiscard]] constexpr keel::StringRef ChromeStyleEnumIdentifier(guikit::ChromeStyle s) noexcept {
    switch (s) {
        case guikit::ChromeStyle::Modern:    return "Modern";
        case guikit::ChromeStyle::Fluent:    return "Fluent";
        case guikit::ChromeStyle::Classic3D: return "Classic3D";
        case guikit::ChromeStyle::Brutalist: return "Brutalist";
        case guikit::ChromeStyle::Glass:     return "Glass";
        case guikit::ChromeStyle::Luminous:  return "Luminous";
    }
    return "Modern";
}

[[nodiscard]] constexpr keel::StringRef FillStyleEnumIdentifier(guikit::FillStyle s) noexcept {
    switch (s) {
        case guikit::FillStyle::Solid:    return "Solid";
        case guikit::FillStyle::Soft:     return "Soft";
        case guikit::FillStyle::Outline:  return "Outline";
        case guikit::FillStyle::Gradient: return "Gradient";
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

[[nodiscard]] constexpr keel::StringRef SliderThumbEnumIdentifier(guikit::SliderThumbShape s) noexcept {
    switch (s) {
        case guikit::SliderThumbShape::Circle:    return "Circle";
        case guikit::SliderThumbShape::Ring:      return "Ring";
        case guikit::SliderThumbShape::Square:    return "Square";
        case guikit::SliderThumbShape::Pointer3D: return "Pointer3D";
        case guikit::SliderThumbShape::Bar:       return "Bar";
    }
    return "Circle";
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

void RenderColorSwatchCard(guikit::Context& ui, keel::StringRef name, guikit::Color color, guikit::Color borderCol, keel::f32 radius) {
    const guikit::Color ink = guikit::Color::BestTextColor(color, guikit::Color::Hex(0x0F172AFF), guikit::Color::Hex(0xFFFFFFFF));
    if (auto tile = guikit::Box(ui, {
        .width = 114.0f,
        .padding = guikit::Padding::Symmetric(9.0f, 7.0f),
        .background = guikit::StateColor{color},
        .border = {.width = 1.0f, .radius = radius, .color = guikit::StateColor{borderCol}}
    })) {
        if (auto col = guikit::Column(ui, {.gap = 2.0f})) {
            guikit::Text(ui, guikit::UiText{name, guikit::StateColor{ink}}, {.role = guikit::TextRole::LabelSmall});
            guikit::Text(ui, guikit::UiText{FormatHexLabel(color).as_ref(), guikit::StateColor{ink.WithAlphaF(0.85f)}}, {.role = guikit::TextRole::CodeSmall});
        }
    }
}

void RenderTokensTab(guikit::Context& ui, const guikit::UiTheme& built) {
    const auto& pal = built.palette;
    const keel::f32 tileRad = built.GetRadius(guikit::UiSize::Sm);

    // 1. Grouped Color Palette
    if (auto card = guikit::Card(ui, {.gap = 12.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Color Palette", .level = guikit::HeadingLevel::H5});

        guikit::Text(ui, "SURFACES", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        if (auto w = guikit::Wrap(ui, {.gap = 8.0f, .runGap = 8.0f})) {
            RenderColorSwatchCard(ui, "Background", pal.background,      pal.border, tileRad);
            RenderColorSwatchCard(ui, "Surface",    pal.surface,         pal.border, tileRad);
            RenderColorSwatchCard(ui, "Elevated",   pal.surfaceElevated, pal.border, tileRad);
            RenderColorSwatchCard(ui, "Hover",      pal.surfaceHover,    pal.border, tileRad);
            RenderColorSwatchCard(ui, "Active",     pal.surfaceActive,   pal.border, tileRad);
            RenderColorSwatchCard(ui, "Input Bg",   pal.inputBackground, pal.border, tileRad);
        }

        guikit::Text(ui, "INK & BORDERS", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        if (auto w = guikit::Wrap(ui, {.gap = 8.0f, .runGap = 8.0f})) {
            RenderColorSwatchCard(ui, "Text Primary",   pal.textPrimary,   pal.border,      tileRad);
            RenderColorSwatchCard(ui, "Text Secondary", pal.textSecondary, pal.border,      tileRad);
            RenderColorSwatchCard(ui, "Text Muted",     pal.textMuted,     pal.border,      tileRad);
            RenderColorSwatchCard(ui, "Border",         pal.border,        pal.borderHover, tileRad);
            RenderColorSwatchCard(ui, "Border Subtle",  pal.borderSubtle,  pal.border,      tileRad);
            RenderColorSwatchCard(ui, "Focus Ring",     pal.focusRing,     pal.border,      tileRad);
        }

        guikit::Text(ui, "SEMANTIC", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        if (auto w = guikit::Wrap(ui, {.gap = 8.0f, .runGap = 8.0f})) {
            RenderColorSwatchCard(ui, "Accent",  pal.accent,                                          pal.border, tileRad);
            RenderColorSwatchCard(ui, "Success", built.GetSemanticAccent(guikit::UiVariant::Success), pal.border, tileRad);
            RenderColorSwatchCard(ui, "Warning", built.GetSemanticAccent(guikit::UiVariant::Warning), pal.border, tileRad);
            RenderColorSwatchCard(ui, "Danger",  built.GetSemanticAccent(guikit::UiVariant::Danger),  pal.border, tileRad);
            RenderColorSwatchCard(ui, "Info",    built.GetSemanticAccent(guikit::UiVariant::Info),    pal.border, tileRad);
        }
    }

    // 2. Compact WCAG 2.1 Contrast Matrix
    if (auto card = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "WCAG 2.1 Contrast", .level = guikit::HeadingLevel::H5});

        auto variantRatio = [&](guikit::UiVariant v) noexcept -> keel::f32 {
            const auto& vc = built.GetVariant(v);
            const guikit::Color bgRef = (vc.bg.a > 0u) ? vc.bg : pal.surface;
            return guikit::Color::ContrastRatio(vc.text, bgRef);
        };

        auto auditCell = [&](keel::StringRef pairLabel, keel::f32 ratio) {
            if (auto cell = guikit::Box(ui, {
                .padding = guikit::Padding::Symmetric(10.0f, 8.0f),
                .background = guikit::StateColor{pal.background},
                .border = {.width = 1.0f, .radius = tileRad, .color = guikit::StateColor{pal.borderSubtle}}
            })) {
                if (auto col = guikit::Column(ui, {.gap = 4.0f, .width = guikit::SizeMode::Stretch})) {
                    guikit::Text(ui, pairLabel, {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
                    if (auto r = guikit::Row(ui, {.gap = 6.0f, .align = guikit::Align::Center, .width = guikit::SizeMode::Stretch})) {
                        guikit::Text(ui, ui.Format("{:.1f}:1", ratio), {.role = guikit::TextRole::TitleSmall});
                        guikit::Spacer(ui);
                        guikit::Tag(ui, ContrastGradeLabel(ratio), {
                            .variant = ContrastBadgeVariant(ratio),
                            .size = guikit::UiSize::Xs
                        });
                    }
                }
            }
        };

        if (auto r1 = guikit::Row(ui, {.gap = 8.0f, .equalWidths = true})) {
            auditCell("Text / Background", guikit::Color::ContrastRatio(pal.textPrimary, pal.background));
            auditCell("Text / Surface",    guikit::Color::ContrastRatio(pal.textPrimary, pal.surface));
            auditCell("Secondary / Bg",    guikit::Color::ContrastRatio(pal.textSecondary, pal.background));
            auditCell("Accent / Ink",      guikit::Color::ContrastRatio(pal.accentText, pal.accent));
        }
        if (auto r2 = guikit::Row(ui, {.gap = 8.0f, .equalWidths = true})) {
            auditCell("Primary", variantRatio(guikit::UiVariant::Primary));
            auditCell("Success", variantRatio(guikit::UiVariant::Success));
            auditCell("Warning", variantRatio(guikit::UiVariant::Warning));
            auditCell("Danger",  variantRatio(guikit::UiVariant::Danger));
        }
    }

    // 3. Geometry Scales (Radius & Spacing)
    static constexpr keel::StaticArray<keel::StringRef, 5> kScaleLabels = {"Xs", "Sm", "Md", "Lg", "Xl"};
    static constexpr keel::StaticArray<guikit::UiSize, 5> kScaleSizes = {
        guikit::UiSize::Xs, guikit::UiSize::Sm, guikit::UiSize::Md, guikit::UiSize::Lg, guikit::UiSize::Xl
    };

    if (auto geoRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        if (auto radCard = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Radius Scale", .level = guikit::HeadingLevel::H6});
            if (auto r = guikit::Row(ui, {.gap = 12.0f, .align = guikit::Align::End})) {
                for (keel::usize i = 0; i < kScaleSizes.size(); ++i) {
                    const keel::f32 rad = built.GetRadius(kScaleSizes[i]);
                    if (auto col = guikit::Column(ui, {.gap = 4.0f, .align = guikit::Align::Center})) {
                        if (auto sample = guikit::Box(ui, {
                            .width = 36.0f,
                            .height = 36.0f,
                            .background = guikit::StateColor{pal.surfaceElevated},
                            .border = {.width = 1.5f, .radius = rad, .color = guikit::StateColor{pal.accent}}
                        })) {}
                        guikit::Text(ui, kScaleLabels[i], {.role = guikit::TextRole::LabelSmall});
                        guikit::Text(ui, ui.Format("{:.1f}dp", rad), {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Muted});
                    }
                }
            }
        }

        if (auto spCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Spacing Scale", .level = guikit::HeadingLevel::H6});
            for (keel::usize i = 0; i < kScaleSizes.size(); ++i) {
                const keel::f32 sp = built.GetGap(kScaleSizes[i]);
                if (auto r = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Align::Center})) {
                    if (auto lblBox = guikit::Box(ui, {.width = 24.0f})) {
                        guikit::Text(ui, kScaleLabels[i], {.role = guikit::TextRole::CodeSmall});
                    }
                    if (auto bar = guikit::Box(ui, {
                        .width = keel::max(4.0f, sp * 6.0f),
                        .height = 10.0f,
                        .background = guikit::StateColor{pal.accent},
                        .border = {.radius = 3.0f}
                    })) {}
                    guikit::Text(ui, ui.Format("{:.1f}dp", sp), {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Muted});
                }
            }
        }
    }
}

void RenderTypographyTab(guikit::Context& ui, const guikit::UiTheme& built) {
    // 1. All Heading Levels H1 .. H6
    if (auto card = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(16.0f)})) {
        guikit::Heading(ui, {
            .label = "Headings (H1 – H6)",
            .level = guikit::HeadingLevel::H5,
            .separator = guikit::HeadingSeparator::Bottom
        });

        struct HeadingSpecimen {
            guikit::HeadingLevel level;
            keel::StringRef tag;
            keel::StringRef title;
        };
        static constexpr keel::StaticArray<HeadingSpecimen, 6> kHeadings = {{
            {guikit::HeadingLevel::H1, "H1", "Display Page Heading"},
            {guikit::HeadingLevel::H2, "H2", "Major Workspace Section"},
            {guikit::HeadingLevel::H3, "H3", "Panel & Dialog Title"},
            {guikit::HeadingLevel::H4, "H4", "Card Group Header"},
            {guikit::HeadingLevel::H5, "H5", "Compact Block Heading"},
            {guikit::HeadingLevel::H6, "H6", "INSPECTOR CATEGORY LABEL"}
        }};

        for (keel::usize i = 0; i < kHeadings.size(); ++i) {
            const auto& spec = kHeadings[i];
            const auto& hm = built.GetHeading(spec.level);
            if (auto row = guikit::Row(ui, {.gap = 14.0f, .align = guikit::Align::Center})) {
                if (auto metaBox = guikit::Box(ui, {.width = 76.0f})) {
                    guikit::Text(ui, ui.Format("{} · {:.0f}dp", spec.tag, hm.titleFontSize), {
                        .role = guikit::TextRole::CodeSmall,
                        .variant = guikit::UiVariant::Muted
                    });
                }
                guikit::Heading(ui, {
                    .label = spec.title,
                    .level = spec.level,
                    .width = guikit::SizeMode::Stretch
                });
            }
            if (i + 1u < kHeadings.size()) {
                guikit::Separator(ui);
            }
        }
    }

    // 2. TextRole Scale & Ink Hierarchy side-by-side
    if (auto twoCol = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        if (auto rolesCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Text Roles", .level = guikit::HeadingLevel::H6, .separator = guikit::HeadingSeparator::Bottom});
            struct RoleSpecimen {
                guikit::TextRole role;
                keel::StringRef name;
                keel::StringRef sample;
            };
            static constexpr keel::StaticArray<RoleSpecimen, 8> kRoles = {{
                {guikit::TextRole::HeadlineSmall, "Headline", "Headline Small"},
                {guikit::TextRole::TitleLarge,    "Title Lg", "Title Large"},
                {guikit::TextRole::TitleMedium,   "Title Md", "Title Medium"},
                {guikit::TextRole::BodyLarge,     "Body Lg",  "Body Large paragraph copy"},
                {guikit::TextRole::BodyMedium,    "Body Md",  "Body Medium standard copy"},
                {guikit::TextRole::BodySmall,     "Body Sm",  "Body Small secondary copy"},
                {guikit::TextRole::LabelMedium,   "Label Md", "Label Medium"},
                {guikit::TextRole::CodeMedium,    "Code Md",  "const f32 dpi = 1.5f;"}
            }};
            for (const auto& r : kRoles) {
                const auto tm = built.GetTypography(r.role);
                if (auto line = guikit::Row(ui, {.gap = 10.0f, .align = guikit::Align::Center})) {
                    if (auto szBox = guikit::Box(ui, {.width = 50.0f})) {
                        guikit::Text(ui, ui.Format("{:.0f}/{:.0f}", tm.fontSize, tm.lineHeight), {
                            .role = guikit::TextRole::CodeSmall,
                            .variant = guikit::UiVariant::Muted
                        });
                    }
                    guikit::Text(ui, r.sample, {.role = r.role});
                }
            }
        }

        if (auto rightCol = guikit::Column(ui, {.gap = 12.0f})) {
            if (auto inkCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
                guikit::Heading(ui, {.label = "Ink Hierarchy", .level = guikit::HeadingLevel::H6, .separator = guikit::HeadingSeparator::Bottom});
                const auto& pal = built.palette;
                auto inkRow = [&](keel::StringRef label, guikit::UiVariant v, guikit::Color c, bool useDirectColor = false) {
                    const keel::f32 ratio = guikit::Color::ContrastRatio(c, pal.surface);
                    if (auto r = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Align::Center})) {
                        if (useDirectColor) {
                            guikit::Text(ui, guikit::UiText{label, guikit::StateColor{c}}, {.role = guikit::TextRole::BodyMedium});
                        } else {
                            guikit::Text(ui, label, {.role = guikit::TextRole::BodyMedium, .variant = v});
                        }
                        guikit::Spacer(ui);
                        guikit::Text(ui, ui.Format("{:.1f}:1", ratio), {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Muted});
                    }
                };
                inkRow("Primary",   guikit::UiVariant::Default,   pal.textPrimary);
                inkRow("Secondary", guikit::UiVariant::Secondary, pal.textSecondary);
                inkRow("Muted",     guikit::UiVariant::Muted,     pal.textMuted);
                inkRow("Accent",    guikit::UiVariant::Primary,   pal.accent);
                inkRow("Disabled",  guikit::UiVariant::Default,   pal.textDisabled, true);
            }

            if (auto rhythmCard = guikit::Card(ui, {.gap = 6.0f, .padding = guikit::Padding::All(14.0f)})) {
                if (auto hdr = guikit::Row(ui, {.align = guikit::Align::Center})) {
                    guikit::Heading(ui, {.label = "Paragraph & Line Height", .level = guikit::HeadingLevel::H6});
                    guikit::Spacer(ui);
                    guikit::Text(ui, ui.Format("{:.2f}x", built.lineHeightMultiplier), {
                        .role = guikit::TextRole::CodeSmall,
                        .variant = guikit::UiVariant::Muted
                    });
                }
                guikit::Text(ui,
                    "Sphinx of black quartz, judge my vow. Crisp typographic rhythm balances density and legibility "
                    "across forms, inspectors, and technical documentation.",
                    {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary}
                );
            }
        }
    }

    // 3. Syntax Highlighting Preview
    if (auto synCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Syntax Palette", .level = guikit::HeadingLevel::H6});
        static constexpr keel::StringRef kSyntaxSnippet =
            "guikit::ThemeSpec spec{\n"
            "    .base = guikit::ThemePreset::Nord,\n"
            "    .chrome = guikit::ChromeStyle::Classic3D,\n"
            "    .fillStyle = guikit::FillStyle::Soft\n"
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

void RenderGlassStageBackdrop(const guikit::Node* node, guikit::ICanvas& canvas, const guikit::UiTheme& theme) {
    if (!node) return;
    const guikit::Rect r = node->computedRect;
    if (r.w <= 1.0f || r.h <= 1.0f) return;

    const auto& pal = theme.palette;
    const bool isDark = (theme.mode == guikit::ThemeMode::Dark);
    const guikit::Color baseBg = isDark
        ? guikit::Color::Lerp(pal.background, pal.accent, 0.14f).WithAlpha(255u)
        : guikit::Color::Lerp(pal.background, pal.accent, 0.10f).WithAlpha(255u);
    canvas.DrawRoundRect(r, node->cornerRadius, baseBg, {}, 0.0f);

    const guikit::Color cAccent  = pal.accent;
    const guikit::Color cInfo    = theme.GetSemanticAccent(guikit::UiVariant::Info);
    const guikit::Color cSuccess = theme.GetSemanticAccent(guikit::UiVariant::Success);
    const guikit::Color cWarning = theme.GetSemanticAccent(guikit::UiVariant::Warning);
    const guikit::Color cDanger  = theme.GetSemanticAccent(guikit::UiVariant::Danger);

    // Subtle geometric grid lines so refractive edge bending is immediately legible
    const guikit::Color gridCol = pal.textPrimary.WithAlphaF(isDark ? 0.10f : 0.09f);
    for (keel::f32 gx = r.x + 24.0f; gx < r.Right(); gx += 24.0f) {
        canvas.DrawLine({gx, r.y}, {gx, r.Bottom()}, gridCol, 1.0f);
    }
    for (keel::f32 gy = r.y + 24.0f; gy < r.Bottom(); gy += 24.0f) {
        canvas.DrawLine({r.x, gy}, {r.Right(), gy}, gridCol, 1.0f);
    }

    // Vivid theme-harmonized geometric orbs & ribbons behind the floating cards
    auto drawOrb = [&](keel::f32 cx, keel::f32 cy, keel::f32 rad, guikit::Color col) {
        canvas.DrawRoundRect(
            guikit::Rect(cx - rad, cy - rad, rad * 2.0f, rad * 2.0f),
            rad,
            col,
            {},
            0.0f
        );
    };
    drawOrb(r.x + r.w * 0.14f, r.y + r.h * 0.28f, 64.0f, cAccent.WithAlphaF(isDark ? 0.85f : 0.72f));
    drawOrb(r.x + r.w * 0.42f, r.y + r.h * 0.78f, 56.0f, cInfo.WithAlphaF(isDark ? 0.80f : 0.68f));
    drawOrb(r.x + r.w * 0.64f, r.y + r.h * 0.24f, 60.0f, cDanger.WithAlphaF(isDark ? 0.78f : 0.65f));
    drawOrb(r.x + r.w * 0.88f, r.y + r.h * 0.68f, 68.0f, cSuccess.WithAlphaF(isDark ? 0.82f : 0.70f));
    drawOrb(r.x + r.w * 0.28f, r.y + r.h * 0.64f, 34.0f, cWarning.WithAlphaF(isDark ? 0.90f : 0.80f));

    // Crisp high-contrast diagonal stripes for testing optical edge dispersion
    canvas.DrawLine(
        {r.x + r.w * 0.06f, r.Bottom() - 10.0f},
        {r.x + r.w * 0.94f, r.y + 10.0f},
        cWarning.WithAlphaF(isDark ? 0.55f : 0.50f),
        6.0f
    );
    canvas.DrawLine(
        {r.x + r.w * 0.10f, r.y + 16.0f},
        {r.x + r.w * 0.90f, r.Bottom() - 16.0f},
        cInfo.WithAlphaF(isDark ? 0.45f : 0.40f),
        4.0f
    );
}

void RenderSurfacesTab(guikit::Context& ui, const guikit::UiTheme& built, ThemeStudioState& st) {
    // 1. SurfaceRole & Elevation Spectrum
    if (auto roleRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        auto roleCard = [&](keel::StringRef title, keel::StringRef meta, guikit::SurfaceRole role) {
            if (auto c = guikit::Card(ui, {.gap = 4.0f, .padding = guikit::Padding::All(12.0f), .surface = role})) {
                guikit::Heading(ui, {.label = title, .level = guikit::HeadingLevel::H6});
                guikit::Text(ui, meta, {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Muted});
            }
        };
        roleCard("Flat",   "Standard card & panel",       guikit::SurfaceRole::Flat);
        roleCard("Sunken", "Recessed well & list tray",   guikit::SurfaceRole::Sunken);
        roleCard("Raised", "Elevated card & dialog",      guikit::SurfaceRole::Raised);
        roleCard("Popup",  "Menu, popover & toast layer", guikit::SurfaceRole::Popup);
    }

    // 1b. Live Backdrop & Glass Refraction Stage
    if (auto stageCard = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
        if (auto hdr = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Align::Center})) {
            guikit::Heading(ui, {.label = "Backdrop & Glass Refraction", .level = guikit::HeadingLevel::H6});
            guikit::Spacer(ui);
            const bool gpuBlur = (ui.GetCanvas() != nullptr && ui.GetCanvas()->SupportsBackdropBlur());
            guikit::Tag(ui, gpuBlur ? "GPU Backdrop Blur" : "Software Fallback", {
                .variant = gpuBlur ? guikit::UiVariant::Success : guikit::UiVariant::Info,
                .size = guikit::UiSize::Xs
            });
        }
        if (auto stage = guikit::Box(ui, {
            .width = guikit::SizeMode::Stretch,
            .padding = guikit::Padding::All(16.0f),
            .border = {.width = 1.0f, .radius = built.GetRadius(guikit::UiSize::Md), .color = guikit::StateColor{built.palette.border}},
            .clipChildren = true
        })) {
            if (guikit::Node* stageNode = ui.GetCurrentContainer()) {
                stageNode->renderFn = &RenderGlassStageBackdrop;
            }
            if (auto overlayRow = guikit::Row(ui, {.gap = 14.0f, .equalWidths = true})) {
                if (auto c1 = guikit::Card(ui, {
                    .gap = 8.0f,
                    .padding = guikit::Padding::All(12.0f),
                    .surface = guikit::SurfaceRole::Raised
                })) {
                    guikit::Heading(ui, {.label = "Raised Surface", .level = guikit::HeadingLevel::H6});
                    guikit::Text(ui, "Switch Chrome to Glass to inspect live refraction, Kawase blur & rim lighting.", {
                        .role = guikit::TextRole::BodySmall,
                        .variant = guikit::UiVariant::Secondary
                    });
                    guikit::Slider(ui, st.sampleSlider, 0.0f, 100.0f, {.showValue = true});
                    if (auto btns = guikit::Row(ui, {.gap = 8.0f})) {
                        guikit::Button(ui, {.text = "Glass Action", .size = guikit::UiSize::Sm});
                        guikit::Button(ui, {.text = "Primary", .size = guikit::UiSize::Sm, .variant = guikit::UiVariant::Primary});
                    }
                }
                if (auto c2 = guikit::Card(ui, {
                    .gap = 8.0f,
                    .padding = guikit::Padding::All(12.0f),
                    .surface = guikit::SurfaceRole::Popup
                })) {
                    guikit::Heading(ui, {.label = "Popup / Overlay", .level = guikit::HeadingLevel::H6});
                    guikit::Text(ui, "Higher frost & tint density keeps overlay copy sharp over vibrant imagery.", {
                        .role = guikit::TextRole::BodySmall,
                        .variant = guikit::UiVariant::Secondary
                    });
                    if (auto tags = guikit::Wrap(ui, {.gap = 6.0f, .runGap = 6.0f})) {
                        guikit::Tag(ui, "Refraction", {.variant = guikit::UiVariant::Primary, .size = guikit::UiSize::Xs});
                        guikit::Tag(ui, "Dual-Kawase", {.variant = guikit::UiVariant::Info, .size = guikit::UiSize::Xs});
                        guikit::Tag(ui, "WCAG Ink", {.variant = guikit::UiVariant::Success, .size = guikit::UiSize::Xs});
                    }
                    guikit::Button(ui, {.text = "Confirm", .icon = guikit::Icon::Check(), .size = guikit::UiSize::Sm, .variant = guikit::UiVariant::Success});
                }
            }
        }
    }

    // 2. Accordion & Live Shell Metrics
    if (auto midRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        if (auto accCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Accordion", .level = guikit::HeadingLevel::H6});
            if (auto a1 = guikit::Accordion(ui, {.label = "Shell Framing Tokens", .defaultOpen = true, .size = guikit::UiSize::Sm})) {
                guikit::Text(ui, ui.Format(
                    "Style: {} · Padding: {:.0f}dp · Gap: {:.0f}dp · Radius: {:.0f}dp · Border: {:.1f}dp",
                    built.shellStyle == guikit::ShellStyle::Floating ? "Floating" : "Docked",
                    built.shell.windowPadding.left,
                    built.shell.panelGap,
                    built.shell.panelRadius,
                    built.shell.panelBorderWidth
                ), {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Secondary});
            }
            if (auto a2 = guikit::Accordion(ui, {.label = "Elevation Shadows", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
                const auto md = built.GetShadow(guikit::Elevation::Md);
                const auto lg = built.GetShadow(guikit::Elevation::Lg);
                guikit::Text(ui, ui.Format(
                    "Enabled: {} · Md blur {:.1f}dp · Lg blur {:.1f}dp",
                    built.shadowsEnabled ? "Yes" : "No",
                    md.blur,
                    lg.blur
                ), {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Secondary});
            }
        }

        // 3. 2D Scrollbar Preview
        if (auto sbCard = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Scrollbar", .level = guikit::HeadingLevel::H6});
            if (auto sbBox = guikit::ScrollArea(ui, {
                .orientation = guikit::Orientation::Both,
                .scrollbar = guikit::VisibilityPolicy::Always,
                .gap = 4.0f,
                .padding = guikit::Padding::All(8.0f),
                .width = guikit::SizeMode::Stretch,
                .height = 120.0f,
                .background = guikit::StateColor{built.palette.inputBackground},
                .border = {.width = 1.0f, .color = guikit::StateColor{built.palette.border}}
            })) {
                for (keel::i32 row = 1; row <= 12; ++row) {
                    guikit::Text(ui, ui.Format(
                        "Row {:02d}  —  Column A · Column B · Column C · Column D · Column E · Column F",
                        row
                    ), {.role = guikit::TextRole::CodeSmall});
                }
            }
        }
    }

    // 4. Per-Surface Gradient Overrides (StateColor::LinearGradient / RadialGradient)
    if (auto gradCard = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Per-Surface & Toolbar Gradients", .level = guikit::HeadingLevel::H6});
        const auto& pal = built.palette;
        const guikit::Color cA = pal.accent;
        const guikit::Color cB = built.GetSemanticAccent(guikit::UiVariant::Info);
        const guikit::Color cC = built.GetSemanticAccent(guikit::UiVariant::Success);
        const bool isDark = (built.mode == guikit::ThemeMode::Dark);
        if (auto tb = guikit::Toolbar(ui, {
            .style = guikit::ToolbarStyle::Floating,
            .buttonStyle = guikit::ButtonStyle::Ghost,
            .background = guikit::StateColor::LinearGradient(
                guikit::Color::Lerp(pal.surfaceElevated, cA, isDark ? 0.28f : 0.16f),
                guikit::Color::Lerp(pal.surfaceElevated, cB, isDark ? 0.24f : 0.14f),
                90.0f
            ),
            .border = {
                .width = 1.0f,
                .color = guikit::StateColor::LinearGradient(cA.WithAlphaF(0.65f), cB.WithAlphaF(0.45f), 90.0f)
            },
            .size = guikit::UiSize::Sm
        })) {
            tb.ToolButton({.text = "Build", .icon = guikit::Icon::Flash()});
            tb.ToolButton({.text = "Deploy", .icon = guikit::Icon::Check(), .style = guikit::ButtonStyle::Solid, .background = guikit::StateColor::Primary().AsGradient(135.0f)});
            tb.Spacer();
            tb.ToolButton({.text = "Settings", .icon = guikit::Icon::Settings()});
        }
        if (auto gRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
            if (auto c1 = guikit::Card(ui, {
                .gap = 4.0f,
                .padding = guikit::Padding::All(12.0f),
                .background = guikit::StateColor::LinearGradient(
                    guikit::Color::Lerp(pal.surface, cA, isDark ? 0.22f : 0.12f),
                    guikit::Color::Lerp(pal.surface, cC, isDark ? 0.18f : 0.10f),
                    135.0f
                ),
                .border = {
                    .width = 1.0f,
                    .color = guikit::StateColor::LinearGradient(cA.WithAlphaF(0.70f), cC.WithAlphaF(0.40f), 135.0f)
                }
            })) {
                guikit::Heading(ui, {.label = "Linear Surface + Border", .level = guikit::HeadingLevel::H6});
                guikit::Text(ui, "StateColor::LinearGradient on Card background & border.", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary});
            }
            if (auto c2 = guikit::Card(ui, {
                .gap = 4.0f,
                .padding = guikit::Padding::All(12.0f),
                .background = guikit::StateColor::RadialGradient(
                    guikit::Color::Lerp(pal.surfaceElevated, cB, isDark ? 0.26f : 0.14f),
                    pal.surface
                ),
                .border = {.width = 1.0f, .color = guikit::StateColor{pal.border}}
            })) {
                guikit::Heading(ui, {.label = "Radial Spotlight Card", .level = guikit::HeadingLevel::H6});
                guikit::Text(ui, "StateColor::RadialGradient adapts to any ChromeStyle (including Glass).", {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary});
            }
        }
    }
}

void RenderActionsTab(guikit::Context& ui, ThemeStudioState& st) {
    // 1. Buttons: Variants, Styles, Disabled
    if (auto card = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Buttons", .level = guikit::HeadingLevel::H5});

        guikit::Text(ui, "VARIANTS", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        if (auto w = guikit::Wrap(ui, {.gap = 8.0f, .runGap = 8.0f})) {
            guikit::Button(ui, {.text = "Default"});
            guikit::Button(ui, {.text = "Secondary", .variant = guikit::UiVariant::Secondary});
            guikit::Button(ui, {.text = "Primary",   .variant = guikit::UiVariant::Primary});
            guikit::Button(ui, {.text = "Success",   .variant = guikit::UiVariant::Success});
            guikit::Button(ui, {.text = "Warning",   .variant = guikit::UiVariant::Warning});
            guikit::Button(ui, {.text = "Danger",    .variant = guikit::UiVariant::Danger});
            guikit::Button(ui, {.text = "Info",      .variant = guikit::UiVariant::Info});
        }

        guikit::Text(ui, "STYLES & ICONS", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        if (auto w = guikit::Wrap(ui, {.gap = 8.0f, .runGap = 8.0f})) {
            guikit::Button(ui, {.text = "Solid",   .style = guikit::ButtonStyle::Solid,   .variant = guikit::UiVariant::Primary});
            guikit::Button(ui, {.text = "Soft",    .style = guikit::ButtonStyle::Soft,    .variant = guikit::UiVariant::Primary});
            guikit::Button(ui, {.text = "Outline", .style = guikit::ButtonStyle::Outline, .variant = guikit::UiVariant::Primary});
            guikit::Button(ui, {.text = "Ghost",   .style = guikit::ButtonStyle::Ghost});
            guikit::Button(ui, {.text = "Save",    .icon = guikit::Icon::Save(), .variant = guikit::UiVariant::Success});
            guikit::Button(ui, {.icon = guikit::Icon::Settings(), .square = true, .tooltip = "Icon button"});
        }

        guikit::Text(ui, "PER-WIDGET GRADIENTS", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        if (auto w = guikit::Wrap(ui, {.gap = 8.0f, .runGap = 8.0f})) {
            guikit::Button(ui, {
                .text = "Pulse Brand",
                .icon = guikit::Icon::Flash(),
                .background = guikit::StateColor::LinearGradient3(
                    guikit::Color::Hex(0xE11D74FF),
                    guikit::Color::Hex(0xF59E0BFF),
                    guikit::Color::Hex(0xB8F200FF),
                    135.0f
                )
            });
            guikit::Button(ui, {.text = "Auto Primary", .icon = guikit::Icon::Flash(), .background = guikit::StateColor::Primary().AsGradient(135.0f)});
            guikit::Button(ui, {.text = "Auto Success", .icon = guikit::Icon::Check(), .background = guikit::StateColor::Success().AsGradient(120.0f)});
            guikit::Button(ui, {
                .text = "Indigo -> Cyan",
                .background = guikit::StateColor::LinearGradient(guikit::Color::Hex(0x6366F1FF), guikit::Color::Hex(0x06B6D4FF), 135.0f)
            });
            guikit::Button(ui, {
                .text = "Sunset 3-Stop",
                .background = guikit::StateColor::LinearGradient3(
                    guikit::Color::Hex(0xF97316FF),
                    guikit::Color::Hex(0xEC4899FF),
                    guikit::Color::Hex(0x8B5CF6FF),
                    135.0f
                )
            });
            guikit::Button(ui, {
                .text = "Radial Glow",
                .background = guikit::StateColor::RadialGradient(guikit::Color::Hex(0x38BDF8FF), guikit::Color::Hex(0x1D4ED8FF))
            });
        }

        guikit::Text(ui, "DISABLED", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        if (auto w = guikit::Wrap(ui, {.gap = 8.0f, .runGap = 8.0f})) {
            guikit::Button(ui, {.text = "Default", .disabled = true});
            guikit::Button(ui, {.text = "Primary", .disabled = true, .variant = guikit::UiVariant::Primary});
            guikit::Button(ui, {.text = "Success", .disabled = true, .variant = guikit::UiVariant::Success});
            guikit::Button(ui, {.text = "Danger",  .disabled = true, .variant = guikit::UiVariant::Danger});
        }
    }

    // 2. MenuButton & SplitButton
    if (auto card = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Dropdown Triggers", .level = guikit::HeadingLevel::H6});
        static constexpr keel::StaticArray<guikit::ContextMenuItem, 3> kDropItems = {{
            {.label = "Export PNG", .icon = guikit::Icon::Save()},
            {.label = "Export SVG", .icon = guikit::Icon::Copy()},
            {.label = "Copy JSON",  .icon = guikit::Icon::Edit()}
        }};
        if (auto w = guikit::Wrap(ui, {.gap = 10.0f, .runGap = 8.0f})) {
            guikit::MenuButton(ui, kDropItems, {.text = "MenuButton", .icon = guikit::Icon::Settings()});
            guikit::MenuButton(ui, kDropItems, {.text = "Primary Menu", .variant = guikit::UiVariant::Primary});
            guikit::SplitButton(ui, kDropItems, {.text = "Build & Run", .icon = guikit::Icon::Play(), .variant = guikit::UiVariant::Primary});
            guikit::SplitButton(ui, kDropItems, {.text = "Secondary Split", .variant = guikit::UiVariant::Secondary});
        }
    }

    // 3. Navigation: Segmented, TabBar, Breadcrumb, Pagination
    if (auto card = guikit::Card(ui, {.gap = 12.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Navigation", .level = guikit::HeadingLevel::H6});

        static constexpr keel::StaticArray<keel::StringRef, 3> kSegItems = {"Day", "Week", "Month"};
        if (auto el = guikit::Element(ui, {.label = "Segmented"})) {
            if (auto r = guikit::Row(ui, {.gap = 12.0f})) {
                guikit::Segmented(ui, st.sampleSegmentedIdx, kSegItems);
                guikit::Segmented(ui, st.sampleSegmentedIdx, kSegItems, {.disabled = true});
            }
        }

        static constexpr keel::StaticArray<guikit::TabItem, 3> kSubTabs = {{
            {.label = "General"},
            {.label = "Appearance"},
            {.label = "Shortcuts"}
        }};
        if (auto el = guikit::Element(ui, {.label = "TabBar"})) {
            guikit::TabBar(ui, st.sampleSubTabIdx, kSubTabs);
        }

        static constexpr keel::StaticArray<keel::StringRef, 4> kCrumbs = {"Workspace", "GuiKit", "Style", "Theme"};
        if (auto el = guikit::Element(ui, {.label = "Breadcrumb"})) {
            guikit::Breadcrumb(ui, st.sampleBreadcrumbIdx, kCrumbs);
        }

        if (auto el = guikit::Element(ui, {.label = "Pagination"})) {
            guikit::Pagination(ui, st.samplePage, keel::usize{12u}, {.showPageInfo = true});
        }
    }
}

void RenderFieldsTab(guikit::Context& ui, ThemeStudioState& st) {
    // 1. InputField Styles & Validation Status
    if (auto card = guikit::Card(ui, {.gap = 12.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Input Fields & Validation", .level = guikit::HeadingLevel::H5});
        if (auto cols = guikit::Row(ui, {.gap = 14.0f, .equalWidths = true})) {
            if (auto left = guikit::Column(ui, {.gap = 10.0f})) {
                if (auto el = guikit::Element(ui, {.label = "Default (Clearable)"})) {
                    guikit::InputField(ui, st.sampleInput, {.prefix = guikit::Icon::Search(), .clearable = true});
                }
                if (auto el = guikit::Element(ui, {.label = "Filled Style"})) {
                    guikit::InputField(ui, st.sampleFilledInput, {.style = guikit::FieldStyle::Filled});
                }
                if (auto el = guikit::Element(ui, {.label = "Disabled", .disabled = true})) {
                    guikit::InputField(ui, st.disabledInput, {.disabled = true});
                }
            }
            if (auto right = guikit::Column(ui, {.gap = 10.0f})) {
                if (auto el = guikit::Element(ui, {.label = "Success Status"})) {
                    guikit::InputField(ui, st.sampleValidInput, {.status = guikit::UiVariant::Success});
                }
                if (auto el = guikit::Element(ui, {.label = "Warning Status"})) {
                    guikit::InputField(ui, st.sampleWarnInput, {.status = guikit::UiVariant::Warning});
                }
                if (auto el = guikit::Element(ui, {.label = "Danger Status"})) {
                    guikit::InputField(ui, st.sampleErrorInput, {.status = guikit::UiVariant::Danger});
                }
            }
        }
    }

    // 2. Selectors & Rich Pickers
    if (auto card = guikit::Card(ui, {.gap = 12.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Selectors & Pickers", .level = guikit::HeadingLevel::H5});
        static constexpr keel::StaticArray<keel::StringRef, 3> kSelectItems = {
            "Low Latency", "Balanced", "High Precision"
        };
        static constexpr keel::StaticArray<keel::StringRef, 4> kMultiItems = {
            "OpenGL", "Vulkan", "WebGL2", "Software"
        };

        if (auto cols = guikit::Row(ui, {.gap = 14.0f, .equalWidths = true})) {
            if (auto left = guikit::Column(ui, {.gap = 10.0f})) {
                if (auto el = guikit::Element(ui, {.label = "Select"})) {
                    guikit::Select(ui, st.sampleSelectIdx, kSelectItems);
                }
                if (auto el = guikit::Element(ui, {.label = "MultiSelect"})) {
                    guikit::MultiSelect(ui, st.sampleMultiSelect, kMultiItems);
                }
                if (auto el = guikit::Element(ui, {.label = "TagInput"})) {
                    guikit::TagInput(ui, st.sampleTags);
                }
            }
            if (auto right = guikit::Column(ui, {.gap = 10.0f})) {
                if (auto el = guikit::Element(ui, {.label = "SpinBox (Inline / Split)"})) {
                    if (auto r = guikit::Row(ui, {.gap = 8.0f, .equalWidths = true})) {
                        guikit::SpinBox(ui, st.sampleSpin, {.suffix = "dp", .min = 0.0, .max = 100.0, .step = 0.5});
                        guikit::SpinBox(ui, st.sampleSpinSplit, {.min = 0.0, .max = 64.0, .step = 1.0, .stepStyle = guikit::SpinBoxStepStyle::Split});
                    }
                }
                if (auto el = guikit::Element(ui, {.label = "DatePicker"})) {
                    guikit::DatePicker(ui, st.sampleDate, {.mode = guikit::DatePickerMode::DateTime});
                }
                if (auto el = guikit::Element(ui, {.label = "ColorPicker"})) {
                    guikit::ColorPicker(ui, st.sampleColor, {.showHex = true, .showAlpha = false, .showSwatches = true});
                }
            }
        }
    }
}

void RenderControlsTab(guikit::Context& ui, ThemeStudioState& st) {
    // 1. Boolean & Choice Controls
    if (auto choiceRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        if (auto c = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Checkbox", .level = guikit::HeadingLevel::H6});
            if (auto el = guikit::Element(ui, {.label = "Checked", .layout = guikit::ElementLayout::Trailing})) {
                guikit::Checkbox(ui, st.sampleCheckbox);
            }
            if (auto el = guikit::Element(ui, {.label = "Unchecked", .layout = guikit::ElementLayout::Trailing})) {
                guikit::Checkbox(ui, st.sampleCheckboxOff);
            }
            if (auto el = guikit::Element(ui, {.label = "Disabled", .layout = guikit::ElementLayout::Trailing, .disabled = true})) {
                bool dis = true;
                guikit::Checkbox(ui, dis, {.disabled = true});
            }
        }

        if (auto c = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Radio", .level = guikit::HeadingLevel::H6});
            if (auto el = guikit::Element(ui, {.label = "Option A", .layout = guikit::ElementLayout::Trailing})) {
                guikit::Radio(ui, st.sampleRadio, 0);
            }
            if (auto el = guikit::Element(ui, {.label = "Option B", .layout = guikit::ElementLayout::Trailing})) {
                guikit::Radio(ui, st.sampleRadio, 1);
            }
            if (auto el = guikit::Element(ui, {.label = "Disabled", .layout = guikit::ElementLayout::Trailing, .disabled = true})) {
                guikit::Radio(ui, st.sampleRadio, 2, {.disabled = true});
            }
        }

        if (auto c = guikit::Card(ui, {.gap = 8.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Switch", .level = guikit::HeadingLevel::H6});
            guikit::Switch(ui, st.sampleSwitch, {.textRight = "Enabled"});
            guikit::Switch(ui, st.sampleSwitchOff, {.textRight = "Off"});
            bool disSw = true;
            guikit::Switch(ui, disSw, {.textRight = "Disabled", .disabled = true});
        }
    }

    // 2. Sliders & Knobs
    if (auto rangeRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        if (auto c = guikit::Card(ui, {.gap = 12.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Sliders", .level = guikit::HeadingLevel::H6});
            if (auto el = guikit::Element(ui, {.label = "Slider"})) {
                guikit::Slider(ui, st.sampleSlider, 0.0f, 100.0f, {.valueFormat = "%.0f %%"});
            }
            static constexpr keel::StaticArray<keel::StringRef, 4> kStepLabels = {"Low", "Med", "High", "Ultra"};
            if (auto el = guikit::Element(ui, {.label = "StepSlider"})) {
                guikit::StepSlider(ui, st.sampleStepIdx, kStepLabels, {.showTicks = true, .showLabels = true});
            }
            if (auto el = guikit::Element(ui, {.label = "RangeSlider"})) {
                guikit::RangeSlider(ui, st.sampleRangeLow, st.sampleRangeHigh, 0.0f, 100.0f, {.valueFormat = "%.0f"});
            }
            static constexpr keel::StaticArray<keel::StringRef, 2> kHybridStops = {"Off", "Auto"};
            if (auto el = guikit::Element(ui, {.label = "HybridSlider"})) {
                guikit::HybridSlider(ui, st.sampleHybridMode, st.sampleHybridVal, 1.0f, 100.0f, kHybridStops, {.valueFormat = "%.0f %%"});
            }
        }

        if (auto c = guikit::Card(ui, {.gap = 12.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "Knobs", .level = guikit::HeadingLevel::H6});
            if (auto kRow = guikit::Row(ui, {.gap = 18.0f, .align = guikit::Align::Center, .justify = guikit::Justify::Center})) {
                if (auto k1 = guikit::Column(ui, {.gap = 4.0f, .align = guikit::Align::Center})) {
                    guikit::Knob(ui, st.sampleKnob, 0.0f, 100.0f, {.valueFormat = "%.0f%%", .color = guikit::StateColor::Primary()});
                    guikit::Text(ui, "Primary", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
                }
                if (auto k2 = guikit::Column(ui, {.gap = 4.0f, .align = guikit::Align::Center})) {
                    guikit::Knob(ui, st.sampleKnobAux, 0.0f, 100.0f, {.valueFormat = "%.0f%%", .color = guikit::StateColor::Info()});
                    guikit::Text(ui, "Info", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
                }
                if (auto k3 = guikit::Column(ui, {.gap = 4.0f, .align = guikit::Align::Center})) {
                    keel::f32 disK = 50.0f;
                    guikit::Knob(ui, disK, 0.0f, 100.0f, {.valueFormat = "%.0f%%", .disabled = true});
                    guikit::Text(ui, "Disabled", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
                }
            }
        }
    }
}

void RenderDisplayTab(guikit::Context& ui, ThemeStudioState& st) {
    // 1. Tags Matrix
    if (auto card = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
        guikit::Heading(ui, {.label = "Tags", .level = guikit::HeadingLevel::H5});
        auto tagRow = [&](keel::StringRef styleLabel, guikit::TagStyle ts) {
            if (auto r = guikit::Row(ui, {.gap = 8.0f, .align = guikit::Align::Center})) {
                if (auto lblBox = guikit::Box(ui, {.width = 58.0f})) {
                    guikit::Text(ui, styleLabel, {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Muted});
                }
                if (auto w = guikit::Wrap(ui, {.gap = 6.0f, .runGap = 6.0f})) {
                    guikit::Tag(ui, "Default", {.variant = guikit::UiVariant::Default, .style = ts, .size = guikit::UiSize::Sm});
                    guikit::Tag(ui, "Primary", {.variant = guikit::UiVariant::Primary, .style = ts, .size = guikit::UiSize::Sm});
                    guikit::Tag(ui, "Success", {.variant = guikit::UiVariant::Success, .style = ts, .size = guikit::UiSize::Sm});
                    guikit::Tag(ui, "Warning", {.variant = guikit::UiVariant::Warning, .style = ts, .size = guikit::UiSize::Sm});
                    guikit::Tag(ui, "Danger",  {.variant = guikit::UiVariant::Danger,  .style = ts, .size = guikit::UiSize::Sm});
                    guikit::Tag(ui, "Info",    {.variant = guikit::UiVariant::Info,    .style = ts, .size = guikit::UiSize::Sm});
                }
            }
        };
        tagRow("Soft",    guikit::TagStyle::Soft);
        tagRow("Outline", guikit::TagStyle::Outline);
        tagRow("Solid",   guikit::TagStyle::Solid);
    }

    // 2. ProgressBar & LevelMeter
    if (auto midRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        if (auto card = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
            if (auto hdr = guikit::Row(ui, {.align = guikit::Align::Center})) {
                guikit::Heading(ui, {.label = "ProgressBar", .level = guikit::HeadingLevel::H6});
                guikit::Spacer(ui);
                if (auto el = guikit::Element(ui, {.label = "Indeterminate", .layout = guikit::ElementLayout::Trailing})) {
                    guikit::Checkbox(ui, st.indeterminateBar, {.size = guikit::UiSize::Sm});
                }
            }
            guikit::ProgressBar(ui, st.sampleSlider / 100.0f, {
                .indeterminate = st.indeterminateBar,
                .color = guikit::StateColor::Primary()
            });
            guikit::ProgressBar(ui, 0.82f, {.color = guikit::StateColor::Success()});
            guikit::ProgressBar(ui, 0.36f, {.color = guikit::StateColor::Warning()});
        }

        if (auto card = guikit::Card(ui, {.gap = 10.0f, .padding = guikit::Padding::All(14.0f)})) {
            guikit::Heading(ui, {.label = "LevelMeter", .level = guikit::HeadingLevel::H6});
            const keel::f64 dbL = -60.0 + keel::as<keel::f64>(st.sampleSlider) * 0.58;
            const keel::f64 dbR = -60.0 + keel::as<keel::f64>(st.sampleKnobAux) * 0.56;
            if (auto el = guikit::Element(ui, {.label = "Left Channel"})) {
                guikit::LevelMeter(ui, dbL, -60.0, 0.0, {.segments = 24u, .multiZoneColor = true, .size = guikit::UiSize::Sm});
            }
            if (auto el = guikit::Element(ui, {.label = "Right Channel"})) {
                guikit::LevelMeter(ui, dbR, -60.0, 0.0, {.segments = 24u, .multiZoneColor = true, .size = guikit::UiSize::Sm});
            }
        }
    }

    // 3. Engineering Readouts
    static constexpr keel::StaticArray<guikit::ReadoutUnit, 2> kVoltUnits = {{
        {.label = "V",  .factor = 1.0},
        {.label = "mV", .factor = 1000.0}
    }};
    static constexpr keel::StaticArray<guikit::ReadoutUnit, 2> kFreqUnits = {{
        {.label = "kHz", .factor = 1.0},
        {.label = "Hz",  .factor = 1000.0}
    }};
    static constexpr keel::StaticArray<guikit::ReadoutUnit, 2> kTempUnits = {{
        {.label = "C", .factor = 1.0, .offset = 0.0},
        {.label = "F", .factor = 1.8, .offset = 32.0}
    }};

    const keel::StaticArray<guikit::ReadoutChannel, 1> chVolt = {{
        {.label = "Supply Voltage", .headerLabel = "BUS VOLTAGE", .value = 24.08, .units = kVoltUnits, .variant = guikit::UiVariant::Primary, .showAction = false}
    }};
    const keel::StaticArray<guikit::ReadoutChannel, 1> chFreq = {{
        {.label = "Clock Rate", .headerLabel = "SAMPLE CLOCK", .value = 48.00, .units = kFreqUnits, .variant = guikit::UiVariant::Info, .showAction = false}
    }};
    const keel::StaticArray<guikit::ReadoutChannel, 1> chTemp = {{
        {.label = "Die Temp", .headerLabel = "THERMAL", .value = 41.6, .units = kTempUnits, .defaultPrecision = 1, .variant = guikit::UiVariant::Success, .showAction = false}
    }};

    if (auto roRow = guikit::Row(ui, {.gap = 12.0f, .equalWidths = true})) {
        guikit::Readout(ui, st.sampleReadoutA, chVolt, {.width = guikit::SizeMode::Stretch, .minWidth = 140.0f});
        guikit::Readout(ui, st.sampleReadoutB, chFreq, {.width = guikit::SizeMode::Stretch, .minWidth = 140.0f});
        guikit::Readout(ui, st.sampleReadoutC, chTemp, {.width = guikit::SizeMode::Stretch, .minWidth = 140.0f});
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
        guikit::ThemePresetName(guikit::ThemePreset::Windows98),
        guikit::ThemePresetName(guikit::ThemePreset::Pulse),
        guikit::ThemePresetName(guikit::ThemePreset::PulseLight)
    }};

    static constexpr keel::StaticArray<keel::StringRef, guikit::kChromeStyleCount> kChromeNames = {{
        guikit::ChromeStyleName(guikit::ChromeStyle::Modern),
        guikit::ChromeStyleName(guikit::ChromeStyle::Fluent),
        guikit::ChromeStyleName(guikit::ChromeStyle::Classic3D),
        guikit::ChromeStyleName(guikit::ChromeStyle::Brutalist),
        guikit::ChromeStyleName(guikit::ChromeStyle::Glass),
        guikit::ChromeStyleName(guikit::ChromeStyle::Luminous)
    }};

    static constexpr keel::StaticArray<keel::StringRef, guikit::kFillStyleCount> kFillNames = {{
        guikit::FillStyleName(guikit::FillStyle::Solid),
        guikit::FillStyleName(guikit::FillStyle::Soft),
        guikit::FillStyleName(guikit::FillStyle::Outline),
        guikit::FillStyleName(guikit::FillStyle::Gradient)
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
    if (auto acc = guikit::Accordion(ui, {.label = "THEME PRESET", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
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
                st.sliderThumb = keel::kNullOpt;
                st.sliderTrackStyle = keel::kNullOpt;
                st.sliderTickStyle = keel::kNullOpt;
                st.sliderTrackCaps = keel::kNullOpt;
                st.overrideSliderTrackHeight = false;
                st.overrideSliderThumbSize = false;
                const auto tok = guikit::ChromeTokens::FromStyle(c);
                st.customScrollbarTokens = guikit::ScrollbarStyleTokens::ForChrome(tok.scrollbar);
                st.customizeScrollbarTokens = false;
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
                if (guikit::Button(ui, {.text = "Pulse x Glass", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Primary}).clicked) {
                    applyCombo(guikit::ThemePreset::Pulse, guikit::ChromeStyle::Glass, guikit::FillStyle::Gradient);
                }
                if (guikit::Button(ui, {.text = "Pulse Light", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Success}).clicked) {
                    applyCombo(guikit::ThemePreset::PulseLight, guikit::ChromeStyle::Luminous, guikit::FillStyle::Gradient);
                }
                if (guikit::Button(ui, {.text = "Nord x Glass", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Primary}).clicked) {
                    applyCombo(guikit::ThemePreset::Nord, guikit::ChromeStyle::Glass, guikit::FillStyle::Gradient);
                }
                if (guikit::Button(ui, {.text = "Light x Glass", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Info}).clicked) {
                    applyCombo(guikit::ThemePreset::Light, guikit::ChromeStyle::Glass, guikit::FillStyle::Gradient);
                }
                if (guikit::Button(ui, {.text = "Dark x Luminous", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Success}).clicked) {
                    applyCombo(guikit::ThemePreset::Dark, guikit::ChromeStyle::Luminous, guikit::FillStyle::Gradient);
                }
                if (guikit::Button(ui, {.text = "Dracula x Luminous", .size = guikit::UiSize::Xs, .style = guikit::ButtonStyle::Soft, .variant = guikit::UiVariant::Danger}).clicked) {
                    applyCombo(guikit::ThemePreset::Dracula, guikit::ChromeStyle::Luminous, guikit::FillStyle::Gradient);
                }
            }
        }
    }

    // 2. APPEARANCE & PALETTE
    const guikit::UiTheme baseRef = guikit::UiTheme::FromPreset(st.preset);
    if (auto acc = guikit::Accordion(ui, {.label = "APPEARANCE & PALETTE", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
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
    static constexpr keel::StaticArray<keel::StringRef, 3> kSbArrowNames = {
        "None", "Triangle", "Chevron"
    };
    if (auto acc = guikit::Accordion(ui, {.label = "SCROLLBAR", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
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
        if (auto el = guikit::Element(ui, {.label = "Custom Width"})) {
            static constexpr keel::StaticArray<keel::StringRef, 1> kAutoStop = {"Auto"};
            const bool wasOverridden = st.overrideScrollbarWidth;
            if (guikit::HybridSlider(ui, st.overrideScrollbarWidth, st.scrollbarWidth, 4.0f, 24.0f, {
                .stops = kAutoStop,
                .valueFormat = "%.0f dp",
                .step = 1.0f,
                .size = guikit::UiSize::Sm
            }).changed) {
                if (!wasOverridden && st.overrideScrollbarWidth && st.scrollbarWidth < 4.0f) {
                    st.scrollbarWidth = ctx.previewTheme.GetScrollbar(guikit::UiSize::Md).trackWidth;
                }
                dirty = true;
            }
        }

        guikit::ScrollbarStyleTokens sbTok = st.customizeScrollbarTokens
            ? st.customScrollbarTokens
            : ctx.previewTheme.scrollbarStyle;
        bool tokEdited = false;
        if (auto el = guikit::Element(ui, {.label = "Arrow Buttons", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, sbTok.showButtons, {.size = guikit::UiSize::Sm}).changed) {
                if (sbTok.showButtons && sbTok.arrowStyle == guikit::ScrollbarArrowStyle::None) {
                    sbTok.arrowStyle = guikit::ScrollbarArrowStyle::Triangle;
                }
                tokEdited = true;
            }
        }
        if (sbTok.showButtons) {
            keel::usize arrowIdx = keel::as<keel::usize>(sbTok.arrowStyle);
            if (auto el = guikit::Element(ui, {.label = "Arrow Glyph"})) {
                if (guikit::Select(ui, arrowIdx, kSbArrowNames, {.size = guikit::UiSize::Sm}).changed) {
                    sbTok.arrowStyle = keel::from_underlying<guikit::ScrollbarArrowStyle>(arrowIdx);
                    tokEdited = true;
                }
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Thumb Inset"})) {
            if (guikit::Slider(ui, sbTok.thumbInset, 0.0f, 4.0f, {.valueFormat = "%.1f dp", .step = 0.5f, .size = guikit::UiSize::Sm}).changed) {
                tokEdited = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Always Show Track", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, sbTok.alwaysShowTrack, {.size = guikit::UiSize::Sm}).changed) {
                tokEdited = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Corner Box", .layout = guikit::ElementLayout::Leading})) {
            if (guikit::Switch(ui, sbTok.showCornerBox, {.size = guikit::UiSize::Sm}).changed) {
                tokEdited = true;
            }
        }
        if (tokEdited) {
            st.customScrollbarTokens = sbTok;
            st.customizeScrollbarTokens = true;
            dirty = true;
        }
    }

    // 2c. SLIDER CUSTOMIZATION
    static constexpr keel::StaticArray<keel::StringRef, 6> kSliderThumbNames = {
        "Auto (Match Chrome)",
        guikit::SliderThumbShapeName(guikit::SliderThumbShape::Circle),
        guikit::SliderThumbShapeName(guikit::SliderThumbShape::Ring),
        guikit::SliderThumbShapeName(guikit::SliderThumbShape::Square),
        guikit::SliderThumbShapeName(guikit::SliderThumbShape::Pointer3D),
        guikit::SliderThumbShapeName(guikit::SliderThumbShape::Bar)
    };
    static constexpr keel::StaticArray<keel::StringRef, 3> kSliderTrackStyleNames = {
        "Auto (Match Chrome)",
        guikit::SliderTrackStyleName(guikit::SliderTrackStyle::Filled),
        guikit::SliderTrackStyleName(guikit::SliderTrackStyle::NeutralGroove)
    };
    static constexpr keel::StaticArray<keel::StringRef, 4> kSliderTickStyleNames = {
        "Auto (Match Chrome)",
        guikit::SliderTickStyleName(guikit::SliderTickStyle::Dot),
        guikit::SliderTickStyleName(guikit::SliderTickStyle::Line),
        guikit::SliderTickStyleName(guikit::SliderTickStyle::Notch)
    };
    static constexpr keel::StaticArray<keel::StringRef, 3> kSliderTrackCapsNames = {
        "Auto (Match Chrome)",
        guikit::SliderTrackCapsName(guikit::SliderTrackCaps::Round),
        guikit::SliderTrackCapsName(guikit::SliderTrackCaps::Square)
    };
    if (auto acc = guikit::Accordion(ui, {.label = "SLIDER", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        auto renderOptSelect = [&]<typename EnumT>(keel::StringRef label, keel::StringRef desc, keel::Optional<EnumT>& optEnum, keel::ArrayRef<keel::StringRef> names) {
            keel::usize idx = optEnum.has_value() ? (keel::as<keel::usize>(*optEnum) + 1u) : 0u;
            if (auto el = guikit::Element(ui, {.label = label, .description = desc})) {
                if (guikit::Select(ui, idx, names, {.size = guikit::UiSize::Sm}).changed) {
                    optEnum = (idx == 0u)
                        ? keel::Optional<EnumT>(keel::kNullOpt)
                        : keel::Optional<EnumT>(keel::from_underlying<EnumT>(idx - 1u));
                    dirty = true;
                }
            }
        };

        renderOptSelect("Thumb Shape", "Circle, Ring (Fluent), Square, Pointer 3D, or Bar", st.sliderThumb, kSliderThumbNames);
        renderOptSelect("Track Style", {}, st.sliderTrackStyle, kSliderTrackStyleNames);
        renderOptSelect("Tick Style", {}, st.sliderTickStyle, kSliderTickStyleNames);
        renderOptSelect("Track Caps", "Pill-rounded or sharp rectangular track ends", st.sliderTrackCaps, kSliderTrackCapsNames);

        static constexpr keel::StaticArray<keel::StringRef, 1> kAutoStop = {"Auto"};
        if (auto el = guikit::Element(ui, {.label = "Track Thickness"})) {
            if (guikit::HybridSlider(ui, st.overrideSliderTrackHeight, st.sliderTrackHeight, 2.0f, 12.0f, {
                .stops = kAutoStop,
                .valueFormat = "%.1f dp",
                .step = 0.5f,
                .size = guikit::UiSize::Sm
            }).changed) {
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {.label = "Thumb Size"})) {
            if (guikit::HybridSlider(ui, st.overrideSliderThumbSize, st.sliderThumbSize, 10.0f, 24.0f, {
                .stops = kAutoStop,
                .valueFormat = "%.0f dp",
                .step = 1.0f,
                .size = guikit::UiSize::Sm
            }).changed) {
                dirty = true;
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
        if (auto el = guikit::Element(ui, {
            .label = "Window Corners",
            .description = "OS window frame rounding (Auto derives from radius)",
            .layout = guikit::ElementLayout::Stacked
        })) {
            if (guikit::Segmented(ui, st.windowCorners, {"Auto", "Square", "Round", "Small"}, {.size = guikit::UiSize::Sm}).changed) {
                dirty = true;
            }
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

    // 6. COLOR MORPHING (UiTheme::LerpColors)
    if (auto acc = guikit::Accordion(ui, {.label = "COLOR MORPHING (LERP)", .defaultOpen = false, .size = guikit::UiSize::Sm})) {
        keel::usize targetIdx = keel::as<keel::usize>(st.lerpTarget);
        if (auto el = guikit::Element(ui, {.label = "Target Palette B"})) {
            if (guikit::Select(ui, targetIdx, kPresetNames, {.size = guikit::UiSize::Sm}).changed) {
                st.lerpTarget = keel::from_underlying<guikit::ThemePreset>(targetIdx);
                dirty = true;
            }
        }
        if (auto el = guikit::Element(ui, {
            .label = "Color Blend",
            .description = "Blends palette colors only (preserves chrome & geometry)"
        })) {
            keel::f32 blendPct = st.lerpT * 100.0f;
            if (guikit::HybridSlider(ui, st.enableLerp, blendPct, 2.0f, 100.0f, {
                .valueFormat = "%.0f%%",
                .step = 2.0f,
                .size = guikit::UiSize::Sm
            }).changed) {
                st.lerpT = st.enableLerp ? (blendPct * 0.01f) : 0.0f;
                dirty = true;
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
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch,
            .background = guikit::StateColor{built.palette.background},
            .shell = true
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
                    if (themeMenu.Checkbox("Color Morph (LerpColors)", st.enableLerp, {.keepOpen = true})) {
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
                    viewMenu.Section("STAGE TAB");
                    keel::i32 tabIdx = keel::as<keel::i32>(st.activeStageTab);
                    if (viewMenu.Radio("Tokens",     tabIdx, 0)) { st.activeStageTab = 0u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Typography", tabIdx, 1)) { st.activeStageTab = 1u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Surfaces",   tabIdx, 2)) { st.activeStageTab = 2u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Actions",    tabIdx, 3)) { st.activeStageTab = 3u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Fields",     tabIdx, 4)) { st.activeStageTab = 4u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Controls",   tabIdx, 5)) { st.activeStageTab = 5u; ui.InvalidateAll(); }
                    if (viewMenu.Radio("Display",    tabIdx, 6)) { st.activeStageTab = 6u; ui.InvalidateAll(); }
                }
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
                    .tooltip = "Toggle UiTheme::LerpColors color morphing",
                    .icon = guikit::Icon::Flash(),
                    .variant = guikit::UiVariant::Warning
                }).changed) {
                    ctx.RebuildPreviewTheme();
                    ui.InvalidateAll();
                }
            }

            if (auto workspace = guikit::Box(ui, {
                .gap = 0.0f,
                .width = guikit::SizeMode::Stretch,
                .height = guikit::SizeMode::Stretch,
                .background = guikit::StateColor{built.palette.background},
                .clipChildren = true
            })) {
                // Pinned taxonomy TabBar at top of workspace
                static constexpr keel::StaticArray<guikit::TabItem, 7> kStageTabs = {{
                    {.label = "Tokens"},
                    {.label = "Typography"},
                    {.label = "Surfaces"},
                    {.label = "Actions"},
                    {.label = "Fields"},
                    {.label = "Controls"},
                    {.label = "Display"}
                }};
                if (st.activeStageTab >= kStageTabs.size()) {
                    st.activeStageTab = 0u;
                }
                if (auto tabStrip = guikit::Row(ui, {
                    .padding = guikit::Padding(14.0f, 8.0f, 14.0f, 0.0f),
                    .align = guikit::Align::Center,
                    .width = guikit::SizeMode::Stretch
                })) {
                    guikit::TabBar(ui, st.activeStageTab, kStageTabs, {.size = guikit::UiSize::Sm});
                }

                if (auto scroll = guikit::ScrollArea(ui, {
                    .width = guikit::SizeMode::Stretch,
                    .height = guikit::SizeMode::Stretch
                })) {
                    if (auto root = guikit::Column(ui, {
                        .gap = 12.0f,
                        .padding = guikit::Padding::All(16.0f),
                        .width = guikit::SizeMode::Stretch
                    })) {
                        switch (st.activeStageTab) {
                            case 0u: RenderTokensTab(ui, built); break;
                            case 1u: RenderTypographyTab(ui, built); break;
                            case 2u: RenderSurfacesTab(ui, built, st); break;
                            case 3u: RenderActionsTab(ui, st); break;
                            case 4u: RenderFieldsTab(ui, st); break;
                            case 5u: RenderControlsTab(ui, st); break;
                            case 6u: RenderDisplayTab(ui, st); break;
                            default: RenderTokensTab(ui, built); break;
                        }
                    }
                }
            }

            if (auto sb = guikit::StatusBar(ui, {.size = guikit::UiSize::Sm})) {
                const keel::StringRef presetLabel = (st.enableLerp && st.lerpT > 0.001f)
                    ? ui.Format("{} <-> {} ({:.0f}%)",
                        guikit::ThemePresetName(st.preset),
                        guikit::ThemePresetName(st.lerpTarget),
                        st.lerpT * 100.0f)
                    : guikit::ThemePresetName(built.preset);
                sb.Item({
                    .text = presetLabel,
                    .dotColor = built.palette.accent,
                    .color = guikit::StateColor::Primary()
                });
                sb.Separator();
                sb.Item({.text = guikit::ChromeStyleName(built.chromeStyle)});
                sb.Separator();
                sb.Item({.text = guikit::FillStyleName(built.fillStyle)});
                sb.Separator();
                sb.Item({.text = (built.shellStyle == guikit::ShellStyle::Floating) ? "Floating Shell" : "Docked Shell"});
                sb.Spacer();
                const keel::f32 txtBgRatio = guikit::Color::ContrastRatio(built.palette.textPrimary, built.palette.background);
                sb.Item({
                    .text = ui.Format("Contrast {:.1f}:1", txtBgRatio),
                    .color = (txtBgRatio >= 7.0f) ? guikit::StateColor::Success() : guikit::StateColor::Warning()
                });
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
    if (spec.sliderThumb.has_value()) {
        keel::format_into(out, ",\n    .sliderThumb = guikit::SliderThumbShape::{}",
            SliderThumbEnumIdentifier(*spec.sliderThumb));
    }
    if (spec.sliderTrackStyle.has_value()) {
        keel::format_into(out, ",\n    .sliderTrackStyle = guikit::SliderTrackStyle::{}",
            (*spec.sliderTrackStyle == guikit::SliderTrackStyle::NeutralGroove) ? "NeutralGroove" : "Filled");
    }
    if (spec.sliderTickStyle.has_value()) {
        const keel::StringRef tickId = (*spec.sliderTickStyle == guikit::SliderTickStyle::Line)
            ? "Line"
            : ((*spec.sliderTickStyle == guikit::SliderTickStyle::Notch) ? "Notch" : "Dot");
        keel::format_into(out, ",\n    .sliderTickStyle = guikit::SliderTickStyle::{}", tickId);
    }
    if (spec.sliderTrackCaps.has_value()) {
        keel::format_into(out, ",\n    .sliderTrackCaps = guikit::SliderTrackCaps::{}",
            (*spec.sliderTrackCaps == guikit::SliderTrackCaps::Square) ? "Square" : "Round");
    }
    if (spec.sliderTrackHeight.has_value()) {
        keel::format_into(out, ",\n    .sliderTrackHeight = {:.1f}f", *spec.sliderTrackHeight);
    }
    if (spec.sliderThumbSize.has_value()) {
        keel::format_into(out, ",\n    .sliderThumbSize = {:.1f}f", *spec.sliderThumbSize);
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
            "const guikit::UiTheme morphed = guikit::UiTheme::LerpColors(\n"
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
