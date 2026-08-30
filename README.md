# Codexion: Conceptual Foundations & System Architecture

Before writing a single line of C code, I spent a day breaking down the project logic, identifying critical edge cases, and deciding on the underlying architecture. This document captures my analysis and design choices.

---

## 1. The Core Model & Edge Cases

### Dining Philosophers with Real-Time Constraints

At its core, Codexion is a variation of the classic **Dining Philosophers Problem**: coders act as philosophers and dongles act as forks. However, it introduces two major twists that change how we handle concurrency:

* **Hard Burnout Deadlines:** In standard Dining Philosophers, starvation only happens if your locking logic is flawed. In Codexion, there is an absolute timer (`time_to_burnout`). Even with perfect resource sharing, if a coder doesn't start compiling in time, they burn out.
* **Active Priority Scheduling:** Instead of just letting threads fight for mutexes or using simple deadlock avoidance, we must control resource access using explicit algorithms (**FIFO** or **EDF**).

### The Single-Coder Edge Case ($N = 1$)

If `number_of_coders = 1`, the subject specifies that there is only **1 dongle** on the table.

* To compile, a coder needs 2 distinct dongles (left and right).
* With only 1 coder, their left and right resource pointers point to the exact same dongle.
* As a result, the coder can never acquire two distinct resources and will **never compile**. They will simply wait until `time_to_burnout` expires and burn out. This isn't an invalid input to throw an error for—it's a legitimate simulation outcome.

### When the Burnout Clock Resets

A coder's burnout clock resets to the current timestamp ($T_{\text{now}}$) under only two conditions:

1. When the simulation starts ($T_0$), for coders that haven't compiled yet.
2. The exact millisecond a coder **starts** compiling.

---

## 2. Real-Time Schedulers (FIFO vs. EDF)

To manage resource contention, we need two different scheduling modes:

* **FIFO (First-In, First-Out):** Grants dongles strictly based on who requested them first.
* **EDF (Earliest Deadline First):** Grants dongles based on deadline urgency ($T_{\text{deadline}} = T_{\text{last\_compile}} + \text{time\_to\_burnout}$).

### Why FIFO Fails Where EDF Succeeds

Imagine Coder A and Coder B competing for the same dongle:

* Coder A compiled a while ago and their burnout deadline is at $t = 100\text{ms}$.
* Coder B compiled recently and their deadline is way off at $t = 180\text{ms}$.

If Coder B happens to ask for the dongle at $t = 70\text{ms}$ and Coder A asks at $t = 71\text{ms}$:

* **Under FIFO:** Coder B gets the dongle simply because they arrived 1ms earlier. If Coder B holds it for 40ms, Coder A is forced to wait until $t = 110\text{ms}$, missing their $100\text{ms}$ deadline and burning out unnecessarily.
* **Under EDF:** The scheduler ignores arrival order and checks urgency instead. It sees Coder A's $100\text{ms}$ deadline is much tighter than Coder B's $180\text{ms}$ deadline, hands the dongle to Coder A first, and keeps both coders alive.

### Breaking Ties in EDF

When two coders have the exact same deadline, I decided to break the tie using **Coder ID (lowest ID wins)**.
Using arrival timestamps for tie-breaking introduces microsecond-level OS scheduling noise, making execution unpredictable. Sorting by Coder ID guarantees **100% deterministic and reproducible runs**, which makes debugging and evaluation much easier.

### Building a Min-Heap from Scratch

Standard C doesn't provide built-in priority queue data structures. Implementing a **Min-Heap** on top of a plain array allows us to achieve $O(\log N)$ insertions and extractions while keeping dynamic queue management entirely under our control.

---

## 3. Concurrency, Monitoring & Resource State

### The High-Precision Monitor Thread (<10ms Latency)

The monitor thread cannot use simple `sleep()` loops—if a thread gets stuck and fails to trigger an event, a passive loop won't detect the burnout fast enough.

To meet the $<10\text{ms}$ precision constraint, the monitor runs inside a loop using `pthread_cond_timedwait()`, targeting the earliest impending deadline across all coders. If no state change wakes it early, it wakes up precisely at the target timestamp, checks if $T_{\text{now}} - T_{\text{last\_compile}} \ge \text{time\_to\_burnout}$, and reports the burnout immediately.

### Why We Need Separate Mutexes (Dongles vs. Logging)

* **Dongle Mutexes:** Protect the state and allocation of physical resources.
* **Logging Mutex:** Serializes output to `STDOUT` so terminal messages don't overlap.

If we used a single global lock for both dongles and printing, any thread trying to log a message would block other threads from grabbing dongles. This would create massive bottlenecks and cause artificial burnouts.

### Handling Resource Cooldown

When a coder drops a dongle, it remains unavailable for `dongle_cooldown` ms.
Holding the mutex locked during this cooldown is a mistake—it blocks other threads from even checking the dongle's status. Instead, we store a `time_released` timestamp inside the dongle struct and release the mutex immediately. When another thread wants the dongle, it locks the mutex briefly, checks if `(now - time_released >= cooldown)`, and either takes it or moves on.

---

## 4. Passing State Without Global Variables

Global variables are strictly forbidden by project rules, which means all shared data must pass through `pthread_create()`'s single `void *` parameter.

To handle this cleanly, I encapsulate everything inside a master `t_simulation` structure that holds:

* Simulation configuration parameters.
* The logging mutex.
* The array of dongles (with their mutexes and cooldown timestamps).
* The array of coders (`t_coder`).

Each coder struct also holds a pointer back to this master context. When a thread starts, it casts `void *arg` to its local `t_coder *`, gaining clean, thread-safe access to both its own private data and the shared environment.
