#include "keel/math.h"
#include "bundle_screens.hpp"
#include "guikit/signal.hpp"

namespace guikit::demo {

namespace {

struct SignalDemoState {
    keel::usize presetIdx = 0;
    keel::f32 sampleRate = 48000.0f;
    keel::Array<keel::f32> samples;
    keel::Array<guikit::SignalMarker> markers;
    guikit::SignalSelection selection{};
    keel::f64 playheadSec = 0.42;
    bool livePlayhead = false;
    keel::u64 lastWallNs = 0;

    guikit::SignalPanelOrder panelOrder = guikit::SignalPanelOrder::WaveformTop;
    keel::f32 splitRatio = 0.45f;
    guikit::AmplitudeScale amplitudeScale = guikit::AmplitudeScale::dBFS;
    guikit::FrequencyScale frequencyScale = guikit::FrequencyScale::Linear;
    guikit::Colormap colormap = guikit::Colormap::Inferno;
    guikit::FftWindowType fftWindow = guikit::FftWindowType::Hann;
    keel::usize fftSizeIdx = 1; // 0=256, 1=512, 2=1024, 3=2048
    keel::f32 dbFloor = -92.0f;
    keel::f32 waveformGain = 1.0f;

    bool showOverviewBar = true;
    bool showTimeRuler = true;
    bool showAmplitudeRuler = true;
    bool showFrequencyRuler = true;
    bool showColorbar = true;
    bool showGrid = true;
    bool showRms = true;
    bool showCrosshair = true;

    bool requestResetView = false;
    keel::Optional<keel::f64> requestViewStartSec{};
    keel::Optional<keel::f64> requestViewEndSec{};
    keel::Optional<guikit::Vec2> requestFreqRangeHz{};

    guikit::SignalViewResponse lastResp{};
    bool initialized = false;

    static constexpr keel::StaticArray<keel::StringRef, 4> presetNames = {
        "1. Harmonic Speech & Formant Chirp (96k smp / 2.0s)",
        "2. LFM Radar & Hopping Telemetry (96k smp / 2.0s)",
        "3. Percussive Transients & Overdrive (96k smp / 2.0s)",
        "4. Ultra-Long Telemetry Stream (1.2M smp / 25.0s)"
    };

    static constexpr keel::StaticArray<keel::StringRef, 2> panelOrderNames = {
        "Waveform Top", "Spectrogram Top"
    };

    static constexpr keel::StaticArray<keel::StringRef, 2> ampScaleNames = {
        "dBFS", "Linear"
    };

    static constexpr keel::StaticArray<keel::StringRef, 2> freqScaleNames = {
        "Linear Hz", "Log Hz"
    };

    static constexpr keel::StaticArray<keel::StringRef, 4> windowNames = {
        "Hann", "Hamming", "Blackman-Harris", "Rectangular"
    };

    static constexpr keel::StaticArray<keel::StringRef, 4> fftSizeNames = {
        "256", "512", "1024", "2048"
    };

