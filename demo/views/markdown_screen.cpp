#include "../demo_context.hpp"

namespace guikit::demo {

namespace {
struct MarkdownDemoState {
    // Zoom & Interactive
    keel::f32 zoom = 1.0f;
    bool enableSelection = true;
    bool interactiveTasks = true;
    keel::SmallString<128> searchQuery{};

    // Typography
    keel::f32 bodyFontSize = 13.5f;
    keel::f32 headingScale = 1.0f;
    keel::f32 inlineCodeFontSize = 12.5f;
    keel::f32 codeFontSize = 12.5f;
    keel::f32 lineHeightScale = 1.15f;

    // Page Layout & Spacing
    keel::f32 documentPadding = 8.0f;
    bool constrainContentWidth = false;
    keel::f32 maxContentWidth = 680.0f;
    keel::f32 blockGap = 12.0f;
    keel::f32 paragraphIndent = 0.0f;
    keel::f32 listIndent = 18.0f;
    keel::f32 blockPadding = 12.0f;
    keel::f32 blockRadius = 6.0f;
    bool headingDividers = true;

    // Code Block Styling (shared with CodeEditor)
    keel::usize codeSchemeIdx = 3; // Default to Dracula
    static constexpr keel::StaticArray<keel::StringRef, 8> codeSchemeLabels = {
        "Theme Default",
        "VS Code Dark+",
        "Monokai Pro",
        "Dracula",
        "One Dark Pro",
        "MATLAB Classic",
        "Solarized Dark",
        "GitHub Light"
    };
    bool codeLineNumbers = true;
    bool syntaxHighlight = true;
    bool showCopyButton = true;
    bool codeShowWhitespace = false;
    bool codeIndentGuides = true;

    // Embedded Images
    bool imageCheckerboard = true;
    keel::f32 imageMaxHeight = 200.0f;
    keel::f32 imageRadius = 8.0f;

    // Status feedback
    keel::String lastClickedLink{};
    keel::i32 lastToggledTask = -1;
    bool lastToggledChecked = false;
    keel::i32 searchMatches = 0;
};
MarkdownDemoState s_state;

keel::Optional<guikit::SyntaxPalette> ResolveCodePalette(keel::usize idx) noexcept {
    switch (idx) {
        case 1: return guikit::SyntaxPalette::VsCodeDark();
        case 2: return guikit::SyntaxPalette::Monokai();
        case 3: return guikit::SyntaxPalette::Dracula();
        case 4: return guikit::SyntaxPalette::OneDark();
        case 5: return guikit::SyntaxPalette::MatlabClassic();
        case 6: return guikit::SyntaxPalette::Solarized();
        case 7: return guikit::SyntaxPalette::GitHubLight();
        default: return keel::kNullOpt;
    }
}

keel::StringRef CodePaletteExpr(keel::usize idx) noexcept {
    switch (idx) {
        case 1: return "guikit::SyntaxPalette::VsCodeDark()";
        case 2: return "guikit::SyntaxPalette::Monokai()";
        case 3: return "guikit::SyntaxPalette::Dracula()";
        case 4: return "guikit::SyntaxPalette::OneDark()";
        case 5: return "guikit::SyntaxPalette::MatlabClassic()";
        case 6: return "guikit::SyntaxPalette::Solarized()";
        case 7: return "guikit::SyntaxPalette::GitHubLight()";
        default: return "keel::kNullOpt";
    }
}

constexpr keel::StringRef kDemoMarkdownDoc =
R"(# GuiKit Rich Markdown Engine

Welcome to **GuiKit's** native vector `guikit::Markdown` viewer! Everything is parsed into a lightweight cached AST and rendered at **60+ FPS** via *ThorVG* with crisp typography, **bold**, *italic*, ~~strikethrough~~, `inline code` pills, and interactive links like [https://github.com/guikit](https://github.com/guikit).

> [!TIP]
> Hold **Ctrl + MouseWheel** (or press `Ctrl+=` / `Ctrl+-` / `Ctrl+0`) to smoothly zoom, click and drag to **select text** across blocks, press `Ctrl+C` to copy, or press `Ctrl+F` to search!

## 1. Embedded Image Asset (`assets/images/lena.png`)

![Lena Standard Test Image (512x512 PNG — Embedded Native & WASM Asset)](assets/images/lena.png)

## 2. Multi-Language Code Blocks (Shared `CodeEditor` Highlighter)

```cpp
// C++20: Autonomous 2D World Camera & Embedded Assets
if (auto scene = guikit::Scene(ui, {.worldSize = {1600.0f, 1000.0f}})) {
    scene.DrawImage("assets/images/lena.png", {0, 0, 512, 512});
    const guikit::Rect visible = scene.GetVisibleWorldRect();
}
```

```matlab
% MATLAB: Signal envelope & spectral peak analysis
function [peakFreq, snrDb] = analyzeSpectrum(x, fs)
    N = length(x);
    X = abs(fft(x .* hann(N))) / N;
    [peakVal, idx] = max(X(1:floor(N/2)));
    peakFreq = (idx - 1) * (fs / N);
    snrDb = 20 * log10(peakVal / ( mean(X) + eps ));
end
```

```json
{
  "widget": "guikit::Markdown",
  "zoomRange": [0.5, 3.0],
  "sharedHighlighter": true,
  "features": ["Selection", "Copy", "Find", "TaskLists"]
}
```

## 3. Interactive Task Checklist (Click to Toggle!)

- [x] **Centralized `KeyAction` & `Keymap`** (`Copy`, `Find`, `SelectAll`, Arrow navigation)
- [x] **Zero-Duplication Code Blocks** reusing `SyntaxConfig::ForLanguage` & `SyntaxPalette`
- [x] **Smooth Zooming & Selection** across headings, paragraphs, tables, and code
- [ ] Try clicking this checkbox or customizing typography & spacing in the Inspector!

| Component | Layer | Coordinate Space | Responsibility |
| :--- | :---: | :---: | :--- |
| `Keymap` | Core | Input Dispatch | Configurable `KeyAction` bindings |
| `guikit::TextArea` | Widget | Line Virtualized | IDE Code Editor & Syntax Engine |
| `guikit::Markdown` | Widget | Flow Layout | Rich AST Document Viewer |

> [!WARNING]
> Hover over any `[link](...)` to preview its target URL tooltip, or click **Copy** on any code block above!
)";
} // namespace

