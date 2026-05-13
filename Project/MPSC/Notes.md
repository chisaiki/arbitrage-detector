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