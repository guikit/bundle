#include "../demo_context.hpp"
#include "keel/math.h"

namespace guikit::demo {

namespace {

static constexpr keel::StaticArray<keel::StringRef, 6> kTaskCategories = {
    "UI", "Layout", "Render", "Docs", "Tests", "Core"
};

struct ProjectTaskRow {
    keel::i32 id{0};
    keel::FixedString<64> title{};
    keel::usize categoryIdx{0};
    keel::FixedString<24> owner{};
    keel::f32 progress{0.0f};
    keel::i32 status{0}; // 0=DONE, 1=IN PROGRESS, 2=IN REVIEW, 3=BLOCKED
    bool completed{false};
    bool archived{false};
};

struct TableDemoState {
    UiSize tableSize{UiSize::Md};
    UiSize cellWidgetSize{UiSize::Sm};
    bool striped{true};
    bool gridLines{true};
    bool stickyHeader{true};
    bool resizableColumns{true};
    bool showRowNumbers{true};
    bool showFooter{true};
    bool multiSelect{false};
    keel::i32 frozenColumns{0};
    keel::i32 wideFrozenColumns{2};

    keel::SmallString<128> filterQuery{};
    keel::i32 selectedRow{0};
    keel::Array<keel::usize> selectedRows{};
    keel::i32 sortCol{1}; // Default sorted by ID
    TableSortDirection sortDir{TableSortDirection::Ascending};
    keel::FixedString<96> lastAction{};

    keel::i32 virtualSelectedRow{0};
    keel::i32 virtualSortCol{0};
    TableSortDirection virtualSortDir{TableSortDirection::Ascending};
    keel::i32 pendingJumpRow{-1};
    keel::usize lastVisibleCount{0};

    keel::i32 wideSelectedRow{0};
    keel::i32 wideSortCol{0};
    TableSortDirection wideSortDir{TableSortDirection::Ascending};
    keel::usize lastFirstCol{0};
    keel::usize lastEndCol{0};

    keel::Array<ProjectTaskRow> rows{};
    bool initialized{false};

    void EnsureInitialized() {
        if (initialized) return;
        initialized = true;

        static constexpr keel::StaticArray<const char*, 12> kTaskTitles = {
            "Finalize keyboard focus ring tokens",
            "Audit responsive breakpoints for SplitView",
            "Add virtualized horizontal column culling",
            "Verify Classic 3D sunken frame alignment",
            "Write migration guide for Table API",
            "Optimize glyph atlas eviction threshold",
            "Review dark theme contrast ratios",
            "Fix scroll wheel delta clamping on high-DPI",
            "Document multi-widget TableScope::Cell usage",
            "Add regression test for 24+ column grids",
            "Polish empty-state placeholder typography",
            "Benchmark 100k row jump-to-end latency"
        };
        static constexpr keel::StaticArray<const char*, 6> kOwners = {
            "Alex", "Maya", "Liam", "Sofia", "Noah", "Elena"
        };
        static constexpr keel::StaticArray<keel::f32, 12> kProgress = {
            100.0f, 75.0f, 60.0f, 100.0f, 40.0f, 85.0f,
            25.0f,  90.0f, 50.0f, 15.0f,  100.0f, 70.0f
        };

        rows.clear();
        rows.reserve(36);
        for (keel::i32 i = 0; i < 36; ++i) {
            const keel::usize idx = keel::as<keel::usize>(i);
            ProjectTaskRow r;
            r.id = 101 + i;
            if (i < 12) {
                r.title = kTaskTitles[idx];
            } else {
                r.title = keel::format_fixed<64>("{} (Phase {})", kTaskTitles[idx % kTaskTitles.size()], (i / 12) + 1);
            }
            r.categoryIdx = (idx * 5 + 1) % kTaskCategories.size();
            r.owner = kOwners[(idx * 3 + 2) % kOwners.size()];
            r.progress = kProgress[idx % kProgress.size()];
            r.completed = (r.progress >= 100.0f);
            r.archived = (i == 11 || i == 23);
            if (r.completed) {
                r.status = 0;
            } else if (i % 9 == 6) {
                r.status = 3; // BLOCKED
                r.progress = 20.0f;
            } else if (r.progress >= 75.0f) {
                r.status = 2; // IN REVIEW
            } else {
                r.status = 1; // IN PROGRESS
            }
            rows.push_back(r);
        }
        SortRows();
    }

