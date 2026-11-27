#include "../demo_context.hpp"

namespace guikit::demo {

namespace {

struct SystemDialogsDemoState {
    bool useAsyncDialogs{true};
    keel::i32 filterPresetIdx{0}; // 0 = C++/Images/All, 1 = C++ Only, 2 = No filters
    keel::i32 msgButtonsIdx{2};   // 0 = Ok, 1 = OkCancel, 2 = YesNo, 3 = YesNoCancel
    keel::i32 msgIconIdx{0};      // 0 = Info, 1 = Warning, 2 = Error, 3 = Question
    keel::String defaultSaveName{"untitled.cpp"};

    keel::String lastDialogStatus{"No file dialog invoked yet"};
    keel::Array<keel::String> lastDialogPaths;
    keel::String lastMsgBoxResult{"No message box invoked yet"};
};

SystemDialogsDemoState s_dlg;

keel::StringRef ChoiceToString(guikit::MessageBoxChoice c) noexcept {
    switch (c) {
        case guikit::MessageBoxChoice::Ok:     return "Ok";
        case guikit::MessageBoxChoice::Yes:    return "Yes";
        case guikit::MessageBoxChoice::No:     return "No";
        case guikit::MessageBoxChoice::Cancel:
        default:                               return "Cancel";
    }
}

guikit::MessageBoxButtons SelectedButtons() noexcept {
    switch (s_dlg.msgButtonsIdx) {
        case 0:  return guikit::MessageBoxButtons::Ok;
        case 1:  return guikit::MessageBoxButtons::OkCancel;
        case 2:  return guikit::MessageBoxButtons::YesNo;
        case 3:
        default: return guikit::MessageBoxButtons::YesNoCancel;
    }
}

guikit::MessageBoxIcon SelectedIcon() noexcept {
    switch (s_dlg.msgIconIdx) {
        case 1:  return guikit::MessageBoxIcon::Warning;
        case 2:  return guikit::MessageBoxIcon::Error;
        case 3:  return guikit::MessageBoxIcon::Question;
        case 0:
        default: return guikit::MessageBoxIcon::Info;
    }
}

keel::ArrayRef<guikit::FileDialogFilter> ActiveFilters() noexcept {
    static constexpr keel::StaticArray<guikit::FileDialogFilter, 3> kAllFilters = {{
        {.name = "C++ Sources", .pattern = "*.cpp;*.hpp;*.h"},
        {.name = "Images",      .pattern = "*.png;*.jpg;*.jpeg"},
        {.name = "All Files",   .pattern = "*.*"}
    }};
    static constexpr keel::StaticArray<guikit::FileDialogFilter, 1> kCppOnly = {{
        {.name = "C++ Sources", .pattern = "*.cpp;*.hpp;*.h"}
    }};
    if (s_dlg.filterPresetIdx == 1) {
        return kCppOnly;
    }
    if (s_dlg.filterPresetIdx == 2) {
        return {};
    }
    return kAllFilters;
}

void RecordFileDialogResult(keel::StringRef label, const guikit::FileDialogResult& res) {
    s_dlg.lastDialogPaths.clear();
    if (!res.supported) {
        s_dlg.lastDialogStatus = keel::format("{}: not supported on this platform/host", label);
        return;
    }
    if (!res.accepted) {
        s_dlg.lastDialogStatus = keel::format("{}: cancelled by user", label);
        return;
    }
    s_dlg.lastDialogStatus = keel::format(
        "{}: accepted ({} path{})",
        label,
        res.paths.size(),
        res.paths.size() == 1u ? "" : "s"
    );
    for (const keel::String& p : res.paths) {
        s_dlg.lastDialogPaths.push_back(p.clone());
    }
}

void TriggerFileDialog(guikit::Context& ui, keel::StringRef label, guikit::FileDialogOptions opts) {
    if (s_dlg.useAsyncDialogs) {
        s_dlg.lastDialogStatus = keel::format("{}: waiting for OS dialog...", label);
        guikit::ShowFileDialogAsync(ui, opts, [label](guikit::FileDialogResult res) {
            RecordFileDialogResult(label, res);
        });
    } else {
        const guikit::FileDialogResult res = guikit::ShowFileDialog(ui, opts);
        RecordFileDialogResult(label, res);
    }
}

} // namespace

void RenderSystemDialogsInspector(guikit::Context& ui, DemoContext& /*ctx*/) {
    guikit::Heading(ui, {
        .label = "File Dialogs",
        .level = guikit::HeadingLevel::H6,
        .separator = guikit::HeadingSeparator::Bottom
    });
    if (auto el = guikit::Element(ui, {
        .label = "Async Mode",
        .description = "Uses ShowFileDialogAsync (non-blocking on SDL3)"
    })) {
        if (guikit::Switch(ui, s_dlg.useAsyncDialogs).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Extension Filters", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_dlg.filterPresetIdx, {"C++ / Img", "C++ Only", "None"}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Default Save Name", .layout = guikit::ElementLayout::Stacked})) {
        guikit::InputField(ui, s_dlg.defaultSaveName, {.placeholder = "untitled.cpp", .size = guikit::UiSize::Sm});
    }

