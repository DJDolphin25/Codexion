# Codexion Recode: turn FIFO into LIFO

## What the evaluator is asking

Change only the arbitration policy used when several coders wait for the same
dongle. FIFO gives the dongle to the oldest request. LIFO gives it to the most
recent request. Do not change cooldown, mutexes, request creation, heap
operations, logging, the monitor, or the coder lifecycle.

`order` is the arrival number. It is assigned in
`src/scheduler_queue_modify.c`, in `scheduler_queue_push`. A smaller `order`
means an earlier request; a larger `order` means a newer request.

## The exact change for the usual recode

The usual request is to replace the behaviour named `fifo` with `lifo`; it is
not necessarily a request to add a third command-line scheduler.

1. Open `src/scheduler_queue_heap.c`.
2. Go to `request_is_before`, currently lines 3-18.
3. Do not change lines 6-16. They handle null values and EDF deadlines.
4. On current line 17, replace the final FIFO comparison so that the request
   with the larger `order` is considered before the other request. This changes
   only the non-EDF tie-break / FIFO branch to LIFO.
5. Leave the two deadline comparisons on lines 12 and 14 unchanged. EDF must
   still choose the earlier deadline and use deterministic arrival order only
   when deadlines are equal.
6. Rebuild. The heap functions `bubble_up` and `bubble_down` already use
   `request_is_before`, so they automatically follow the new policy.

Why this is enough: the comparator decides which request becomes index 0, and
`scheduler_queue_pop_front` grants exactly index 0. Changing a log line would
not change who receives a dongle and fails the recode.

## Only if the evaluator explicitly asks for a new `lifo` argument

Do this *instead of* replacing `fifo`:

1. In `src/parser.c`, current lines 32-36, extend the accepted scheduler
   values to include `lifo`.
2. Update the error text on lines 34-35 so it lists `fifo`, `edf`, and `lifo`.
3. In `src/scheduler_queue_heap.c`, current line 17, select the reverse order
   only when the scheduler value is `lifo`; retain ascending order for `fifo`.
4. Do not alter EDF deadline comparisons.

Ask the evaluator which version they want if their wording is unclear. Adding
`lifo` when they asked to transform `fifo` adds surface area without benefit.

## What must not change

- `src/scheduler_queue_modify.c`: keep incrementing `next_order` when a
  request enters a queue.
- `src/scheduler_queue_access.c`: keep granting the heap head only.
- `src/scheduler.c`: do not change cooldown waits, state changes, or request
  destruction.
- `src/dongles.c`, `src/logger.c`, `src/monitor_*.c`: do not alter resource
  ownership, output, or burnout detection for this recode.

## Proof during the defense

1. Use several coders and a long cooldown to force requests to queue.
2. Record the order in which coders ask for one dongle.
3. Show that, under LIFO, the last arrival receives the dongle before older
   waiting requests.
4. Confirm logs remain one line per event and no two coders hold one dongle at
   the same time.
5. Run the normal tests again: compilation, a cooldown case, FIFO/EDF or LIFO
   as requested, Memcheck, and Helgrind.

## Defense explanation

"Each request receives a monotonically increasing arrival order on insertion.
The heap comparator defines which request is at the head. FIFO uses the oldest
arrival; LIFO reverses that one comparison so the newest arrival is at the
head. The queue mechanics and synchronization remain unchanged."
