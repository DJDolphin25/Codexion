*This project has been created as part of the 42 curriculum by theoppon.*

# Codexion

> Master the race for resources before the deadline masters you.

## Description

Codexion is an operating-systems problem about **resource allocation**. It is a variation of the classic Dining Philosophers problem: the **coders** play the role of processes (implemented as POSIX threads), and the **USB dongles** are the shared resources. The coders must share the dongles in a synchronised way, without ever violating the rules of the simulation, even when several of them try to use the same dongle at the same time.

The rules of the simulation:

- `N` coders sit in a circle, with one dongle between each pair of neighbours.
- To **compile**, a coder needs **both** adjacent dongles. A dongle can be held by only one coder at a time.
- After compiling, a coder **debugs**, then **refactors**, and then tries to compile again.
- If `time_to_burnout` ms pass since the start of a coder's last compile (or since the start of the simulation), the coder **burns out** and the simulation stops.
- Once released, a dongle enters a **cooldown** and cannot be used again until `dongle_cooldown` ms have passed.
- When several coders want the same dongle, a **scheduler** decides who gets it: `fifo` (arrival order) or `edf` (earliest deadline first, where the deadline is the moment the coder would burn out).
- The simulation succeeds when every coder has compiled `number_of_compiles_required` times.

The program is written in C, uses only `pthread` primitives, and uses **no global variables**.

## Instructions

### Build

```bash
make         # builds ./codexion (flags: -Wall -Wextra -Werror -pthread)
make clean   # removes the object files
make fclean  # removes the object files and the binary
make re      # fclean + all
```

### Run

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| # | Argument | Meaning | Constraint |
|---|---|---|---|
| 1 | `number_of_coders` | Number of coders (and dongles) | integer > 0 |
| 2 | `time_to_burnout` | Maximum ms between the starts of two compiles | integer > 0 |
| 3 | `time_to_compile` | ms spent compiling | integer > 0 |
| 4 | `time_to_debug` | ms spent debugging | integer > 0 |
| 5 | `time_to_refactor` | ms spent refactoring | integer > 0 |
| 6 | `number_of_compiles_required` | Compiles each coder must complete | integer > 0 |
| 7 | `dongle_cooldown` | ms a released dongle stays unavailable | integer >= 0 |
| 8 | `scheduler` | Arbitration policy | `fifo` or `edf` (lowercase) |

Every numeric argument must be made only of digits and must fit in an `int`. Anything else (empty strings, signs, spaces, decimals, wrong number of arguments, an unknown scheduler) is rejected with a message on `stderr`, nothing is printed on `stdout`, and the program exits with status 1.

Example:

```bash
./codexion 5 2000 200 200 200 10 0 fifo
```

### Output format

Every line has the form `<timestamp_ms> <coder_id> <state>`, where the state is one of:

```
has taken a dongle
is compiling
is debugging
is refactoring
burned out
```

Timestamps are measured in milliseconds from the start of the simulation. A burnout is reported with the same format, and it is the last line of the log.

## Core concepts

### Process vs. thread

A process owns its resources (memory, file descriptors). The threads of a process **share** all of them (heap, globals, open files), but each thread has its **own stack and execution state**. Sharing memory makes communication cheap, and it is also the reason why synchronisation is needed. In Codexion, each coder is a thread, and one more thread (the monitor) watches the simulation.

### Concurrency vs. parallelism

Concurrency means that several tasks make progress during overlapping periods of time. Parallelism means that they literally run at the same instant on different cores. A concurrent program must be correct under **every** possible interleaving of its threads, whether or not they run in parallel.

### Race conditions

A race condition happens when two threads access the same data, at least one of them writes, and nothing orders the accesses. The result then depends on timing. Two coders reading `state == FREE` on the same dongle and both taking it is a race condition.

### Mutexes

A `pthread_mutex_t` is a lock: only one thread can hold it. The life cycle is `pthread_mutex_init`, then `pthread_mutex_lock` / `pthread_mutex_unlock` around every access to the protected data, and finally `pthread_mutex_destroy` once no thread can use it any more. In this project every mutex is initialised before the threads are created, and destroyed only after all the threads have been joined.

### Condition variables

