#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct TreeDemoState {
    keel::i32 selectedIndex = 0;
    guikit::Id selectedId = guikit::Id::Null();
    keel::SmallString<128> filterQuery{};

    guikit::TreeGuides guides = guikit::TreeGuides::Connected;
    guikit::TreeSelectionStyle selectionStyle = guikit::TreeSelectionStyle::Pill;
    keel::f32 indent = 18.0f;
    bool showColumns = false;
    bool gridLines = true;
    bool searchable = true;
    bool showToolbar = false;
    bool striped = false;
    bool expandOnSelect = false;
    bool showCheckboxes = false;
    bool showEmbeddedControls = false;
    bool showDescriptions = false;
    bool showBadgesAndDetails = true;

    // Checked states for tree items
    bool chkSun = true;
    bool chkFog = true;
    bool chkAntiAlias = true;
    bool chkBloom = true;
    bool chkAo = false;
    bool chkShaderTonemap = true;
    bool chkShaderBloom = true;
    bool chkThemeJson = true;

    // Embedded control states inside TreeItem rows
    bool sceneActive = true;
    keel::f32 envExposure = 1.15f;
    bool sunEnabled = true;
    keel::f32 fogDensity = 35.0f;
    keel::usize aaModeIdx = 0;
    bool bloomEnabled = true;
    bool aoEnabled = true;
    keel::i32 resetCount = 0;
};

TreeDemoState s_tree;

constexpr keel::StringRef GuidesEnumName(guikit::TreeGuides g) noexcept {
    switch (g) {
        case guikit::TreeGuides::None:      return "None";
        case guikit::TreeGuides::Vertical:  return "Vertical";
        case guikit::TreeGuides::Connected: return "Connected";
    }
    return "Connected";
}

} // namespace

