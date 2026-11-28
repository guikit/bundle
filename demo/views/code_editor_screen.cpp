#include "../demo_context.hpp"
#include "keel/time.h"

namespace guikit::demo {

namespace {

struct CodeEditorState {
    bool showLabel = false;
    bool showDesc = false;
    bool lineNumbers = true;
    bool copyButton = true;
    bool monospace = true;
    bool wordWrap = false;
    bool readOnly = false;
    bool allowZoom = true;
    bool showMinimap = true;
    bool showStatusBar = true;
    bool codeFolding = true;
    bool showIndentGuides = true;
    bool showDebugToolbar = true;
    bool showInlineValues = true;
    bool showHoverDataTips = true;
    bool showDiagnostics = true;
    bool showWhitespace = false;
    keel::f32 zoom = 1.0f;

    keel::usize syntaxMode = 0; // 0=C++20, 1=MATLAB, 2=JSON, 3=Python, 4=Custom, 5=1M Lines (C++20), 6=10MB Single Line (JSON)
    static constexpr keel::StaticArray<keel::StringRef, 7> syntaxLabels = {
        "C++20",
        "MATLAB",
        "JSON",
        "Python",
        "Custom",
        "1M Lines (C++20 Stress)",
        "10 MB Single Line (JSON)"
    };
    keel::usize colorScheme = 0;
    static constexpr keel::StaticArray<keel::StringRef, 8> colorSchemeLabels = {
        "Theme Default",
        "VS Code Dark+",
        "Monokai Pro",
        "Dracula",
        "One Dark Pro",
        "MATLAB Classic",
        "Solarized Dark",
        "GitHub Light"
    };
    static constexpr keel::StaticArray<keel::StringRef, 5> customKeywords = {"rule", "when", "then", "emit", "alert"};
    static constexpr keel::StaticArray<keel::StringRef, 4> customTypes = {"Sensor", "Signal", "Threshold", "Millis"};
    keel::usize debugToolbarAlignIdx = 1;
    static constexpr keel::StaticArray<keel::StringRef, 3> debugToolbarAlignLabels = {
        "Left", "Center", "Right"
    };

    guikit::TextDocument editorDoc{};
    keel::f64 lastLoadMs = 0.0;
    keel::Array<keel::usize> breakpoints{};
    keel::usize executionLine = 14;
    keel::i32 stepCounter = 0;
    keel::FixedString<128> lastDebugEvent = "Paused at breakpoint";

    keel::Array<keel::String> ownedStrings{};
    keel::Array<CodeDiagnostic> diagnostics{};
    keel::Array<CodeInlineValue> inlineValues{};
    keel::Array<CodeDebugVar> debugVariables{};

    CodeEditorState() {
        LoadPreset(0);
    }

    keel::StringRef StoreString(keel::String s) {
        ownedStrings.push_back(keel::move(s));
        return ownedStrings.back().as_ref();
    }

    keel::usize FindLineContaining(keel::StringRef needle, keel::usize fallbackLine) const {
        if (needle.empty() || editorDoc.LineCount() > 1000u) return fallbackLine;
        const keel::usize nLines = editorDoc.LineCount();
        keel::String scratch;
        for (keel::usize i = 0u; i < nLines; ++i) {
            editorDoc.ReadLineSlice(i, 0u, 256u, scratch);
            if (scratch.as_ref().contains(needle)) {
                return i + 1u;
            }
        }
        return fallbackLine;
    }