A `pthread_cond_t` lets a thread **sleep until something changes**, instead of spinning in a busy loop that wastes CPU. `pthread_cond_wait` atomically releases the mutex and sleeps; `pthread_cond_timedwait` also wakes up at an absolute time; `pthread_cond_signal` / `pthread_cond_broadcast` wake one or all waiters. Two rules are followed everywhere in this code: the wait always sits inside a `while` loop that re-checks the condition (waits can wake up spuriously), and a condition variable is always used with the same mutex.

### Deadlock and starvation

- **Deadlock**: a group of threads waits forever, each one holding something the next one needs. It requires the four Coffman conditions (see below).
- **Starvation**: a thread never gets the resource it needs because others keep getting it first. In Codexion, starvation has an exact definition: the burnout.

## Why a simple implementation is not enough

The simple solution is that each coder locks its left dongle, then its right dongle, compiles, and unlocks both. It is easy to write and it is wrong in several ways:

- **Deadlock**: if all coders lock their left dongle at the same time, every one of them waits for the right dongle forever.
- **No fairness**: the operating system decides who wins a contested mutex, so the order is neither FIFO nor deadline-aware.
- **No cooldown**: a mutex only knows "locked" or "unlocked".
- **No precise burnout detection**: a thread blocked on a mutex cannot notice that it is running out of time.
- **No clean shutdown**: a thread blocked on a mutex cannot be woken up when another coder burns out.

The rest of this document explains how each of these problems is solved.

## Architecture overview

- **One thread per coder** plus **one monitor thread**. The monitor is created first, so that burnouts are watched from the very beginning.
- **No global variables.** All the shared state lives in one `t_global` structure that is declared in `main` and passed by pointer. Each coder receives a pointer to its own `t_coder`, which points back to the shared context. This is safe because `main` joins every thread before returning.
- **One mutex per dongle** (`state_lock`). It protects the dongle state, its release time and its request queue. Coders fighting for different dongles never block each other.
- **One priority queue per dongle**, implemented as a hand-written **min-heap** stored in an array.
- **One condition variable per request.** Each time a coder asks for a dongle, it creates a request with its own condition variable, and sleeps on it.

Life cycle of a coder (`src/execute.c`):

1. Acquire the two dongles (in the order described below).
2. Record the start of the compile (this resets the burnout clock), log it and sleep for `time_to_compile`.
3. Release both dongles immediately, so that the neighbours can use them while this coder debugs and refactors.
4. Count the compile, then debug and refactor.
5. Repeat until the required number of compiles is reached or the simulation ends.

## Blocking cases handled

### Deadlock prevention (Coffman's conditions)

A deadlock needs four conditions to hold at the same time. This is how each one is treated:

| Coffman condition | Status in Codexion |
|---|---|
| **Mutual exclusion** | Present. It cannot be removed: a dongle is used by one coder at a time. |
| **Hold and wait** | Present. A coder holds its first dongle while it waits for the second one. If the second one cannot be obtained because the simulation ends, the first one is released. |
| **No preemption** | Present. A dongle is never taken away from a coder. It is only released voluntarily after compiling. |
| **Circular wait** | **Broken.** This is how deadlock is prevented. |

Circular wait is broken with an **asymmetric acquisition order** (`acquire_dongles` in `src/dongles.c`): coders with an **even** id take their **left** dongle first and then the right one, while coders with an **odd** id take their **right** dongle first and then the left one. A circular wait would need every coder to wait in the same direction around the table. With at least two coders there is always at least one odd and one even coder, who wait in opposite directions, so the chain of waits can never close into a circle.

For example, with four coders (and dongles `d0` to `d3`): coder 1 takes `d0` then `d3`, coder 2 takes `d0` then `d1`, coder 3 takes `d2` then `d1`, and coder 4 takes `d3` then `d2`. Coders 1 and 2 compete for the same first dongle, so they can never each hold one dongle and wait for the other's.

### Starvation prevention and scheduling

Avoiding deadlock does not guarantee fairness. Each dongle has a **priority queue of requests**, and the policy is chosen at start-up:

- **`fifo`**: requests are served in arrival order.
- **`edf`**: requests are served by earliest deadline, where `deadline = last_compile_start + time_to_burnout`. This is exactly the moment at which the coder would burn out.

Ties are broken by the arrival order of the requests, which is deterministic. This matters at the start of the simulation, because every coder has the same deadline then.