    static constexpr keel::StaticArray<keel::u32, 4> fftSizeValues = {
        256, 512, 1024, 2048
    };
};

SignalDemoState s_sig;

keel::u32 NextRng(keel::u32& state) noexcept {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

keel::f32 WhiteNoise(keel::u32& state) noexcept {
    return (keel::as<keel::f32>(NextRng(state) & 0x00FFFFFFu) / 8388608.0f) - 1.0f;
}

void BuildSignalPreset(SignalDemoState& st, keel::usize preset) {
    st.presetIdx = preset;
    st.sampleRate = 48000.0f;
    st.markers.clear();
    keel::u32 rng = 0x9E3779B9u + keel::as<keel::u32>(preset * 101u);

    if (preset == 0) {
        // 2.0 seconds @ 48 kHz = 96,000 samples:
        // Segment A (0.0 - 0.55s): Voiced harmonic series with formant sweep
        // Segment B (0.55 - 0.85s): High-frequency sibilant burst + FM bell
        // Segment C (0.85 - 1.45s): Multi-harmonic exponential & linear chirp
        // Segment D (1.45 - 2.00s): Vibrato chord + decaying echoes
        const keel::usize N = 96000;
        st.samples.assign(N, 0.0f);
        keel::f64 phaseF0 = 0.0;
        keel::f64 phaseChirp = 0.0;
        for (keel::usize i = 0; i < N; ++i) {
            const keel::f32 t = keel::as<keel::f32>(i) / st.sampleRate;
            keel::f32 v = 0.002f * WhiteNoise(rng);

            if (t < 0.55f) {
                const keel::f32 env = keel::sin(keel::kPiF * (t / 0.55f));
                const keel::f32 f0 = 220.0f + 45.0f * keel::sin(keel::kTauF * 3.2f * t);
                phaseF0 += keel::as<keel::f64>(keel::kTauF * f0 / st.sampleRate);
                const keel::f32 f1Center = 900.0f + 700.0f * (t / 0.55f);
                const keel::f32 f2Center = 2400.0f + 1600.0f * keel::sin(keel::kPiF * (t / 0.55f));
                keel::f32 sum = 0.0f;
                for (keel::i32 h = 1; h <= 28; ++h) {
                    const keel::f32 fh = f0 * keel::as<keel::f32>(h);
                    if (fh >= st.sampleRate * 0.48f) break;
                    const keel::f32 d1 = (fh - f1Center) / 380.0f;
                    const keel::f32 d2 = (fh - f2Center) / 520.0f;
                    const keel::f32 formant = 0.75f * keel::exp(-0.5f * d1 * d1) +
                                          0.65f * keel::exp(-0.5f * d2 * d2) +
                                          0.18f / keel::as<keel::f32>(h);
                    sum += formant * keel::sin(keel::as<keel::f32>(phaseF0 * h));
                }
                v += 0.48f * env * sum;
            } else if (t < 0.85f) {
                const keel::f32 localT = (t - 0.55f) / 0.30f;
                const keel::f32 env = keel::sin(keel::kPiF * localT);
                const keel::f32 carrier = keel::sin(keel::kTauF * 6800.0f * t + 4.5f * keel::sin(keel::kTauF * 1100.0f * t));
                const keel::f32 sub = keel::sin(keel::kTauF * 1250.0f * t) * keel::exp(-localT * 4.0f);
                v += env * (0.35f * WhiteNoise(rng) * keel::sin(keel::kTauF * 9500.0f * t) + 0.45f * carrier + 0.30f * sub);
            } else if (t < 1.48f) {
                const keel::f32 localT = (t - 0.85f) / 0.63f;
                const keel::f32 env = keel::sin(keel::kPiF * localT);
                const keel::f32 instFreq = 350.0f + 9200.0f * localT * localT;
                phaseChirp += keel::as<keel::f64>(keel::kTauF * instFreq / st.sampleRate);
                v += env * (0.58f * keel::sin(keel::as<keel::f32>(phaseChirp)) +
                            0.26f * keel::sin(keel::as<keel::f32>(phaseChirp * 2.0)) +
                            0.12f * keel::sin(keel::as<keel::f32>(phaseChirp * 3.0)));
            } else {
                const keel::f32 localT = (t - 1.48f) / 0.52f;
                const keel::f32 env = keel::exp(-localT * 3.6f) * keel::min(1.0f, localT * 25.0f);
                const keel::f32 vib = 1.0f + 0.015f * keel::sin(keel::kTauF * 6.5f * t);
                const keel::f32 c1 = keel::sin(keel::kTauF * 523.25f * vib * t);
                const keel::f32 c2 = keel::sin(keel::kTauF * 659.25f * vib * t);
                const keel::f32 c3 = keel::sin(keel::kTauF * 783.99f * vib * t);
                const keel::f32 c4 = keel::sin(keel::kTauF * 1567.98f * vib * t);
                const keel::f32 c5 = keel::sin(keel::kTauF * 3135.96f * vib * t);
                v += 0.85f * env * (0.32f * c1 + 0.25f * c2 + 0.20f * c3 + 0.14f * c4 + 0.09f * c5);
            }
            st.samples[i] = keel::clamp(v, -0.98f, 0.98f);
        }

        st.markers.push_back({0.04, "Formant Onset", guikit::Color{56, 189, 248, 255}});
        st.markers.push_back({0.55, "FM Sibilant",   guikit::Color{245, 158, 11, 255}});
        st.markers.push_back({0.85, "Harmonic Chirp",guikit::Color{168, 85, 247, 255}});
        st.markers.push_back({1.48, "Vibrato Chord", guikit::Color{34, 197, 94, 255}});
        st.selection = {
            .active = true,
            .startTimeSec = 0.88,
            .endTimeSec = 1.38,
            .hasSpectralBox = true,
            .lowFreqHz = 1200.0f,
            .highFreqHz = 14800.0f
        };
        st.playheadSec = 0.42;
    } else if (preset == 1) {
        // Preset 1: LFM Radar & Frequency-Hopping Telemetry (96,000 samples / 2.0s)
        const keel::usize N = 96000;
        st.samples.assign(N, 0.0f);
        keel::f64 phase = 0.0;
        static constexpr keel::StaticArray<keel::f32, 8> hopFreqs = {
            1800.0f, 6400.0f, 3600.0f, 11200.0f, 4800.0f, 15400.0f, 8200.0f, 19200.0f
        };
        for (keel::usize i = 0; i < N; ++i) {
            const keel::f32 t = keel::as<keel::f32>(i) / st.sampleRate;
            keel::f32 v = 0.008f * WhiteNoise(rng);
            // Continuous pilot beacon at 1.0 kHz
            v += 0.07f * keel::sin(keel::kTauF * 1000.0f * t);

            if (t >= 0.06f && t < 0.46f) {
                // Up-Chirp 1.5 kHz -> 20 kHz
                const keel::f32 u = (t - 0.06f) / 0.40f;
                const keel::f32 win = keel::sin(keel::kPiF * u);
                const keel::f32 f = 1500.0f + 18500.0f * u;
                phase += keel::as<keel::f64>(keel::kTauF * f / st.sampleRate);
                v += 0.78f * win * keel::sin(keel::as<keel::f32>(phase));
            } else if (t >= 0.54f && t < 0.94f) {
                // Down-Chirp 21 kHz -> 2 kHz + Simultaneous Counter-Chirp
                const keel::f32 u = (t - 0.54f) / 0.40f;
                const keel::f32 win = keel::sin(keel::kPiF * u);
                const keel::f32 fDown = 21000.0f - 19000.0f * u;
                const keel::f32 fUp = 2000.0f + 14000.0f * u;
                phase += keel::as<keel::f64>(keel::kTauF * fDown / st.sampleRate);
                v += win * (0.52f * keel::sin(keel::as<keel::f32>(phase)) +
                            0.34f * keel::sin(keel::kTauF * fUp * t));
            } else if (t >= 1.02f && t < 1.94f) {
                // Stepped FHSS Telemetry Hops
                const keel::f32 hopDur = (1.94f - 1.02f) / keel::as<keel::f32>(hopFreqs.size());
                const keel::usize hopIdx = keel::min(hopFreqs.size() - 1, keel::as<keel::usize>((t - 1.02f) / hopDur));
                const keel::f32 hopLocal = keel::fmod(t - 1.02f, hopDur) / hopDur;
                const keel::f32 env = keel::sin(keel::kPiF * hopLocal);
                const keel::f32 f = hopFreqs[hopIdx];
                v += 0.72f * env * (keel::sin(keel::kTauF * f * t) + 0.25f * keel::sin(keel::kTauF * (f + 950.0f) * t));
            }
            st.samples[i] = keel::clamp(v, -0.98f, 0.98f);
        }

        st.markers.push_back({0.06, "LFM Up-Chirp",  guikit::Color{56, 189, 248, 255}});
        st.markers.push_back({0.54, "Dual X-Chirp",  guikit::Color{245, 158, 11, 255}});
        st.markers.push_back({1.02, "FHSS Hopping",  guikit::Color{34, 197, 94, 255}});
        st.selection = {
            .active = true,
            .startTimeSec = 0.54,
            .endTimeSec = 0.94,
            .hasSpectralBox = true,
            .lowFreqHz = 2000.0f,
            .highFreqHz = 21000.0f
        };
        st.playheadSec = 0.74;
    } else if (preset == 2) {
        // Preset 2: Percussive Transients & Overdrive Saturation (96,000 samples / 2.0s)
        const keel::usize N = 96000;
        st.samples.assign(N, 0.0f);
        static constexpr keel::StaticArray<keel::f32, 6> hitTimes = {0.08f, 0.38f, 0.68f, 0.98f, 1.28f, 1.58f};
        for (keel::usize i = 0; i < N; ++i) {
            const keel::f32 t = keel::as<keel::f32>(i) / st.sampleRate;
            keel::f32 v = 0.003f * WhiteNoise(rng);
            for (keel::usize h = 0; h < hitTimes.size(); ++h) {
                const keel::f32 dt = t - hitTimes[h];
                if (dt >= 0.0f && dt < 0.28f) {
                    const keel::f32 pitchDrop = 140.0f + 580.0f * keel::exp(-dt * 42.0f);
                    const keel::f32 bodyEnv = keel::exp(-dt * 14.0f);
                    const keel::f32 clickEnv = keel::exp(-dt * 140.0f);
                    const keel::f32 body = keel::sin(keel::kTauF * pitchDrop * dt);
                    const keel::f32 snare = (h % 2 == 1) ? (WhiteNoise(rng) * keel::exp(-dt * 28.0f) * 0.65f) : 0.0f;
                    const keel::f32 metallic = keel::sin(keel::kTauF * (3400.0f + 900.0f * keel::as<keel::f32>(h)) * dt) *
                                           keel::sin(keel::kTauF * 5120.0f * dt) * keel::exp(-dt * 22.0f);
                    keel::f32 raw = 0.85f * bodyEnv * body + 0.55f * clickEnv * WhiteNoise(rng) + snare + 0.35f * metallic;
                    // Intentional warm soft-clipping on 4th & 6th hits
                    if (h == 3 || h == 5) {
                        raw = keel::tanh(raw * 2.8f);
                    }
                    v += raw;
                }
            }
            st.samples[i] = keel::clamp(v, -0.99f, 0.99f);
        }

        st.markers.push_back({0.08, "Kick Transient", guikit::Color{56, 189, 248, 255}});
        st.markers.push_back({0.38, "Snare Burst",    guikit::Color{245, 158, 11, 255}});
        st.markers.push_back({0.98, "Clipped Hit",    guikit::Color{239, 68, 68, 255}});
        st.selection = {
            .active = true,
            .startTimeSec = 0.36,
            .endTimeSec = 0.60,
            .hasSpectralBox = true,
            .lowFreqHz = 800.0f,
            .highFreqHz = 18000.0f
        };
        st.playheadSec = 0.38;
    } else {
        // Preset 3: Ultra-Long 1,200,000 samples (25.0 seconds @ 48 kHz)
        // Demonstrates O(W) multi-resolution Mipmap Pyramid + on-demand STFT
        const keel::usize N = 1200000;
        st.samples.assign(N, 0.0f);
        keel::f64 phase = 0.0;
        for (keel::usize i = 0; i < N; ++i) {
            const keel::f32 t = keel::as<keel::f32>(i) / st.sampleRate;
            const keel::f32 macroEnv = 0.55f + 0.40f * keel::sin(keel::kTauF * 0.18f * t);
            const keel::f32 sweepFreq = 400.0f + 8500.0f * (0.5f + 0.5f * keel::sin(keel::kTauF * 0.12f * t));
            phase += keel::as<keel::f64>(keel::kTauF * sweepFreq / st.sampleRate);

            const keel::f32 pulseMod = keel::max(0.0f, keel::sin(keel::kTauF * 1.5f * t));
            const keel::f32 harmonic1 = keel::sin(keel::as<keel::f32>(phase));
            const keel::f32 harmonic2 = 0.45f * keel::sin(keel::as<keel::f32>(phase * 2.0));
            const keel::f32 carrierHi = 0.28f * pulseMod * keel::sin(keel::kTauF * (12000.0f + 4000.0f * keel::cos(keel::kTauF * 0.4f * t)) * t);
            const keel::f32 noise = 0.015f * WhiteNoise(rng);

            const keel::f32 v = macroEnv * (0.55f * harmonic1 + harmonic2) + carrierHi + noise;
            st.samples[i] = keel::clamp(v, -0.98f, 0.98f);
        }

        st.markers.push_back({2.50,  "Sweep Crest #1", guikit::Color{56, 189, 248, 255}});
        st.markers.push_back({8.33,  "Mid Telemetry",  guikit::Color{245, 158, 11, 255}});
        st.markers.push_back({16.60, "Burst Window",   guikit::Color{168, 85, 247, 255}});
        st.markers.push_back({22.00, "Tail Marker",    guikit::Color{34, 197, 94, 255}});
        st.selection = {
            .active = true,
            .startTimeSec = 6.0,
            .endTimeSec = 11.5,
            .hasSpectralBox = true,
            .lowFreqHz = 1500.0f,
            .highFreqHz = 16500.0f
        };
        st.playheadSec = 8.33;
    }
}

void EnsureSignalInitialized() {
    if (!s_sig.initialized) {
        s_sig.initialized = true;
        s_sig.lastWallNs = keel::monotonic_ns();
        BuildSignalPreset(s_sig, s_sig.presetIdx);
    }
}

void StepSignalPlayback(SignalDemoState& st) {
    const keel::u64 nowNs = keel::monotonic_ns();
    keel::f64 dt = keel::as<keel::f64>(nowNs - st.lastWallNs) * 1e-9;
    st.lastWallNs = nowNs;
    dt = keel::clamp(dt, 0.0, 0.08);
    if (!st.livePlayhead || st.samples.empty()) {
        return;
    }
    if (dt < 0.004) {
        dt = 0.016;
    }
    const keel::f64 totalDur = keel::as<keel::f64>(st.samples.size()) / keel::as<keel::f64>(keel::max(1.0f, st.sampleRate));
    const keel::f64 vStart = (st.lastResp.viewEndSec > st.lastResp.viewStartSec) ? st.lastResp.viewStartSec : 0.0;
    const keel::f64 vEnd   = (st.lastResp.viewEndSec > st.lastResp.viewStartSec) ? st.lastResp.viewEndSec : totalDur;
    const keel::f64 span   = keel::max(1e-4, vEnd - vStart);
    st.playheadSec += dt;
    if (st.playheadSec > vEnd || st.playheadSec < vStart) {
        st.playheadSec = vStart + keel::fmod(keel::max(0.0, st.playheadSec - vStart), span);
    }
}

} // namespace

void RenderSignalInspector(guikit::Context& ui, DemoContext&) {
    EnsureSignalInitialized();

    if (auto el = ui.Element({
        .label = "Live Playhead Sweep",
        .description = "Animate playhead at 60 Hz across active viewport"
    })) {
        ui.Switch(s_sig.livePlayhead);
    }

    if (auto el = ui.Element({.label = "Test Signal Generator"})) {
        if (ui.Select(s_sig.presetIdx, SignalDemoState::presetNames, {
            .prefix = guikit::Icon::Play()
        }).changed) {
            BuildSignalPreset(s_sig, s_sig.presetIdx);
            s_sig.requestResetView = true;
        }
    }

    if (auto el = ui.Element({.label = "Panel Stack Order"})) {
        ui.Segmented(s_sig.panelOrder, SignalDemoState::panelOrderNames);
    }

    keel::f32 splitPct = s_sig.splitRatio * 100.0f;
    if (auto el = ui.Element({.label = "Top / Bottom Split Ratio"})) {
        if (ui.Slider(splitPct, 0.0f, 100.0f, {
            .valueFormat = "%.0f %%",
            .step = 1.0f
        }).changed) {
            s_sig.splitRatio = splitPct / 100.0f;
        }
    }

    if (auto el = ui.Element({.label = "Waveform Amplitude Scale"})) {
        ui.Segmented(s_sig.amplitudeScale, SignalDemoState::ampScaleNames);
    }

    if (auto el = ui.Element({.label = "Spectrogram Frequency Scale"})) {
        ui.Segmented(s_sig.frequencyScale, SignalDemoState::freqScaleNames);
    }

    keel::usize cmapIdx = keel::as<keel::usize>(s_sig.colormap);
    if (auto el = ui.Element({.label = "Spectrogram Colormap"})) {
        if (ui.Select(cmapIdx, guikit::GetAllColormapNames()).changed) {
            s_sig.colormap = static_cast<guikit::Colormap>(cmapIdx);
        }
    }

    if (auto el = ui.Element({.label = "STFT Window Size (Bins)"})) {
        ui.StepSlider(s_sig.fftSizeIdx, SignalDemoState::fftSizeNames, {
            .showTicks = true,
            .showLabels = true
        });
    }

    if (auto el = ui.Element({.label = "FFT Window Function"})) {
        ui.Select(s_sig.fftWindow, SignalDemoState::windowNames);
    }

    if (auto el = ui.Element({.label = "Spectrogram Noise Floor"})) {
        ui.Slider(s_sig.dbFloor, -120.0f, -36.0f, {
            .valueFormat = "%.0f dBFS",
            .step = 2.0f
        });
    }

    if (auto el = ui.Element({.label = "Waveform Vertical Gain"})) {
        ui.Slider(s_sig.waveformGain, 0.25f, 8.0f, {
            .valueFormat = "%.2fx",
            .step = 0.05f
        });
    }

    if (auto el = ui.Element({.label = "Overview Minimap Navigator"})) {
        ui.Switch(s_sig.showOverviewBar);
    }
    if (auto el = ui.Element({.label = "Adaptive Time & Cue Ruler"})) {
        ui.Switch(s_sig.showTimeRuler);
    }
    if (auto el = ui.Element({.label = "Waveform Amplitude Ruler"})) {
        ui.Switch(s_sig.showAmplitudeRuler);
    }
    if (auto el = ui.Element({.label = "Spectrogram Frequency Ruler"})) {
        ui.Switch(s_sig.showFrequencyRuler);
    }
    if (auto el = ui.Element({.label = "Right dBFS Heatmap Legend"})) {
        ui.Switch(s_sig.showColorbar);
    }
    if (auto el = ui.Element({.label = "Dual Peak + RMS Envelope"})) {
        ui.Switch(s_sig.showRms);
    }
    if (auto el = ui.Element({.label = "Synchronized Overlay Grid"})) {
        ui.Switch(s_sig.showGrid);
    }
    if (auto el = ui.Element({.label = "Crosshair & Live Readout HUD"})) {
        ui.Switch(s_sig.showCrosshair);
    }

    if (auto btnRow = ui.Row({.gap = 8.0f})) {
        if (ui.Button({
            .text = "Zoom Selection",
            .icon = guikit::Icon::Search(),
            .disabled = !s_sig.selection.active || s_sig.selection.DurationSec() <= 1e-5,
            .size = guikit::UiSize::Sm,
            .width = guikit::SizeMode::Stretch,
            .color = guikit::StateColor::Primary()
        })) {
            s_sig.requestViewStartSec = s_sig.selection.MinTimeSec();
            s_sig.requestViewEndSec = s_sig.selection.MaxTimeSec();
            if (s_sig.selection.hasSpectralBox && s_sig.selection.BandwidthHz() >= 50.0f) {
                s_sig.requestFreqRangeHz = guikit::Vec2{s_sig.selection.MinFreqHz(), s_sig.selection.MaxFreqHz()};
            }
        }
        if (ui.Button({
            .text = "Reset View",
            .icon = guikit::Icon::Refresh(),
            .size = guikit::UiSize::Sm,
            .width = guikit::SizeMode::Stretch,
            .color = guikit::StateColor::Secondary()
        })) {
            s_sig.requestResetView = true;
            s_sig.waveformGain = 1.0f;
        }
    }

    if (auto telCard = ui.Card({
        .gap = 4.0f,
        .padding = guikit::Padding::All(8.0f),
        .border = {.radius = 6.0f}
    })) {
        ui.Text(
            ui.Format("Signal: {} smp ({:.2f} s @ {:.0f} Hz)",
                      s_sig.lastResp.totalSamples,
                      s_sig.lastResp.totalDurationSec,
                      s_sig.sampleRate),
            {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Primary}
        );
        ui.Text(
            ui.Format("Viewport: [{:.4f} s .. {:.4f} s] | [{:.0f} .. {:.0f} Hz]",
                      s_sig.lastResp.viewStartSec,
                      s_sig.lastResp.viewEndSec,
                      s_sig.lastResp.viewMinFreqHz,
                      s_sig.lastResp.viewMaxFreqHz),
            {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Info}
        );
        if (s_sig.lastResp.selection.active) {
            ui.Text(
                ui.Format("Selection: Δt={:.3f}s | Peak={:.1f} dB | RMS={:.1f} dB | Fc={:.0f} Hz",
                          s_sig.lastResp.selection.DurationSec(),
                          s_sig.lastResp.selectionPeakDbFs,
                          s_sig.lastResp.selectionRmsDbFs,
                          s_sig.lastResp.selectionCentroidHz),
                {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Success}
            );
        }
        ui.Text(
            "Tip: Wheel=Zoom Time, Drag=2D Spectral Select, Alt+Drag=Pan, Double-Click Ruler=Add Cue",
            {.role = guikit::TextRole::LabelSmall, .variant = guikit::UiVariant::Muted}
        );
    }
}

void RenderSignalStage(guikit::Context& ui, DemoContext&) {
    EnsureSignalInitialized();
    StepSignalPlayback(s_sig);

    if (s_sig.livePlayhead) {
        ui.RequestRepaint(2);
    }

    const keel::u32 fftSize = SignalDemoState::fftSizeValues[s_sig.fftSizeIdx];
    s_sig.lastResp = guikit::signal::SignalView(ui, s_sig.samples, {
        .sampleRate = s_sig.sampleRate,
        .panelOrder = s_sig.panelOrder,
        .splitRatio = s_sig.splitRatio,
        .amplitudeScale = s_sig.amplitudeScale,
        .frequencyScale = s_sig.frequencyScale,
        .colormap = s_sig.colormap,
        .fftWindow = s_sig.fftWindow,
        .fftSize = fftSize,
        .dbFloor = s_sig.dbFloor,
        .dbCeil = 0.0f,
        .waveformGain = s_sig.waveformGain,
        .showOverviewBar = s_sig.showOverviewBar,
        .showTimeRuler = s_sig.showTimeRuler,
        .showAmplitudeRuler = s_sig.showAmplitudeRuler,
        .showFrequencyRuler = s_sig.showFrequencyRuler,
        .showColorbar = s_sig.showColorbar,
        .showGrid = s_sig.showGrid,
        .showRms = s_sig.showRms,
        .showCrosshair = s_sig.showCrosshair,
        .splitRatioBinding = &s_sig.splitRatio,
        .waveformGainBinding = &s_sig.waveformGain,
        .playheadTimeSec = &s_sig.playheadSec,
        .selection = &s_sig.selection,
        .markers = &s_sig.markers,
        .requestViewStartSec = s_sig.requestViewStartSec,
        .requestViewEndSec = s_sig.requestViewEndSec,
        .requestFreqRangeHz = s_sig.requestFreqRangeHz,
        .requestResetView = s_sig.requestResetView,
        .height = guikit::SizeMode::Stretch
    });

    s_sig.requestResetView = false;
    s_sig.requestViewStartSec.reset();
    s_sig.requestViewEndSec.reset();
    s_sig.requestFreqRangeHz.reset();
}

keel::String GenerateSignalCode(const DemoContext&) {
    static constexpr keel::StaticArray<keel::StringRef, 2> orderEnums = {
        "WaveformTop", "SpectrogramTop"
    };
    static constexpr keel::StaticArray<keel::StringRef, 2> ampEnums = {
        "dBFS", "Linear"
    };
    static constexpr keel::StaticArray<keel::StringRef, 2> freqEnums = {
        "Linear", "Logarithmic"
    };
    static constexpr keel::StaticArray<keel::StringRef, 4> winEnums = {
        "Hann", "Hamming", "BlackmanHarris", "Rectangular"
    };

    const keel::u32 fftSize = SignalDemoState::fftSizeValues[s_sig.fftSizeIdx];
    return keel::format(
        "// Synchronized DAW Waveform (Mipmap Pyramid) + STFT Spectrogram Analyzer\n"
        "auto resp = guikit::signal::SignalView(ui, samples, {{\n"
        "    .sampleRate = {:.1f}f,\n"
        "    .panelOrder = guikit::SignalPanelOrder::{},\n"
        "    .splitRatio = {:.2f}f,\n"
        "    .amplitudeScale = guikit::AmplitudeScale::{},\n"
        "    .frequencyScale = guikit::FrequencyScale::{},\n"
        "    .colormap = guikit::Colormap::{},\n"
        "    .fftWindow = guikit::FftWindowType::{},\n"
        "    .fftSize = {}u,\n"
        "    .dbFloor = {:.1f}f,\n"
        "    .waveformGain = {:.2f}f,\n"
        "    .showOverviewBar = {},\n"
        "    .showTimeRuler = {},\n"
        "    .showAmplitudeRuler = {},\n"
        "    .showFrequencyRuler = {},\n"
        "    .showColorbar = {},\n"
        "    .showRms = {},\n"
        "    .splitRatioBinding = &splitRatio,\n"
        "    .playheadTimeSec = &playheadSec,\n"
        "    .selection = &selection,\n"
        "    .markers = &cueMarkers,\n"
        "    .height = guikit::SizeMode::Stretch\n"
        "}});\n",
        s_sig.sampleRate,
        orderEnums[keel::as<keel::usize>(s_sig.panelOrder)],
        s_sig.splitRatio,
        ampEnums[keel::as<keel::usize>(s_sig.amplitudeScale)],
        freqEnums[keel::as<keel::usize>(s_sig.frequencyScale)],
        guikit::GetColormapName(s_sig.colormap),
        winEnums[keel::as<keel::usize>(s_sig.fftWindow)],
        fftSize,
        s_sig.dbFloor,
        s_sig.waveformGain,
        s_sig.showOverviewBar ? "true" : "false",
        s_sig.showTimeRuler ? "true" : "false",
        s_sig.showAmplitudeRuler ? "true" : "false",
        s_sig.showFrequencyRuler ? "true" : "false",
        s_sig.showColorbar ? "true" : "false",
        s_sig.showRms ? "true" : "false"
    );
}

void ReleaseSignalDemoBuffers() noexcept {
    s_sig.samples = {};
    s_sig.markers = {};
    s_sig.initialized = false;
}

} // namespace guikit::demo
