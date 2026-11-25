#include "../demo_context.hpp"
#include "keel/math.h"

namespace guikit::demo {

namespace {

struct ServiceNodeRow {
    keel::i32 id{0};
    keel::FixedString<64> name{};
    keel::FixedString<32> region{};
    keel::f32 latencyMs{0.0f};
    keel::f32 deltaPct{0.0f};
    keel::f32 cpuLoadPct{0.0f};
    keel::i32 status{0}; // 0=HEALTHY, 1=ACTIVE, 2=DEGRADED, 3=OVERLOAD
    bool enabled{true};
};

struct TableDemoState {
    UiSize tableSize{UiSize::Md};
    bool striped{true};
    bool gridLines{true};
    bool stickyHeader{true};
    bool resizableColumns{true};
    bool showRowNumbers{true};
    bool showFooter{true};
    bool liveMetrics{true};

    keel::SmallString<128> filterQuery{};
    keel::i32 selectedRow{0};
    keel::i32 sortCol{2}; // Default sorted by Service
    TableSortDirection sortDir{TableSortDirection::Ascending};

    keel::i32 virtualSelectedRow{0};
    keel::i32 virtualSortCol{0};
    TableSortDirection virtualSortDir{TableSortDirection::Ascending};
    keel::i32 pendingJumpRow{-1};
    keel::usize lastVisibleCount{0};

    keel::Array<ServiceNodeRow> rows{};
    bool initialized{false};

    void EnsureInitialized() {
        if (initialized) return;
        initialized = true;

        static constexpr keel::StaticArray<const char*, 8> kServices = {
            "API-GATEWAY", "AUTH-SERVICE", "CACHE-CLUSTER", "SEARCH-INDEX",
            "MEDIA-WORKER", "BILLING-JOB", "STORAGE-NODE", "WEB-FRONTEND"
        };
        static constexpr keel::StaticArray<const char*, 6> kRegions = {
            "us-east", "eu-west", "eu-central", "ap-south", "us-west", "ap-east"
        };

        rows.clear();
        rows.reserve(48);
        for (keel::i32 i = 0; i < 48; ++i) {
            ServiceNodeRow r;
            r.id = 1001 + i;
            r.name = keel::format_fixed<64>(
                "{}-{}",
                kServices[keel::as<keel::usize>(i) % kServices.size()],
                (i / 8) + 1
            );
            r.region = kRegions[keel::as<keel::usize>(i * 3) % kRegions.size()];
            r.latencyMs = 8.5f + keel::as<keel::f32>(i) * 1.35f;
            r.deltaPct = -8.0f + keel::fmod(keel::as<keel::f32>(i) * 2.9f, 16.0f);
            r.cpuLoadPct = 18.0f + keel::fmod(keel::as<keel::f32>(i) * 17.3f, 76.0f);
            r.status = (r.cpuLoadPct > 84.0f) ? 3 : ((r.cpuLoadPct > 62.0f) ? 2 : ((r.cpuLoadPct > 38.0f) ? 1 : 0));
            r.enabled = (i % 3 != 2);
            rows.push_back(r);
        }
        SortRows();
    }

    void SortRows() {
        if (sortCol < 0 || sortDir == TableSortDirection::None) return;
        const bool asc = (sortDir == TableSortDirection::Ascending);
        keel::sort(rows, [&](const ServiceNodeRow& a, const ServiceNodeRow& b) {
            switch (sortCol) {
                case 0: return asc ? (a.enabled > b.enabled) : (a.enabled < b.enabled);
                case 1: return asc ? (a.id < b.id) : (a.id > b.id);
                case 2: return asc ? (a.name.as_ref() < b.name.as_ref()) : (a.name.as_ref() > b.name.as_ref());
                case 3: return asc ? (a.region.as_ref() < b.region.as_ref()) : (a.region.as_ref() > b.region.as_ref());
                case 4: return asc ? (a.latencyMs < b.latencyMs) : (a.latencyMs > b.latencyMs);
                case 5: return asc ? (a.deltaPct < b.deltaPct) : (a.deltaPct > b.deltaPct);
                case 6: return asc ? (a.cpuLoadPct < b.cpuLoadPct) : (a.cpuLoadPct > b.cpuLoadPct);
                case 7: return asc ? (a.status < b.status) : (a.status > b.status);
                default: return false;
            }
        });
    }
};

TableDemoState g_tableDemo;

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