Example where EDF avoids a burnout that FIFO causes: coder A's deadline is `t = 100 ms` and coder B's is `t = 180 ms`. B asks for the shared dongle at `t = 69` and A at `t = 70`, and compiling takes 40 ms. With FIFO, B gets the dongle first and releases it at `t = 109`, so A cannot start compiling before its deadline and burns out. With EDF, the scheduler compares deadlines, gives the dongle to A at `t = 70`, and both coders survive.

Because a dongle is contested by at most two neighbours, the heap is small in practice. It was still written by hand (instead of using a library), because C has no standard priority queue, and because one structure serves both policies with `O(log n)` insertion and extraction.

### Cooldown handling

When a coder releases a dongle, the dongle does not become free: it moves to a `COOLDOWN` state and stores its release time (`drop_dongle` in `src/dongles.c`). The mutex is **not** held during the cooldown, which would block everybody who just wants to look at the dongle. A waiting coder checks the timestamps instead (`cooldown_expired` in `src/scheduler_time.c`) and, if the cooldown is still running, sleeps with `pthread_cond_timedwait` until the **exact** end of the cooldown. There is no polling and no `usleep`.

### Precise burnout detection

A dedicated monitor thread checks all the coders every millisecond (`src/monitor_thread.c`) and compares the current time with `last_compile_start + time_to_burnout` (`src/monitor_burnout.c`). In the tests the burnout is reported with a delay of about 0 ms, and the checker accepts up to 10 ms. Coders that have already completed all their compiles are ignored, otherwise they would be reported as burned out while the others finish. The clock is read **before** the coder's data, so a coder that has just started a compile can never be reported by mistake.

When a burnout is detected, the monitor raises the `finished` flag first, then prints `burned out`, then wakes up every sleeping thread.

### Log serialisation

Logging is a different problem from protecting the dongles: it protects the **terminal**, not a resource of the simulation, so it has its own mutex (`log_mutex`). The timestamp is computed **inside** the critical section, so the order of the lines always matches the order of the timestamps. Using the dongle mutexes for printing would make every print block coders that only want a dongle, and that would cause artificial burnouts.

### Single coder

With one coder there is one dongle, so the left and the right dongle are the same object. The coder takes it, can never take a second one, and burns out after `time_to_burnout`. This is a valid outcome of the simulation, not an error, and no special-case code is needed.

### Clean termination

When the simulation ends (success or burnout), `finished` is set, every coder sleeping in a phase is woken with a broadcast, and every coder waiting for a dongle is woken with a signal on its own request. Then all threads are joined, and only then are the mutexes destroyed and the memory freed. Helgrind, DRD, Memcheck and ThreadSanitizer report no errors or leaks in the provided tests.

## Thread synchronisation mechanisms

| Primitive | Protects / coordinates | Used by |
|---|---|---|
| `pthread_mutex_t state_lock` (one per dongle) | Dongle state (`FREE`, `TAKEN`, `COOLDOWN`), release time and its request queue | Coders, and the monitor at shutdown |
| `pthread_cond_t request_cond` (one per request) | Wakes up exactly the request that is next in line | Coder that releases a dongle; monitor at shutdown |
| `pthread_mutex_t finished_mutex` | The `finished` flag, each coder's `compiles_done` and `last_compile_start` | Coders and monitor |
| `pthread_cond_t finish_cond` | Wakes coders that are sleeping in a compile, debug or refactor phase | Waits: coders. Broadcast: whoever ends the simulation |
| `pthread_cond_t scheduler_cond` | The monitor's 1 ms timed sleep | Monitor |
| `pthread_mutex_t log_mutex` | One log line at a time, with ordered timestamps | Everyone |

Each request works as a small **custom event**: it is created when a coder asks for a dongle, only one thread ever sleeps on it, and it is signalled either by the coder who releases the dongle or by the monitor when the simulation ends. This gives an exact FIFO/EDF order, because the thread that wakes up is the one chosen by the scheduler and not the one chosen by the operating system. It also avoids waking `N` threads so that `N - 1` of them go back to sleep.

**Lock ordering.** A dongle's `state_lock` may be held while `finished_mutex` is taken (to read the `finished` flag), never the other way round. `log_mutex` is never held together with another lock. Because the order is always the same, two threads can never wait for each other's locks.

