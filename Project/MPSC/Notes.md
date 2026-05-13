# Errors:

- **Stack Smashing:**
     I set the capacity to 1024, when the buffer I was using had the capacity of 4. This caused a "segmentation fault" but on the stack called "stack smashing". It's an out-of-bound error.

---

## Atomic compare_exchange_weak Type Mismatch Error

**Error:**
```
no matching function for call to ‘std::atomic<long unsigned int>::compare_exchange_weak(uint32_t*, uint32_t, std::memory_order, std::memory_order)’
```

**Cause:**
- The type of the local variable (e.g., `uint32_t index`) did not match the type of the atomic variable (e.g., `std::atomic<size_t> tail_`).
- `compare_exchange_weak` expects the first argument to be a pointer to the atomic's type.

**Solution:**
- Ensure the local variable used with `compare_exchange_weak` matches the atomic's type.
- Either:
          - Change the local variable to match the atomic (e.g., `size_t index` if `tail_` is `std::atomic<size_t>`), **or**
          - Change the atomic to match the local variable (e.g., `std::atomic<uint32_t> tail_` if you want to use `uint32_t index`).

**Example Fix:**
- If using `uint32_t` for `index`, declare `tail_` as `std::atomic<uint32_t>`.
- If using `size_t` for `tail_`, declare `index` as `size_t`.


# Design Notescompare_exchange_w
Why compare_exchange_weak instead of compare_exchange_strong?
C++ gives you two versions of this function: compare_exchange_strong and compare_exchange_weak.

To understand why low-latency engineers use weak, you have to look at non-x86 CPU architectures (like ARM or PowerPC). These chips don't have a single LOCK CMPXCHG instruction. Instead, they use a two-step mechanism called Load-Link / Store-Conditional (LL/SC).

On ARM chips, a CAS can experience a Spurious Failure. This means the instruction can fail and return false even if tail_ perfectly matches current_tail. Spurious failures happen if the OS triggers a context switch, or if an unrelated hardware thread simply reads the same cache line at the wrong microscopic instant.

compare_exchange_strong: Checks for spurious failures internally. If the hardware reports a failure but the values actually matched, the C++ standard library wraps the instruction in its own hidden assembly loop to try again.

compare_exchange_weak: Does absolutely no checking. If the hardware aborts, it instantly returns false and lets your code handle it.

The Verdict: Because my try_push logic is already sitting inside a while(true) loop, a spurious failure simply spins your loop around and tries again natively. Using weak avoids generating redundant, nested retry loops in the compiler's assembly output. (Note: On x86 hardware, strong and weak generate the exact same assembly, but using weak inside loops is standard low-latency C++ practice for cross-platform mechanical sympathy).