    guikit::Heading(ui, {
        .label = "Message Box",
        .level = guikit::HeadingLevel::H6,
        .separator = guikit::HeadingSeparator::Bottom
    });
    if (auto el = guikit::Element(ui, {.label = "Buttons", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_dlg.msgButtonsIdx, {"Ok", "Ok/Can", "Yes/No", "Y/N/C"}).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Icon", .layout = guikit::ElementLayout::Stacked})) {
        if (guikit::Segmented(ui, s_dlg.msgIconIdx, {"Info", "Warn", "Error", "Ask"}).changed) {
            ui.InvalidateAll();
        }
    }
}

void RenderSystemDialogsStage(guikit::Context& ui, DemoContext& /*ctx*/) {
    if (auto scroll = guikit::ScrollArea(ui, {
        .gap = 18.0f,
        .padding = guikit::Padding::All(18.0f),
        .width = guikit::SizeMode::Stretch,
        .height = guikit::SizeMode::Stretch
    })) {
        const auto& host = ui.GetWindowHostService();
        const bool supportsFileDlgs = host.supportsFileDialogs && host.supportsFileDialogs();

        guikit::Heading(ui, {
            .label = "Native OS File & Folder Pickers",
            .description = "Invokes platform-native dialogs via IPlatformWindow (Win32 IFileOpenDialog/IFileSaveDialog, SDL3 Dialog API, Linux zenity/kdialog).",
            .separator = guikit::HeadingSeparator::Bottom
        });

        if (auto card = guikit::Card(ui, {
            .width = guikit::SizeMode::Stretch,
            .gap = 12.0f,
            .padding = guikit::Padding::All(16.0f)
        })) {
            const auto filters = ActiveFilters();
            if (auto row = guikit::Wrap(ui, {.gap = 8.0f})) {
                if (guikit::Button(ui, {
                    .text = "Open File...",
                    .icon = guikit::Icon::Folder(),
                    .variant = guikit::UiVariant::Primary
                }).clicked) {
                    TriggerFileDialog(ui, "OpenFile", {
                        .kind = guikit::FileDialogKind::OpenFile,
                        .title = "Open File",
                        .filters = filters
                    });
                }
                if (guikit::Button(ui, {
                    .text = "Open Multiple Files...",
                    .icon = guikit::Icon::Copy(),
                    .variant = guikit::UiVariant::Secondary
                }).clicked) {
                    TriggerFileDialog(ui, "OpenFiles", {
                        .kind = guikit::FileDialogKind::OpenFiles,
                        .title = "Select Multiple Files",
                        .filters = filters
                    });
                }
                if (guikit::Button(ui, {
                    .text = "Save File As...",
                    .icon = guikit::Icon::Save(),
                    .variant = guikit::UiVariant::Secondary
                }).clicked) {
                    TriggerFileDialog(ui, "SaveFile", {
                        .kind = guikit::FileDialogKind::SaveFile,
                        .title = "Save Document As",
                        .defaultName = s_dlg.defaultSaveName.as_ref(),
                        .filters = filters
                    });
                }
                if (guikit::Button(ui, {
                    .text = "Select Folder...",
                    .icon = guikit::Icon::Folder(),
                    .variant = guikit::UiVariant::Secondary
                }).clicked) {
                    TriggerFileDialog(ui, "SelectFolder", {
                        .kind = guikit::FileDialogKind::SelectFolder,
                        .title = "Select Workspace Directory"
                    });
                }
            }

            guikit::Separator(ui);
            guikit::Text(ui, ui.Format("Status: {}", s_dlg.lastDialogStatus.as_ref()), {.role = guikit::TextRole::BodyMedium});
            for (keel::usize i = 0; i < s_dlg.lastDialogPaths.size(); ++i) {
                auto sc = ui.IdScope(i);
                guikit::Text(ui, s_dlg.lastDialogPaths[i].as_ref(), {.role = guikit::TextRole::CodeSmall, .variant = guikit::UiVariant::Info});
            }
            guikit::Text(
                ui,
                ui.Format(
                    "Mode: {}  |  Backend supports native file dialogs: {}",
                    s_dlg.useAsyncDialogs ? "Async (ShowFileDialogAsync)" : "Sync (ShowFileDialog)",
                    supportsFileDlgs ? "Yes" : "No"
                ),
                {.role = guikit::TextRole::BodySmall, .variant = guikit::UiVariant::Secondary}
            );
        }

        guikit::Heading(ui, {
            .label = "Native OS Message Boxes",
            .description = "Invokes modal OS alerts via guikit::ShowMessageBox() (Win32 MessageBoxW, SDL_ShowMessageBox, X11 zenity/kdialog, Web alert/confirm).",
            .separator = guikit::HeadingSeparator::Bottom
        });

        if (auto card = guikit::Card(ui, {
            .width = guikit::SizeMode::Stretch,
            .gap = 12.0f,
            .padding = guikit::Padding::All(16.0f)
        })) {
            if (auto row = guikit::Wrap(ui, {.gap = 8.0f})) {
                if (guikit::Button(ui, {
                    .text = "Show Configured MessageBox",
                    .icon = guikit::Icon::Info(),
                    .variant = guikit::UiVariant::Primary
                }).clicked) {
                    const auto c = guikit::ShowMessageBox(ui, {
                        .title = "GuiKit System Dialog",
                        .message = "Native OS message box configured from the Inspector panel.",
                        .buttons = SelectedButtons(),
                        .icon = SelectedIcon()
                    });
                    s_dlg.lastMsgBoxResult = keel::format("Configured -> {}", ChoiceToString(c));
                }
                if (guikit::Button(ui, {
                    .text = "Info (Ok)",
                    .icon = guikit::Icon::Info(),
                    .variant = guikit::UiVariant::Info
                }).clicked) {
                    const auto c = guikit::ShowMessageBox(ui, {
                        .title = "Export Complete",
                        .message = "All project artifacts were exported successfully.",
                        .buttons = guikit::MessageBoxButtons::Ok,
                        .icon = guikit::MessageBoxIcon::Info
                    });
                    s_dlg.lastMsgBoxResult = keel::format("Info (Ok) -> {}", ChoiceToString(c));
                }
                if (guikit::Button(ui, {
                    .text = "Warning (Yes/No)",
                    .icon = guikit::Icon::Warning(),
                    .variant = guikit::UiVariant::Warning
                }).clicked) {
                    const auto c = guikit::ShowMessageBox(ui, {
                        .title = "Unsaved Changes",
                        .message = "Save changes before closing the workspace?",
                        .buttons = guikit::MessageBoxButtons::YesNo,
                        .icon = guikit::MessageBoxIcon::Warning
                    });
                    s_dlg.lastMsgBoxResult = keel::format("Warning (Yes/No) -> {}", ChoiceToString(c));
                }
                if (guikit::Button(ui, {
                    .text = "Error (Ok/Cancel)",
                    .icon = guikit::Icon::Error(),
                    .variant = guikit::UiVariant::Danger
                }).clicked) {
                    const auto c = guikit::ShowMessageBox(ui, {
                        .title = "Build Failed",
                        .message = "Retry compilation with a clean build cache?",
                        .buttons = guikit::MessageBoxButtons::OkCancel,
                        .icon = guikit::MessageBoxIcon::Error
                    });
                    s_dlg.lastMsgBoxResult = keel::format("Error (Ok/Cancel) -> {}", ChoiceToString(c));
                }
            }

            guikit::Separator(ui);
            guikit::Text(ui, ui.Format("Last Choice: {}", s_dlg.lastMsgBoxResult.as_ref()), {.role = guikit::TextRole::BodyMedium});
        }
    }
}

keel::String GenerateSystemDialogsCode(const DemoContext& /*ctx*/) {
    return keel::String(
        "static constexpr keel::StaticArray<guikit::FileDialogFilter, 2> kFilters = {{\n"
        "    {.name = \"C++ Sources\", .pattern = \"*.cpp;*.hpp;*.h\"},\n"
        "    {.name = \"All Files\",   .pattern = \"*.*\"}\n"
        "}};\n\n"
        "// Non-blocking async file picker (recommended on SDL3 & cross-platform)\n"
        "if (guikit::Button(ui, {.text = \"Open File...\"}).clicked) {\n"
        "    guikit::ShowFileDialogAsync(ui, {\n"
        "        .kind = guikit::FileDialogKind::OpenFile,\n"
        "        .title = \"Open Source File\",\n"
        "        .filters = kFilters\n"
        "    }, [](guikit::FileDialogResult res) {\n"
        "        if (res.accepted && !res.paths.empty()) {\n"
        "            // Open UTF-8 path res.paths[0]\n"
        "        }\n"
        "    });\n"
        "}\n\n"
        "// Native OS confirmation dialog\n"
        "if (guikit::Button(ui, {.text = \"Close Project\"}).clicked) {\n"
        "    auto choice = guikit::ShowMessageBox(ui, {\n"
        "        .title = \"Unsaved Changes\",\n"
        "        .message = \"Save changes before closing?\",\n"
        "        .buttons = guikit::MessageBoxButtons::YesNoCancel,\n"
        "        .icon = guikit::MessageBoxIcon::Warning\n"
        "    });\n"
        "}\n"
    );
}

} // namespace guikit::demo