    void RebuildDynamicDebugData() {
        ownedStrings.clear();
        ownedStrings.reserve(32);
        diagnostics.clear();
        inlineValues.clear();
        debugVariables.clear();

        if (syntaxMode == 0) { // C++20
            if (showDiagnostics) {
                diagnostics.push_back({
                    .line = 18,
                    .startCol = 16,
                    .endCol = 28,
                    .severity = DiagnosticSeverity::Warning,
                    .message = "narrowing conversion from 'double' to 'float' [-Wnarrowing]",
                    .code = "C4244",
                    .anchor = "coeffs[k] * gain"
                });
                diagnostics.push_back({
                    .line = 23,
                    .startCol = 8,
                    .endCol = 20,
                    .severity = DiagnosticSeverity::Info,
                    .message = "Consider constexpr evaluation for filter normalization",
                    .code = "modernize-constexpr",
                    .anchor = "static_cast<float>"
                });
            }
            if (showInlineValues && executionLine > 0) {
                const keel::f32 gainVal = 1.85f + 0.05f * keel::as<keel::f32>(stepCounter % 5);
                const keel::f32 rmsVal = 0.707f * gainVal;
                const keel::usize loopLine = FindLineContaining("for (size_t k", 16);
                const keel::usize accLine = FindLineContaining("float weighted", 18);
                inlineValues.push_back({11, "sampleRate = 48000.0 Hz, nyquist = 24000.0 Hz", UiVariant::Info, "nyquist"});
                inlineValues.push_back({14, StoreString(keel::format("gain = {:.2f}, cutoffHz = 2400.0f", gainVal)), UiVariant::Success, "energyAcc = 0.0"});
                if (executionLine >= loopLine) {
                    inlineValues.push_back({16, StoreString(keel::format("k = {}, omega = {:.4f} rad", stepCounter % 4, 0.3142f * ((stepCounter % 4) + 1))), UiVariant::Info, "for (size_t k"});
                }
                if (executionLine >= accLine) {
                    inlineValues.push_back({18, StoreString(keel::format("energyAcc = {:.4f}, rms = {:.3f}", rmsVal * 4.0f, rmsVal)), UiVariant::Warning, "float weighted"});
                }
            }
            if (showHoverDataTips) {
                const keel::f32 gainVal = 1.85f + 0.05f * keel::as<keel::f32>(stepCounter % 5);
                debugVariables.assign({
                    {"sampleRate", "48000.0", "double"},
                    {"cutoffHz", "2400.0f", "float"},
                    {"gain", StoreString(keel::format("{:.3f}f", gainVal)), "float"},
                    {"energyAcc", StoreString(keel::format("{:.5f}", 2.828f * gainVal)), "double"},
                    {"coeffs", "size=4 [0.25f, 0.50f, 0.25f, 0.0f]", "keel::ArrayRef<float>"},
                    {"nyquist", "24000.0", "const double"}
                });
            }
        } else if (syntaxMode == 1) { // MATLAB
            if (showDiagnostics) {
                diagnostics.push_back({
                    .line = 18,
                    .startCol = 8,
                    .endCol = 15,
                    .severity = DiagnosticSeverity::Warning,
                    .message = "Preallocate 'P_dB' before loop for faster MATLAB execution",
                    .code = "MLINT-AGROW",
                    .anchor = "P_dB(k)"
                });
            }
            if (showInlineValues && executionLine > 0) {
                const keel::usize fftLine = FindLineContaining("H = fft(", 15);
                const keel::usize snrLine = FindLineContaining("snr_dB = max(", 20);
                inlineValues.push_back({9, "Fs = 10000, N = 1024, f0 = 1250", UiVariant::Info, "N = length(x)"});
                inlineValues.push_back({13, "w = 1024x1 double [hann window]", UiVariant::Success, "w = hann("});
                if (executionLine >= fftLine) {
                    inlineValues.push_back({15, StoreString(keel::format("H = 1024x1 complex, peakBin = {}", 128 + (stepCounter % 3))), UiVariant::Info, "H = fft("});
                }
                if (executionLine >= snrLine) {
                    inlineValues.push_back({20, "snr_dB = +28.42 dB, peakFreq = 1250.0 Hz", UiVariant::Success, "snr_dB = max("});
                }
            }
            if (showHoverDataTips) {
                debugVariables.assign({
                    {"Fs", "10000", "double (1x1)"},
                    {"N", "1024", "double (1x1)"},
                    {"f0", "1250", "double (1x1)"},
                    {"x", "[0.000, 0.707, 1.000, 0.707, ...]", "double (1x1024)"},
                    {"w", "hann(1024) [0.000 .. 1.000]", "double (1024x1)"},
                    {"H", "max |H| = 512.0 + 0.0i", "complex double (1024x1)"},
                    {"P_dB", "[-42.1, -38.9, ..., 0.0 dB]", "double (1024x1)"},
                    {"snr_dB", "28.4215", "double (1x1)"}
                });
            }
        } else if (syntaxMode == 2) { // JSON
            if (showDiagnostics) {
                diagnostics.push_back({
                    .line = 15,
                    .startCol = 8,
                    .endCol = 20,
                    .severity = DiagnosticSeverity::Info,
                    .message = "Schema hint: maxThreads > 16 enables NUMA pool allocation",
                    .code = "JSON-SCHEMA",
                    .anchor = "\"maxThreads\""
                });
            }
            if (showInlineValues && executionLine > 0) {
                inlineValues.push_back({2, "schema = \"guikit.ide.v2\"", UiVariant::Info, "\"schema\""});
                inlineValues.push_back({15, "maxThreads = 32 (active)", UiVariant::Success, "\"maxThreads\""});
            }
            if (showHoverDataTips) {
                debugVariables.assign({
                    {"engine", "{ name: \"GuiKit Code Editor\", version: \"2.4.0\" }", "object"},
                    {"maxThreads", "32", "number"},
                    {"simd", "\"AVX-512\"", "string"},
                    {"languages", "[\"C++20\", \"MATLAB\", \"JSON\", \"Python\"]", "array[4]"}
                });
            }
        } else if (syntaxMode == 3) { // Python
            if (showDiagnostics) {
                diagnostics.push_back({
                    .line = 12,
                    .startCol = 12,
                    .endCol = 24,
                    .severity = DiagnosticSeverity::Warning,
                    .message = "Type hint expected 'float', got 'np.float64'",
                    .code = "mypy-type",
                    .anchor = "math.sin("
                });
            }
            if (showInlineValues && executionLine > 0) {
                inlineValues.push_back({6, "epochs = 12, lr = 0.0015", UiVariant::Info, "self.epochs"});
                inlineValues.push_back({11, StoreString(keel::format("idx = {}, loss = {:.5f}", (stepCounter % 12), 0.42f / ((stepCounter % 6) + 1))), UiVariant::Success, "for idx, w"});
            }
            if (showHoverDataTips) {
                debugVariables.assign({
                    {"epochs", "12", "int"},
                    {"lr", "0.0015", "float"},
                    {"loss", "0.08412", "float"},
                    {"weights", "device='cuda:0', dtype=float32", "Tensor[64, 64]"}
                });
            }
        }
    }