### How race conditions are prevented

1. **Two coders, one dongle.** The state check and the change to `TAKEN` happen inside the same `state_lock` critical section (`take_dongle` in `src/scheduler.c`), so two coders can never both see the dongle as free.
2. **Monitor reading data that coders write.** `last_compile_start` and `compiles_done` are only read and written with `finished_mutex` held, so the monitor never sees a half-updated value.
3. **Lost wakeups.** A coder checks `finished` and goes to sleep while holding the dongle's `state_lock`, and the one who wakes it (a releasing coder or the monitor) also takes that lock before signalling. A signal can therefore never fall between "I checked" and "I am asleep".
4. **Spurious wakeups.** Every wait is inside a `while` loop that evaluates everything again: has the simulation ended, am I first in the queue, is the dongle free, has the cooldown expired.
5. **Interleaved output.** Only one thread at a time can compute a timestamp and print a line.

### Communication between coders and the monitor

- **Coder to monitor:** each coder publishes `last_compile_start` when it starts a compile and increments `compiles_done` when it finishes one, both under `finished_mutex`. The monitor reads them with the same mutex.
- **Monitor to coders:** the monitor sets `finished`, broadcasts `finish_cond` (wakes the sleepers in a phase) and signals every pending request while holding each dongle's lock (wakes the coders waiting for a dongle). Coders check `finished` after every wait and before starting anything new.
- **Phase sleeps** are `pthread_cond_timedwait` calls on `finish_cond` with an absolute deadline, so a coder that is sleeping for 200 ms stops immediately when the simulation ends, instead of finishing a `usleep`.

## Technical choices

- **Argument parsing** is strict: only digits are accepted, and the `int` limit is checked by comparing the string length and then the digits, before calling `atoi`. Invalid input never reaches the simulation.
- **Zero cooldown is valid** (it means "no cooldown"); zero is rejected for every other numeric argument.
- **The scheduler name is case-sensitive**: only `fifo` and `edf` are accepted.
- **A compile is counted when it ends**, so the simulation succeeds as soon as every coder has finished its last required compile.
- **One clock everywhere** (`CLOCK_REALTIME`). A condition variable waits against that clock by default, so mixing clocks is avoided.
- **Initialisation and clean-up are symmetrical**: if any step fails, everything created so far is destroyed in reverse order, and `t_global` starts zeroed so that clean-up is always safe.

## Resources

### Concepts and tutorials

- Deadlocks: https://medium.com/@jalal92/deadlocks-b059eed3e6c3
- The Dining Philosophers: https://medium.com/@jalal92/the-dining-philosophers-7157cc05315
- Multithreaded programming tutorial (Michigan Tech): https://pages.mtu.edu/~shene/NSF-3/e-Book/index.html
- Introduction to POSIX threads (CUHK): https://www.cse.cuhk.edu.hk/~ericlo/teaching/os/lab/9-PThread/Introduction.html
- Video tutorials on threads, mutexes and condition variables (CodeVault and others):
  - https://youtu.be/zOpzGHwJ3MU
  - https://www.youtube.com/watch?v=HqPJF2L5h9U
  - https://www.youtube.com/watch?v=FYUi-u7UWgw
  - https://www.youtube.com/playlist?list=PLc4DnsRQbs6b8vR0Y5p_bWNy7icCSWsHu
  - https://www.youtube.com/watch?v=tcYo6hipaSA&list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY&index=2

### References

- `man 3 pthread_create`, `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`, `pthread_cond_signal`, `pthread_cond_broadcast`, `clock_gettime`
- Coffman conditions and deadlock characterisation (any operating-systems textbook)
- Earliest Deadline First scheduling, and binary heaps stored in arrays
- Valgrind (Helgrind, DRD, Memcheck) and ThreadSanitizer documentation

### Use of AI

AI (Claude) was used as a **learning and review tool**.

- **Understanding the subject :** answering and correcting a list of conceptual questions (the single-coder case, FIFO vs. EDF, the meaning of burnout, cooldown, monitor precision, why logging needs its own mutex, how to pass state without globals) before any code was written.
- **Final review and documentation:** a detailed walkthrough of the finished code (flow, design decisions, alternatives and their consequences), a static review that pointed out possible weaknesses, and help in drafting and structuring this README, which I then reviewed and adapted.
