#include "demo_window.hpp"
#include "preview_viewport.hpp"
#if defined(__GLIBC__)
#include <malloc.h>
#endif

namespace guikit::demo {

namespace {

void ReleaseInactiveSceneBuffers(
    NavCategory leavingCat,
    keel::i32 leavingExt,
    const keel::Array<DemoExtensionScreen>& extensions
) noexcept {
    bool releasedHeavyBuffers = false;
    if (leavingExt >= 0 && keel::as<keel::usize>(leavingExt) < extensions.size()) {
        if (extensions[keel::as<keel::usize>(leavingExt)].releaseBuffers != nullptr) {
            extensions[keel::as<keel::usize>(leavingExt)].releaseBuffers();
            releasedHeavyBuffers = true;
        }
    } else if (leavingCat == NavCategory::ViewImageView) {
        ReleaseImageViewDemoBuffers();
        releasedHeavyBuffers = true;
    }
    if (releasedHeavyBuffers) {
#if defined(__GLIBC__)
        ::malloc_trim(0);
#endif
    }
}

} // namespace

DemoWindow::DemoWindow()
    : guikit::MainWindow({
#if defined(GUIKIT_DEMO_USE_NATIVE) && GUIKIT_DEMO_USE_NATIVE
#if defined(__EMSCRIPTEN__)
          .title = "GuiKit Demo (C++20 Immediate Mode + Native WebGL2)",
#elif defined(_WIN32)
          .title = "GuiKit Demo (C++20 Immediate Mode + Native Win32 OpenGL 3.3)",
#else
          .title = "GuiKit Demo (C++20 Immediate Mode + Native OpenGL ES3)",
#endif
#else
          .title = "GuiKit Vector GUI Demo (C++20 Immediate Mode + ThorVG)",
#endif
          .width = 1380,
          .height = 800,
          .minWidth = 320,
          .minHeight = 240,
          .idleHeartbeatMs = 500
      }) {}

void DemoWindow::RegisterExtension(DemoExtensionScreen ext) {
    m_extensions.push_back(ext);
}

void DemoWindow::OnInit() {
    m_demoCtx.displayPxPerMm = GetDisplayPixelsPerMm();
    m_demoCtx.shellTheme = GetShellTheme();
    m_demoCtx.shellLightMode = IsShellLightMode();
    m_demoCtx.shellFloating = IsShellFloating();
    SyncPreviewThemeFromStudio(m_demoCtx);
}

void DemoWindow::OnResize(keel::i32 /*width*/, keel::i32 /*height*/) {
    m_demoCtx.displayPxPerMm = GetDisplayPixelsPerMm();
}

void DemoWindow::OnRender(guikit::Context& ui) {
    m_demoCtx.displayPxPerMm = GetDisplayPixelsPerMm();
    m_demoCtx.shellTheme = GetShellTheme();
    m_demoCtx.shellLightMode = IsShellLightMode();
    m_demoCtx.shellFloating = IsShellFloating();

    RenderTopToolbar(ui);
    RenderWorkspace(ui);
    RenderBottomStatusBar(ui);

    if (m_demoCtx.pendingShellTheme.has_value()) {
        SetShellTheme(*m_demoCtx.pendingShellTheme);
        m_demoCtx.pendingShellTheme = keel::kNullOpt;
    }

    if (m_selectedCategory != m_prevRenderedCategory || m_selectedExtension != m_prevRenderedExtension) {
        ReleaseInactiveSceneBuffers(m_prevRenderedCategory, m_prevRenderedExtension, m_extensions);
        m_prevRenderedCategory = m_selectedCategory;
        m_prevRenderedExtension = m_selectedExtension;
    }
    if (!m_demoCtx.previewShowCode && m_cachedCodeSnippet.capacity() > 0) {
        m_cachedCodeSnippet = {};
    }
}

void DemoWindow::RenderTopToolbar(guikit::Context& ui) {
    if (auto tb = guikit::Toolbar(ui)) {
        m_shellControls.RenderSidebarCycleButton(*this, tb, m_navSidebarMode, "Nav", "Navigation Sidebar", guikit::Icon::Folder());
        m_shellControls.RenderSidebarToggle(*this, tb, m_inspectorSidebarMode, "Inspector", "Inspector Sidebar", guikit::Icon::Settings());

        tb.Separator();

        m_shellControls.RenderThemeControls(*this, tb);
        m_shellControls.RenderDebugControls(*this, tb);

        tb.Separator();

        m_shellControls.RenderEngineControls(*this, tb);

        tb.Spacer();
        if (guikit::Text(ui, "https://github.com/guikit/", {.role = guikit::TextRole::TitleMedium, .variant = guikit::UiVariant::Info})) {
            OpenUrl("https://github.com/guikit/");
        }
    }
}

void DemoWindow::RenderCatalogSidebar(guikit::Context& ui) {
    if (auto railSidebar = guikit::Sidebar(ui, m_navSidebarMode, {
        .side = guikit::SidebarSide::Left,
        .maxRatio = 0.35f,
        .resizable = true,
        .allowCompact = true,
        .allowCollapse = true
    })) {
        const NavCategory prevCat = m_selectedCategory;
        const keel::i32 prevExt = m_selectedExtension;
        if (auto menu = guikit::SidebarMenu(ui, {.size = guikit::UiSize::Sm})) {
            constexpr keel::i32 kBaseCount = keel::as<keel::i32>(kNavCategoryCount);
            keel::i32 activeIdx = (m_selectedExtension >= 0)
                ? (kBaseCount + m_selectedExtension)
                : keel::as<keel::i32>(m_selectedCategory);

            menu.Section("STYLE");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::StyleThemeStudio),   {.text = "Theme Studio", .icon = guikit::Icon::Eye()});

            menu.Section("LAYOUT");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::LayoutSplitView),    {.text = "SplitView",    .icon = guikit::Icon::Copy()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::LayoutElement),      {.text = "Element",      .icon = guikit::Icon::Settings()});

            menu.Section("SURFACES");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::SurfaceCard),        {.text = "Card",         .icon = guikit::Icon::Copy()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::SurfaceMenuBar),     {.text = "MenuBar",      .icon = guikit::Icon::Settings()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::SurfaceToolbar),     {.text = "Toolbar",      .icon = guikit::Icon::Settings()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::SurfaceSidebar),     {.text = "Sidebar",      .icon = guikit::Icon::Folder()});

            menu.Section("OVERLAYS");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::OverlayOverlays),    {.text = "Overlays",     .icon = guikit::Icon::Flash()});

            menu.Section("ACTIONS");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ActionButton),       {.text = "Button",       .icon = guikit::Icon::Play()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ActionSegmented),    {.text = "Segmented",    .icon = guikit::Icon::Settings()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ActionTabBar),       {.text = "TabBar",       .icon = guikit::Icon::Folder()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ActionBreadcrumb),   {.text = "Breadcrumb",   .icon = guikit::Icon::Folder()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ActionPagination),   {.text = "Pagination",   .icon = guikit::Icon::ArrowRight()});

            menu.Section("FIELDS");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::FieldInputField),    {.text = "InputField",   .icon = guikit::Icon::Edit()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::FieldTextArea),      {.text = "TextArea",     .icon = guikit::Icon::Edit()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::FieldSelect),        {.text = "Select",       .icon = guikit::Icon::Plus()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::FieldSpinBox),       {.text = "SpinBox",      .icon = guikit::Icon::Plus()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::FieldDatePicker),    {.text = "DatePicker",   .icon = guikit::Icon::Edit()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::FieldTagInput),      {.text = "TagInput",     .icon = guikit::Icon::Edit()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::FieldColorPicker),   {.text = "ColorPicker",  .icon = guikit::Icon::Eye()});

            menu.Section("CONTROLS");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ControlCheckbox),    {.text = "Checkbox",     .icon = guikit::Icon::Check()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ControlRadio),       {.text = "Radio",        .icon = guikit::Icon::Check()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ControlSwitch),      {.text = "Switch",       .icon = guikit::Icon::Refresh()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ControlSlider),      {.text = "Slider",       .icon = guikit::Icon::Settings()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ControlKnob),        {.text = "Knob",         .icon = guikit::Icon::Refresh()});

            menu.Section("DISPLAY");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::DisplayHeading),     {.text = "Heading",      .icon = guikit::Icon::Edit()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::DisplayTag),         {.text = "Tag",          .icon = guikit::Icon::Check()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::DisplayProgressBar), {.text = "ProgressBar",  .icon = guikit::Icon::Flash()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::DisplayLevelMeter),  {.text = "LevelMeter",   .icon = guikit::Icon::Play()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::DisplayReadout),     {.text = "Readout",      .icon = guikit::Icon::Target()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::DisplayImage),       {.text = "Image",        .icon = guikit::Icon::Eye()});

            menu.Section("VIEWS");
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ViewTable),          {.text = "Table",        .icon = guikit::Icon::Copy()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ViewTreeView),       {.text = "TreeView",     .icon = guikit::Icon::Folder()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ViewMarkdown),       {.text = "Markdown",     .icon = guikit::Icon::Edit()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ViewImageView),      {.text = "Image View",   .icon = guikit::Icon::Eye()});
            menu.Item(activeIdx, keel::as<keel::i32>(NavCategory::ViewSchematic2D),    {.text = "Schematic 2D", .icon = guikit::Icon::Search()});

            if (!m_extensions.empty()) {
                menu.Section("MODULES");
                for (keel::usize ei = 0; ei < m_extensions.size(); ++ei) {
                    const auto& ext = m_extensions[ei];
                    menu.Item(activeIdx, kBaseCount + keel::as<keel::i32>(ei), {
                        .text = ext.navLabel,
                        .icon = ext.icon
                    });
                }
            }

            if (activeIdx >= kBaseCount) {
                m_selectedExtension = activeIdx - kBaseCount;
            } else {
                m_selectedExtension = -1;
                m_selectedCategory = keel::from_underlying<NavCategory>(activeIdx);
            }
        }
        if (m_selectedCategory != prevCat || m_selectedExtension != prevExt) {
            ui.InvalidateAll();
        }
    }
}