    void LoadPreset(keel::usize mode) {
        syntaxMode = mode;
        stepCounter = 0;
        const keel::u64 t0 = keel::monotonic_ns();
        if (mode == 0) { // C++20
            editorDoc.SetText(
                "#include <cmath>\n"
                "#include \"keel/keel.h\"\n\n"
                "constexpr double kPi = 3.14159265358979323846;\n"
                "// === DSP Biquad & Spectral Energy Kernel ===\n"
                "/* Multi-line block comment:\n"
                "   Try Ctrl+F (Find), Ctrl+H (Replace), Ctrl+G (Go to Line),\n"
                "   F9 (Breakpoint), F10 (Step Over), F5 (Continue), or click gutter chevrons! */\n"
                "namespace dsp::kernel {\n\n"
                "float ComputeBandEnergy(keel::ArrayRef<float> coeffs, double sampleRate) {\n"
                "    const double nyquist = sampleRate * 0.5;\n"
                "    float cutoffHz = 2400.0f;\n"
                "    float gain = 1.85f;\n"
                "    double energyAcc = 0.0;\n"
                "    for (size_t k = 0; k < coeffs.size(); ++k) {\n"
                "        const double omega = (2.0 * kPi * cutoffHz) / nyquist;\n"
                "        float weighted = coeffs[k] * gain * std::cos(omega * double(k));\n"
                "        energyAcc += weighted * weighted;\n"
                "    }\n"
                "    if (energyAcc > 1.0) {\n"
                "        gain *= 0.95f;\n"
                "        return static_cast<float>(std::sqrt(energyAcc));\n"
                "    }\n"
                "    return 0.0f;\n"
                "}\n\n"
                "} // namespace dsp::kernel\n"
            );
            breakpoints.assign({14, 18, 22});
            executionLine = 14;
            showDebugToolbar = true;
            lastDebugEvent = "Paused at dsp::kernel::ComputeBandEnergy (Ln 14)";
        } else if (mode == 1) { // MATLAB
            editorDoc.SetText(
                "%% MATLAB Spectral Beamforming & Welch PSD Analysis\n"
                "%{\n"
                "   Block comment: demonstrates MATLAB transpose (x') vs strings ('periodic'),\n"
                "   section headers (%%), and foldable function / for / if ... end blocks.\n"
                "%}\n"
                "function [P_dB, snr_dB] = analyzeSpectrum(x, Fs)\n"
                "    if nargin < 2\n"
                "        Fs = 10000;\n"
                "    end\n"
                "    N = length(x);\n"
                "    f0 = 1250;\n\n"
                "    %% Windowing & Hermitian Transpose Projection\n"
                "    w = hann(N, 'periodic');\n"
                "    x_win = (x(:) .* w)' * w; % Transpose x' vs string 'periodic'\n"
                "    H = fft(x(:) .* w, N);\n"
                "    for k = 1:N\n"
                "        magSq = abs(H(k)).^2 / (Fs * sum(w.^2));\n"
                "        P_dB(k) = 10 * log10(max(magSq, eps));\n"
                "    end\n"
                "    snr_dB = max(P_dB) - median(P_dB);\n"
                "    disp(['Estimated SNR: ', num2str(snr_dB), ' dB']);\n"
                "end\n"
            );
            breakpoints.assign({13, 15, 20});
            executionLine = 13;
            showDebugToolbar = true;
            lastDebugEvent = "Paused in analyzeSpectrum.m (Ln 13)";
        } else if (mode == 2) { // JSON
            editorDoc.SetText(
                "{\n"
                "    \"schema\": \"guikit.ide.v2\",\n"
                "    \"engine\": {\n"
                "        \"name\": \"GuiKit Code Editor\",\n"
                "        \"version\": \"2.4.0\",\n"
                "        \"hardwareAccelerated\": true\n"
                "    },\n"
                "    \"languages\": [\n"
                "        \"C++20\",\n"
                "        \"MATLAB\",\n"
                "        \"JSON\",\n"
                "        \"Python\"\n"
                "    ],\n"
                "    \"debugger\": {\n"
                "        \"maxThreads\": 32,\n"
                "        \"simd\": \"AVX-512\",\n"
                "        \"inlineValues\": true,\n"
                "        \"hoverDataTips\": true\n"
                "    }\n"
                "}\n"
            );
            breakpoints.assign({2, 15});
            executionLine = 15;
            showDebugToolbar = true;
            lastDebugEvent = "Inspecting JSON configuration node (Ln 15)";
        } else if (mode == 3) { // Python
            editorDoc.SetText(
                "# Python Neural Spectral Filter Trainer\n"
                "import math\n\n"
                "class SpectralTrainer:\n"
                "    def __init__(self, epochs: int = 12, lr: float = 0.0015):\n"
                "        self.epochs = epochs\n"
                "        self.lr = lr\n\n"
                "    def fit_epoch(self, weights: list[float]) -> float:\n"
                "        loss = 0.0\n"
                "        for idx, w in enumerate(weights):\n"
                "            grad = math.sin(w * (idx + 1)) * self.lr\n"
                "            loss += abs(grad) * 0.25\n"
                "        return round(loss, 5)\n"
            );
            breakpoints.assign({10, 12});
            executionLine = 11;
            showDebugToolbar = true;
            lastDebugEvent = "Paused in SpectralTrainer.fit_epoch (Ln 11)";
        } else if (mode == 4) { // Custom DSL
            editorDoc.SetText(
                "# Custom Rule Engine DSL (Declarative SyntaxConfig)\n"
                "rule OverheatProtection {\n"
                "    when Sensor.temp > 85.5 then {\n"
                "        emit Signal(\"EMERGENCY_STOP\", 100);\n"
                "        alert Threshold(85.5);\n"
                "    }\n"
                "}\n"
            );
            breakpoints.assign({3, 4});
            executionLine = 4;
            showDebugToolbar = true;
            lastDebugEvent = "Rule engine triggered on line 4";
        } else if (mode == 5) { // 1,000,000 Lines Stress Test (C++20)
            constexpr keel::usize kTargetLines = 1000000u;
            keel::String big;
            big.reserve(kTargetLines * 44u);
            big.append("// === GuiKit Piece-Tree Stress Test: 1,000,000 Lines ===\n");
            big.append("// Try Ctrl+G (e.g. 500800 or 999990), Ctrl+F, Word Wrap, Folding, or live typing!\n");
            keel::usize lineNo = 2u;
            auto appendUsize = [&](keel::usize v) {
                char numBuf[24];
                keel::usize d = 0u;
                do {
                    numBuf[d++] = keel::as<char>('0' + (v % 10u));
                    v /= 10u;
                } while (v > 0u);
                while (d > 0u) big.push_back(numBuf[--d]);
            };
            while (lineNo < kTargetLines) {
                if (lineNo == 500000u) {
                    big.append("/* === 1,500-Line Multiline Block Comment Checkpoint Test ===\n");
                    ++lineNo;
                    for (keel::usize k = 0u; k < 1498u && lineNo + 1u < kTargetLines; ++k, ++lineNo) {
                        big.append("   checkpointed comment body line ");
                        appendUsize(lineNo + 1u);
                        big.push_back('\n');
                    }
                    big.append("   End of 1,500-line multiline block comment */\n");
                    ++lineNo;
                    continue;
                }
                const keel::usize mod = lineNo % 5u;
                if (mod == 0u) {
                    big.append("inline keel::f64 ComputeStage_");
                    appendUsize(lineNo);
                    big.append("(keel::f64 x) noexcept {\n");
                } else if (mod == 1u) {
                    big.append("    const keel::f64 gain = 1.25 + 0.001 * ");
                    appendUsize(lineNo);
                    big.append(".0;\n");
                } else if (mod == 2u) {
                    big.append("    keel::f64 acc = x * gain + 42.0; // line ");
                    appendUsize(lineNo + 1u);
                    big.push_back('\n');
                } else if (mod == 3u) {
                    big.append("    return acc * 0.5;\n");
                } else {
                    if (lineNo + 1u < kTargetLines) big.append("}\n");
                    else big.append("}");
                }
                ++lineNo;
            }
            editorDoc.SetText(big.as_ref());
            breakpoints.clear();
            executionLine = 0;
            showDebugToolbar = false;
            lastDebugEvent = "1,000,000-line Piece-Tree document ready";
        } else { // 10 MB Single Line Stress Test (JSON)
            constexpr keel::usize kTargetBytes = 10u * 1024u * 1024u;
            keel::String hugeLine;
            hugeLine.reserve(kTargetBytes);
            hugeLine.append("{\"dataset\":\"guikit_10mb_single_line\",\"items\":[");
            const keel::StringRef item = "{\"id\":12345,\"sensor\":\"radar_ch0\",\"gain\":1.85,\"ok\":true},";
            while (hugeLine.size() + item.size() + 32u < kTargetBytes) {
                hugeLine.append(item);
            }
            hugeLine.append("{\"id\":999999,\"tail\":true}]}");
            editorDoc.SetText(hugeLine.as_ref());
            breakpoints.clear();
            executionLine = 0;
            showDebugToolbar = false;
            lastDebugEvent = "10 MB single-line JSON document ready";
        }
        lastLoadMs = keel::as<keel::f64>(keel::monotonic_ns() - t0) * 1e-6;
        RebuildDynamicDebugData();
    }