void RenderTreeViewInspector(guikit::Context& ui, DemoContext& ctx) {
    if (auto el = guikit::Element(ui, {.label = "Tree Size"})) {
        if (guikit::StepSlider(ui, ctx.controlsSize, {.showTicks = true, .showLabels = true, .includeNone = true}).changed) {
            ui.InvalidateAll();
        }
    }
    ctx.RenderVariantPicker(ui);

    static constexpr keel::StaticArray<keel::StringRef, 2> kSelStyleNames = {"Pill", "Indicator"};
    keel::usize selStyleIdx = (s_tree.selectionStyle == guikit::TreeSelectionStyle::Indicator) ? 1 : 0;
    if (auto el = guikit::Element(ui, {.label = "Selection Style"})) {
        if (guikit::Segmented(ui, selStyleIdx, kSelStyleNames).changed) {
            s_tree.selectionStyle = (selStyleIdx == 1)
                ? guikit::TreeSelectionStyle::Indicator
                : guikit::TreeSelectionStyle::Pill;
            ui.InvalidateAll();
        }
    }

    static constexpr keel::StaticArray<keel::StringRef, 3> kGuideNames = {"None", "Vertical", "Connected"};
    keel::usize guideIdx = keel::as<keel::usize>(s_tree.guides);
    if (auto el = guikit::Element(ui, {.label = "Hierarchy Guides"})) {
        if (guikit::Segmented(ui, guideIdx, kGuideNames).changed) {
            s_tree.guides = keel::from_underlying<guikit::TreeGuides>(guideIdx);
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Indent Step"})) {
        if (guikit::Slider(ui, s_tree.indent, 12.0f, 32.0f, {.valueFormat = "%.0f px", .step = 2.0f}).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Multi-Column Mode (TreeTable)"})) {
        if (guikit::Switch(ui, s_tree.showColumns).changed) ui.InvalidateAll();
    }
    if (s_tree.showColumns) {
        if (auto el = guikit::Element(ui, {.label = "Column Grid Lines"})) {
            if (guikit::Switch(ui, s_tree.gridLines).changed) ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Search Filter Bar"})) {
        if (guikit::Switch(ui, s_tree.searchable).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Expand/Collapse Toolbar"})) {
        if (guikit::Switch(ui, s_tree.showToolbar).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Striped Rows"})) {
        if (guikit::Switch(ui, s_tree.striped).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Expand Branch on Select"})) {
        if (guikit::Switch(ui, s_tree.expandOnSelect).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Tri-State Checkboxes"})) {
        if (guikit::Switch(ui, s_tree.showCheckboxes).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Embedded Controls"})) {
        if (guikit::Switch(ui, s_tree.showEmbeddedControls).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Row Descriptions"})) {
        if (guikit::Switch(ui, s_tree.showDescriptions).changed) ui.InvalidateAll();
    }
    if (!s_tree.showColumns) {
        if (auto el = guikit::Element(ui, {.label = "Show Details & Badges"})) {
            if (guikit::Switch(ui, s_tree.showBadgesAndDetails).changed) ui.InvalidateAll();
        }
    }
}

void RenderTreeViewStage(guikit::Context& ui, DemoContext& ctx) {
    // Compute tri-state parent checkboxes from leaf states
    const keel::i32 envCheckedCount = (s_tree.chkSun ? 1 : 0) + (s_tree.chkFog ? 1 : 0);
    bool chkEnvAll = (envCheckedCount == 2);
    const bool indEnv = (envCheckedCount > 0 && envCheckedCount < 2);

    const keel::i32 postCheckedCount = (s_tree.chkAntiAlias ? 1 : 0) + (s_tree.chkBloom ? 1 : 0) + (s_tree.chkAo ? 1 : 0);
    bool chkPostAll = (postCheckedCount == 3);
    const bool indPost = (postCheckedCount > 0 && postCheckedCount < 3);

    const keel::i32 sceneCheckedCount = envCheckedCount + postCheckedCount;
    bool chkSceneAll = (sceneCheckedCount == 5);
    const bool indScene = (sceneCheckedCount > 0 && sceneCheckedCount < 5);

    const keel::i32 shaderCheckedCount = (s_tree.chkShaderTonemap ? 1 : 0) + (s_tree.chkShaderBloom ? 1 : 0);
    bool chkShadersAll = (shaderCheckedCount == 2);
    const bool indShaders = (shaderCheckedCount > 0 && shaderCheckedCount < 2);

    const keel::i32 assetsCheckedCount = shaderCheckedCount + (s_tree.chkThemeJson ? 1 : 0);
    bool chkAssetsAll = (assetsCheckedCount == 3);
    const bool indAssets = (assetsCheckedCount > 0 && assetsCheckedCount < 3);

    static constexpr keel::StaticArray<guikit::TableColumn, 4> kTreeCols = {{
        {.title = "Node / Asset", .flex = 1.6f,    .minWidth = 200.0f, .align = guikit::Align::Start},
        {.title = "Spec",         .width = 104.0f, .minWidth = 72.0f,  .align = guikit::Align::Start},
        {.title = "Status",       .width = 96.0f,  .minWidth = 72.0f,  .align = guikit::Align::Center},
        {.title = "Controls",     .width = 176.0f, .minWidth = 120.0f, .align = guikit::Align::End}
    }};

    const bool useCols = s_tree.showColumns;
    const bool inlineDecor = !useCols && s_tree.showBadgesAndDetails;

    if (auto tree = guikit::TreeView(ui, {
        .title = "SCENE & ASSET HIERARCHY",
        .columns = useCols ? keel::ArrayRef<guikit::TableColumn>(kTreeCols) : keel::ArrayRef<guikit::TableColumn>{},
        .gridLines = useCols && s_tree.gridLines,
        .guides = s_tree.guides,
        .selectionStyle = s_tree.selectionStyle,
        .color = ctx.ResolveControlColor(),
        .indent = s_tree.indent,
        .striped = s_tree.striped,
        .expandOnSelect = s_tree.expandOnSelect,
        .searchable = s_tree.searchable,
        .filterText = &s_tree.filterQuery,
        .showToolbar = s_tree.showToolbar,
        .selectedIndex = &s_tree.selectedIndex,
        .selectedId = &s_tree.selectedId,
        .size = ctx.controlsSize
    })) {
        // 1. Root Branch: Scene Hierarchy
        if (auto sceneItem = guikit::TreeItem(ui, {
            .label = "Scene Hierarchy",
            .description = s_tree.showDescriptions ? "Active viewport nodes & real-time subsystems" : "",
            .badge = inlineDecor ? guikit::UiText{"5 nodes", guikit::StateColor::Primary()} : guikit::UiText{},
            .icon = guikit::Icon::Folder(),
            .defaultOpen = true,
            .checked = s_tree.showCheckboxes ? &chkSceneAll : nullptr,
            .indeterminate = s_tree.showCheckboxes && indScene
        })) {
            if (sceneItem.checkChanged) {
                s_tree.chkSun = s_tree.chkFog = s_tree.chkAntiAlias = s_tree.chkBloom = s_tree.chkAo = chkSceneAll;
            }
            if (useCols) {
                guikit::Text(ui, "Viewport", {.variant = guikit::UiVariant::Secondary});
                guikit::Element(ui, {.badge = {"5 nodes", guikit::UiVariant::Primary}, .width = guikit::SizeMode::Fit});
            }
            if (s_tree.showEmbeddedControls) {
                guikit::Switch(ui, s_tree.sceneActive);
            }

            // 1.1 Environment & Lighting
            if (auto envItem = guikit::TreeItem(ui, {
                .label = "Environment & Lighting",
                .description = s_tree.showDescriptions ? "PBR skybox, directional sun, and volumetric fog" : "",
                .icon = guikit::Icon::Eye(),
                .defaultOpen = true,
                .checked = s_tree.showCheckboxes ? &chkEnvAll : nullptr,
                .indeterminate = s_tree.showCheckboxes && indEnv
            })) {
                if (envItem.checkChanged) {
                    s_tree.chkSun = s_tree.chkFog = chkEnvAll;
                }
                if (useCols) {
                    guikit::Text(ui, "PBR Group", {.variant = guikit::UiVariant::Secondary});
                    guikit::Element(ui, {.badge = {"2 passes", guikit::UiVariant::Info}, .width = guikit::SizeMode::Fit});
                }
                if (s_tree.showEmbeddedControls) {
                    guikit::Slider(ui, s_tree.envExposure, 0.0f, 2.5f, {
                        .valueFormat = "%.2f EV",
                        .width = useCols ? guikit::AxisSize::Auto() : guikit::AxisSize::Fixed(128.0f)
                    });
                }

                if (auto sunItem = guikit::TreeItem(ui, {
                    .label = "Directional Sun",
                    .description = s_tree.showDescriptions ? "Cascaded shadow map primary light source" : "",
                    .detail = inlineDecor ? "6500K" : "",
                    .badge = inlineDecor ? guikit::UiText{"HDR", guikit::StateColor::Warning()} : guikit::UiText{},
                    .icon = guikit::Icon::Eye(),
                    .checked = s_tree.showCheckboxes ? &s_tree.chkSun : nullptr
                })) {
                    if (useCols) {
                        guikit::Text(ui, "6500K", {.variant = guikit::UiVariant::Secondary});
                        guikit::Element(ui, {.badge = {"HDR", guikit::UiVariant::Warning}, .width = guikit::SizeMode::Fit});
                    }
                    if (s_tree.showEmbeddedControls) {
                        guikit::Switch(ui, s_tree.sunEnabled);
                    }
                }

                if (auto fogItem = guikit::TreeItem(ui, {
                    .label = "Volumetric Fog",
                    .description = s_tree.showDescriptions ? "Exponential height fog with temporal reprojection" : "",
                    .detail = inlineDecor ? "Raymarched" : "",
                    .icon = guikit::Icon::Eye(),
                    .checked = s_tree.showCheckboxes ? &s_tree.chkFog : nullptr
                })) {
                    if (useCols) {
                        guikit::Text(ui, "Raymarched", {.variant = guikit::UiVariant::Secondary});
                        guikit::Element(ui, {.badge = {"Froxel", guikit::UiVariant::Default}, .width = guikit::SizeMode::Fit});
                    }
                    if (s_tree.showEmbeddedControls) {
                        guikit::Slider(ui, s_tree.fogDensity, 0.0f, 100.0f, {
                            .valueFormat = "%.0f%%",
                            .width = useCols ? guikit::AxisSize::Auto() : guikit::AxisSize::Fixed(110.0f)
                        });
                    }
                }
            }

            // 1.2 Post-Processing
            if (auto postItem = guikit::TreeItem(ui, {
                .label = "Post-Processing",
                .description = s_tree.showDescriptions ? "Anti-aliasing, HDR bloom, and ambient occlusion" : "",
                .badge = inlineDecor ? guikit::UiText{"3 passes", guikit::StateColor::Info()} : guikit::UiText{},
                .icon = guikit::Icon::Search(),
                .defaultOpen = true,
                .checked = s_tree.showCheckboxes ? &chkPostAll : nullptr,
                .indeterminate = s_tree.showCheckboxes && indPost
            })) {
                if (postItem.checkChanged) {
                    s_tree.chkAntiAlias = s_tree.chkBloom = s_tree.chkAo = chkPostAll;
                }
                if (useCols) {
                    guikit::Text(ui, "Render Graph", {.variant = guikit::UiVariant::Secondary});
                    guikit::Element(ui, {.badge = {"3 passes", guikit::UiVariant::Info}, .width = guikit::SizeMode::Fit});
                }

                static constexpr keel::StaticArray<keel::StringRef, 3> kAaModes = {"FXAA", "TAA", "MSAA"};
                if (auto aaItem = guikit::TreeItem(ui, {
                    .label = "Anti-Aliasing",
                    .description = s_tree.showDescriptions ? "Subpixel edge smoothing & temporal resolve" : "",
                    .detail = inlineDecor ? "4x MSAA" : "",
                    .icon = guikit::Icon::Play(),
                    .checked = s_tree.showCheckboxes ? &s_tree.chkAntiAlias : nullptr
                })) {
                    if (useCols) {
                        guikit::Text(ui, "4x MSAA", {.variant = guikit::UiVariant::Secondary});
                        guikit::Element(ui, {.badge = {"Auto", guikit::UiVariant::Primary}, .width = guikit::SizeMode::Fit});
                    }
                    if (s_tree.showEmbeddedControls) {
                        guikit::Select(ui, s_tree.aaModeIdx, kAaModes, {
                            .width = useCols ? guikit::AxisSize::Auto() : guikit::AxisSize::Fixed(100.0f)
                        });
                    }
                }

                if (auto bloomItem = guikit::TreeItem(ui, {
                    .label = "HDR Bloom & Glow",
                    .description = s_tree.showDescriptions ? "Dual-kawase threshold blur & lens flare pass" : "",
                    .detail = inlineDecor ? "Half-Res" : "",
                    .badge = inlineDecor ? guikit::UiText{"ACTIVE", guikit::StateColor::Success()} : guikit::UiText{},
                    .icon = guikit::Icon::Play(),
                    .checked = s_tree.showCheckboxes ? &s_tree.chkBloom : nullptr
                })) {
                    if (useCols) {
                        guikit::Text(ui, "Half-Res", {.variant = guikit::UiVariant::Secondary});
                        guikit::Element(ui, {.badge = {"ACTIVE", guikit::UiVariant::Success}, .width = guikit::SizeMode::Fit});
                    }
                    if (s_tree.showEmbeddedControls) {
                        // Multi-widget cell: Switch + Button grouped inside a single column cell
                        if (auto cell = bloomItem.Cell()) {
                            guikit::Switch(ui, s_tree.bloomEnabled);
                            if (guikit::Button(ui, {
                                .text = "Reset",
                                .variant = guikit::UiVariant::Secondary
                            })) {
                                ++s_tree.resetCount;
                            }
                        }
                    }
                }

                if (auto aoItem = guikit::TreeItem(ui, {
                    .label = "Ambient Occlusion",
                    .description = s_tree.showDescriptions ? "Screen-space contact shadows & bounce shading" : "",
                    .detail = inlineDecor ? "SSAO" : "",
                    .icon = guikit::Icon::Settings(),
                    .checked = s_tree.showCheckboxes ? &s_tree.chkAo : nullptr
                })) {
                    if (useCols) {
                        guikit::Text(ui, "SSAO", {.variant = guikit::UiVariant::Secondary});
                        guikit::Element(ui, {.badge = {"Screen", guikit::UiVariant::Default}, .width = guikit::SizeMode::Fit});
                    }
                    if (s_tree.showEmbeddedControls) {
                        guikit::Switch(ui, s_tree.aoEnabled);
                    }
                }
            }
        }

        // 2. Root Branch: Build & Assets
        if (auto assetsItem = guikit::TreeItem(ui, {
            .label = "Build & Assets",
            .description = s_tree.showDescriptions ? "Compiled GLSL shaders and runtime theme bundles" : "",
            .detail = inlineDecor ? "14.2 MB" : "",
            .icon = guikit::Icon::Folder(),
            .defaultOpen = true,
            .checked = s_tree.showCheckboxes ? &chkAssetsAll : nullptr,
            .indeterminate = s_tree.showCheckboxes && indAssets
        })) {
            if (assetsItem.checkChanged) {
                s_tree.chkShaderTonemap = s_tree.chkShaderBloom = s_tree.chkThemeJson = chkAssetsAll;
            }
            if (useCols) {
                guikit::Text(ui, "14.2 MB", {.variant = guikit::UiVariant::Secondary});
                guikit::Element(ui, {.badge = {"Bundle", guikit::UiVariant::Default}, .width = guikit::SizeMode::Fit});
            }

            if (auto shadersItem = guikit::TreeItem(ui, {
                .label = "Shaders (GLSL 330)",
                .description = s_tree.showDescriptions ? "Hardware-accelerated GPU post-process programs" : "",
                .badge = inlineDecor ? guikit::UiText{"2 files", guikit::StateColor::Muted()} : guikit::UiText{},
                .icon = guikit::Icon::Folder(),
                .defaultOpen = true,
                .checked = s_tree.showCheckboxes ? &chkShadersAll : nullptr,
                .indeterminate = s_tree.showCheckboxes && indShaders
            })) {
                if (shadersItem.checkChanged) {
                    s_tree.chkShaderTonemap = s_tree.chkShaderBloom = chkShadersAll;
                }
                if (useCols) {
                    guikit::Text(ui, "GLSL 330", {.variant = guikit::UiVariant::Secondary});
                    guikit::Element(ui, {.badge = {"2 files", guikit::UiVariant::Secondary}, .width = guikit::SizeMode::Fit});
                }

                if (auto s1 = guikit::TreeItem(ui, {
                    .label = "tonemap.frag",
                    .detail = inlineDecor ? "4.8 KB" : "",
                    .badge = inlineDecor ? guikit::UiText{"OK", guikit::StateColor::Success()} : guikit::UiText{},
                    .icon = guikit::Icon::Edit(),
                    .checked = s_tree.showCheckboxes ? &s_tree.chkShaderTonemap : nullptr
                })) {
                    if (useCols) {
                        guikit::Text(ui, "4.8 KB", {.variant = guikit::UiVariant::Secondary});
                        guikit::Element(ui, {.badge = {"OK", guikit::UiVariant::Success}, .width = guikit::SizeMode::Fit});
                    }
                }

                if (auto s2 = guikit::TreeItem(ui, {
                    .label = "bloom_blur.frag",
                    .detail = inlineDecor ? "6.1 KB" : "",
                    .badge = inlineDecor ? guikit::UiText{"OK", guikit::StateColor::Success()} : guikit::UiText{},
                    .icon = guikit::Icon::Edit(),
                    .checked = s_tree.showCheckboxes ? &s_tree.chkShaderBloom : nullptr
                })) {
                    if (useCols) {
                        guikit::Text(ui, "6.1 KB", {.variant = guikit::UiVariant::Secondary});
                        guikit::Element(ui, {.badge = {"OK", guikit::UiVariant::Success}, .width = guikit::SizeMode::Fit});
                    }
                }
            }

            if (auto themeItem = guikit::TreeItem(ui, {
                .label = "theme_tokens.json",
                .description = s_tree.showDescriptions ? "Design system color & typography specification" : "",
                .detail = inlineDecor ? "2.1 KB" : "",
                .icon = guikit::Icon::Settings(),
                .checked = s_tree.showCheckboxes ? &s_tree.chkThemeJson : nullptr
            })) {
                if (useCols) {
                    guikit::Text(ui, "2.1 KB", {.variant = guikit::UiVariant::Secondary});
                    guikit::Element(ui, {.badge = {"JSON", guikit::UiVariant::Info}, .width = guikit::SizeMode::Fit});
                }
            }
        }
    }
}

keel::String GenerateTreeViewCode(const DemoContext& ctx) {
    const auto sizePrefix = FormatUiSizePrefix(ctx.controlsSize);
    const bool useCols = s_tree.showColumns;
    keel::String out;

    if (useCols) {
        out.append(
            "static constexpr keel::StaticArray<guikit::TableColumn, 4> kTreeCols = {{\n"
            "    {.title = \"Node / Asset\", .flex = 1.6f,    .minWidth = 200.0f, .align = guikit::Align::Start},\n"
            "    {.title = \"Spec\",         .width = 104.0f, .minWidth = 72.0f,  .align = guikit::Align::Start},\n"
            "    {.title = \"Status\",       .width = 96.0f,  .minWidth = 72.0f,  .align = guikit::Align::Center},\n"
            "    {.title = \"Controls\",     .width = 176.0f, .minWidth = 120.0f, .align = guikit::Align::End}\n"
            "}};\n\n"
        );
    }

    keel::format_into(
        out,
        "if (auto tree = guikit::TreeView(ui, {{\n"
        "    .title = \"SCENE & ASSET HIERARCHY\",\n"
    );
    if (useCols) {
        keel::format_into(
            out,
            "    .columns = kTreeCols,\n"
            "    .gridLines = {},\n",
            s_tree.gridLines ? "true" : "false"
        );
    }
    keel::format_into(
        out,
        "    .guides = guikit::TreeGuides::{},\n"
        "    .selectionStyle = guikit::TreeSelectionStyle::{},\n"
        "    .color = guikit::StateColor::{}(),\n"
        "    .indent = {:.1f}f,\n"
        "    .striped = {},\n"
        "    .expandOnSelect = {},\n"
        "    .searchable = {},\n"
        "    .filterText = &filterQuery,\n"
        "    .showToolbar = {},\n"
        "    .selectedIndex = &selectedIndex,\n"
        "    .selectedId = &selectedId{}\n"
        "}})) {{\n",
        GuidesEnumName(s_tree.guides),
        s_tree.selectionStyle == guikit::TreeSelectionStyle::Indicator ? "Indicator" : "Pill",
        VariantName(ctx.btnVariant),
        s_tree.indent,
        s_tree.striped ? "true" : "false",
        s_tree.expandOnSelect ? "true" : "false",
        s_tree.searchable ? "true" : "false",
        s_tree.showToolbar ? "true" : "false",
        sizePrefix
    );

    const bool inlineDecor = !useCols && s_tree.showBadgesAndDetails;

    auto appendItemOpts = [&](keel::StringRef indent,
                              keel::StringRef varName,
                              keel::StringRef label,
                              keel::StringRef desc,
                              keel::StringRef detail,
                              keel::StringRef badgeExpr,
                              keel::StringRef iconExpr,
                              bool defaultOpen,
                              keel::StringRef checkedVar,
                              keel::StringRef indVar) {
        keel::format_into(out, "{}if (auto {} = guikit::TreeItem(ui, {{\n", indent, varName);
        keel::format_into(out, "{}    .label = \"{}\",\n", indent, label);
        if (s_tree.showDescriptions && !desc.empty()) {
            keel::format_into(out, "{}    .description = \"{}\",\n", indent, desc);
        }
        if (inlineDecor && !detail.empty()) {
            keel::format_into(out, "{}    .detail = \"{}\",\n", indent, detail);
        }
        if (inlineDecor && !badgeExpr.empty()) {
            keel::format_into(out, "{}    .badge = {},\n", indent, badgeExpr);
        }
        keel::format_into(out, "{}    .icon = {}", indent, iconExpr);
        if (defaultOpen) {
            keel::format_into(out, ",\n{}    .defaultOpen = true", indent);
        }
        if (s_tree.showCheckboxes && !checkedVar.empty()) {
            keel::format_into(out, ",\n{}    .checked = &{}", indent, checkedVar);
            if (!indVar.empty()) {
                keel::format_into(out, ",\n{}    .indeterminate = {}", indent, indVar);
            }
        }
        keel::format_into(out, "\n{}}})) {{\n", indent);
    };

    // 1. Root Branch: Scene Hierarchy
    appendItemOpts(
        "    ", "sceneItem", "Scene Hierarchy",
        "Active viewport nodes & real-time subsystems", "",
        "{\"5 nodes\", guikit::StateColor::Primary()}",
        "guikit::Icon::Folder()", true, "chkSceneAll", "indScene"
    );
    if (s_tree.showCheckboxes) {
        out.append("        if (sceneItem.checkChanged) {\n"
                   "            chkSun = chkFog = chkAntiAlias = chkBloom = chkAo = chkSceneAll;\n"
                   "        }\n");
    }
    if (useCols) {
        out.append("        guikit::Text(ui, \"Viewport\", {.variant = guikit::UiVariant::Secondary});\n"
                   "        guikit::Element(ui, {.badge = {\"5 nodes\", guikit::UiVariant::Primary}, .width = guikit::SizeMode::Fit});\n");
    }
    if (s_tree.showEmbeddedControls) {
        out.append("        guikit::Switch(ui, sceneActive);\n");
    }
    out.append("\n");

    // 1.1 Environment & Lighting
    appendItemOpts(
        "        ", "envItem", "Environment & Lighting",
        "PBR skybox, directional sun, and volumetric fog", "", "",
        "guikit::Icon::Eye()", true, "chkEnvAll", "indEnv"
    );
    if (s_tree.showCheckboxes) {
        out.append("            if (envItem.checkChanged) {\n"
                   "                chkSun = chkFog = chkEnvAll;\n"
                   "            }\n");
    }
    if (useCols) {
        out.append("            guikit::Text(ui, \"PBR Group\", {.variant = guikit::UiVariant::Secondary});\n"
                   "            guikit::Element(ui, {.badge = {\"2 passes\", guikit::UiVariant::Info}, .width = guikit::SizeMode::Fit});\n");
    }
    if (s_tree.showEmbeddedControls) {
        out.append(useCols
            ? "            guikit::Slider(ui, envExposure, 0.0f, 2.5f, {.valueFormat = \"%.2f EV\"});\n"
            : "            guikit::Slider(ui, envExposure, 0.0f, 2.5f, {.valueFormat = \"%.2f EV\", .width = 128.0f});\n");
    }
    out.append("\n");

    appendItemOpts(
        "            ", "sunItem", "Directional Sun",
        "Cascaded shadow map primary light source", "6500K",
        "{\"HDR\", guikit::StateColor::Warning()}",
        "guikit::Icon::Eye()", false, "chkSun", ""
    );
    if (useCols) {
        out.append("                guikit::Text(ui, \"6500K\", {.variant = guikit::UiVariant::Secondary});\n"
                   "                guikit::Element(ui, {.badge = {\"HDR\", guikit::UiVariant::Warning}, .width = guikit::SizeMode::Fit});\n");
    }
    if (s_tree.showEmbeddedControls) {
        out.append("                guikit::Switch(ui, sunEnabled);\n");
    }
    out.append("            }\n\n");

    appendItemOpts(
        "            ", "fogItem", "Volumetric Fog",
        "Exponential height fog with temporal reprojection", "Raymarched", "",
        "guikit::Icon::Eye()", false, "chkFog", ""
    );
    if (useCols) {
        out.append("                guikit::Text(ui, \"Raymarched\", {.variant = guikit::UiVariant::Secondary});\n"
                   "                guikit::Element(ui, {.badge = {\"Froxel\", guikit::UiVariant::Default}, .width = guikit::SizeMode::Fit});\n");
    }
    if (s_tree.showEmbeddedControls) {
        out.append(useCols
            ? "                guikit::Slider(ui, fogDensity, 0.0f, 100.0f, {.valueFormat = \"%.0f%%\"});\n"
            : "                guikit::Slider(ui, fogDensity, 0.0f, 100.0f, {.valueFormat = \"%.0f%%\", .width = 110.0f});\n");
    }
    out.append("            }\n");
    out.append("        }\n\n");

    // 1.2 Post-Processing
    appendItemOpts(
        "        ", "postItem", "Post-Processing",
        "Anti-aliasing, HDR bloom, and ambient occlusion", "",
        "{\"3 passes\", guikit::StateColor::Info()}",
        "guikit::Icon::Search()", true, "chkPostAll", "indPost"
    );
    if (s_tree.showCheckboxes) {
        out.append("            if (postItem.checkChanged) {\n"
                   "                chkAntiAlias = chkBloom = chkAo = chkPostAll;\n"
                   "            }\n");
    }
    if (useCols) {
        out.append("            guikit::Text(ui, \"Render Graph\", {.variant = guikit::UiVariant::Secondary});\n"
                   "            guikit::Element(ui, {.badge = {\"3 passes\", guikit::UiVariant::Info}, .width = guikit::SizeMode::Fit});\n");
    }

    appendItemOpts(
        "            ", "aaItem", "Anti-Aliasing",
        "Subpixel edge smoothing & temporal resolve", "4x MSAA", "",
        "guikit::Icon::Play()", false, "chkAntiAlias", ""
    );
    if (useCols) {
        out.append("                guikit::Text(ui, \"4x MSAA\", {.variant = guikit::UiVariant::Secondary});\n"
                   "                guikit::Element(ui, {.badge = {\"Auto\", guikit::UiVariant::Primary}, .width = guikit::SizeMode::Fit});\n");
    }
    if (s_tree.showEmbeddedControls) {
        out.append(useCols
            ? "                guikit::Select(ui, aaModeIdx, kAaModes);\n"
            : "                guikit::Select(ui, aaModeIdx, kAaModes, {.width = 100.0f});\n");
    }
    out.append("            }\n\n");

    appendItemOpts(
        "            ", "bloomItem", "HDR Bloom & Glow",
        "Dual-kawase threshold blur & lens flare pass", "Half-Res",
        "{\"ACTIVE\", guikit::StateColor::Success()}",
        "guikit::Icon::Play()", false, "chkBloom", ""
    );
    if (useCols) {
        out.append("                guikit::Text(ui, \"Half-Res\", {.variant = guikit::UiVariant::Secondary});\n"
                   "                guikit::Element(ui, {.badge = {\"ACTIVE\", guikit::UiVariant::Success}, .width = guikit::SizeMode::Fit});\n");
    }
    if (s_tree.showEmbeddedControls) {
        out.append("                // Multi-widget cell: Switch + Button inside a single column cell\n"
                   "                if (auto cell = bloomItem.Cell()) {\n"
                   "                    guikit::Switch(ui, bloomEnabled);\n"
                   "                    if (guikit::Button(ui, {.text = \"Reset\", .variant = guikit::UiVariant::Secondary})) {\n"
                   "                        ++resetCount;\n"
                   "                    }\n"
                   "                }\n");
    }
    out.append("            }\n\n");

    appendItemOpts(
        "            ", "aoItem", "Ambient Occlusion",
        "Screen-space contact shadows & bounce shading", "SSAO", "",
        "guikit::Icon::Settings()", false, "chkAo", ""
    );
    if (useCols) {
        out.append("                guikit::Text(ui, \"SSAO\", {.variant = guikit::UiVariant::Secondary});\n"
                   "                guikit::Element(ui, {.badge = {\"Screen\", guikit::UiVariant::Default}, .width = guikit::SizeMode::Fit});\n");
    }
    if (s_tree.showEmbeddedControls) {
        out.append("                guikit::Switch(ui, aoEnabled);\n");
    }
    out.append("            }\n");
    out.append("        }\n");
    out.append("    }\n\n");

    // 2. Root Branch: Build & Assets
    appendItemOpts(
        "    ", "assetsItem", "Build & Assets",
        "Compiled GLSL shaders and runtime theme bundles", "14.2 MB", "",
        "guikit::Icon::Folder()", true, "chkAssetsAll", "indAssets"
    );
    if (s_tree.showCheckboxes) {
        out.append("        if (assetsItem.checkChanged) {\n"
                   "            chkShaderTonemap = chkShaderBloom = chkThemeJson = chkAssetsAll;\n"
                   "        }\n");
    }
    if (useCols) {
        out.append("        guikit::Text(ui, \"14.2 MB\", {.variant = guikit::UiVariant::Secondary});\n"
                   "        guikit::Element(ui, {.badge = {\"Bundle\", guikit::UiVariant::Default}, .width = guikit::SizeMode::Fit});\n");
    }

    appendItemOpts(
        "        ", "shadersItem", "Shaders (GLSL 330)",
        "Hardware-accelerated GPU post-process programs", "",
        "{\"2 files\", guikit::StateColor::Muted()}",
        "guikit::Icon::Folder()", true, "chkShadersAll", "indShaders"
    );
    if (s_tree.showCheckboxes) {
        out.append("            if (shadersItem.checkChanged) {\n"
                   "                chkShaderTonemap = chkShaderBloom = chkShadersAll;\n"
                   "            }\n");
    }
    if (useCols) {
        out.append("            guikit::Text(ui, \"GLSL 330\", {.variant = guikit::UiVariant::Secondary});\n"
                   "            guikit::Element(ui, {.badge = {\"2 files\", guikit::UiVariant::Secondary}, .width = guikit::SizeMode::Fit});\n");
    }

    appendItemOpts(
        "            ", "s1", "tonemap.frag",
        "", "4.8 KB", "{\"OK\", guikit::StateColor::Success()}",
        "guikit::Icon::Edit()", false, "chkShaderTonemap", ""
    );
    if (useCols) {
        out.append("                guikit::Text(ui, \"4.8 KB\", {.variant = guikit::UiVariant::Secondary});\n"
                   "                guikit::Element(ui, {.badge = {\"OK\", guikit::UiVariant::Success}, .width = guikit::SizeMode::Fit});\n");
    }
    out.append("            }\n\n");

    appendItemOpts(
        "            ", "s2", "bloom_blur.frag",
        "", "6.1 KB", "{\"OK\", guikit::StateColor::Success()}",
        "guikit::Icon::Edit()", false, "chkShaderBloom", ""
    );
    if (useCols) {
        out.append("                guikit::Text(ui, \"6.1 KB\", {.variant = guikit::UiVariant::Secondary});\n"
                   "                guikit::Element(ui, {.badge = {\"OK\", guikit::UiVariant::Success}, .width = guikit::SizeMode::Fit});\n");
    }
    out.append("            }\n");
    out.append("        }\n\n");

    appendItemOpts(
        "        ", "themeItem", "theme_tokens.json",
        "Design system color & typography specification", "2.1 KB", "",
        "guikit::Icon::Settings()", false, "chkThemeJson", ""
    );
    if (useCols) {
        out.append("            guikit::Text(ui, \"2.1 KB\", {.variant = guikit::UiVariant::Secondary});\n"
                   "            guikit::Element(ui, {.badge = {\"JSON\", guikit::UiVariant::Info}, .width = guikit::SizeMode::Fit});\n");
    }
    out.append("        }\n");
    out.append("    }\n");
    out.append("}");

    return out;
}

} // namespace guikit::demo
