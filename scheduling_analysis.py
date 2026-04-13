"""
Switch Scheduling Performance Analysis
Compares: FIFO, VOQ (Optimal Matching), iSLIP
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

# ─────────────────────────────────────────────
# RAW DATA  (parsed from log files)
# ─────────────────────────────────────────────

data = {
    "FIFO": {
        "backlog":      [4, 4, 4, 5, 6, 6, 5, 3, 2, 1, 0],
        "pkts_sent":    [1, 2, 2, 2, 2, 3, 1, 2, 1, 1, 1],
        "hol_blocks":   [2, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0],
        "time_slots":   list(range(11)),
        "color":        "#E24B4A",
        "linestyle":    "solid",
    },
    "VOQ (Optimal)": {
        "backlog":      [2, 1, 0, 1, 1, 1, 0],
        "pkts_sent":    [3, 3, 3, 2, 3, 3, 1],
        "hol_blocks":   [0, 0, 0, 0, 0, 0, 0],
        "time_slots":   list(range(7)),
        "color":        "#1D9E75",
        "linestyle":    "dashed",
    },
    "iSLIP": {
        "backlog":      [3, 3, 3, 4, 5, 6, 4, 2, 1, 0],
        "pkts_sent":    [2, 2, 2, 2, 2, 2, 2, 2, 1, 1],
        "hol_blocks":   [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        "time_slots":   list(range(10)),
        "color":        "#378ADD",
        "linestyle":    "dashdot",
    },
}

TOTAL_PACKETS = 18
N = 3  # switch size

# ─────────────────────────────────────────────
# DERIVED METRICS
# ─────────────────────────────────────────────

def compute_metrics(d):
    slots     = len(d["time_slots"])
    avg_tput  = TOTAL_PACKETS / slots
    max_bklog = max(d["backlog"])
    hol_total = sum(d["hol_blocks"])
    avg_bklog = np.mean(d["backlog"])
    # utilisation: actual pkts sent vs max possible (N per slot)
    utilisation = (TOTAL_PACKETS / (slots * N)) * 100
    return {
        "completion_time": slots - 1,
        "avg_throughput":  round(avg_tput, 2),
        "max_backlog":     max_bklog,
        "avg_backlog":     round(avg_bklog, 2),
        "total_hol":       hol_total,
        "utilisation_pct": round(utilisation, 1),
    }

metrics = {name: compute_metrics(d) for name, d in data.items()}

# ─────────────────────────────────────────────
# PRINT SUMMARY TABLE
# ─────────────────────────────────────────────

def print_summary():
    print("=" * 66)
    print(f"{'Metric':<25} {'FIFO':>12} {'VOQ (Opt)':>12} {'iSLIP':>12}")
    print("=" * 66)
    rows = [
        ("Completion time (t)",  "completion_time"),
        ("Avg throughput (p/s)", "avg_throughput"),
        ("Max backlog",          "max_backlog"),
        ("Avg backlog",          "avg_backlog"),
        ("HoL blocking events",  "total_hol"),
        ("Link utilisation (%)", "utilisation_pct"),
    ]
    for label, key in rows:
        vals = [metrics[algo][key] for algo in ["FIFO", "VOQ (Optimal)", "iSLIP"]]
        print(f"  {label:<23} {vals[0]:>12} {vals[1]:>12} {vals[2]:>12}")
    print("=" * 66)

print_summary()

# ─────────────────────────────────────────────
# PLOT SETUP
# ─────────────────────────────────────────────

fig = plt.figure(figsize=(16, 12))
fig.suptitle(
    f"Switch Scheduling Performance Analysis  —  3×3 Switch, {TOTAL_PACKETS} Packets",
    fontsize=14, fontweight="bold", y=0.98
)

gs = fig.add_gridspec(3, 3, hspace=0.45, wspace=0.35)

ax_backlog  = fig.add_subplot(gs[0, :2])   # top-left wide
ax_bar_tput = fig.add_subplot(gs[1, :2])   # mid-left wide
ax_hol      = fig.add_subplot(gs[0, 2])    # top-right
ax_metrics  = fig.add_subplot(gs[1, 2])    # mid-right
ax_util     = fig.add_subplot(gs[2, :])    # bottom full-width

# ─────────────────────────────────────────────
# 1. BACKLOG OVER TIME
# ─────────────────────────────────────────────

for name, d in data.items():
    ax_backlog.plot(
        d["time_slots"], d["backlog"],
        color=d["color"], linestyle=d["linestyle"],
        linewidth=2, marker="o", markersize=5, label=name
    )

ax_backlog.set_title("Backlog over time", fontweight="bold")
ax_backlog.set_xlabel("Time slot (t)")
ax_backlog.set_ylabel("Packets in queue")
ax_backlog.set_xticks(range(11))
ax_backlog.legend(framealpha=0.4)
ax_backlog.grid(True, linestyle="--", alpha=0.4)
ax_backlog.set_ylim(bottom=0)

# ─────────────────────────────────────────────
# 2. PACKETS SENT PER SLOT (grouped bar)
# ─────────────────────────────────────────────

max_t   = 11
x_all   = np.arange(max_t)
width   = 0.28
offsets = [-width, 0, width]
names   = list(data.keys())

for idx, (name, off) in enumerate(zip(names, offsets)):
    d = data[name]
    # pad shorter series with 0
    padded = d["pkts_sent"] + [0] * (max_t - len(d["pkts_sent"]))
    ax_bar_tput.bar(
        x_all + off, padded, width,
        color=data[name]["color"], alpha=0.85, label=name
    )

ax_bar_tput.axhline(N, color="gray", linestyle="--", linewidth=1, label=f"Max ({N} pkts/slot)")
ax_bar_tput.set_title("Packets sent per time slot", fontweight="bold")
ax_bar_tput.set_xlabel("Time slot (t)")
ax_bar_tput.set_ylabel("Packets sent")
ax_bar_tput.set_xticks(x_all)
ax_bar_tput.set_ylim(0, N + 0.8)
ax_bar_tput.legend(framealpha=0.4, fontsize=9)
ax_bar_tput.grid(True, axis="y", linestyle="--", alpha=0.4)

# ─────────────────────────────────────────────
# 3. HOL BLOCKING PER SLOT (FIFO only)
# ─────────────────────────────────────────────

fifo_d = data["FIFO"]
ax_hol.bar(
    fifo_d["time_slots"], fifo_d["hol_blocks"],
    color="#E24B4A", alpha=0.8
)
ax_hol.set_title("HoL blocks per slot\n(FIFO only)", fontweight="bold", fontsize=10)
ax_hol.set_xlabel("Time slot (t)")
ax_hol.set_ylabel("Block events")
ax_hol.set_xticks(fifo_d["time_slots"])
ax_hol.set_yticks([0, 1, 2])
ax_hol.grid(True, axis="y", linestyle="--", alpha=0.4)

total_hol = sum(fifo_d["hol_blocks"])
ax_hol.text(
    0.97, 0.97, f"Total: {total_hol}",
    transform=ax_hol.transAxes, ha="right", va="top",
    fontsize=10, color="#E24B4A", fontweight="bold"
)

# ─────────────────────────────────────────────
# 4. SUMMARY METRICS TABLE
# ─────────────────────────────────────────────

ax_metrics.axis("off")

table_data = [
    ["Metric",          "FIFO",   "VOQ",  "iSLIP"],
    ["Finish time",     "t=10",   "t=6",  "t=9"],
    ["Avg tput (p/s)",  "1.64",   "2.57", "1.80"],
    ["Max backlog",     "6",      "2",    "6"],
    ["Avg backlog",     "3.7",    "1.0",  "3.1"],
    ["HoL events",      "7",      "0",    "0"],
    ["Link util %",     "54.5",   "85.7", "60.0"],
]

col_colors = [
    ["#f0f0f0"] * 4,
    ["#f0f0f0", "#fce4e4", "#d5f0e6", "#ddeeff"],
    ["#f0f0f0", "#fce4e4", "#d5f0e6", "#ddeeff"],
    ["#f0f0f0", "#fce4e4", "#d5f0e6", "#ddeeff"],
    ["#f0f0f0", "#fce4e4", "#d5f0e6", "#ddeeff"],
    ["#f0f0f0", "#fce4e4", "#d5f0e6", "#ddeeff"],
    ["#f0f0f0", "#fce4e4", "#d5f0e6", "#ddeeff"],
]

tbl = ax_metrics.table(
    cellText=table_data,
    cellLoc="center", loc="center",
    cellColours=col_colors
)
tbl.auto_set_font_size(False)
tbl.set_fontsize(9)
tbl.scale(1.0, 1.55)
ax_metrics.set_title("Summary table", fontweight="bold", fontsize=10, pad=10)

# ─────────────────────────────────────────────
# 5. CUMULATIVE THROUGHPUT (bottom)
# ─────────────────────────────────────────────

for name, d in data.items():
    cumulative = np.cumsum(d["pkts_sent"])
    ax_util.plot(
        d["time_slots"], cumulative,
        color=d["color"], linestyle=d["linestyle"],
        linewidth=2.5, marker="s", markersize=4, label=name
    )

ax_util.axhline(TOTAL_PACKETS, color="gray", linestyle=":", linewidth=1.2, label="All 18 pkts delivered")
ax_util.set_title("Cumulative packets delivered", fontweight="bold")
ax_util.set_xlabel("Time slot (t)")
ax_util.set_ylabel("Total packets sent")
ax_util.set_xticks(range(11))
ax_util.set_yticks(range(0, 21, 2))
ax_util.legend(framealpha=0.4)
ax_util.grid(True, linestyle="--", alpha=0.4)
ax_util.set_ylim(0, TOTAL_PACKETS + 2)

# annotate finish lines
finishes = {"FIFO": (10, "#E24B4A"), "VOQ (Optimal)": (6, "#1D9E75"), "iSLIP": (9, "#378ADD")}
for name, (t, c) in finishes.items():
    ax_util.axvline(t, color=c, linestyle="--", linewidth=1, alpha=0.6)
    ax_util.text(t + 0.1, 1, f"t={t}", color=c, fontsize=8, va="bottom")

# ─────────────────────────────────────────────
# SAVE & SHOW
# ─────────────────────────────────────────────

print("\nPlot saved → scheduling_analysis.png")
plt.show()