    keel::usize CountLines() const {
        return editorDoc.LineCount();
    }

    bool IsLineExecutable(keel::usize line1Based) const {
        if (line1Based == 0u || line1Based > editorDoc.LineCount()) return false;
        keel::String scratch;
        editorDoc.ReadLineSlice(line1Based - 1u, 0u, 128u, scratch);
        const keel::StringRef trimmed = scratch.as_ref().trim();
        if (trimmed.empty()) return false;
        if (trimmed.starts_with("//") || trimmed.starts_with("#") || trimmed.starts_with("%") ||
            trimmed.starts_with("/*") || trimmed.starts_with("*") || trimmed == "{" || trimmed == "}") {
            return false;
        }
        return true;
    }

    void HandleDebugAction(CodeDebugAction action) {
        const keel::usize totalLines = CountLines();
        if (action == CodeDebugAction::Stop) {
            executionLine = 0;
            lastDebugEvent = "Debugger stopped (Press F5 or Restart to begin)";
        } else if (action == CodeDebugAction::Restart) {
            stepCounter = 0;
            executionLine = !breakpoints.empty() ? breakpoints.front() : 1;
            lastDebugEvent = keel::format_fixed<128>("Debugger restarted at Ln {}", executionLine);
        } else if (action == CodeDebugAction::StepOver || action == CodeDebugAction::StepInto) {
            ++stepCounter;
            keel::usize nextLine = (executionLine == 0) ? 1 : (executionLine + 1);
            const keel::usize maxTries = keel::min<keel::usize>(totalLines, 512u);
            for (keel::usize tries = 0; tries < maxTries; ++tries) {
                if (nextLine > totalLines) nextLine = 1;
                if (IsLineExecutable(nextLine)) break;
                ++nextLine;
            }
            executionLine = nextLine;
            lastDebugEvent = keel::format_fixed<128>("Stepped to Ln {} (step #{})", executionLine, stepCounter);
        } else if (action == CodeDebugAction::StepOut) {
            ++stepCounter;
            executionLine = keel::min(totalLines, keel::max<keel::usize>(1, executionLine + 3));
            lastDebugEvent = keel::format_fixed<128>("Stepped out to Ln {}", executionLine);
        } else if (action == CodeDebugAction::Continue) {
            ++stepCounter;
            if (breakpoints.empty()) {
                executionLine = (executionLine == 0) ? 1 : 0;
                lastDebugEvent = (executionLine > 0) ? "Running — paused at entry" : "Program finished";
            } else {
                keel::usize foundBp = breakpoints.front();
                for (keel::usize bp : breakpoints) {
                    if (bp > executionLine) {
                        foundBp = bp;
                        break;
                    }
                }
                executionLine = foundBp;
                lastDebugEvent = keel::format_fixed<128>("Continued to breakpoint at Ln {}", executionLine);
            }
        }
        RebuildDynamicDebugData();
    }
};

CodeEditorState s_state;

} // namespace

void RenderCodeEditorInspector(guikit::Context& ui, DemoContext& ctx) {
    ctx.RenderFieldStyleControls(ui);
    if (auto el = guikit::Element(ui, {.label = "Language Preset"})) {
        if (guikit::Select(ui, s_state.syntaxMode, CodeEditorState::syntaxLabels).changed) {
            s_state.LoadPreset(s_state.syntaxMode);
            ui.InvalidateAll();
        }
    }

    if (s_state.syntaxMode >= 5u) {
        const keel::f64 sizeMb = keel::as<keel::f64>(s_state.editorDoc.ByteLength()) / (1024.0 * 1024.0);
        guikit::Text(
            ui,
            ui.Format(
                "Piece Tree: {} lines | {:.2f} MB | {} pieces | load {:.1f} ms",
                s_state.editorDoc.LineCount(),
                sizeMb,
                s_state.editorDoc.PieceCount(),
                s_state.lastLoadMs
            ),
            {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Info}
        );
    }

    if (auto el = guikit::Element(ui, {.label = "Syntax Color Scheme"})) {
        if (guikit::Select(ui, s_state.colorScheme, CodeEditorState::colorSchemeLabels).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Show Hidden Whitespace"})) {
        if (guikit::Switch(ui, s_state.showWhitespace).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Code Minimap"})) {
        if (guikit::Switch(ui, s_state.showMinimap).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Code Folding (Gutter)"})) {
        if (guikit::Switch(ui, s_state.codeFolding).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Indent Guides"})) {
        if (guikit::Switch(ui, s_state.showIndentGuides).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show IDE Status Bar"})) {
        if (guikit::Switch(ui, s_state.showStatusBar).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Floating Debug Toolbar"})) {
        if (guikit::Switch(ui, s_state.showDebugToolbar).changed) ui.InvalidateAll();
    }
    if (s_state.showDebugToolbar) {
        if (auto el = guikit::Element(ui, {.label = "Debug Toolbar Position"})) {
            if (guikit::Segmented(ui, s_state.debugToolbarAlignIdx, CodeEditorState::debugToolbarAlignLabels).changed) {
                ui.InvalidateAll();
            }
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Inline Debug Values"})) {
        if (guikit::Switch(ui, s_state.showInlineValues).changed) {
            s_state.RebuildDynamicDebugData();
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Hover Variable DataTips"})) {
        if (guikit::Switch(ui, s_state.showHoverDataTips).changed) {
            s_state.RebuildDynamicDebugData();
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Diagnostic Squiggles"})) {
        if (guikit::Switch(ui, s_state.showDiagnostics).changed) {
            s_state.RebuildDynamicDebugData();
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Line Numbers Gutter"})) {
        if (guikit::Switch(ui, s_state.lineNumbers).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Word Wrap"})) {
        if (guikit::Switch(ui, s_state.wordWrap).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Read-Only Mode"})) {
        if (guikit::Switch(ui, s_state.readOnly).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Editor Zoom"})) {
        if (guikit::Slider(ui, s_state.zoom, 0.5f, 2.5f, {.valueFormat = "%.2fx"}).changed) ui.InvalidateAll();
    }
}

void RenderCodeEditorStage(guikit::Context& ui, DemoContext& ctx) {
    guikit::SyntaxConfig synCfg = guikit::SyntaxConfig::Plain();
    if (s_state.syntaxMode == 0 || s_state.syntaxMode == 5) {
        synCfg = guikit::SyntaxConfig::Cpp();
    } else if (s_state.syntaxMode == 1) {
        synCfg = guikit::SyntaxConfig::Matlab();
    } else if (s_state.syntaxMode == 2 || s_state.syntaxMode == 6) {
        synCfg = guikit::SyntaxConfig::Json();
    } else if (s_state.syntaxMode == 3) {
        synCfg = guikit::SyntaxConfig::Python();
    } else {
        synCfg = guikit::SyntaxConfig{
            .enabled = true,
            .languageName = "RuleDSL",
            .keywords = CodeEditorState::customKeywords,
            .types = CodeEditorState::customTypes,
            .lineComment = "#",
            .blockCommentStart = "/*",
            .blockCommentEnd = "*/",
            .preprocessorPrefix = '\0',
            .highlightStrings = true,
            .highlightNumbers = true,
            .highlightFunctions = true,
            .highlightPunctuation = true
        };
    }

    keel::Optional<guikit::SyntaxPalette> synPalette = keel::kNullOpt;
    switch (s_state.colorScheme) {
        case 1: synPalette = guikit::SyntaxPalette::VsCodeDark(); break;
        case 2: synPalette = guikit::SyntaxPalette::Monokai(); break;
        case 3: synPalette = guikit::SyntaxPalette::Dracula(); break;
        case 4: synPalette = guikit::SyntaxPalette::OneDark(); break;
        case 5: synPalette = guikit::SyntaxPalette::MatlabClassic(); break;
        case 6: synPalette = guikit::SyntaxPalette::Solarized(); break;
        case 7: synPalette = guikit::SyntaxPalette::GitHubLight(); break;
        default: break;
    }

    const guikit::Align dbgAlign = (s_state.debugToolbarAlignIdx == 0)
        ? guikit::Align::Start
        : ((s_state.debugToolbarAlignIdx == 2) ? guikit::Align::End : guikit::Align::Center);

    guikit::TextAreaResponse resp{};
    if (auto el = guikit::Element(ui, {
        .label = s_state.showLabel ? "Code Editor & Debugger" : "",
        .description = s_state.showDesc ? s_state.lastDebugEvent.as_ref() : keel::StringRef{},
        .size = ctx.controlsSize
    })) {
        resp = guikit::CodeEditor(ui, s_state.editorDoc, {
            .autoHeight = false,
            .readOnly = s_state.readOnly,
            .showLineNumbers = s_state.lineNumbers,
            .showCopyButton = s_state.copyButton,
            .monospace = s_state.monospace,
            .wordWrap = s_state.wordWrap,
            .allowZoom = s_state.allowZoom,
            .zoom = s_state.zoom,
            .syntax = synCfg,
            .syntaxPalette = synPalette,
            .size = ctx.controlsSize,
            .height = guikit::SizeMode::Stretch,
            .style = ctx.ctrlFieldStyle,
            .status = ctx.ctrlFieldStatus,
            .background = guikit::StateColor{},
            .showMinimap = s_state.showMinimap,
            .showStatusBar = s_state.showStatusBar,
            .showIndentGuides = s_state.showIndentGuides,
            .showWhitespace = s_state.showWhitespace,
            .codeFolding = s_state.codeFolding,
            .autoCloseBrackets = true,
            .enableSearch = true,
            .highlightOccurrences = true,
            .matchBrackets = true,
            .enableBreakpoints = true,
            .breakpoints = &s_state.breakpoints,
            .executionLine = s_state.executionLine,
            .executionLinePtr = &s_state.executionLine,
            .showDebugToolbar = s_state.showDebugToolbar,
            .debugToolbarAlign = dbgAlign,
            .diagnostics = s_state.diagnostics,
            .inlineValues = s_state.inlineValues,
            .debugVariables = s_state.debugVariables
        });
    }

    if (resp.lineDelta != 0 && s_state.syntaxMode < 5u) {
        s_state.RebuildDynamicDebugData();
    }

    if (resp.debugAction != guikit::CodeDebugAction::None &&
        resp.debugAction != guikit::CodeDebugAction::ToggleBreakpoint) {
        s_state.HandleDebugAction(resp.debugAction);
        ui.InvalidateAll();
    }
}

keel::String GenerateCodeEditorCode(const DemoContext& ctx) {
    keel::StringRef synLine = "guikit::SyntaxConfig::Cpp()";
    if (s_state.syntaxMode == 1) synLine = "guikit::SyntaxConfig::Matlab()";
    else if (s_state.syntaxMode == 2 || s_state.syntaxMode == 6) synLine = "guikit::SyntaxConfig::Json()";
    else if (s_state.syntaxMode == 3) synLine = "guikit::SyntaxConfig::Python()";
    else if (s_state.syntaxMode == 4) synLine = "customSyntaxConfig";

    keel::StringRef palLine = "keel::kNullOpt";
    if (s_state.colorScheme == 1) palLine = "guikit::SyntaxPalette::VsCodeDark()";
    else if (s_state.colorScheme == 2) palLine = "guikit::SyntaxPalette::Monokai()";
    else if (s_state.colorScheme == 3) palLine = "guikit::SyntaxPalette::Dracula()";
    else if (s_state.colorScheme == 4) palLine = "guikit::SyntaxPalette::OneDark()";
    else if (s_state.colorScheme == 5) palLine = "guikit::SyntaxPalette::MatlabClassic()";
    else if (s_state.colorScheme == 6) palLine = "guikit::SyntaxPalette::Solarized()";
    else if (s_state.colorScheme == 7) palLine = "guikit::SyntaxPalette::GitHubLight()";

    const keel::StringRef dbgAlignStr = (s_state.debugToolbarAlignIdx == 0)
        ? "guikit::Align::Start"
        : ((s_state.debugToolbarAlignIdx == 2) ? "guikit::Align::End" : "guikit::Align::Center");

    const keel::String sizeLine = (ctx.controlsSize != guikit::UiSize::None && ctx.controlsSize != guikit::UiSize::Md)
        ? keel::format("    .size = guikit::UiSize::{},\n", SizeName(ctx.controlsSize))
        : keel::String{};
    const keel::String zoomLine = (keel::abs(s_state.zoom - 1.0f) > 0.01f)
        ? keel::format("    .zoom = {:.2f}f,\n", s_state.zoom)
        : keel::String{};
    const keel::String diagLine = s_state.showDiagnostics
        ? keel::String(",\n    .diagnostics = diagnostics")
        : keel::String{};
    const keel::String inlineLine = s_state.showInlineValues
        ? keel::String(",\n    .inlineValues = inlineValues")
        : keel::String{};
    const keel::String hoverLine = s_state.showHoverDataTips
        ? keel::String(",\n    .debugVariables = debugVariables")
        : keel::String{};

    return keel::format(
        "// Zero-copy Piece-Tree document (1M+ lines & multi-MB single lines):\n"
        "// guikit::TextDocument doc(sourceText);\n"
        "auto resp = guikit::CodeEditor(ui, doc, {{\n"
        "    .readOnly = {},\n"
        "    .showLineNumbers = {},\n"
        "    .wordWrap = {},\n"
        "{}"
        "    .syntax = {},\n"
        "    .syntaxPalette = {},\n"
        "{}"
        "    .height = guikit::SizeMode::Stretch,\n"
        "    .style = guikit::FieldStyle::{},\n"
        "    .status = guikit::UiVariant::{},\n"
        "    .showMinimap = {},\n"
        "    .showStatusBar = {},\n"
        "    .showIndentGuides = {},\n"
        "    .showWhitespace = {},\n"
        "    .codeFolding = {},\n"
        "    .breakpoints = &breakpoints,\n"
        "    .executionLine = debugExecLine,\n"
        "    .showDebugToolbar = {},\n"
        "    .debugToolbarAlign = {}{}{}{}\n"
        "}});\n"
        "if (resp.debugAction == guikit::CodeDebugAction::StepOver) {{\n"
        "    StepDebugger();\n"
        "}}",
        s_state.readOnly ? "true" : "false",
        s_state.lineNumbers ? "true" : "false",
        s_state.wordWrap ? "true" : "false",
        zoomLine,
        synLine,
        palLine,
        sizeLine,
        FieldStyleEnumName(ctx.ctrlFieldStyle),
        VariantName(ctx.ctrlFieldStatus),
        s_state.showMinimap ? "true" : "false",
        s_state.showStatusBar ? "true" : "false",
        s_state.showIndentGuides ? "true" : "false",
        s_state.showWhitespace ? "true" : "false",
        s_state.codeFolding ? "true" : "false",
        s_state.showDebugToolbar ? "true" : "false",
        dbgAlignStr,
        diagLine,
        inlineLine,
        hoverLine
    );
}

} // namespace guikit::demo