    if (datasetMode == 0u) {
        if (auto el = guikit::Element(ui, {
            .label = "Filter Rows",
            .layout = ElementLayout::Top
        })) {
            if (guikit::InputField(ui, g_tableDemo.filterQuery, {
                .placeholder = "Service or region...",
                .prefix = Icon::Search(),
                .clearable = true,
                .size = UiSize::Sm
            }).changed) {
                g_tableDemo.selectedRow = 0;
            }
        }
        if (auto el = guikit::Element(ui, {
            .label = "Live Metrics Simulation",
            .description = "Animate CPU load bars and delta values in real time"
        })) {
            if (guikit::Switch(ui, g_tableDemo.liveMetrics).changed) {
                ui.InvalidateAll();
            }
        }
    } else {
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
    }
}

void RenderTableStage(guikit::Context& ui, DemoContext& ctx) {
    g_tableDemo.EnsureInitialized();
    const keel::usize datasetMode = ctx.GetDemoVariant(NavCategory::ViewTable);

    if (datasetMode == 0u) {
        // Animate live metrics if enabled
        if (g_tableDemo.liveMetrics) {
            const keel::f32 t = keel::as<keel::f32>((ui.GetCurrentTime)());
            for (keel::usize i = 0; i < g_tableDemo.rows.size(); ++i) {
                auto& r = g_tableDemo.rows[i];
                if (!r.enabled) continue;
                const keel::f32 wave = keel::sin(t * 2.2f + keel::as<keel::f32>(i) * 0.7f);
                r.cpuLoadPct = keel::clamp(50.0f + wave * 35.0f + keel::as<keel::f32>(i % 5) * 3.5f, 8.0f, 98.5f);
                r.deltaPct = keel::cos(t * 1.1f + keel::as<keel::f32>(i)) * 9.5f;
                r.status = (r.cpuLoadPct > 84.0f) ? 3 : ((r.cpuLoadPct > 62.0f) ? 2 : ((r.cpuLoadPct > 38.0f) ? 1 : 0));
            }
            ui.RequestRepaint(2);
        }

        // Build filtered indices
        keel::SmallArray<keel::usize, 64> filteredIndices;
        const keel::StringRef query = g_tableDemo.filterQuery.as_ref();
        for (keel::usize i = 0; i < g_tableDemo.rows.size(); ++i) {
            if (query.empty()) {
                filteredIndices.push_back(i);
            } else {
                const auto& r = g_tableDemo.rows[i];
                if (r.name.as_ref().contains(query) || r.region.as_ref().contains(query)) {
                    filteredIndices.push_back(i);
                }
            }
        }

        static const keel::StaticArray<TableColumn, 8> kServiceCols = {{
            {.title = "Active",   .width = 62.0f,  .minWidth = 52.0f, .align = Align::Center, .sortable = true},
            {.title = "Node ID",  .width = 86.0f,  .minWidth = 68.0f, .align = Align::Start,  .sortable = true},
            {.title = "Service",  .flex = 1.5f,    .minWidth = 130.0f,.align = Align::Start,  .sortable = true},
            {.title = "Region",   .width = 102.0f, .minWidth = 80.0f, .align = Align::Center, .sortable = true},
            {.title = "Latency",  .width = 116.0f, .minWidth = 90.0f, .align = Align::End,    .sortable = true},
            {.title = "Delta",    .width = 96.0f,  .minWidth = 80.0f, .align = Align::End,    .sortable = true},
            {.title = "CPU Load", .flex = 1.4f,    .minWidth = 150.0f,.align = Align::Start,  .sortable = true},
            {.title = "Status",   .width = 104.0f, .minWidth = 84.0f, .align = Align::Center, .sortable = true}
        }};

        const keel::StringRef selName =
            (g_tableDemo.selectedRow >= 0 && keel::as<keel::usize>(g_tableDemo.selectedRow) < filteredIndices.size())
                ? g_tableDemo.rows[filteredIndices[keel::as<keel::usize>(g_tableDemo.selectedRow)]].name.as_ref()
                : keel::StringRef("None");

        char footerBuf[128];
        keel::format_buf(
            footerBuf, sizeof(footerBuf),
            "Selected: {} | Double-click column divider to reset width",
            selName
        );

        if (auto table = guikit::Table(ui, kServiceCols, {
            .totalRows = filteredIndices.size(),
            .selectedRow = &g_tableDemo.selectedRow,
            .sortColumn = &g_tableDemo.sortCol,
            .sortDirection = &g_tableDemo.sortDir,
            .stickyHeader = g_tableDemo.stickyHeader,
            .striped = g_tableDemo.striped,
            .gridLines = g_tableDemo.gridLines,
            .resizableColumns = g_tableDemo.resizableColumns,
            .showRowNumbers = g_tableDemo.showRowNumbers,
            .showFooter = g_tableDemo.showFooter,
            .footerText = footerBuf,
            .size = g_tableDemo.tableSize
        })) {
            if (table.SortChanged()) {
                g_tableDemo.SortRows();
            }

            for (keel::usize visRow = table.FirstVisibleRow(); visRow < table.EndVisibleRow(); ++visRow) {
                if (visRow >= filteredIndices.size()) break;
                ServiceNodeRow& item = g_tableDemo.rows[filteredIndices[visRow]];

                TableRowOptions rowOpts{};
                if (item.status == 3) {
                    rowOpts.tint = UiVariant::Danger;
                }

                if (auto row = table.Row(visRow, rowOpts)) {
                    // Col 0: Embedded Checkbox widget
                    guikit::Checkbox(ui, item.enabled, {.size = UiSize::Sm, .color = StateColor::Primary()});

                    // Col 1: Node ID
                    guikit::Text(ui, ui.Format("SRV-{:04d}", item.id), {.variant = UiVariant::Secondary});

                    // Col 2: Service name with leading icon
                    guikit::Element(ui, {.label = item.name.as_ref(), .icon = Icon::Play(), .width = SizeMode::Fit});

                    // Col 3: Region badge
                    guikit::Element(ui, {.badge = {item.region.as_ref(), UiVariant::Info}, .width = SizeMode::Fit});

                    // Col 4: Latency
                    guikit::Text(ui, ui.Format("{:.1f} ms", item.latencyMs));

                    // Col 5: Trend delta
                    guikit::Text(ui,
                        ui.Format("{:+04.1f}%", item.deltaPct),
                        {.variant = (item.deltaPct <= 0.0f) ? UiVariant::Success : UiVariant::Default}
                    );

                    // Col 6: Embedded Segmented ProgressBar widget
                    guikit::ProgressBar(ui, item.cpuLoadPct, 0.0f, 100.0f, {
                        .valueFormat = "%.0f%%",
                        .segments = 12,
                        .multiZoneColor = true,
                        .size = UiSize::Sm
                    });

                    // Col 7: Status pill badge
                    static constexpr keel::StaticArray<const char*, 4> kStatusLabels = {"HEALTHY", "ACTIVE", "DEGRADED", "OVERLOAD"};
                    static constexpr keel::StaticArray<UiVariant, 4> kStatusVars = {
                        UiVariant::Success, UiVariant::Primary, UiVariant::Warning, UiVariant::Danger
                    };
                    const keel::usize stIdx = keel::as<keel::usize>(keel::clamp(item.status, 0, 3));
                    guikit::Element(ui, {.badge = {kStatusLabels[stIdx], kStatusVars[stIdx]}, .width = SizeMode::Fit});
                }
            }
        }
    } else {
        // Mode 1: 100,000 Rows Virtualized Stress-Test
        constexpr keel::usize kTotalVirtualRows = 100000;

        static const keel::StaticArray<TableColumn, 6> kVirtualCols = {{
            {.title = "Event ID",  .width = 110.0f, .minWidth = 80.0f,  .align = Align::Start,  .sortable = true},
            {.title = "Timestamp", .width = 125.0f, .minWidth = 95.0f,  .align = Align::Start,  .sortable = true},
            {.title = "Service",   .flex = 1.3f,    .minWidth = 120.0f, .align = Align::Start,  .sortable = true},
            {.title = "Latency",   .width = 125.0f, .minWidth = 95.0f,  .align = Align::End,    .sortable = true},
            {.title = "CPU Load",  .flex = 1.2f,    .minWidth = 140.0f, .align = Align::Start,  .sortable = true},
            {.title = "Result",    .width = 105.0f, .minWidth = 85.0f,  .align = Align::Center, .sortable = true}
        }};

        char ftrBuf[128];
        keel::format_buf(
            ftrBuf, sizeof(ftrBuf),
            "Virtualized O(1) Clipper: Materialized {} rows in DOM out of 100,000",
            g_tableDemo.lastVisibleCount
        );

        if (auto table = guikit::Table(ui, kVirtualCols, {
            .totalRows = kTotalVirtualRows,
            .selectedRow = &g_tableDemo.virtualSelectedRow,
            .sortColumn = &g_tableDemo.virtualSortCol,
            .sortDirection = &g_tableDemo.virtualSortDir,
            .stickyHeader = g_tableDemo.stickyHeader,
            .striped = g_tableDemo.striped,
            .gridLines = g_tableDemo.gridLines,
            .resizableColumns = g_tableDemo.resizableColumns,
            .showRowNumbers = g_tableDemo.showRowNumbers,
            .showFooter = g_tableDemo.showFooter,
            .footerText = ftrBuf,
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

            for (keel::usize r = firstRow; r < endRow; ++r) {
                const keel::usize logicalIdx = desc ? (kTotalVirtualRows - 1 - r) : r;
                if (auto row = table.Row(r)) {
                    guikit::Text(ui, ui.Format("EVT-{:06}", logicalIdx + 1), {.variant = UiVariant::Secondary});

                    const keel::u32 mins = keel::as<keel::u32>((logicalIdx / 600) % 60);
                    const keel::u32 secs = keel::as<keel::u32>((logicalIdx / 10) % 60);
                    const keel::u32 tenths = keel::as<keel::u32>(logicalIdx % 10);
                    guikit::Text(ui, ui.Format("12:{:02}:{:02}.{}00", mins, secs, tenths));

                    static constexpr keel::StaticArray<const char*, 6> kNodes = {
                        "API-GATEWAY-01", "AUTH-SERVICE-04", "CACHE-CLUSTER-02",
                        "SEARCH-INDEX-09", "MEDIA-WORKER-03", "STORAGE-NODE-07"
                    };
                    guikit::Text(ui, kNodes[logicalIdx % kNodes.size()]);

                    const keel::f32 latency = 4.0f + keel::as<keel::f32>(logicalIdx % 250) * 0.35f;
                    guikit::Text(ui, ui.Format("{:.1f} ms", latency));

                    const keel::f32 load = 12.0f + keel::as<keel::f32>((logicalIdx * 17) % 85);
                    guikit::ProgressBar(ui, load, 0.0f, 100.0f, {
                        .valueFormat = "%.0f%%",
                        .segments = 10,
                        .multiZoneColor = true,
                        .size = UiSize::Xs
                    });

                    const bool ok = (logicalIdx % 19 != 0);
                    guikit::Element(ui, {
                        .badge = {ok ? "200 OK" : "RETRY", ok ? UiVariant::Success : UiVariant::Warning},
                        .width = SizeMode::Fit
                    });
                }
            }
        }
    }
}

keel::String GenerateTableCode(const DemoContext& /*ctx*/) {
    return keel::format(
        "static const keel::StaticArray<guikit::TableColumn, 5> cols = {{{{\n"
        "    {{.title = \"ID\",       .width = 80.0f,  .sortable = true}},\n"
        "    {{.title = \"Service\",  .flex = 1.5f,    .sortable = true}},\n"
        "    {{.title = \"Latency\",  .width = 120.0f, .align = guikit::Align::End, .sortable = true}},\n"
        "    {{.title = \"CPU Load\", .flex = 1.4f,    .sortable = true}},\n"
        "    {{.title = \"Status\",   .width = 100.0f, .align = guikit::Align::Center, .sortable = true}}\n"
        "}}}};\n\n"
        "if (auto table = guikit::Table(ui, cols, {{\n"
        "    .totalRows = rows.size(),\n"
        "    .selectedRow = &selectedRow,\n"
        "    .sortColumn = &sortCol,\n"
        "    .sortDirection = &sortDir,\n"
        "    .stickyHeader = {},\n"
        "    .striped = {},\n"
        "    .gridLines = {},\n"
        "    .resizableColumns = {},\n"
        "    .showRowNumbers = {},\n"
        "    .showFooter = {}{}\n"
        "}})) {{\n"
        "    for (size_t r = table.FirstVisibleRow(); r < table.EndVisibleRow(); ++r) {{\n"
        "        if (auto row = table.Row(r)) {{\n"
        "            guikit::Text(ui, ui.Format(\"SRV-{{:04d}}\", rows[r].id));\n"
        "            guikit::Text(ui, rows[r].name);\n"
        "            guikit::Text(ui, ui.Format(\"{{:.1f}} ms\", rows[r].latencyMs));\n"
        "            guikit::ProgressBar(ui, rows[r].cpuLoadPct, 0.0f, 100.0f, {{.segments = 12, .multiZoneColor = true}});\n"
        "            guikit::Element(ui, {{.badge = {{\"HEALTHY\", guikit::UiVariant::Success}}, .width = guikit::SizeMode::Fit}});\n"
        "        }}\n"
        "    }}\n"
        "}}",
        g_tableDemo.stickyHeader ? "true" : "false",
        g_tableDemo.striped ? "true" : "false",
        g_tableDemo.gridLines ? "true" : "false",
        g_tableDemo.resizableColumns ? "true" : "false",
        g_tableDemo.showRowNumbers ? "true" : "false",
        g_tableDemo.showFooter ? "true" : "false",
        FormatUiSizePrefix(g_tableDemo.tableSize)
    );
}

} // namespace guikit::demo