    void SortRows() {
        if (sortCol < 0 || sortDir == TableSortDirection::None) return;
        const bool asc = (sortDir == TableSortDirection::Ascending);
        keel::sort(rows, [&](const ProjectTaskRow& a, const ProjectTaskRow& b) {
            switch (sortCol) {
                case 0: return asc ? (a.completed > b.completed) : (a.completed < b.completed);
                case 1: return asc ? (a.id < b.id) : (a.id > b.id);
                case 2: return asc ? (a.title.as_ref() < b.title.as_ref()) : (a.title.as_ref() > b.title.as_ref());
                case 3: {
                    const keel::StringRef ca(kTaskCategories[a.categoryIdx % kTaskCategories.size()]);
                    const keel::StringRef cb(kTaskCategories[b.categoryIdx % kTaskCategories.size()]);
                    return asc ? (ca < cb) : (ca > cb);
                }
                case 4: return asc ? (a.owner.as_ref() < b.owner.as_ref()) : (a.owner.as_ref() > b.owner.as_ref());
                case 5: return asc ? (a.progress < b.progress) : (a.progress > b.progress);
                case 6: return asc ? (a.status < b.status) : (a.status > b.status);
                default: return false;
            }
        });
    }
};

TableDemoState g_tableDemo;

bool ContainsCaseInsensitive(keel::StringRef haystack, keel::StringRef needle) noexcept {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;
    auto toLower = [](char c) noexcept -> char {
        return (c >= 'A' && c <= 'Z') ? keel::as<char>(c + ('a' - 'A')) : c;
    };
    const keel::usize limit = haystack.size() - needle.size();
    for (keel::usize i = 0; i <= limit; ++i) {
        bool match = true;
        for (keel::usize j = 0; j < needle.size(); ++j) {
            if (toLower(haystack[i + j]) != toLower(needle[j])) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

} // namespace

void RenderTableInspector(guikit::Context& ui, DemoContext& ctx) {
    g_tableDemo.EnsureInitialized();
    const keel::usize datasetMode = ctx.GetDemoVariant(NavCategory::ViewTable);

    if (auto el = guikit::Element(ui, {.label = "Row & Header Size"})) {
        if (guikit::StepSlider(ui, g_tableDemo.tableSize, {
            .showTicks = true,
            .showLabels = true
        }).changed) {
            ui.InvalidateAll();
        }
    }
    if (auto el = guikit::Element(ui, {.label = "Cell Widget Size"})) {
        if (guikit::StepSlider(ui, g_tableDemo.cellWidgetSize, {
            .showTicks = true,
            .showLabels = true
        }).changed) {
            ui.InvalidateAll();
        }
    }

    if (auto el = guikit::Element(ui, {.label = "Zebra Striped Rows"})) {
        if (guikit::Switch(ui, g_tableDemo.striped).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Cell Grid Lines"})) {
        if (guikit::Switch(ui, g_tableDemo.gridLines).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Sticky Header"})) {
        if (guikit::Switch(ui, g_tableDemo.stickyHeader).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Resizable Columns"})) {
        if (guikit::Switch(ui, g_tableDemo.resizableColumns).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Row Numbers (#)"})) {
        if (guikit::Switch(ui, g_tableDemo.showRowNumbers).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {.label = "Show Summary Footer"})) {
        if (guikit::Switch(ui, g_tableDemo.showFooter).changed) ui.InvalidateAll();
    }
    if (auto el = guikit::Element(ui, {
        .label = "Multi-Row Selection",
        .description = "Click, Ctrl+Click, Shift+Click, Ctrl+A"
    })) {
        if (guikit::Switch(ui, g_tableDemo.multiSelect).changed) {
            g_tableDemo.selectedRows.clear();
            if (g_tableDemo.selectedRow >= 0) {
                g_tableDemo.selectedRows.push_back(keel::as<keel::usize>(g_tableDemo.selectedRow));
            }
            ui.InvalidateAll();
        }
    }

    keel::i32& activeFrozen = (datasetMode == 2u) ? g_tableDemo.wideFrozenColumns : g_tableDemo.frozenColumns;
    const keel::f32 maxFrozen = (datasetMode == 2u) ? 4.0f : 3.0f;
    keel::f32 frozenVal = keel::as<keel::f32>(activeFrozen);
    if (auto el = guikit::Element(ui, {.label = "Frozen Columns"})) {
        if (guikit::Slider(ui, frozenVal, 0.0f, maxFrozen, {
            .valueFormat = "%.0f",
            .step = 1.0f,
            .size = UiSize::Sm
        }).changed) {
            activeFrozen = keel::as<keel::i32>(keel::round(frozenVal));
            ui.InvalidateAll();
        }
    }

    if (datasetMode == 0u) {
        if (auto el = guikit::Element(ui, {
            .label = "Filter Rows",
            .layout = ElementLayout::Top
        })) {
            if (guikit::InputField(ui, g_tableDemo.filterQuery, {
                .placeholder = "Task, category, or owner...",
                .prefix = Icon::Search(),
                .clearable = true,
                .size = UiSize::Sm
            }).changed) {
                g_tableDemo.selectedRow = 0;
                g_tableDemo.selectedRows.clear();
            }
        }
    } else if (datasetMode == 1u) {
        guikit::Heading(ui, {.label = "VIRTUAL JUMP (100,000 ROWS)", .level = HeadingLevel::H6, .separator = HeadingSeparator::Bottom});
        if (auto row = guikit::Row(ui, {.gap = 6.0f})) {
            if (guikit::Button(ui, {.text = "Top (#1)", .size = UiSize::Sm, .width = SizeMode::Stretch, .variant = UiVariant::Secondary})) {
                g_tableDemo.pendingJumpRow = 0;
            }
            if (guikit::Button(ui, {.text = "Mid (#50k)", .size = UiSize::Sm, .width = SizeMode::Stretch, .variant = UiVariant::Secondary})) {
                g_tableDemo.pendingJumpRow = 49999;
            }
            if (guikit::Button(ui, {.text = "End (#100k)", .size = UiSize::Sm, .width = SizeMode::Stretch, .variant = UiVariant::Primary})) {
                g_tableDemo.pendingJumpRow = 99999;
            }
        }
    } else {
        guikit::Heading(ui, {.label = "WIDE GRID (24 COLUMNS)", .level = HeadingLevel::H6, .separator = HeadingSeparator::Bottom});
        guikit::Text(ui, {
            "Left columns stay pinned while panning across all 24 columns using O(1) CellAt() virtualization. Drag the scrollbar or hold Shift + MouseWheel.",
            TextWrap::Word()
        }, {
            .role = TextRole::BodySmall,
            .variant = UiVariant::Secondary
        });
    }
}

void RenderTableStage(guikit::Context& ui, DemoContext& ctx) {
    g_tableDemo.EnsureInitialized();
    const keel::usize datasetMode = ctx.GetDemoVariant(NavCategory::ViewTable);

    if (datasetMode == 0u) {
        // Variant 0: Interactive Project Tasks Grid (static, zero idle repaint)
        keel::SmallArray<keel::usize, 64> filteredIndices;
        const keel::StringRef query = g_tableDemo.filterQuery.as_ref();
        for (keel::usize i = 0; i < g_tableDemo.rows.size(); ++i) {
            const auto& r = g_tableDemo.rows[i];
            const keel::StringRef catStr(kTaskCategories[r.categoryIdx % kTaskCategories.size()]);
            if (query.empty() ||
                ContainsCaseInsensitive(r.title.as_ref(), query) ||
                ContainsCaseInsensitive(catStr, query) ||
                ContainsCaseInsensitive(r.owner.as_ref(), query)) {
                filteredIndices.push_back(i);
            }
        }

        static const keel::StaticArray<TableColumn, 8> kTaskCols = {{
            {.title = "Done",     .width = 56.0f,  .minWidth = 48.0f,  .align = Align::Center, .sortable = true},
            {.title = "ID",       .width = 80.0f,  .minWidth = 64.0f,  .align = Align::Start,  .sortable = true},
            {.title = "Task",     .flex = 2.0f,    .minWidth = 170.0f, .align = Align::Start,  .sortable = true},
            {.title = "Category", .width = 114.0f, .minWidth = 92.0f,  .align = Align::Start,  .sortable = true},
            {.title = "Owner",    .width = 86.0f,  .minWidth = 70.0f,  .align = Align::Start,  .sortable = true},
            {.title = "Progress", .flex = 1.1f,    .minWidth = 120.0f, .align = Align::Start,  .sortable = true},
            {.title = "Status",   .width = 110.0f, .minWidth = 90.0f,  .align = Align::Center, .sortable = true},
            {.title = "Actions",  .width = 90.0f,  .minWidth = 84.0f,  .align = Align::Center, .sortable = false, .resizable = false}
        }};

        char footerBuf[128];
        if (!g_tableDemo.lastAction.empty()) {
            keel::format_buf(footerBuf, sizeof(footerBuf), "{}", g_tableDemo.lastAction.as_ref());
        } else if (g_tableDemo.multiSelect && g_tableDemo.selectedRows.size() > 1) {
            keel::format_buf(
                footerBuf, sizeof(footerBuf),
                "{} tasks selected | Ctrl+Click toggle, Shift+Click range, Ctrl+A all",
                g_tableDemo.selectedRows.size()
            );
        } else {
            const keel::StringRef selTitle =
                (g_tableDemo.selectedRow >= 0 && keel::as<keel::usize>(g_tableDemo.selectedRow) < filteredIndices.size())
                    ? g_tableDemo.rows[filteredIndices[keel::as<keel::usize>(g_tableDemo.selectedRow)]].title.as_ref()
                    : keel::StringRef("None");
            keel::format_buf(
                footerBuf, sizeof(footerBuf),
                "Selected: {} | Double-click divider to reset widths",
                selTitle
            );
        }

        if (auto table = guikit::Table(ui, kTaskCols, {
            .totalRows = filteredIndices.size(),
            .selectedRow = &g_tableDemo.selectedRow,
            .multiSelect = g_tableDemo.multiSelect,
            .selectedRows = g_tableDemo.multiSelect ? IndexListSink(&g_tableDemo.selectedRows) : IndexListSink{},
            .sortColumn = &g_tableDemo.sortCol,
            .sortDirection = &g_tableDemo.sortDir,
            .stickyHeader = g_tableDemo.stickyHeader,
            .frozenColumns = keel::as<keel::u32>(keel::max(0, g_tableDemo.frozenColumns)),
            .striped = g_tableDemo.striped,
            .gridLines = g_tableDemo.gridLines,
            .resizableColumns = g_tableDemo.resizableColumns,
            .showRowNumbers = g_tableDemo.showRowNumbers,
            .showFooter = g_tableDemo.showFooter,
            .footerText = footerBuf,
            .emptyText = "No matching tasks",
            .size = g_tableDemo.tableSize
        })) {
            if (table.SortChanged()) {
                g_tableDemo.SortRows();
                g_tableDemo.lastAction.clear();
            }
            if (table.SelectionChanged()) {
                g_tableDemo.lastAction.clear();
            }

            for (keel::usize visRow = table.FirstVisibleRow(); visRow < table.EndVisibleRow(); ++visRow) {
                if (visRow >= filteredIndices.size()) break;
                ProjectTaskRow& item = g_tableDemo.rows[filteredIndices[visRow]];

                TableRowOptions rowOpts{};
                rowOpts.disabled = item.archived;
                if (item.status == 3 && !item.archived) {
                    rowOpts.tint = UiVariant::Danger;
                }

                if (auto row = table.Row(visRow, rowOpts)) {
                    if (row.DoubleClicked()) {
                        g_tableDemo.lastAction = keel::format_fixed<96>("Opened TSK-{}", item.id);
                    }

                    // Col 0: Embedded Checkbox
                    if (guikit::Checkbox(ui, item.completed, {.size = g_tableDemo.cellWidgetSize, .color = StateColor::Primary()}).changed) {
                        item.progress = item.completed ? 100.0f : 50.0f;
                        item.status = item.completed ? 0 : 1;
                        g_tableDemo.lastAction = keel::format_fixed<96>(
                            "TSK-{} marked {}", item.id, item.completed ? "Done" : "In Progress"
                        );
                    }

                    // Col 1: Task ID
                    guikit::Text(ui, ui.Format("TSK-{}", item.id), {.variant = UiVariant::Secondary});

                    // Col 2: Task title
                    guikit::Text(ui, item.title.as_ref());

                    // Col 3: Category ComboBox (searchable Select)
                    if (guikit::Select(ui, item.categoryIdx, kTaskCategories, {
                        .searchable = true,
                        .size = g_tableDemo.cellWidgetSize,
                        .width = SizeMode::Stretch
                    }).changed) {
                        g_tableDemo.lastAction = keel::format_fixed<96>(
                            "TSK-{} category -> {}", item.id, kTaskCategories[item.categoryIdx % kTaskCategories.size()]
                        );
                    }

                    // Col 4: Owner
                    guikit::Text(ui, item.owner.as_ref(), {.variant = UiVariant::Secondary});

                    // Col 5: Static ProgressBar
                    const StateColor barCol = (item.status == 0)
                        ? StateColor::Success()
                        : ((item.status == 3) ? StateColor::Danger() : StateColor::Primary());
                    guikit::ProgressBar(ui, item.progress, 0.0f, 100.0f, {
                        .valueFormat = "%.0f%%",
                        .size = g_tableDemo.cellWidgetSize,
                        .color = barCol
                    });

                    // Col 6: Status Tag
                    static constexpr keel::StaticArray<const char*, 4> kStatusLabels = {
                        "DONE", "IN PROGRESS", "IN REVIEW", "BLOCKED"
                    };
                    static constexpr keel::StaticArray<UiVariant, 4> kStatusVars = {
                        UiVariant::Success, UiVariant::Primary, UiVariant::Info, UiVariant::Danger
                    };
                    const keel::usize stIdx = keel::as<keel::usize>(keel::clamp(item.status, 0, 3));
                    guikit::Tag(ui, item.archived ? "ARCHIVED" : kStatusLabels[stIdx], {
                        .variant = item.archived ? UiVariant::Muted : kStatusVars[stIdx],
                        .style = TagStyle::Soft,
                        .size = g_tableDemo.cellWidgetSize
                    });

                    // Col 7: Multi-widget cell via TableScope::Cell()
                    if (auto cell = table.Cell()) {
                        if (guikit::Button(ui, {
                            .icon = Icon::Plus(),
                            .square = true,
                            .tooltip = "Advance progress +25%",
                            .disabled = item.archived || item.progress >= 100.0f,
                            .size = g_tableDemo.cellWidgetSize,
                            .style = ButtonStyle::Ghost
                        }).clicked) {
                            item.progress = keel::min(100.0f, item.progress + 25.0f);
                            item.completed = (item.progress >= 100.0f);
                            item.status = item.completed ? 0 : ((item.progress >= 75.0f) ? 2 : 1);
                            g_tableDemo.lastAction = keel::format_fixed<96>(
                                "TSK-{} progress -> {:.0f}%", item.id, item.progress
                            );
                        }
                        if (guikit::Button(ui, {
                            .icon = Icon::Refresh(),
                            .square = true,
                            .tooltip = "Reset task progress",
                            .disabled = item.archived || item.progress <= 0.0f,
                            .size = g_tableDemo.cellWidgetSize,
                            .style = ButtonStyle::Ghost
                        }).clicked) {
                            item.progress = 0.0f;
                            item.completed = false;
                            item.status = 1;
                            g_tableDemo.lastAction = keel::format_fixed<96>("TSK-{} reset to 0%", item.id);
                        }
                    }
                }
            }
        }
    } else if (datasetMode == 1u) {
        // Variant 1: 100,000 Rows Virtualized Stress-Test (static deterministic records)
        constexpr keel::usize kTotalVirtualRows = 100000;

        static const keel::StaticArray<TableColumn, 6> kVirtualCols = {{
            {.title = "Record ID", .width = 112.0f, .minWidth = 84.0f,  .align = Align::Start,  .sortable = true},
            {.title = "Item Name", .flex = 1.6f,    .minWidth = 150.0f, .align = Align::Start,  .sortable = true},
            {.title = "Category",  .width = 110.0f, .minWidth = 84.0f,  .align = Align::Center, .sortable = true},
            {.title = "Updated",   .width = 140.0f, .minWidth = 110.0f, .align = Align::Start,  .sortable = true},
            {.title = "Size",      .width = 98.0f,  .minWidth = 78.0f,  .align = Align::End,    .sortable = true},
            {.title = "Status",    .width = 104.0f, .minWidth = 84.0f,  .align = Align::Center, .sortable = true}
        }};

        char ftrBuf[128];
        keel::format_buf(
            ftrBuf, sizeof(ftrBuf),
            "O(1) Virtualized: Materialized {} rows out of 100,000",
            g_tableDemo.lastVisibleCount
        );

        if (auto table = guikit::Table(ui, kVirtualCols, {
            .totalRows = kTotalVirtualRows,
            .selectedRow = &g_tableDemo.virtualSelectedRow,
            .multiSelect = g_tableDemo.multiSelect,
            .sortColumn = &g_tableDemo.virtualSortCol,
            .sortDirection = &g_tableDemo.virtualSortDir,
            .stickyHeader = g_tableDemo.stickyHeader,
            .frozenColumns = keel::as<keel::u32>(keel::max(0, g_tableDemo.frozenColumns)),
            .striped = g_tableDemo.striped,
            .gridLines = g_tableDemo.gridLines,
            .resizableColumns = g_tableDemo.resizableColumns,
            .showRowNumbers = g_tableDemo.showRowNumbers,
            .showFooter = g_tableDemo.showFooter,
            .footerText = ftrBuf,
            .emptyText = "No records",
            .size = g_tableDemo.tableSize
        })) {
            if (g_tableDemo.pendingJumpRow >= 0) {
                g_tableDemo.virtualSelectedRow = g_tableDemo.pendingJumpRow;
                table.ScrollToRow(keel::as<keel::usize>(g_tableDemo.pendingJumpRow));
                g_tableDemo.pendingJumpRow = -1;
            }

            const keel::usize firstRow = table.FirstVisibleRow();
            const keel::usize endRow = table.EndVisibleRow();
            g_tableDemo.lastVisibleCount = (endRow >= firstRow) ? (endRow - firstRow) : 0;

            const bool desc = (g_tableDemo.virtualSortDir == TableSortDirection::Descending);

            static constexpr keel::StaticArray<const char*, 6> kCategories = {
                "Document", "Image", "Archive", "Dataset", "Config", "Report"
            };
            static constexpr keel::StaticArray<const char*, 6> kPrefixes = {
                "spec_v", "banner_", "backup_", "export_", "settings_", "summary_"
            };
            static constexpr keel::StaticArray<const char*, 6> kExts = {
                ".md", ".png", ".zip", ".csv", ".json", ".pdf"
            };

            for (keel::usize r = firstRow; r < endRow; ++r) {
                const keel::usize logicalIdx = desc ? (kTotalVirtualRows - 1 - r) : r;
                if (auto row = table.Row(r)) {
                    const keel::usize catIdx = logicalIdx % kCategories.size();
                    guikit::Text(ui, ui.Format("REC-{:06}", logicalIdx + 1), {.variant = UiVariant::Secondary});
                    guikit::Text(ui, ui.Format("{}{}{}", kPrefixes[catIdx], logicalIdx + 1, kExts[catIdx]));
                    guikit::Tag(ui, kCategories[catIdx], {
                        .variant = (catIdx % 2 == 0) ? UiVariant::Info : UiVariant::Secondary,
                        .style = TagStyle::Soft,
                        .size = g_tableDemo.cellWidgetSize
                    });

                    const keel::u32 day = keel::as<keel::u32>(1 + (logicalIdx % 28));
                    const keel::u32 hour = keel::as<keel::u32>((logicalIdx / 60) % 24);
                    const keel::u32 min = keel::as<keel::u32>(logicalIdx % 60);
                    guikit::Text(ui, ui.Format("2026-03-{:02} {:02}:{:02}", day, hour, min), {.variant = UiVariant::Secondary});

                    const keel::f32 sizeKb = 12.4f + keel::as<keel::f32>((logicalIdx * 37) % 4800) * 0.5f;
                    if (sizeKb >= 1024.0f) {
                        guikit::Text(ui, ui.Format("{:.2f} MB", sizeKb / 1024.0f));
                    } else {
                        guikit::Text(ui, ui.Format("{:.1f} KB", sizeKb));
                    }

                    const keel::usize st = logicalIdx % 17;
                    const char* stText = (st == 0) ? "ARCHIVED" : ((st == 5) ? "PENDING" : "READY");
                    const UiVariant stVar = (st == 0) ? UiVariant::Muted : ((st == 5) ? UiVariant::Warning : UiVariant::Success);
                    guikit::Tag(ui, stText, {.variant = stVar, .style = TagStyle::Soft, .size = g_tableDemo.cellWidgetSize});
                }
            }
        }
    } else {
        // Variant 2: Wide Grid (24 columns x 200 rows) — proves unlimited columns, frozen columns & O(1) CellAt() culling
        constexpr keel::usize kWideRows = 200;
        static const keel::StaticArray<TableColumn, 24> kWideCols = {{
            {.title = "ID",       .width = 76.0f,  .minWidth = 60.0f, .align = Align::Start,  .sortable = true},
            {.title = "Project",  .width = 148.0f, .minWidth = 96.0f, .align = Align::Start,  .sortable = true},
            {.title = "Team",     .width = 104.0f, .minWidth = 76.0f, .align = Align::Center, .sortable = false},
            {.title = "Region",   .width = 96.0f,  .minWidth = 72.0f, .align = Align::Center, .sortable = false},
            {.title = "Jan",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Feb",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Mar",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Q1 Total", .width = 88.0f,  .minWidth = 64.0f, .align = Align::End,    .sortable = false},
            {.title = "Apr",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "May",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Jun",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Q2 Total", .width = 88.0f,  .minWidth = 64.0f, .align = Align::End,    .sortable = false},
            {.title = "Jul",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Aug",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Sep",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Q3 Total", .width = 88.0f,  .minWidth = 64.0f, .align = Align::End,    .sortable = false},
            {.title = "Oct",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Nov",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Dec",      .width = 76.0f,  .minWidth = 56.0f, .align = Align::End,    .sortable = false},
            {.title = "Q4 Total", .width = 88.0f,  .minWidth = 64.0f, .align = Align::End,    .sortable = false},
            {.title = "Budget",   .width = 92.0f,  .minWidth = 72.0f, .align = Align::End,    .sortable = false},
            {.title = "Actual",   .width = 92.0f,  .minWidth = 72.0f, .align = Align::End,    .sortable = false},
            {.title = "Variance", .width = 92.0f,  .minWidth = 72.0f, .align = Align::End,    .sortable = false},
            {.title = "Status",   .width = 104.0f, .minWidth = 84.0f, .align = Align::Center, .sortable = false}
        }};

        char ftrBuf[128];
        keel::format_buf(
            ftrBuf, sizeof(ftrBuf),
            "24 Cols (O(1) CellAt) | Frozen: {} | Scroll cols {}..{}",
            g_tableDemo.wideFrozenColumns,
            g_tableDemo.lastFirstCol + 1,
            g_tableDemo.lastEndCol
        );

        if (auto table = guikit::Table(ui, kWideCols, {
            .totalRows = kWideRows,
            .selectedRow = &g_tableDemo.wideSelectedRow,
            .multiSelect = g_tableDemo.multiSelect,
            .sortColumn = &g_tableDemo.wideSortCol,
            .sortDirection = &g_tableDemo.wideSortDir,
            .stickyHeader = g_tableDemo.stickyHeader,
            .frozenColumns = keel::as<keel::u32>(keel::max(0, g_tableDemo.wideFrozenColumns)),
            .striped = g_tableDemo.striped,
            .gridLines = g_tableDemo.gridLines,
            .resizableColumns = g_tableDemo.resizableColumns,
            .showRowNumbers = g_tableDemo.showRowNumbers,
            .showFooter = g_tableDemo.showFooter,
            .footerText = ftrBuf,
            .emptyText = "No projects",
            .size = g_tableDemo.tableSize
        })) {
            const keel::usize frozenN = table.FrozenColumnCount();
            const keel::usize firstCol = table.FirstVisibleColumn();
            const keel::usize endCol = table.EndVisibleColumn();
            g_tableDemo.lastFirstCol = firstCol;
            g_tableDemo.lastEndCol = endCol;
            const bool desc = (g_tableDemo.wideSortDir == TableSortDirection::Descending);

            static constexpr keel::StaticArray<const char*, 5> kTeams = {
                "Platform", "Design", "Mobile", "Cloud", "Core"
            };
            static constexpr keel::StaticArray<const char*, 4> kRegions = {
                "NA", "EMEA", "APAC", "LATAM"
            };

            for (keel::usize r = table.FirstVisibleRow(); r < table.EndVisibleRow(); ++r) {
                const keel::usize idx = desc ? (kWideRows - 1 - r) : r;
                if (auto row = table.Row(r)) {
                    keel::StaticArray<keel::i32, 12> months{};
                    keel::StaticArray<keel::i32, 4> qTotals{};
                    keel::i32 yearTotal = 0;
                    for (keel::usize q = 0; q < 4; ++q) {
                        keel::i32 qSum = 0;
                        for (keel::usize m = 0; m < 3; ++m) {
                            const keel::i32 val = 12 + keel::as<keel::i32>(((idx + 1) * 7 + q * 11 + m * 5) % 45);
                            months[q * 3 + m] = val;
                            qSum += val;
                        }
                        qTotals[q] = qSum;
                        yearTotal += qSum;
                    }
                    const keel::i32 budget = 340 + keel::as<keel::i32>((idx * 19) % 160);
                    const keel::i32 variance = budget - yearTotal;

                    auto emitWideCell = [&](keel::usize c) {
                        if (auto cell = table.CellAt(c)) {
                            if (c == 0) {
                                guikit::Text(ui, ui.Format("PRJ-{:03}", idx + 1), {.variant = UiVariant::Secondary});
                            } else if (c == 1) {
                                guikit::Text(ui, ui.Format("Initiative #{}", idx + 1));
                            } else if (c == 2) {
                                guikit::Tag(ui, kTeams[idx % kTeams.size()], {.variant = UiVariant::Info, .style = TagStyle::Soft, .size = g_tableDemo.cellWidgetSize});
                            } else if (c == 3) {
                                guikit::Text(ui, kRegions[(idx * 3) % kRegions.size()], {.variant = UiVariant::Secondary});
                            } else if (c >= 4 && c < 20) {
                                const keel::usize rel = c - 4;
                                const keel::usize q = rel / 4;
                                const keel::usize slot = rel % 4;
                                if (slot < 3) {
                                    guikit::Text(ui, ui.Format("{}k", months[q * 3 + slot]));
                                } else {
                                    guikit::Text(ui, ui.Format("{}k", qTotals[q]), {.variant = UiVariant::Primary});
                                }
                            } else if (c == 20) {
                                guikit::Text(ui, ui.Format("{}k", budget));
                            } else if (c == 21) {
                                guikit::Text(ui, ui.Format("{}k", yearTotal));
                            } else if (c == 22) {
                                guikit::Text(ui, ui.Format("{:+}k", variance), {
                                    .variant = (variance >= 0) ? UiVariant::Success : UiVariant::Danger
                                });
                            } else if (c == 23) {
                                guikit::Tag(ui, (variance >= 0) ? "ON TRACK" : "OVER", {
                                    .variant = (variance >= 0) ? UiVariant::Success : UiVariant::Warning,
                                    .style = TagStyle::Soft,
                                    .size = g_tableDemo.cellWidgetSize
                                });
                            }
                        }
                    };

                    // Emit frozen left columns + visible scrollable columns only (O(1) 2D virtualization)
                    for (keel::usize c = 0; c < frozenN; ++c) {
                        emitWideCell(c);
                    }
                    for (keel::usize c = firstCol; c < endCol; ++c) {
                        emitWideCell(c);
                    }
                }
            }
        }
    }
}

keel::String GenerateTableCode(const DemoContext& ctx) {
    const keel::usize datasetMode = ctx.GetDemoVariant(NavCategory::ViewTable);
    if (datasetMode == 2u) {
        return keel::format(
            "// Unlimited columns + frozen left columns + O(1) CellAt() 2D virtualization\n"
            "if (auto table = guikit::Table(ui, wideColumns, {{\n"
            "    .totalRows = 200,\n"
            "    .selectedRow = &selectedRow,\n"
            "    .multiSelect = {},\n"
            "    .stickyHeader = {},\n"
            "    .frozenColumns = {},\n"
            "    .striped = {},\n"
            "    .gridLines = {},\n"
            "    .resizableColumns = {},\n"
            "    .showRowNumbers = {},\n"
            "    .showFooter = {},\n"
            "    .emptyText = \"No projects\"{}\n"
            "}})) {{\n"
            "    for (size_t r = table.FirstVisibleRow(); r < table.EndVisibleRow(); ++r) {{\n"
            "        if (auto row = table.Row(r)) {{\n"
            "            for (size_t c = 0; c < table.FrozenColumnCount(); ++c) {{\n"
            "                if (auto cell = table.CellAt(c)) guikit::Text(ui, FormatCell(r, c));\n"
            "            }}\n"
            "            for (size_t c = table.FirstVisibleColumn(); c < table.EndVisibleColumn(); ++c) {{\n"
            "                if (auto cell = table.CellAt(c)) guikit::Text(ui, FormatCell(r, c));\n"
            "            }}\n"
            "        }}\n"
            "    }}\n"
            "}}",
            g_tableDemo.multiSelect ? "true" : "false",
            g_tableDemo.stickyHeader ? "true" : "false",
            g_tableDemo.wideFrozenColumns,
            g_tableDemo.striped ? "true" : "false",
            g_tableDemo.gridLines ? "true" : "false",
            g_tableDemo.resizableColumns ? "true" : "false",
            g_tableDemo.showRowNumbers ? "true" : "false",
            g_tableDemo.showFooter ? "true" : "false",
            FormatUiSizePrefix(g_tableDemo.tableSize)
        );
    }

    const keel::StringRef cellSizeName = SizeName(g_tableDemo.cellWidgetSize);
    return keel::format(
        "static const keel::StaticArray<guikit::TableColumn, 7> cols = {{{{\n"
        "    {{.title = \"Done\",     .width = 56.0f,  .align = guikit::Align::Center, .sortable = true}},\n"
        "    {{.title = \"ID\",       .width = 80.0f,  .sortable = true}},\n"
        "    {{.title = \"Task\",     .flex = 2.0f,    .sortable = true}},\n"
        "    {{.title = \"Category\", .width = 114.0f, .sortable = true}},\n"
        "    {{.title = \"Progress\", .flex = 1.1f,    .sortable = true}},\n"
        "    {{.title = \"Status\",   .width = 110.0f, .align = guikit::Align::Center, .sortable = true}},\n"
        "    {{.title = \"Actions\",  .width = 90.0f,  .align = guikit::Align::Center}}\n"
        "}}}};\n\n"
        "if (auto table = guikit::Table(ui, cols, {{\n"
        "    .totalRows = rows.size(),\n"
        "    .selectedRow = &selectedRow,\n"
        "    .multiSelect = {},\n"
        "    .sortColumn = &sortCol,\n"
        "    .sortDirection = &sortDir,\n"
        "    .stickyHeader = {},\n"
        "    .frozenColumns = {},\n"
        "    .striped = {},\n"
        "    .gridLines = {},\n"
        "    .resizableColumns = {},\n"
        "    .showRowNumbers = {},\n"
        "    .showFooter = {},\n"
        "    .emptyText = \"No matching tasks\"{}\n"
        "}})) {{\n"
        "    for (size_t r = table.FirstVisibleRow(); r < table.EndVisibleRow(); ++r) {{\n"
        "        if (auto row = table.Row(r)) {{\n"
        "            guikit::Checkbox(ui, rows[r].completed, {{.size = guikit::UiSize::{}}});\n"
        "            guikit::Text(ui, ui.Format(\"TSK-{{}}\", rows[r].id));\n"
        "            guikit::Text(ui, rows[r].title);\n"
        "            guikit::Select(ui, rows[r].categoryIdx, categories, {{.searchable = true, .size = guikit::UiSize::{}, .width = guikit::SizeMode::Stretch}});\n"
        "            guikit::ProgressBar(ui, rows[r].progress, 0.0f, 100.0f, {{.valueFormat = \"%.0f%%\", .size = guikit::UiSize::{}}});\n"
        "            guikit::Tag(ui, rows[r].statusLabel, {{.variant = guikit::UiVariant::Success, .size = guikit::UiSize::{}}});\n"
        "            if (auto cell = table.Cell()) {{\n"
        "                guikit::Button(ui, {{.icon = guikit::Icon::Plus(), .square = true, .size = guikit::UiSize::{}}});\n"
        "                guikit::Button(ui, {{.icon = guikit::Icon::Refresh(), .square = true, .size = guikit::UiSize::{}}});\n"
        "            }}\n"
        "        }}\n"
        "    }}\n"
        "}}",
        g_tableDemo.multiSelect ? "true" : "false",
        g_tableDemo.stickyHeader ? "true" : "false",
        g_tableDemo.frozenColumns,
        g_tableDemo.striped ? "true" : "false",
        g_tableDemo.gridLines ? "true" : "false",
        g_tableDemo.resizableColumns ? "true" : "false",
        g_tableDemo.showRowNumbers ? "true" : "false",
        g_tableDemo.showFooter ? "true" : "false",
        FormatUiSizePrefix(g_tableDemo.tableSize),
        cellSizeName,
        cellSizeName,
        cellSizeName,
        cellSizeName,
        cellSizeName,
        cellSizeName
    );
}

} // namespace guikit::demo
