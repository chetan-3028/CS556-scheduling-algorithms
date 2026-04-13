# CS556-scheduling-algorithms

# 🔀 Network Switch Scheduling Algorithms

<div align="center">

![Language](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus&logoColor=white)
![Python](https://img.shields.io/badge/Python-3.7%2B-yellow?style=for-the-badge&logo=python&logoColor=white)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=for-the-badge)
![Switch](https://img.shields.io/badge/Switch-3%C3%973%20Crossbar-orange?style=for-the-badge)
![Packets](https://img.shields.io/badge/Packets-18-purple?style=for-the-badge)

**A simulation-based comparison of FIFO, VOQ (Optimal Matching), and iSLIP scheduling algorithms on a 3×3 crossbar network switch.**

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Project Structure](#-project-structure)
- [Prerequisites](#-prerequisites)
- [Quick Start](#-quick-start)
- [Part 1 — FIFO](#-part-1--fifo)
- [Part 2 — VOQ Optimal Matching](#-part-2--voq-optimal-matching)
- [Part 3 — iSLIP](#-part-3--islip)
- [Part 4 — Data Analysis](#-part-4--data-analysis)
- [Output Files](#-output-files)
- [Algorithm Reference](#-algorithm-reference)
- [Results](#-results)

---

## 🧭 Overview

This project simulates three progressively sophisticated packet scheduling strategies for a **3×3 network switch** with inputs I₀, I₁, I₂ and outputs O₀, O₁, O₂. All simulations run on the **same 18-packet input trace**, enabling a direct controlled performance comparison.

| Part | File | Algorithm | Complexity |
|:----:|------|-----------|:----------:|
| 1 | `performance.cpp` | FIFO (single queue per input) | O(N) |
| 2 | `performance2.cpp` | VOQ + Optimal Brute-Force Matching | O(N!) |
| 3 | `performance3.cpp` | VOQ + iSLIP Round-Robin Arbitration | O(N) |
| 4 | `scheduling_analysis.py` | Performance Visualization & Analysis | — |

> [!NOTE]
> The switch operates in **synchronous time slots** (t = 0, 1, 2, …). Each port can transmit or receive exactly **one packet per time slot**.

---

## 📁 Project Structure

```
network-switch-scheduling/
│
├── 📄 performance.cpp            # Part 1 — FIFO simulation
├── 📄 performance2.cpp           # Part 2 — VOQ with optimal (exhaustive) matching
├── 📄 performance3.cpp           # Part 3 — VOQ with iSLIP scheduling
├── 📄 scheduling_analysis.py     # Part 4 — Data analysis & graph generation
│
├── 📋 fifo_log.txt               # Generated output log — Part 1
├── 📋 voq_log.txt                # Generated output log — Part 2
├── 📋 islip_log.txt              # Generated output log — Part 3
│
└── 📖 README.md
```

---

## ⚙️ Prerequisites

### C++ Compiler

Requires a **C++17** compatible compiler.

```bash
# Check GCC version (needs 7.0+)
g++ --version

# Check Clang version (needs 5.0+)
clang++ --version
```

### Python Packages

```bash
pip install matplotlib numpy
```

> [!TIP]
> Using a virtual environment is recommended to avoid dependency conflicts.
> ```bash
> python3 -m venv venv
> source venv/bin/activate      # macOS/Linux
> venv\Scripts\activate         # Windows
> pip install matplotlib numpy
> ```

---

## ⚡ Quick Start

Run all three simulations and generate the analysis in one go:

```bash
# Compile all three
g++ -std=c++17 -O2 -o fifo   performance.cpp
g++ -std=c++17 -O2 -o voq    performance2.cpp
g++ -std=c++17 -O2 -o islip  performance3.cpp

# Run all three (generates the three log files)
./fifo
./voq
./islip

# Run analysis (reads the log files, generates the chart)
python3 scheduling_analysis.py
```

> [!IMPORTANT]
> Run all three C++ binaries **before** running the Python script — the analysis reads from `fifo_log.txt`, `voq_log.txt`, and `islip_log.txt`.

---

## 🟥 Part 1 — FIFO

### How It Works

Each input port maintains a **single First-In-First-Out queue**. Only the packet at the absolute head of each queue is eligible for forwarding in any given slot. If the head packet's destination output is already claimed, the **entire queue stalls** — this is Head-of-Line (HoL) blocking.

> [!WARNING]
> **Head-of-Line Blocking:** A packet destined for a free output port can be trapped behind a blocked head packet, wasting available switch capacity.

### Compile & Run

```bash
g++ -std=c++17 -O2 -o fifo performance.cpp
./fifo
```

<details>
<summary>📄 Expected Output (click to expand)</summary>

```
t=0: HoL BLOCK at input 1 for pid 3
t=0: HoL BLOCK at input 2 for pid 5
t=0: Sent (1,0,0)
t=0: Backlog = 4
t=1: HoL BLOCK at input 2 for pid 5
t=1: Sent (2,0,1) (3,1,0)
t=1: Backlog = 4
t=2: HoL BLOCK at input 1 for pid 4
t=2: Sent (6,0,2) (5,2,0)
t=2: Backlog = 4
t=3: HoL BLOCK at input 2 for pid 7
t=3: Sent (10,0,1) (4,1,2)
t=3: Backlog = 5
t=4: HoL BLOCK at input 2 for pid 7
t=4: Sent (13,0,0) (8,1,1)
t=4: Backlog = 6
t=5: Sent (16,0,2) (11,1,0) (7,2,1)
t=5: Backlog = 6
t=6: HoL BLOCK at input 2 for pid 9
t=6: Sent (14,1,2)
t=6: Backlog = 5
t=7: Sent (17,1,1) (9,2,2)
t=7: Backlog = 3
t=8: Sent (12,2,1)
t=8: Backlog = 2
t=9: Sent (15,2,2)
t=9: Backlog = 1
t=10: Sent (18,2,0)
t=10: Backlog = 0
Logs written to fifo_log.txt
```

</details>

**Log format:** `(packet_id, source_input, destination_output)`

**Output file generated:** `fifo_log.txt`

---

## 🟩 Part 2 — VOQ Optimal Matching

### How It Works

Each input port maintains **N = 3 separate Virtual Output Queues** — one per destination output (VOQ\[input]\[output]). This eliminates HoL blocking entirely. A brute-force scheduler enumerates **all N! = 6 permutations** of output assignments each slot and selects the one that maximises simultaneously forwarded packets.

### Compile & Run

```bash
g++ -std=c++17 -O2 -o voq performance2.cpp
./voq
```

<details>
<summary>📄 Expected Output (click to expand)</summary>

```
t=0: Sent [(2,0,1), (4,1,2), (5,2,0)]
t=0: Backlog = 2
t=1: Sent [(6,0,2), (3,1,0), (7,2,1)]
t=1: Backlog = 1
t=2: Sent [(1,0,0), (8,1,1), (9,2,2)]
t=2: Backlog = 0
t=3: Sent [(10,0,1), (11,1,0)]
t=3: Backlog = 1
t=4: Sent [(13,0,0), (14,1,2), (12,2,1)]
t=4: Backlog = 1
t=5: Sent [(16,0,2), (17,1,1), (18,2,0)]
t=5: Backlog = 1
t=6: Sent [(15,2,2)]
t=6: Backlog = 0
Logs written to voq_log.txt
```

</details>

**Log format:** `[(packet_id, source_input, destination_output), ...]`

**Output file generated:** `voq_log.txt`

---

## 🟦 Part 3 — iSLIP

### How It Works

Uses the same VOQ architecture as Part 2, but replaces the O(N!) exhaustive search with the **iSLIP algorithm** — an O(N) iterative round-robin arbitration scheme viable for real hardware.

Each time slot runs **one iSLIP iteration** comprising three phases:

```
┌─────────────────────────────────────────────────────┐
│  iSLIP — Single Iteration                           │
│                                                     │
│  1. REQUEST  →  Input i requests output o           │
│                 for all non-empty VOQ[i][o]         │
│                                                     │
│  2. GRANT    →  Output o grants one input           │
│                 via round-robin from output_ptr[o]  │
│                                                     │
│  3. ACCEPT   →  Input i accepts one grant           │
│                 via round-robin from input_ptr[i]   │
│                                                     │
│  4. UPDATE   →  Matched pointers advance:           │
│                 input_ptr[i]  = (accepted_o+1) % N  │
│                 output_ptr[o] = (accepted_i+1) % N  │
└─────────────────────────────────────────────────────┘
```

> [!NOTE]
> **Crucial Rule:** Pointers only advance when a match is made. Unmatched arbiters keep their pointer position for the next slot. This **desynchronisation** ensures long-run fairness across all flows.

### Compile & Run

```bash
g++ -std=c++17 -O2 -o islip performance3.cpp
./islip
```

<details>
<summary>📄 Expected Output (click to expand)</summary>

```
t=0: Sent [(1,0,0), (4,1,2)]
t=0: Backlog = 3
t=1: Sent [(2,0,1), (3,1,0)]
t=1: Backlog = 3
t=2: Sent [(8,1,1), (5,2,0)]
t=2: Backlog = 3
t=3: Sent [(11,1,0), (7,2,1)]
t=3: Backlog = 4
t=4: Sent [(13,0,0), (9,2,2)]
t=4: Backlog = 5
t=5: Sent [(10,0,1), (18,2,0)]
t=5: Backlog = 6
t=6: Sent [(6,0,2), (17,1,1)]
t=6: Backlog = 4
t=7: Sent [(14,1,2), (12,2,1)]
t=7: Backlog = 2
t=8: Sent [(15,2,2)]
t=8: Backlog = 1
t=9: Sent [(16,0,2)]
t=9: Backlog = 0
Logs written to islip_log.txt
```

</details>

**Log format:** `[(packet_id, source_input, destination_output), ...]`

**Output file generated:** `islip_log.txt`

---

## 📊 Part 4 — Data Analysis

### Run

```bash
python3 scheduling_analysis.py
```

### What It Generates

**Console — Metrics Summary Table:**

```
==================================================================
Metric                            FIFO    VOQ (Opt)        iSLIP
==================================================================
  Completion time (t)               10            6            9
  Avg throughput (p/s)            1.64         2.57          1.8
  Max backlog                        6            2            6
  Avg backlog                     3.64         0.86          3.1
  HoL blocking events                7            0            0
  Link utilisation (%)            54.5         85.7         60.0
==================================================================
```

**`scheduling_analysis.png` — 5-panel figure:**

| Panel | Chart Type | Description |
|-------|-----------|-------------|
| Top-left | Line chart | Backlog over time — all 3 algorithms |
| Mid-left | Grouped bar | Packets sent per time slot |
| Top-right | Bar chart | HoL blocking events per slot (FIFO only) |
| Mid-right | Table | Summary metrics |
| Bottom | Line chart | Cumulative packets delivered with finish-time markers |

**Output file generated:** `scheduling_analysis.png`

---

## 📂 Output Files

| File | Created by | Description |
|------|-----------|-------------|
| `fifo_log.txt` | `./fifo` | Slot-by-slot log: sent packets, HoL events, backlog |
| `voq_log.txt` | `./voq` | Slot-by-slot log: optimal matched packets, backlog |
| `islip_log.txt` | `./islip` | Slot-by-slot log: iSLIP matched packets, backlog |
| `scheduling_analysis.png` | `python3 scheduling_analysis.py` | Combined 5-panel performance chart |

---

## 📚 Algorithm Reference

<details>
<summary><b>FIFO — Standard Input Queuing</b></summary>

```
For each time slot t:
  1. Enqueue arriving packets to input queue[i]
  2. For i = 0 → N-1:
       pkt ← queue[i].front()
       if pkt.output not in used_outputs:
           send pkt, mark output used
       else:
           log HoL block for pkt.pid
```

**Tie-breaking:** Lowest-numbered input port wins (I₀ beats I₁ beats I₂).

</details>

<details>
<summary><b>VOQ Optimal — Exhaustive Bipartite Matching</b></summary>

```
For each time slot t:
  1. Enqueue arriving packets to VOQ[input][output]
  2. best_match ← []
  3. For each permutation P of [0, 1, ..., N-1]:
       match ← []
       for i = 0 → N-1:
           o ← P[i]
           if VOQ[i][o] not empty and o not used:
               match.append((i, o))
       if |match| > |best_match|:
           best_match ← match
  4. For each (i, o) in best_match:
       send VOQ[i][o].front()
```

</details>

<details>
<summary><b>iSLIP — Round-Robin Request/Grant/Accept</b></summary>

```
Initialise: input_ptr[0..N-1] = 0, output_ptr[0..N-1] = 0

For each time slot t:
  1. Enqueue arriving packets to VOQ[input][output]

  2. REQUEST:
       for i = 0 → N-1:
           requests[o].append(i) for all o where VOQ[i][o] not empty

  3. GRANT:
       for o = 0 → N-1:
           grants[o] ← first i in requests[o] starting from output_ptr[o]

  4. ACCEPT:
       for i = 0 → N-1:
           accepts[i] ← first o in grants where grants[o]==i, starting from input_ptr[i]

  5. SEND & UPDATE:
       for each (i, o) in accepts:
           send VOQ[i][o].front()
           input_ptr[i]  ← (o + 1) % N
           output_ptr[o] ← (i + 1) % N
```

</details>

---

> **Key takeaway:** VOQ + iSLIP is the practical winner — it eliminates HoL blocking and runs in O(N) time, finishing only 3 slots after the theoretically optimal result. In real switches (N = 64, 128, …), the O(N!) exhaustive matching is completely infeasible; iSLIP with multiple iterations converges to near-optimal performance.