void RenderMarkdownInspector(guikit::Context& ui, DemoContext&) {
    guikit::Text(ui, "Zoom, Selection & Search", {.role = guikit::TextRole::LabelMedium, .variant = guikit::UiVariant::Primary});
    if (auto el = guikit::Element(ui, {.label = "Document Zoom (Ctrl+Wheel)"})) {
        if (guikit::Slider(ui, s_state.zoom, 0.5f, 2.5f, {.valueFormat = "%.2fx"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Highlight Search Query (Ctrl+F)"})) {
        if (guikit::InputField(ui, s_state.searchQuery, {.placeholder = "Type to highlight matches..."}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Text Selection & Cursor (Ctrl+C)"})) {
        if (guikit::Switch(ui, s_state.enableSelection).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Interactive Task Checkboxes"})) {
        if (guikit::Switch(ui, s_state.interactiveTasks).changed) ui.InvalidateAll();
    }

    guikit::Text(ui, "Typography & Font Sizing", {.role = guikit::TextRole::LabelMedium, .variant = guikit::UiVariant::Primary});
    if (auto el = guikit::Element(ui, {.label = "Body Font Size"})) {
        if (guikit::Slider(ui, s_state.bodyFontSize, 10.0f, 22.0f, {.valueFormat = "%.1f pt"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Heading Scale (H1..H6)"})) {
        if (guikit::Slider(ui, s_state.headingScale, 0.7f, 1.6f, {.valueFormat = "%.2fx"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Inline Code Font Size (`code`)"})) {
        if (guikit::Slider(ui, s_state.inlineCodeFontSize, 9.0f, 20.0f, {.valueFormat = "%.1f pt"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Display Code Font Size (```)"})) {
        if (guikit::Slider(ui, s_state.codeFontSize, 9.0f, 20.0f, {.valueFormat = "%.1f pt"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Line Height Multiplier"})) {
        if (guikit::Slider(ui, s_state.lineHeightScale, 0.9f, 1.8f, {.valueFormat = "%.2fx"}).changed) ui.InvalidateAll();
    }

    guikit::Text(ui, "Page Layout, Indents & Spacing", {.role = guikit::TextRole::LabelMedium, .variant = guikit::UiVariant::Primary});
    if (auto el = guikit::Element(ui, {.label = "Document Padding"})) {
        if (guikit::Slider(ui, s_state.documentPadding, 0.0f, 40.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Constrain Reading Column Width"})) {
        if (guikit::Switch(ui, s_state.constrainContentWidth).changed) ui.InvalidateAll();
    }
    if (s_state.constrainContentWidth) {
        if (auto el = guikit::Element(ui, {.label = "Max Content Width"})) {
            if (guikit::Slider(ui, s_state.maxContentWidth, 420.0f, 960.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Block Vertical Gap"})) {
        if (guikit::Slider(ui, s_state.blockGap, 4.0f, 28.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Paragraph First-Line Indent"})) {
        if (guikit::Slider(ui, s_state.paragraphIndent, 0.0f, 36.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "List Level Indent"})) {
        if (guikit::Slider(ui, s_state.listIndent, 10.0f, 36.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Callout / Code Inner Padding"})) {
        if (guikit::Slider(ui, s_state.blockPadding, 6.0f, 24.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Block Corner Radius"})) {
        if (guikit::Slider(ui, s_state.blockRadius, 0.0f, 16.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "H1 / H2 Divider Lines"})) {
        if (guikit::Switch(ui, s_state.headingDividers).changed) ui.InvalidateAll();
    }

    guikit::Text(ui, "Code Blocks (Shared CodeEditor Engine)", {.role = guikit::TextRole::LabelMedium, .variant = guikit::UiVariant::Primary});
    if (auto el = guikit::Element(ui, {.label = "Code Color Scheme"})) {
        if (guikit::Select(ui, s_state.codeSchemeIdx, MarkdownDemoState::codeSchemeLabels).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Multi-Language Syntax Highlight"})) {
        if (guikit::Switch(ui, s_state.syntaxHighlight).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Line Numbers Gutter"})) {
        if (guikit::Switch(ui, s_state.codeLineNumbers).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Hidden Whitespace (Space/Tab)"})) {
        if (guikit::Switch(ui, s_state.codeShowWhitespace).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Indentation Guides"})) {
        if (guikit::Switch(ui, s_state.codeIndentGuides).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Interactive Copy Button"})) {
        if (guikit::Switch(ui, s_state.showCopyButton).changed) ui.InvalidateAll();
    }

    guikit::Text(ui, "Embedded Images", {.role = guikit::TextRole::LabelMedium, .variant = guikit::UiVariant::Primary});
    if (auto el = guikit::Element(ui, {.label = "Embedded Image Checkerboard"})) {
        if (guikit::Switch(ui, s_state.imageCheckerboard).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Image Max Height"})) {
        if (guikit::Slider(ui, s_state.imageMaxHeight, 100.0f, 320.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Image Corner Radius"})) {
        if (guikit::Slider(ui, s_state.imageRadius, 0.0f, 20.0f, {.valueFormat = "%.0f px"}).changed) ui.InvalidateAll();
    }

    if (!s_state.lastClickedLink.empty()) {
        guikit::Text(ui, ui.Format("Clicked link: {}", s_state.lastClickedLink.as_ref()), {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Info});
    }
    if (s_state.lastToggledTask >= 0) {
        guikit::Text(ui,
            ui.Format("Task #{} -> {}", s_state.lastToggledTask + 1, s_state.lastToggledChecked ? "Checked" : "Unchecked"),
            {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Success}
        );
    }
}

void RenderMarkdownStage(guikit::Context& ui, DemoContext& ctx) {
    if (auto mdScroll = guikit::ScrollArea(ui)) {
        const auto mdRes = guikit::Markdown(ui, kDemoMarkdownDoc, {
            .allowZoom = true,
            .zoom = &s_state.zoom,
            .bodyFontSize = s_state.bodyFontSize,
            .headingScale = s_state.headingScale,
            .inlineCodeFontSize = s_state.inlineCodeFontSize,
            .codeFontSize = s_state.codeFontSize,
            .lineHeightScale = s_state.lineHeightScale,
            .documentPadding = s_state.documentPadding,
            .maxContentWidth = s_state.constrainContentWidth ? keel::Optional<keel::f32>(s_state.maxContentWidth) : keel::kNullOpt,
            .blockGap = s_state.blockGap,
            .paragraphIndent = s_state.paragraphIndent,
            .listIndent = s_state.listIndent,
            .blockPadding = s_state.blockPadding,
            .blockRadius = s_state.blockRadius,
            .headingDividers = s_state.headingDividers,
            .codeLineNumbers = s_state.codeLineNumbers,
            .syntaxHighlight = s_state.syntaxHighlight,
            .showCopyButton = s_state.showCopyButton,
            .codeShowWhitespace = s_state.codeShowWhitespace,
            .codeIndentGuides = s_state.codeIndentGuides,
            .codePalette = ResolveCodePalette(s_state.codeSchemeIdx),
            .imageCheckerboard = s_state.imageCheckerboard,
            .imageMaxHeight = s_state.imageMaxHeight,
            .imageRadius = s_state.imageRadius,
            .enableSelection = s_state.enableSelection,
            .enableSearch = true,
            .interactiveTasks = s_state.interactiveTasks,
            .searchQuery = s_state.searchQuery.as_ref(),
            .size = ctx.controlsSize
        });
        s_state.searchMatches = mdRes.searchMatchCount;
        if (!mdRes.clickedLink.empty()) {
            s_state.lastClickedLink = keel::String::from(mdRes.clickedLink);
            ui.OpenUrl(mdRes.clickedLink);
        }
        if (mdRes.toggledTaskIndex >= 0) {
            s_state.lastToggledTask = mdRes.toggledTaskIndex;
            s_state.lastToggledChecked = mdRes.toggledTaskChecked;
        }
    }
}

keel::String GenerateMarkdownCode(const DemoContext& ctx) {
    const auto maxColStr = s_state.constrainContentWidth
        ? keel::format_fixed<32>("{:.0f}f", s_state.maxContentWidth)
        : keel::FixedString<32>("keel::kNullOpt");

    return keel::format(
        "if (auto scroll = guikit::ScrollArea(ui)) {{\n"
        "    auto res = guikit::Markdown(ui, markdownSource, {{\n"
        "        .allowZoom = true,\n"
        "        .zoom = &zoomFactor, // {:.2f}x\n"
        "        .bodyFontSize = {:.1f}f,\n"
        "        .headingScale = {:.2f}f,\n"
        "        .inlineCodeFontSize = {:.1f}f,\n"
        "        .codeFontSize = {:.1f}f,\n"
        "        .lineHeightScale = {:.2f}f,\n"
        "        .documentPadding = {:.0f}f,\n"
        "        .maxContentWidth = {},\n"
        "        .blockGap = {:.0f}f,\n"
        "        .paragraphIndent = {:.0f}f,\n"
        "        .listIndent = {:.0f}f,\n"
        "        .blockPadding = {:.0f}f,\n"
        "        .blockRadius = {:.0f}f,\n"
        "        .headingDividers = {},\n"
        "        .codeLineNumbers = {},\n"
        "        .syntaxHighlight = {},\n"
        "        .showCopyButton = {},\n"
        "        .codeShowWhitespace = {},\n"
        "        .codeIndentGuides = {},\n"
        "        .codePalette = {},\n"
        "        .imageCheckerboard = {},\n"
        "        .imageMaxHeight = {:.0f}f,\n"
        "        .imageRadius = {:.0f}f,\n"
        "        .enableSelection = {},\n"
        "        .enableSearch = true,\n"
        "        .interactiveTasks = {},\n"
        "        .searchQuery = \"{}\"{}\n"
        "    }});\n"
        "    if (!res.clickedLink.empty()) ui.OpenUrl(res.clickedLink);\n"
        "}}",
        s_state.zoom,
        s_state.bodyFontSize,
        s_state.headingScale,
        s_state.inlineCodeFontSize,
        s_state.codeFontSize,
        s_state.lineHeightScale,
        s_state.documentPadding,
        maxColStr,
        s_state.blockGap,
        s_state.paragraphIndent,
        s_state.listIndent,
        s_state.blockPadding,
        s_state.blockRadius,
        s_state.headingDividers ? "true" : "false",
        s_state.codeLineNumbers ? "true" : "false",
        s_state.syntaxHighlight ? "true" : "false",
        s_state.showCopyButton ? "true" : "false",
        s_state.codeShowWhitespace ? "true" : "false",
        s_state.codeIndentGuides ? "true" : "false",
        CodePaletteExpr(s_state.codeSchemeIdx),
        s_state.imageCheckerboard ? "true" : "false",
        s_state.imageMaxHeight,
        s_state.imageRadius,
        s_state.enableSelection ? "true" : "false",
        s_state.interactiveTasks ? "true" : "false",
        s_state.searchQuery.as_ref(),
        FormatUiSizePrefix(ctx.controlsSize)
    );
}

} // namespace guikit::demo
