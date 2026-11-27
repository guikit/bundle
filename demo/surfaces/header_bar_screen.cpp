#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct HeaderBarDemoState {
    guikit::WindowControlsPlacement placement{guikit::WindowControlsPlacement::Right};
    bool centerTitle{false};
    bool showMenuBar{true};
    bool showSearch{true};
    bool showSubtitle{true};
    bool simMaximized{false};
    bool simFocused{true};
    guikit::UiSize size{guikit::UiSize::Md};

    keel::String searchText;
    keel::String lastHeaderAction{"Click any HeaderBar button or menu item to inspect events"};
};

HeaderBarDemoState s_hb;

} // namespace

void RenderHeaderBarInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    if (auto el = guikit::Element(ui, {.label = "Controls Placement", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_hb.placement, {"Auto", "Right", "Left", "None"}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Header Size", .layout = guikit::ElementLayout::Stacked})) {
        keel::i32 szIdx = 1;
        if (s_hb.size == guikit::UiSize::Sm) szIdx = 0;
        else if (s_hb.size == guikit::UiSize::Lg) szIdx = 2;
        if (guikit::Segmented(ui, szIdx, {"Sm", "Md", "Lg"}).changed) {
            s_hb.size = (szIdx == 0) ? guikit::UiSize::Sm : (szIdx == 2 ? guikit::UiSize::Lg : guikit::UiSize::Md);
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Center Title", .description = "Center window title across full header width"})) {
        if (guikit::Switch(ui, s_hb.centerTitle).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Subtitle"})) {
        if (guikit::Switch(ui, s_hb.showSubtitle).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Embed MenuBar"})) {
        if (guikit::Switch(ui, s_hb.showMenuBar).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Embed Search Input"})) {
        if (guikit::Switch(ui, s_hb.showSearch).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Simulate Maximized", .description = "Switches Maximize to Restore glyph"})) {
        if (guikit::Switch(ui, s_hb.simMaximized).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Simulate Focused", .description = "Dims title & buttons when window is inactive"})) {
        if (guikit::Switch(ui, s_hb.simFocused).changed) ui.InvalidateAll();
    }
}

void RenderHeaderBarStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    if (auto scroll = guikit::ScrollArea(ui, {
        .gap = 18.0f,
        .padding = guikit::Padding::All(18.0f),
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch
    })) {
        // 1. Rich IDE HeaderBar (CSD) Preview
        guikit::Heading(ui, {
            .label = "HeaderBar — Client-Side Window Decorations (CSD)",
            .description = "Run guikit_demo with --custom-header to test native Win32/SDL3/X11 Aero Snap, 8-edge resize & DWM shadow on the real OS window.",
            .separator = guikit::HeadingSeparator::Bottom
        });

        if (auto frame = guikit::Box(ui, {
            .gap = 0.0f,
            .width = guikit::SizeMode::Stretch,
            .padding = guikit::Padding::All(0.0f),
            .border = {.width = 1.0f, .radius = 8.0f}
        })) {
            if (auto hb = guikit::HeaderBar(ui, {
                .title = "GuiKit Studio",
                .subtitle = s_hb.showSubtitle ? guikit::UiText("bundle/main.cpp") : guikit::UiText{},
                .icon = guikit::Icon::Copy(),
                .centerTitle = s_hb.centerTitle,
                .showWindowControls = (s_hb.placement != guikit::WindowControlsPlacement::None),
                .controlsPlacement = s_hb.placement,
                .bindToWindow = false,
                .maximizedOverride = s_hb.simMaximized,
                .focusedOverride = s_hb.simFocused,
                .size = s_hb.size
            })) {
                if (hb.minimizeClicked) {
                    s_hb.lastHeaderAction = keel::String::from("HeaderBar: Minimize clicked (simulated)");
                }
                if (hb.maximizeClicked) {
                    s_hb.simMaximized = !s_hb.simMaximized;
                    s_hb.lastHeaderAction = keel::String::from(
                        s_hb.simMaximized
                            ? "HeaderBar: Maximized window (simulated)"
                            : "HeaderBar: Restored window (simulated)"
                    );
                }
                if (hb.closeClicked) {
                    s_hb.lastHeaderAction = keel::String::from("HeaderBar: Close clicked (simulated)");
                }

                if (s_hb.showMenuBar) {
                    if (auto mb = guikit::MenuBar(ui, {.style = guikit::MenuBarStyle::Compact, .size = guikit::UiSize::Sm})) {
                        if (auto m = mb.Menu("File")) {
                            if (m.Item("New File", {.shortcut = "Ctrl+N"}).clicked) {
                                s_hb.lastHeaderAction = keel::String::from("MenuBar: File -> New File");
                            }
                            if (m.Item("Open...", {.shortcut = "Ctrl+O"}).clicked) {
                                s_hb.lastHeaderAction = keel::String::from("MenuBar: File -> Open...");
                            }
                        }
                        if (auto m = mb.Menu("View")) {
                            if (m.Item("Toggle Fullscreen", {.shortcut = "F11"}).clicked) {
                                s_hb.simMaximized = !s_hb.simMaximized;
                                s_hb.lastHeaderAction = keel::String::from("MenuBar: View -> Toggle Fullscreen");
                            }
                        }
                    }
                }

                hb.Spacer();

                if (s_hb.showSearch) {
                    guikit::InputField(ui, s_hb.searchText, {
                        .placeholder = "Search commands...",
                        .prefix = guikit::Icon::Search(),
                        .size = guikit::UiSize::Xs,
                        .width = guikit::AxisSize::Fixed(180.0f)
                    });
                }

                if (guikit::Button(ui, {
                    .text = "Run",
                    .icon = guikit::Icon::Play(),
                    .size = guikit::UiSize::Xs,
                    .variant = guikit::UiVariant::Primary
                }).clicked) {
                    s_hb.lastHeaderAction = keel::String::from("HeaderBar: Run action triggered");
                }
            }

            if (auto body = guikit::Column(ui, {.gap = 8.0f, .padding = guikit::Padding::All(16.0f)})) {
                guikit::Text(ui, s_hb.lastHeaderAction.as_ref(), {.role = guikit::TextRole::BodyMedium});
                const auto& host = ui.GetWindowHostService();
                const bool hostCsd = host.isCustomTitleBar && host.isCustomTitleBar();
                const bool hostSupportsCsd = host.supportsCustomTitleBar && host.supportsCustomTitleBar();
                guikit::Text(
                    ui,
                    ui.Format(
                        "Host CSD active: {}  |  Backend supports CSD: {}  (toggle anytime in Engine -> Custom HeaderBar)",
                        hostCsd ? "Yes" : "No",
                        hostSupportsCsd ? "Yes" : "No"
                    ),
                    {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary}
                );
            }
        }

        // 2. Minimal Zero-Boilerplate HeaderBar (auto-pins controls without manual hb.Spacer())
        guikit::Heading(ui, {
            .label = "Minimal HeaderBar (Auto-Aligned Controls)",
            .description = "Even with an empty scope body (no hb.Spacer()), right-edge window controls automatically stay pinned to the trailing edge.",
            .separator = guikit::HeadingSeparator::Bottom
        });

        if (auto minimalFrame = guikit::Box(ui, {
            .gap = 0.0f,
            .width = guikit::SizeMode::Stretch,
            .padding = guikit::Padding::All(0.0f),
            .border = {.width = 1.0f, .radius = 8.0f}
        })) {
            if (auto hb = guikit::HeaderBar(ui, {
                .title = "Minimal Utility Window",
                .centerTitle = true,
                .controlsPlacement = s_hb.placement,
                .bindToWindow = false,
                .maximizedOverride = s_hb.simMaximized,
                .focusedOverride = s_hb.simFocused,
                .size = guikit::UiSize::Sm
            })) {
                // Intentionally empty
            }
            if (auto body = guikit::Column(ui, {.padding = guikit::Padding::All(14.0f)})) {
                guikit::Text(
                    ui,
                    "Drag anywhere on the free caption space; interactive widgets automatically keep Client input.",
                    {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary}
                );
            }
        }
    }
}

keel::String GenerateHeaderBarCode(const DemoContext& /*ctx*/) {
    return keel::String(
        "// Enable Client-Side Decorations in MainWindowConfig: { .customTitleBar = true }\n"
        "if (auto hb = guikit::HeaderBar(ui, {\n"
        "    .title = \"GuiKit Studio\",\n"
        "    .subtitle = \"main.cpp\",\n"
        "    .icon = guikit::Icon::Copy(),\n"
        "    .centerTitle = false\n"
        "})) {\n"
        "    if (auto mb = guikit::MenuBar(ui, {.style = guikit::MenuBarStyle::Compact, .size = guikit::UiSize::Sm})) {\n"
        "        if (auto m = mb.Menu(\"File\")) {\n"
        "            if (m.Item(\"New File\", {.shortcut = \"Ctrl+N\"}).clicked) { /* ... */ }\n"
        "            if (m.Item(\"Open...\",  {.shortcut = \"Ctrl+O\"}).clicked) { /* ... */ }\n"
        "        }\n"
        "    }\n"
        "    hb.Spacer();\n"
        "    guikit::InputField(ui, searchText, {\n"
        "        .placeholder = \"Search commands...\",\n"
        "        .prefix = guikit::Icon::Search(),\n"
        "        .size = guikit::UiSize::Xs,\n"
        "        .width = guikit::AxisSize::Fixed(180.0f)\n"
        "    });\n"
        "    if (guikit::Button(ui, {.text = \"Run\", .size = guikit::UiSize::Xs, .variant = guikit::UiVariant::Primary}).clicked) {\n"
        "        // Run action\n"
        "    }\n"
        "}\n"
    );
}

} // namespace guikit::demo
