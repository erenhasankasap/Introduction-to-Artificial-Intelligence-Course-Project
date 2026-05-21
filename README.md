# 🧩 Shortest Path in Maze — IAI 2026 Project (Topic 4)

> **Introduction to Artificial Intelligence** | Spring 2026  
> Yıldız Technical University · Mathematical Engineering  
> **Group 13:** Eren Hasan Kasap, Mehmet Erdem, Bahadır Ay, Kübra Yeşilova, Görkem Yeşilyurt, Zeynep Naz Tuzcu

---

## 📌 Table of Contents

- [Problem Definition](#-problem-definition)
- [Algorithms Implemented](#-algorithms-implemented)
- [Heuristic Function](#-heuristic-function)
- [Maze Configurations](#-maze-configurations)
- [Project Structure](#-project-structure)
- [File Descriptions](#-file-descriptions)
- [Data Structures](#-data-structures)
- [How to Build & Run](#-how-to-build--run)
- [Sample Interaction](#-sample-interaction)
- [Results & Findings](#-results--findings)
- [Documents](#-documents)

---

## 🗺️ Problem Definition

The goal is to find the **shortest path** from a user-defined start position to a goal position inside a 2D maze. The agent navigates a grid where each cell is either a **free path (0)** or a **wall (1)**.

| Component | Description |
|---|---|
| **State** | `(row, col)` — current cell position of the agent |
| **Initial State** | User-specified valid (non-wall) cell |
| **Goal State** | User-specified valid (non-wall) cell |
| **Actions** | `Move_Up`, `Move_Down`, `Move_Left`, `Move_Right` |
| **Transition Model** | Move in chosen direction if the target cell is in bounds and not a wall; step cost = 1 |
| **Path Cost** | Total number of steps from start to goal |

---

## ⚙️ Algorithms Implemented

The program supports **8 search algorithms**, selectable at runtime:

| # | Algorithm | Type | Optimal? | Complete? |
|---|---|---|---|---|
| 1 | Breadth-First Search (BFS) | Blind | ✅ | ✅ |
| 2 | Uniform-Cost Search (UCS) | Blind | ✅ | ✅ |
| 3 | Depth-First Search (DFS) | Blind | ❌ | ❌ |
| 4 | Depth-Limited Search (DLS) | Blind | ❌ | ❌ |
| 5 | Iterative Deepening Search (IDS) | Blind | ✅ | ✅ |
| 6 | Greedy Search | Heuristic | ❌ | ❌ |
| 7 | A\* Search | Heuristic | ✅ | ✅ |
| 8 | Generalised A\* Search | Heuristic | Configurable (α) | ✅ |

> **Generalised A\*** uses a weighted evaluation function:  
> `f(n) = (1 - α) · g(n) + α · h(n)`  
> where `α ∈ [0, 1]`. Setting `α = 0.5` reproduces standard A\*.

---

## 📐 Heuristic Function

The heuristic used for informed search algorithms is the **Manhattan Distance**:

```
h(n) = |row_current - row_goal| + |col_current - col_goal|
```

This is **admissible** (never overestimates the true cost) in a grid where each step costs 1 and diagonal movement is not allowed — guaranteeing optimality for A\*.

---

## 🏗️ Maze Configurations

Two mazes are built into the program:

### Maze 1 — Small (20 × 20)
- Statically defined as a 2D integer array in `SpecificToProblem.c`
- Start: `(1, 1)` · Goal: `(18, 18)` (user-selectable)
- Suitable for testing all algorithms including slower ones (DFS, IDS)

### Maze 2 — Large (50 × 50)  
- Algorithmically generated at runtime using horizontal wall rows with evenly spaced gaps and staggered vertical obstacles
- 1378 path cells vs 1326 wall cells (~40% / ~60% split)
- Designed to increase complexity through long corridors and dead ends
- Start: `(1, 1)` · Goal: `(48, 48)` (user-selectable)

**Cell representation in terminal output:**

| Symbol | Meaning |
|---|---|
| `S` | Start position |
| `G` | Goal position |
| `#` | Wall cell |
| `.` | Free path cell |
| `*` | Solution path |

---

## 📁 Project Structure

```
.
├── data_types.h              # Global type definitions (State, Node, Queue, enums)
├── GRAPH_SEARCH.h            # Macro constants & all public function declarations
├── GRAPH_SEARCH.c            # Entry point (main), algorithm dispatcher
├── SpecificToProblem.c       # Problem-specific logic: maze, heuristic, transitions
├── Standart_Search.c         # Generic search infrastructure (frontier, expansion)
├── HashTable.h               # Hash table interface & configuration constants
├── HashTable.c               # Open-addressing hash table (explored set)
├── IAI_2026_PROJECT_GROUP13_REPORT.pdf
└── IAI_2026_PROJECT_GROUP13_PRESENTATION.pdf
```

---

## 📄 File Descriptions

### `data_types.h` — Global Type Definitions
Single source of truth for all shared data structures. Must be included first by every other translation unit.

- **`enum ACTIONS`** — `Move_Up(0)`, `Move_Down(1)`, `Move_Left(2)`, `Move_Right(3)`
- **`typedef struct State`** — holds `row`, `col`, and `h_n` (heuristic value)
- **`typedef struct Node`** — search tree node with `state`, `path_cost`, `action`, `parent`, and child count
- **`typedef struct Queue`** — linked-list frontier element
- **`enum METHODS`** — maps integer input (1–8) to the chosen search algorithm

### `GRAPH_SEARCH.h` — Constants & Declarations

Key macros:

```c
#define MAZE_ROWS_20       20        // Small maze rows
#define MAZE_COLS_20       20        // Small maze cols
#define MAZE_ROWS_50       50        // Large maze rows
#define MAZE_COLS_50       50        // Large maze cols
#define PREDETERMINED_GOAL_STATE  1 // Goal is always user-defined
#define ACTIONS_NUMBER     4         // Four movement directions
#define MAX_SEARCHED_NODE  100000000 // Node expansion cap
```

### `GRAPH_SEARCH.c` — Entry Point
Reads user inputs (maze, start, goal, algorithm, alpha), initialises the root node, dispatches to the selected algorithm, and prints results (solution path, path cost, nodes produced, nodes in memory, computation time).

### `SpecificToProblem.c` — Problem-Specific Logic
All functions that encode the maze problem:
- `Init_Maze(int)` — loads maze 1 (static) or builds maze 2 (procedural) into the global `MAZE` array
- `Create_State()` — prompts user for valid `(row, col)`, validates against walls and bounds
- `Result()` — applies an action to a state; returns `FALSE` if the resulting cell is a wall or out of bounds
- `Compute_Heuristic_Function()` — Manhattan distance to the goal
- `Goal_Test()` — checks if the current state matches the goal
- `Print_Maze_Visual()` — renders the maze to the terminal
- `Print_Maze_With_Solution()` — traces back through parent pointers and marks the solution path with `*`

### `Standart_Search.c` — Generic Search Infrastructure
Algorithm-agnostic frontier management and node expansion:
- `Insert_FIFO` / `Insert_LIFO` — for BFS and DFS respectively
- `Insert_Priority_Queue_UniformSearch` — ordered by `g(n)`
- `Insert_Priority_Queue_GreedySearch` — ordered by `h(n)`
- `Insert_Priority_Queue_A_Star` — ordered by `f(n) = g(n) + h(n)`
- `Insert_Priority_Queue_GENERALIZED_A_Star` — ordered by `f(n) = (1-α)·g(n) + α·h(n)`

### `HashTable.c` / `HashTable.h` — Explored Set
Open-addressing hash table used in graph search variants to avoid revisiting already-expanded states.

Configuration (in `HashTable.h`):
```c
#define HASH_TABLE_BASED_SIZE      2503  // Initial bucket count (prime)
#define HASH_TABLE_INCREASING_RATE   70  // Load factor threshold (%)
#define MAX_KEY_SIZE                 12  // Max key size in bytes
```

`Generate_HashTable_Key(state, key)` encodes `(row, col)` as a fixed-length byte string that uniquely identifies each state.

---

## 🔨 How to Build & Run

### Requirements
- GCC (or any C99-compatible compiler)
- Standard C library

### Compile

```bash
gcc -o maze_solver \
    GRAPH_SEARCH.c \
    SpecificToProblem.c \
    Standart_Search.c \
    HashTable.c \
    -lm -Wall -O2
```

### Run

```bash
./maze_solver
```

### Runtime Prompts

```
Select maze (1 = 20x20, 2 = 50x50): 2

Enter start row (0-49): 1
Enter start col (0-49): 1

Enter goal row (0-49): 48
Enter goal col (0-49): 48

Select algorithm:
  1: BFS   2: UCS   3: DFS   4: DLS
  5: IDS   6: Greedy   7: A*   8: Generalised A*
Choice: 7
```

For **Generalised A\***, you will also be prompted:

```
Enter alpha (0.0 = pure UCS, 1.0 = pure Greedy, 0.5 = standard A*): 0.5
```

---

## 💻 Sample Interaction

```
=========== MAZE VISUAL ===========
# # # # # # # # # # # # # # # # # #
# S . . # . . . . # . . . . # . . #
# # # . # . # # # # # . # # # . # #
...
# . . . . . . . . . . . . . . . G #
# # # # # # # # # # # # # # # # # #
===================================

Algorithm     : A* Search
Solution Path : (1,1) → ... → (48,48)
Path Cost     : 96
Nodes Produced: 1842
Nodes in Mem  : 743
Time Elapsed  : 0.003 s

========== MAZE WITH SOLUTION ==========
# # # # ...
# S * * # ...
...
# . . * * * * G #
========================================
```

---

## 📊 Results & Findings

Testing on both maze sizes and multiple start/goal configurations revealed the following general trends:

| Algorithm | Path Quality | Speed | Memory Usage |
|---|---|---|---|
| BFS | Optimal | Moderate | High |
| UCS | Optimal | Moderate | High |
| DFS | Suboptimal | Fast (but unreliable) | Low |
| DLS | Suboptimal | Fast (depth-bounded) | Low |
| IDS | Optimal | Moderate | Very Low |
| Greedy | Suboptimal | Very Fast | Low |
| A\* | Optimal | Fast | Moderate |
| Gen. A\* (α=0.7) | Near-optimal | Faster than A\* | Lower than A\* |

**Key observations:**
- A\* consistently found optimal paths with far fewer node expansions than BFS/UCS on the large maze.
- Greedy Search was the fastest but frequently found non-optimal routes due to ignoring path cost.
- Generalised A\* with a tuned `α` offers a controllable trade-off between solution quality and search speed.
- DFS failed to find paths in many large maze configurations due to hitting the node cap without exploring the goal branch.

---

## 📎 Documents

| File | Description |
|---|---|
| [`IAI_2026_PROJECT_GROUP13_REPORT.pdf`](./IAI_2026_PROJECT_GROUP13_REPORT.pdf) | Full project report: problem formulation, heuristic design, algorithm analysis, simulation results with screenshots |
| [`IAI_2026_PROJECT_GROUP13_PRESENTATION.pdf`](./IAI_2026_PROJECT_GROUP13_PRESENTATION.pdf) | Presentation slides used during the course presentation |

---

## 📜 License

This project was developed as a university course assignment for **Introduction to Artificial Intelligence (IAI) — Spring 2026** at Yıldız Technical University. The base framework (`GRAPH_SEARCH.c`, `Standart_Search.c`) was provided by the course instructor. Problem-specific code was written by the group members.