void DemoWindow::RenderInspectorSidebar(guikit::Context& ui) {
    if (auto inspectorSidebar = guikit::Sidebar(ui, m_inspectorSidebarMode, {
        .side = guikit::SidebarSide::Left,
        .width = 320.0f,
        .maxRatio = 0.45f,
        .resizable = true,
        .allowCollapse = true,
        .gap = 8.0f,
        .padding = guikit::Padding::All(12.0f)
    })) {
        guikit::Heading(ui, {
            .label = "INSPECTOR",
            .level = guikit::HeadingLevel::H6,
            .separator = guikit::HeadingSeparator::Bottom
        });

        if (m_selectedExtension >= 0 && keel::as<keel::usize>(m_selectedExtension) < m_extensions.size()) {
            if (auto fn = m_extensions[keel::as<keel::usize>(m_selectedExtension)].renderInspector) {
                fn(ui, m_demoCtx);
            }
        } else if (IsControlCategory(m_selectedCategory)) {
            if (auto elemAcc = guikit::Accordion(ui, {
                .label = "ELEMENT",
                .defaultOpen = false,
                .size = guikit::UiSize::Sm
            })) {
                m_demoCtx.RenderElementSection(ui);
            }

            if (auto elemLayoutAcc = guikit::Accordion(ui, {
                .label = "ELEMENT LAYOUT",
                .defaultOpen = false,
                .size = guikit::UiSize::Sm
            })) {
                m_demoCtx.RenderElementLayoutSection(ui, m_selectedCategory);
            }

            const bool hasCtrlLayout = DemoContext::HasCtrlLayoutCategory(m_selectedCategory);
            bool collapsedOpen = false;
            guikit::AccordionOptions ctrlLayoutAccOpts{
                .label = "CONTROL LAYOUT",
                .defaultOpen = false,
                .size = guikit::UiSize::Sm,
                .disabled = !hasCtrlLayout
            };
            if (!hasCtrlLayout) {
                ctrlLayoutAccOpts.badge = {"Fit", guikit::UiVariant::Secondary};
            }
            if (auto ctrlLayoutAcc = guikit::Accordion(ui,
                hasCtrlLayout ? m_demoCtx.ctrlLayoutOpen : collapsedOpen,
                ctrlLayoutAccOpts
            )) {
                m_demoCtx.RenderControlLayoutSection(ui, m_selectedCategory);
            }

            if (auto ctrlStyleAcc = guikit::Accordion(ui, {
                .label = "CONTROL STYLE",
                .defaultOpen = false,
                .size = guikit::UiSize::Sm
            })) {
                m_demoCtx.RenderControlStyleSection(ui, m_selectedCategory);
            }

            if (auto specAcc = guikit::Accordion(ui, {
                .label = "CONTROL SPECIFIC",
                .badge = {CategoryControlName(m_selectedCategory), guikit::UiVariant::Secondary},
                .defaultOpen = true,
                .size = guikit::UiSize::Sm
            })) {
                DispatchInspector(ui, m_selectedCategory, m_demoCtx);
            }
        } else {
            DispatchInspector(ui, m_selectedCategory, m_demoCtx);
        }
    }
}

