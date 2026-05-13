# MPSC Lock-Free Ring Buffer — Bug Notes

## 1. The Sequence Hole Deadlock (Feared, But Not Present)

### The Concern
The worry was this scenario:
1. CAS succeeds → `tail_` advances from `5` to `6`, producer owns slot `5`
2. Producer discovers `can_overwrite == false` (queue full)
3. Producer executes `return false`, abandoning slot `5` unwritten
4. Consumer arrives at slot `5`, waits forever → **permanent deadlock**

### Why It Doesn't Apply Here
The check order in the current code prevents this:

```cpp
// 1. Check the slot FIRST — before touching tail_
if (!slot_data.can_overwrite.load(std::memory_order_acquire)) {
    return false;  // bail BEFORE the CAS
}

// 2. THEN claim tail
if (tail_.compare_exchange_weak(index, index + 1, ...)) {
    // write data...
}
```

Because the full-queue bail happens **before** the CAS, `tail_` is never advanced for a full slot. No hole is created.

---

## 2. The Real Race Condition

The gap between the slot check (step 1) and the CAS (step 2) introduces a genuine TOCTOU race:

```
Producer A: reads slot 5 → can_overwrite == true ✓
Producer B: reads slot 5 → can_overwrite == true ✓  (same snapshot)
Producer A: CAS wins    → owns slot 5, writes data, sets can_overwrite = false
Consumer:   reads slot 5 → sets can_overwrite = true
Producer B: CAS wins    → owns slot 5 again, but checked stale state
```

Producer B won the CAS based on a snapshot that is no longer valid.

---

## 3. The Temporary Stall (Resolves Itself, Still Dangerous)

A softer version of the hole scenario **can** occur under preemption:

```
Producer A: CAS wins → owns slot 5, tail_ = 6
            ← OS preempts Producer A here
Producer B: CAS wins → owns slot 6, writes data, signals done
Consumer:   arrives at slot 5, data not yet written → spins waiting
Producer A: resumes → writes slot 5, consumer unblocks
```

This resolves eventually, but in a **real-time or low-latency** context the spin is unacceptable.

---

## 4. Root Cause: Dual-Meaning Flag

`can_overwrite` is doing two jobs with conflicting semantics:

| Flag Value | Producer Reads As | Consumer Reads As |
|---|---|---|
| `true` | "safe to claim this slot" | — |
| `false` | "slot is full, back off" | "data is ready to read" |

A single atomic cannot cleanly serve both roles simultaneously under concurrent producers.

---

## 5. Planned Fix: Per-Slot Sequence Numbers

Replace the boolean flag with a **generation counter** per slot. This is the approach used in Dmitry Vyukov's MPSC queue.

```cpp
struct Slot {
    DataType data;
    std::atomic<uint32_t> sequence;  // replaces can_overwrite
};
```

### State Transitions

| `sequence` value | Meaning |
|---|---|
| `index` | Slot is free; producer may claim it |
| `index + 1` | Data written; consumer may read it |
| `head + Capacity` | Consumer done; slot recycled for next generation |

### Producer Logic
```cpp
uint32_t seq = slot.sequence.load(std::memory_order_acquire);
if (seq != index) {
    // slot not ready to overwrite — queue full or stale index
}
// after winning CAS and writing data:
slot.sequence.store(index + 1, std::memory_order_release);
```

### Consumer Logic
```cpp
if (slot.sequence.load(std::memory_order_acquire) == head_ + 1) {
    // data is ready — safe to read
}
// after reading:
slot.sequence.store(head_ + Capacity, std::memory_order_release);
```

The sequence number encodes **which generation** the slot belongs to. Producers and the consumer independently determine slot state without ambiguity, eliminating the dual-meaning flag problem entirely.

---

## TODO

- [ ] Replace `can_overwrite: std::atomic<bool>` with `sequence: std::atomic<uint32_t>` in `Slot`
- [ ] Update producer CAS check to compare against sequence instead of bool flag
- [ ] Update consumer to advance sequence by `Capacity` after reading
- [ ] Add `static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of two")`
- [ ] Revisit CAS memory order: success should be `acq_rel`, not `release`