void DemoWindow::RenderPreviewToolbar(guikit::Context& ui) {
    if (auto previewTb = guikit::Toolbar(ui)) {
        guikit::Text(ui, "PREVIEW", {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted});
        previewTb.Separator();

        if (guikit::Select(ui, m_demoCtx.previewFormFactorIdx, kPreviewFormFactorNames, {.size = guikit::UiSize::Sm, .width = 96.0f}).changed) {
            if (m_demoCtx.previewFormFactorIdx == 1) {
                m_demoCtx.previewLandscape = true;  // Tablet defaults to Landscape
                if (m_demoCtx.previewZoomModeIdx == keel::as<keel::usize>(PreviewZoomMode::Zoom100)) {
                    m_demoCtx.previewZoomModeIdx = keel::as<keel::usize>(PreviewZoomMode::Fit);
                }
            } else if (m_demoCtx.previewFormFactorIdx == 2) {
                m_demoCtx.previewLandscape = false; // Phone defaults to Portrait
                if (m_demoCtx.previewZoomModeIdx == keel::as<keel::usize>(PreviewZoomMode::Zoom100)) {
                    m_demoCtx.previewZoomModeIdx = keel::as<keel::usize>(PreviewZoomMode::Fit);
                }
            }
            ui.InvalidateAll();
        }

        static constexpr keel::StaticArray<keel::StringRef, 2> kOrientationVariants = {"Port", "Land"};
        const bool canRotateDevice = (m_demoCtx.previewFormFactorIdx == 1 || m_demoCtx.previewFormFactorIdx == 2);
        if (previewTb.Toggle(m_demoCtx.previewLandscape, {
            .text = {m_demoCtx.previewLandscape ? "Land" : "Port", kOrientationVariants},
            .tooltip = "Rotate Device Orientation (Portrait / Landscape)",
            .icon = guikit::Icon::Refresh(),
            .variant = guikit::UiVariant::Info,
            .disabled = !canRotateDevice
        }).changed) {
            ui.InvalidateAll();
        }

        previewTb.Separator();

        const keel::i32 effZoomPct = keel::round_i32(m_demoCtx.previewEffectiveZoom * 100.0f);
        guikit::SelectOptions zoomSelectOpts{
            .tooltip = "Preview Zoom / Scale (Fit, Real 1:1 physical mm, or Ctrl + MouseWheel)",
            .size = guikit::UiSize::Sm,
            .width = 116.0f
        };
        if (m_demoCtx.previewZoomModeIdx == keel::as<keel::usize>(PreviewZoomMode::Fit) ||
            m_demoCtx.previewZoomModeIdx == keel::as<keel::usize>(PreviewZoomMode::Real1To1)) {
            zoomSelectOpts.suffix.text = {ui.Format("{}%", effZoomPct), guikit::StateColor::Muted()};
        }

        bool zoomChanged = false;
        if (m_demoCtx.previewZoomModeIdx >= kPreviewZoomPresetNames.size()) {
            const keel::StringRef customLabel = ui.AllocString(ui.Format("{}%", effZoomPct));
            const keel::StaticArray<keel::StringRef, 9> zoomItemsWithCustom = {
                kPreviewZoomPresetNames[0],
                kPreviewZoomPresetNames[1],
                kPreviewZoomPresetNames[2],
                kPreviewZoomPresetNames[3],
                kPreviewZoomPresetNames[4],
                kPreviewZoomPresetNames[5],
                kPreviewZoomPresetNames[6],
                kPreviewZoomPresetNames[7],
                customLabel
            };
            zoomChanged = guikit::Select(ui, m_demoCtx.previewZoomModeIdx, zoomItemsWithCustom, zoomSelectOpts).changed;
        } else {
            zoomChanged = guikit::Select(ui, m_demoCtx.previewZoomModeIdx, kPreviewZoomPresetNames, zoomSelectOpts).changed;
        }
        if (zoomChanged) {
            ui.InvalidateAll();
        }

        static constexpr keel::StaticArray<keel::StringRef, guikit::kThemePresetCount> kPreviewPresetLabels = {{
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
        keel::usize previewPresetIdx = keel::as<keel::usize>(GetStudioPreset());
        if (previewPresetIdx >= guikit::kThemePresetCount) {
            previewPresetIdx = 0u;
        }
        if (guikit::Select(ui, previewPresetIdx, kPreviewPresetLabels, {
            .tooltip = "Preview Theme Palette Preset (synced with Theme Studio)",
            .size = guikit::UiSize::Sm,
            .width = 140.0f
        }).changed) {
            SelectStudioPreset(m_demoCtx, keel::from_underlying<guikit::ThemePreset>(previewPresetIdx));
            ui.InvalidateAll();
        }

        static constexpr keel::StaticArray<keel::StringRef, guikit::kChromeStyleCount> kPreviewChromeLabels = {{
            guikit::ChromeStyleName(guikit::ChromeStyle::Modern),
            guikit::ChromeStyleName(guikit::ChromeStyle::Fluent),
            guikit::ChromeStyleName(guikit::ChromeStyle::Classic3D),
            guikit::ChromeStyleName(guikit::ChromeStyle::Brutalist)
        }};
        keel::usize previewChromeIdx = keel::as<keel::usize>(GetStudioChrome());
        if (guikit::Select(ui, previewChromeIdx, kPreviewChromeLabels, {
            .tooltip = "Preview Structural Chrome Style (synced with Theme Studio)",
            .size = guikit::UiSize::Sm,
            .width = 104.0f
        }).changed) {
            SelectStudioChrome(m_demoCtx, keel::from_underlying<guikit::ChromeStyle>(previewChromeIdx));
            ui.InvalidateAll();
        }

        static constexpr keel::StaticArray<keel::StringRef, guikit::kFillStyleCount> kPreviewFillLabels = {{
            guikit::FillStyleName(guikit::FillStyle::Solid),
            guikit::FillStyleName(guikit::FillStyle::Soft),
            guikit::FillStyleName(guikit::FillStyle::Outline)
        }};
        keel::usize previewFillIdx = keel::as<keel::usize>(GetStudioFillStyle());
        if (guikit::Select(ui, previewFillIdx, kPreviewFillLabels, {
            .tooltip = "Preview Control Fill Style (synced with Theme Studio)",
            .size = guikit::UiSize::Sm,
            .width = 86.0f
        }).changed) {
            SelectStudioFillStyle(m_demoCtx, keel::from_underlying<guikit::FillStyle>(previewFillIdx));
            ui.InvalidateAll();
        }

        previewTb.Toggle(m_demoCtx.previewBounds, {
            .text = "Bounds",
            .icon = m_demoCtx.previewBounds ? guikit::Icon::Eye() : guikit::Icon::EyeOff(),
            .variant = guikit::UiVariant::Info
        });

        previewTb.Toggle(m_demoCtx.previewInspect, {
            .text = "Inspect",
            .icon = guikit::Icon::Search(),
            .variant = guikit::UiVariant::Warning
        });

        if (m_selectedExtension >= 0 || IsSandboxCategory(m_selectedCategory)) {
            if (previewTb.Toggle(m_demoCtx.previewShowCode, {
                .text = "Code",
                .icon = m_demoCtx.previewShowCode ? guikit::Icon::Eye() : guikit::Icon::EyeOff(),
                .variant = guikit::UiVariant::Primary
            }).changed) {
                ui.RequestRepaint(3);
            }
        }
    }
}

void DemoWindow::RenderCodePanel(guikit::Context& ui) {
    if (auto codePanel = guikit::BottomPanel(ui, m_demoCtx.previewShowCode, {
        .height = 240.0f,
        .resizable = true,
        .allowCollapse = true
    })) {
        if (auto codeTb = guikit::Toolbar(ui, {
            .style = guikit::ToolbarStyle::Header,
            .size = guikit::UiSize::Sm
        })) {
            guikit::Text(ui, "GENERATED C++20 CODE", {
                .role = guikit::TextRole::LabelSmall,
                .variant = guikit::UiVariant::Secondary
            });
            codeTb.Spacer();
            if (codeTb.Action({
                .text = "Close",
                .tooltip = "Close Code Panel",
                .icon = guikit::Icon::Close(),
                .square = true
            }).clicked) {
                m_demoCtx.previewShowCode = false;
                ui.InvalidateAll();
            }
        }

        if (!IsLiveResizeFrame() || m_cachedCodeSnippet.empty() ||
            m_cachedCodeCategory != m_selectedCategory || m_cachedCodeExtension != m_selectedExtension) {
            if (m_selectedExtension >= 0 && keel::as<keel::usize>(m_selectedExtension) < m_extensions.size()) {
                auto fn = m_extensions[keel::as<keel::usize>(m_selectedExtension)].generateCode;
                m_cachedCodeSnippet = fn ? fn(m_demoCtx) : keel::String{};
            } else {
                m_cachedCodeSnippet = DispatchCodeGenerator(m_selectedCategory, m_demoCtx);
            }
            m_cachedCodeCategory = m_selectedCategory;
            m_cachedCodeExtension = m_selectedExtension;
        }
        guikit::TextArea(ui, m_cachedCodeSnippet, {
            .autoHeight = false,
            .readOnly = true,
            .showLineNumbers = true,
            .showCopyButton = true,
            .monospace = true,
            .allowZoom = true,
            .syntax = guikit::SyntaxConfig::Cpp(),
            .height = guikit::SizeMode::Stretch,
            .background = guikit::StateColor{},
            .border = {.radius = 0.0f}
        });
    }
}

void DemoWindow::RenderWorkspace(guikit::Context& ui) {
    if (auto workspaceRow = guikit::Row(ui, {.height = guikit::SizeMode::Stretch})) {
        RenderCatalogSidebar(ui);
        RenderInspectorSidebar(ui);

        if (auto contentCol = guikit::Column(ui, {
            .width = guikit::SizeMode::Stretch,
            .height = guikit::SizeMode::Stretch
        })) {
            RenderPreviewToolbar(ui);

            const bool extActive = (m_selectedExtension >= 0 && keel::as<keel::usize>(m_selectedExtension) < m_extensions.size());
            if (extActive || IsSandboxCategory(m_selectedCategory)) {
                const bool isFullStage = extActive ? true : IsFullStageCategory(m_selectedCategory);
                const bool isShellScreen = !extActive && (m_selectedCategory == NavCategory::SurfaceSidebar);
                const keel::StringRef title = extActive
                    ? m_extensions[keel::as<keel::usize>(m_selectedExtension)].title
                    : CategoryDisplayTitle(m_selectedCategory);
                RenderPreviewViewport(ui, m_demoCtx, isFullStage, isShellScreen, title, [&]() {
                    if (extActive) {
                        if (auto fn = m_extensions[keel::as<keel::usize>(m_selectedExtension)].renderStage) {
                            fn(ui, m_demoCtx);
                        }
                    } else {
                        DispatchSandboxStage(ui, m_selectedCategory, m_demoCtx);
                    }
                });
                RenderCodePanel(ui);
            }
        }
    }
}

void DemoWindow::RenderBottomStatusBar(guikit::Context& ui) {
    if (auto sb = guikit::StatusBar(ui)) {
        const bool isGpuScene = (m_selectedExtension >= 0 &&
                                 keel::as<keel::usize>(m_selectedExtension) < m_extensions.size() &&
                                 m_extensions[keel::as<keel::usize>(m_selectedExtension)].isGpuScene);
        m_shellControls.RenderTelemetryStatus(*this, sb, isGpuScene);

        sb.Spacer();

        static const keel::StaticArray<guikit::CommandPaletteItem, 16> kGlobalCommands = {{
            {.label = "Go to Theme Studio (Palette × Chrome × Fill)", .category = "Style",    .description = "Presets, ChromeStyle, FillStyle, WCAG & JSON", .icon = guikit::Icon::Eye()},
            {.label = "Go to Element Form-Field Wrapper",             .category = "Layout",   .description = "5 ElementLayout modes & composite slots",      .icon = guikit::Icon::Settings()},
            {.label = "Go to SplitView Resizable Panes",              .category = "Layout",   .description = "Horizontal & Vertical splitters",              .icon = guikit::Icon::Copy()},
            {.label = "Go to Overlays (Modal / Toast / Cmd)",         .category = "Overlays", .description = "Modals, context menus & palettes",             .icon = guikit::Icon::Flash()},
            {.label = "Go to Breadcrumb Path Navigation",             .category = "Actions",  .description = "Hierarchical path trail & collapse",           .icon = guikit::Icon::Folder()},
            {.label = "Go to Pagination Control",                     .category = "Actions",  .description = "Page numbers & smart ellipsis jump",           .icon = guikit::Icon::ArrowRight()},
            {.label = "Go to Select / MultiSelect / ComboBox",        .category = "Fields",   .description = "Dropdown, live filter & tag chips",            .icon = guikit::Icon::Plus()},
            {.label = "Go to DatePicker & TimePicker",                .category = "Fields",   .description = "Calendar grid & time stepper",                 .icon = guikit::Icon::Edit()},
            {.label = "Go to SpinBox Numeric Stepper",                .category = "Fields",   .description = "Inline, Detached & Split steppers",            .icon = guikit::Icon::Plus()},
            {.label = "Go to TagInput Editable Chips",                .category = "Fields",   .description = "Editable tag pills & delimiter split",         .icon = guikit::Icon::Edit()},
            {.label = "Go to ColorPicker HSV/HEX",                    .category = "Fields",   .description = "2D SV canvas, hue & alpha sliders",            .icon = guikit::Icon::Eye()},
            {.label = "Go to LevelMeter / Segmented VU Meter",        .category = "Display",  .description = "LED dB meters, peak-hold & clip LED",          .icon = guikit::Icon::Play()},
            {.label = "Go to Table / Virtualized Grid",               .category = "Views",    .description = "100,000 rows virtualized data grid",           .icon = guikit::Icon::Copy()},
            {.label = "Toggle Preview Dark / Light Theme",            .category = "Style",    .description = "Switch stage palette mode (Theme Studio)",     .icon = guikit::Icon::Refresh()},
            {.label = "Toggle Generated C++20 Code Panel",            .category = "View",     .description = "Show or hide bottom code generator",           .icon = guikit::Icon::Eye()},
            {.label = "Toggle Docked / Floating Island Shell",        .category = "View",     .description = "Switch between flush IDE & floating island frame", .icon = guikit::Icon::Copy()}
        }};

        const auto cmdResp = m_shellControls.RenderCommandPalette(*this, sb, kGlobalCommands);

        if (m_selectedExtension >= 0 || IsSandboxCategory(m_selectedCategory)) {
            if (sb.Action({
                .text = "C++ Code",
                .icon = m_demoCtx.previewShowCode ? guikit::Icon::Eye() : guikit::Icon::EyeOff(),
                .tooltip = "Toggle BottomPanel C++20 Code Generator",
                .active = m_demoCtx.previewShowCode
            }).clicked) {
                m_demoCtx.previewShowCode = !m_demoCtx.previewShowCode;
                ui.InvalidateAll();
            }
        }

        if (cmdResp.changed && cmdResp.selectedIndex >= 0) {
            switch (cmdResp.selectedIndex) {
                case 0:  m_selectedExtension = -1; m_selectedCategory = NavCategory::StyleThemeStudio; break;
                case 1:  m_selectedExtension = -1; m_selectedCategory = NavCategory::LayoutElement; break;
                case 2:  m_selectedExtension = -1; m_selectedCategory = NavCategory::LayoutSplitView; break;
                case 3:  m_selectedExtension = -1; m_selectedCategory = NavCategory::OverlayOverlays; break;
                case 4:  m_selectedExtension = -1; m_selectedCategory = NavCategory::ActionBreadcrumb; break;
                case 5:  m_selectedExtension = -1; m_selectedCategory = NavCategory::ActionPagination; break;
                case 6:  m_selectedExtension = -1; m_selectedCategory = NavCategory::FieldSelect; break;
                case 7:  m_selectedExtension = -1; m_selectedCategory = NavCategory::FieldDatePicker; break;
                case 8:  m_selectedExtension = -1; m_selectedCategory = NavCategory::FieldSpinBox; break;
                case 9:  m_selectedExtension = -1; m_selectedCategory = NavCategory::FieldTagInput; break;
                case 10: m_selectedExtension = -1; m_selectedCategory = NavCategory::FieldColorPicker; break;
                case 11: m_selectedExtension = -1; m_selectedCategory = NavCategory::DisplayLevelMeter; break;
                case 12: m_selectedExtension = -1; m_selectedCategory = NavCategory::ViewTable; break;
                case 13:
                    ToggleStudioDarkLight(m_demoCtx);
                    break;
                case 14:
                    m_demoCtx.previewShowCode = !m_demoCtx.previewShowCode;
                    break;
                case 15:
                    SetShellFloating(!IsShellFloating());
                    break;
                default: break;
            }
            ui.InvalidateAll();
        }
    }
}

void DemoWindow::DispatchInspector(guikit::Context& ui, NavCategory cat, DemoContext& ctx) {
    using InspectorFn = void (*)(guikit::Context&, DemoContext&);
    static constexpr keel::StaticArray<InspectorFn, kNavCategoryCount> kInspectors = {
        // 1. STYLE
        &RenderThemeStudioInspector,
        // 2. LAYOUT
        &RenderSplitViewInspector,
        &RenderElementInspector,
        // 3. SURFACES
        &RenderCardInspector,
        &RenderMenuBarInspector,
        &RenderToolbarInspector,
        &RenderSidebarInspector,
        // 4. OVERLAYS
        &RenderOverlaysInspector,
        // 5. ACTIONS
        &RenderButtonInspector,
        &RenderSegmentedInspector,
        &RenderTabBarInspector,
        &RenderBreadcrumbInspector,
        &RenderPaginationInspector,
        // 6. FIELDS
        &RenderInputInspector,
        &RenderCodeEditorInspector,
        &RenderSelectInspector,
        &RenderSpinBoxInspector,
        &RenderDatePickerInspector,
        &RenderTagInputInspector,
        &RenderColorPickerInspector,
        // 7. CONTROLS
        &RenderCheckboxInspector,
        &RenderRadioInspector,
        &RenderSwitchInspector,
        &RenderSliderInspector,
        &RenderKnobInspector,
        // 8. DISPLAY
        &RenderHeadingInspector,
        &RenderTagInspector,
        &RenderProgressBarInspector,
        &RenderLevelMeterInspector,
        &RenderReadoutInspector,
        &RenderImageInspector,
        // 9. VIEWS
        &RenderTableInspector,
        &RenderTreeViewInspector,
        &RenderMarkdownInspector,
        &RenderImageViewInspector,
        &RenderSchematicInspector,
    };
    static_assert(kInspectors.size() == kNavCategoryCount);

    const keel::usize idx = keel::as<keel::usize>(cat);
    if (idx < kInspectors.size()) {
        kInspectors[idx](ui, ctx);
    }
}

void DemoWindow::DispatchSandboxStage(guikit::Context& ui, NavCategory cat, DemoContext& ctx) {
    using StageFn = void (*)(guikit::Context&, DemoContext&);
    static constexpr keel::StaticArray<StageFn, kNavCategoryCount> kStages = {
        // 1. STYLE
        &RenderThemeStudioStage,
        // 2. LAYOUT
        &RenderSplitViewStage,
        &RenderElementStage,
        // 3. SURFACES
        &RenderCardStage,
        &RenderMenuBarStage,
        &RenderToolbarStage,
        &RenderSidebarStage,
        // 4. OVERLAYS
        &RenderOverlaysStage,
        // 5. ACTIONS
        &RenderButtonStage,
        &RenderSegmentedStage,
        &RenderTabBarStage,
        &RenderBreadcrumbStage,
        &RenderPaginationStage,
        // 6. FIELDS
        &RenderInputStage,
        &RenderCodeEditorStage,
        &RenderSelectStage,
        &RenderSpinBoxStage,
        &RenderDatePickerStage,
        &RenderTagInputStage,
        &RenderColorPickerStage,
        // 7. CONTROLS
        &RenderCheckboxStage,
        &RenderRadioStage,
        &RenderSwitchStage,
        &RenderSliderStage,
        &RenderKnobStage,
        // 8. DISPLAY
        &RenderHeadingStage,
        &RenderTagStage,
        &RenderProgressBarStage,
        &RenderLevelMeterStage,
        &RenderReadoutStage,
        &RenderImageStage,
        // 9. VIEWS
        &RenderTableStage,
        &RenderTreeViewStage,
        &RenderMarkdownStage,
        &RenderImageViewStage,
        &RenderSchematicStage,
    };
    static_assert(kStages.size() == kNavCategoryCount);

    const keel::usize idx = keel::as<keel::usize>(cat);
    if (idx < kStages.size()) {
        kStages[idx](ui, ctx);
    }
}

keel::String DemoWindow::DispatchCodeGenerator(NavCategory cat, const DemoContext& ctx) {
    using CodeGenFn = keel::String (*)(const DemoContext&);
    static constexpr keel::StaticArray<CodeGenFn, kNavCategoryCount> kGenerators = {
        // 1. STYLE
        &GenerateThemeStudioCode,
        // 2. LAYOUT
        &GenerateSplitViewCode,
        &GenerateElementCode,
        // 3. SURFACES
        &GenerateCardCode,
        &GenerateMenuBarCode,
        &GenerateToolbarCode,
        &GenerateSidebarCode,
        // 4. OVERLAYS
        &GenerateOverlaysCode,
        // 5. ACTIONS
        &GenerateButtonCode,
        &GenerateSegmentedCode,
        &GenerateTabBarCode,
        &GenerateBreadcrumbCode,
        &GeneratePaginationCode,
        // 6. FIELDS
        &GenerateInputCode,
        &GenerateCodeEditorCode,
        &GenerateSelectCode,
        &GenerateSpinBoxCode,
        &GenerateDatePickerCode,
        &GenerateTagInputCode,
        &GenerateColorPickerCode,
        // 7. CONTROLS
        &GenerateCheckboxCode,
        &GenerateRadioCode,
        &GenerateSwitchCode,
        &GenerateSliderCode,
        &GenerateKnobCode,
        // 8. DISPLAY
        &GenerateHeadingCode,
        &GenerateTagCode,
        &GenerateProgressBarCode,
        &GenerateLevelMeterCode,
        &GenerateReadoutCode,
        &GenerateImageCode,
        // 9. VIEWS
        &GenerateTableCode,
        &GenerateTreeViewCode,
        &GenerateMarkdownCode,
        &GenerateImageViewCode,
        &GenerateSchematicCode,
    };
    static_assert(kGenerators.size() == kNavCategoryCount);

    const keel::usize idx = keel::as<keel::usize>(cat);
    if (idx < kGenerators.size()) {
        return kGenerators[idx](ctx);
    }
    return {};
}

} // namespace guikit::demo
