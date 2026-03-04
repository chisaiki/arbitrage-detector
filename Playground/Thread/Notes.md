## Goal

Start with the simplest possible version:
- Write a program where two threads each randomly update a "price" in shared memory, and a third thread detects when the difference exceeds a threshold. That's the core of what you're building. It's maybe 80 lines of C++. Struggle with that first. The mutex you write there is the same concept you'll use everywhere else.

IMPORTANT: Lock EVERY access to shared data, both reads and writes.


## Next Steps
- Test locks actually work
- Test lock-free
- Measure performance

----------------------------------------------

## C++ Naming Conventions

| Element | Convention | Example | Notes |
|---------|-----------|---------|-------|
| **Classes/Structs** | PascalCase | `Prices`, `ArbitrageDetector` | Type names start with capital letter |
| **Functions** | snake_case or camelCase | `update_price()`, `updatePrice()` | Be consistent within project |
| **Variables** | snake_case or camelCase | `price_tracker`, `priceTracker` | Be consistent within project |
| **Member Variables** | snake_case or camelCase | `price_a`, `priceA` | Match your variable convention |
| **Constants** | UPPER_SNAKE_CASE | `MAX_PRICE`, `THRESHOLD_VALUE` | All caps with underscores |
| **Namespaces** | snake_case | `arbitrage_utils`, `price_data` | Lowercase, often short |
| **Macros** | UPPER_SNAKE_CASE | `#define DEBUG_MODE` | All caps, avoid when possible |
| **Enum Values** | UPPER_SNAKE_CASE or PascalCase | `PRICE_UPDATE`, `PriceUpdate` | Depends on style guide |
| **Template Parameters** | PascalCase | `template<typename T>` | Single capital letter or PascalCase |
| **Private Members** | prefix with `_` or `m_` | `_price`, `m_price` | Optional, indicates private |
| **Mutex/Lock** | suffix with `_mutex` or `_lock` | `price_data_access`, `prices_mutex` | Clear indication of purpose |
| **Boolean Variables** | prefix with `is_`, `has_`, `can_` | `is_running`, `has_data` | Makes intent clear |

### Key Principles:
- **Consistency is paramount** - Pick one style and stick to it throughout your project
- **Be descriptive** - `price_data_access` is better than `mtx` for learning/maintenance
- **snake_case** - Common in C++ standard library and systems programming
- **camelCase** - Common in modern C++, Google C++ Style Guide

## Issues Encountered & Solutions

### 1. Naming Conventions
- **Problem**: Used lowercase `prices` for struct and PascalCase `Tracker` for variable
- **Solution**: Structs/classes use PascalCase (`Prices`), variables use snake_case (`price_tracker`)
- **Lesson**: Type names are capitalized, instance names are lowercase

### 2. Accessing Struct Members by Index
- **Problem**: Wanted to access `price_a`, `price_b` using index like `prices[i]`
- **Solution**: Changed struct to use array: `double prices[2]` instead of named members
- **Lesson**: Named members can't be indexed; use arrays when you need index access

### 3. Thread Array Syntax
- **Problem**: Tried `std::thread thread[i](function, args)` to create individual thread
- **Solution**: Declare array first `std::thread threads[2]`, then assign in loop: `threads[i] = std::thread(...)`
- **Lesson**: Arrays must be declared before indexing; can't declare individual elements

### 4. Passing Functions to Threads
- **Problem**: Called function directly: `std::thread(update_price(tracker, i))`
- **Solution**: Pass function name and arguments separately: `std::thread(update_price, std::ref(tracker), i)`
- **Lesson**: Thread constructor takes function pointer, then its arguments as separate parameters

### 5. Passing References to Threads
- **Problem**: Thread tried to copy reference parameter instead of passing by reference
- **Solution**: Wrap reference arguments with `std::ref()`: `std::thread(func, std::ref(data))`
- **Lesson**: By default, thread arguments are copied; use `std::ref()` for references

### 6. Random Float Generation
- **Problem**: `std::uniform_int_distribution<double>` caused compilation error
- **Solution**: Use `std::uniform_real_distribution<double>` for floating-point numbers
- **Lesson**: `uniform_int_distribution` only works with integral types; use `uniform_real_distribution` for float/double

### 7. Keeping Threads Running
- **Problem**: Thread function ran once and exited; wanted continuous execution
- **Solution**: Added `while(condition)` loop inside thread function, not in main
- **Lesson**: To simulate continuous updates (like API streams), loop inside the thread function

### 8. Shared Stop Flag with atomic<bool>
- **Problem**: `std::atomic<bool>` couldn't be passed to thread - "cannot be copied" error
- **Solution**: Pass by reference using `std::ref(keep_running)` and update function signature to take `std::atomic<bool>&`
- **Lesson**: `std::atomic` types are non-copyable; must always pass by reference to threads

### 9. User-Controlled Termination
- **Problem**: Wanted threads to run until user stops them
- **Solution**: Used `std::cin.get()` in main to block until user presses Enter, then set flag to false
- **Lesson**: Main thread can wait for input while worker threads run; flag signals graceful shutdown

### 10. Macro Parentheses
- **Best Practice**: Always wrap macro values in parentheses: `#define THRESHOLD (1.25)`
- **Why**: Macros do textual replacement before compilation. Without parentheses, expressions like `100 / THRESHOLD` can fail if the macro is an expression (e.g., `#define THRESHOLD 1 + 0.25` becomes `100 / 1 + 0.25` = 100.25 instead of `100 / (1 + 0.25)` = 80)
- **Lesson**: Parentheses ensure the macro is treated as a single unit regardless of surrounding operators

### 11. Integer abs() vs Double std::abs()
- **Problem**: Used `abs()` on doubles - result was whole numbers only (e.g., 83.0 instead of 83.25)
- **Root Cause**: `abs()` is the integer version - it converts double to int, discarding fractional part
- **Solution**: Use `std::abs()` or `fabs()` from `<cmath>` for floating-point numbers
- **Example**: If `abs(95.73 - 12.48)` → converts 83.25 to int 83, returns 83. But `std::abs(95.73 - 12.48)` → preserves 83.25
- **Lesson**: Always use typed math functions for proper precision: `std::abs()` for doubles, `abs()` only for ints

### 12. ⚠️ CRITICAL: Missing Mutex in Writer Threads (Race Condition)
- **Problem**: Only locked mutex in `find_difference()` (reader) but NOT in `update_price()` (writers)
- **Root Cause**: Thought only the reading thread needed protection, didn't realize ALL access to shared data needs synchronization
- **Consequence**: **Race condition** - reader can access prices while writers are updating them, causing:
  - Reading partially-updated values (torn reads)
  - Inconsistent state (reading price[0] from one update, price[1] from another)
  - Undefined behavior on some architectures
- **Solution**: Add mutex lock/unlock around the write operation in `update_price()`:
  ```cpp
  price_data_access_lock.lock();
  price_ds.prices[i_value] = distrib(gen);
  price_data_access_lock.unlock();
  ```
- **Testing Strategy**: To verify mutex is working:
  1. Add delays inside critical sections to make race conditions obvious
  2. Add thread ID logging to see which thread accesses when
  3. Comment out locks and observe corrupted/garbled output
- **Lesson**: **GOLDEN RULE of mutexes**: Lock EVERY access (read OR write) to shared data. If even one thread accesses without locking, the entire protection fails. Both readers and writers must use the same mutex.

### 13. Atomic Initialization Syntax (C++ Standard Differences)
- **Problem**: `std::atomic<bool> keep_running = true;` compiled on MinGW but failed on WSL/GCC 9 with error: "use of deleted function 'std::atomic<bool>::atomic(const std::atomic<bool>&)'"
- **Root Cause**: C++ standard version differences:
  - **C++17+** (MinGW default): Has mandatory copy elision - `= true` directly constructs the atomic, no copy attempted
  - **C++14/C++11** (WSL/GCC 9 default): Treats `= true` as copy initialization - tries to copy temporary to atomic, but atomics are non-copyable (copy constructor deleted)
- **Solution**: Use direct initialization syntax that works across all C++ standards:
  ```cpp
  std::atomic<bool> keep_running(true);   // ✓ Direct initialization
  std::atomic<bool> keep_running{true};   // ✓ Uniform initialization (C++11+)
  std::atomic<bool> keep_running = true;  // ❌ Only works in C++17+
  ```
- **Alternative Solution**: Compile with C++17 standard: `g++ -std=c++17 ...`
- **Lesson**: Atomics cannot be copied (by design - copying wouldn't have clear thread-safety semantics). Use direct initialization `()` or uniform initialization `{}` for maximum portability across C++ standards. This ensures code compiles on older compilers and different platforms.

## Questions & Deeper Understanding

### Q1: Why does `std::thread` need `std::ref()` instead of normal reference passing?

**Answer:** When you pass arguments to `std::thread`, it **copies everything by default** into internal storage. This is a safety feature because:
- The thread might start running after the original variables have gone out of scope
- To be safe, `std::thread` makes copies of all arguments

Even if your function signature says `void update_price(Prices& data)`:
```cpp
std::thread t(update_price, price_tracker);  // Copies price_tracker!
```

The thread copies `price_tracker`, then passes that copy by reference to the function. You'd be modifying a copy, not the original!

`std::ref()` tells the thread: "Don't copy this - store a reference to the original object instead."

**Key Difference:**
- **Normal function calls**: Arguments evaluated immediately, references bind directly
- **Thread construction**: Arguments stored/copied first, then passed when thread runs
- **Solution**: Use `std::ref()` to override the copying behavior

---

### Q2: Should primitive values like `i` use `std::ref()` when passed to threads?

**Answer:** No, just pass by value (e.g., `i` not `std::ref(i)`).

Primitive values like `int` should be **copied** so each thread gets its own independent value:
- Thread 0 needs the value `0`
- Thread 1 needs the value `1`

If you used `std::ref(i)` with a loop variable, both threads would share a reference to the same variable which:
1. Changes as the loop continues
2. Goes out of scope after the loop ends
3. Could cause both threads to see the wrong value

**Rule of thumb:**
- Small values (int, double, bool) → copy them (no `std::ref`)
- Large objects or when you need to modify the original → use `std::ref()`

---

### Q3: Should function parameters use `const int&` or just `int` for copied values?

**Answer:** For small primitives passed by value to threads, use `int i_value`, not `const int& i_value`.

When you pass `i` by value to the thread (copying it), the function should accept it by value. Using `const int&` for a small primitive adds unnecessary indirection with no benefit - it's actually **less efficient** than copying the 4-byte int directly.

**Use references for:**
- Large objects (structs, vectors, etc.)
- When you need to modify the original
- Objects that can't be copied (like `std::atomic`)

**Use plain values for:**
- Small primitives (int, double, bool, char, etc.)

---

### Q4: Why use `std::atomic<bool>` for `keep_running` but not `std::atomic<int>` for `i_value`?

**Answer:** Because `i_value` is **not shared** between threads - each thread gets its own separate copy!

**`keep_running` (needs atomic):**
- ONE variable shared by ALL threads
- All threads read it, main thread writes to it
- Multiple threads accessing the same memory = race condition
- Needs `std::atomic` for thread safety

**`i_value` (doesn't need atomic):**
- Each thread gets its **own private copy**
- Thread 0 has `i_value = 0`, thread 1 has `i_value = 1`
- They never share or modify it
- No other thread touches it = no race condition = no atomic needed

**Thread safety is only needed when:**
1. Multiple threads access the **same memory location**
2. At least one thread **writes** to it

---

### Q5: Could we use a mutex instead of `std::atomic` for `keep_running`?

**Answer:** Yes, but `std::atomic<bool>` is better for simple flags.

**Mutex approach:**
```cpp
bool keep_running = true;
std::mutex flag_mutex;

// In threads: must lock/unlock every check
flag_mutex.lock();
bool should_continue = keep_running;
flag_mutex.unlock();

// In main: must lock/unlock to change
flag_mutex.lock();
keep_running = false;
flag_mutex.unlock();
```

**Why `std::atomic<bool>` is better:**
- **No locking overhead** - atomic operations are lock-free (usually just a CPU instruction)
- **Simpler code** - no manual lock/unlock
- **Faster** - no heavyweight mutex machinery
- **Designed for this** - simple shared flags are exactly what atomics are for

**Use mutex when:**
- You need to protect multiple related variables together
- You have complex operations (read-modify-write sequences)
- You're accessing non-atomic data structures

**Use atomic when:**
- Single variable that needs thread-safe read/write
- Simple flags, counters, or status values

---

## Testing & Verifying Mutex Locks Are Working

Once you've added mutex locks to your code, you need to verify they're actually working and protecting shared data correctly. Here are methods used in practice:

### Learning/Development Methods

#### Method 1: Add Timing to Show Serialization (Best for Learning)
**Approach:** Add timing measurements to prove threads are waiting for each other

```cpp
#include <chrono>

void update_price(Prices& price_ds, int i_value, std::atomic<bool>& continue_running){
    while(continue_running){
        auto start = std::chrono::steady_clock::now();
        price_data_access_lock.lock();
        auto lock_acquired = std::chrono::steady_clock::now();
        
        price_ds.prices[i_value] = distrib(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Deliberate delay
        
        price_data_access_lock.unlock();
        
        auto wait_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            lock_acquired - start).count();
        if (wait_time > 5) {
            std::cout << "Thread " << i_value << " waited " << wait_time << "ms for lock\n";
        }
    }
}
```

**Pros:**
- Visual proof threads are serializing (waiting for each other)
- Shows actual wait times - helps understand contention
- Good for learning how locks affect performance

**Cons:**
- Requires code modifications
- Adds overhead from timing calls
- Only shows contention, doesn't prove correctness completely

---

#### Method 2: Test Without Locks (Break It On Purpose)
**Approach:** Comment out the locks and observe if things break

```cpp
// price_data_access_lock.lock();    // COMMENT OUT
price_ds.prices[i_value] = distrib(gen);
// price_data_access_lock.unlock();   // COMMENT OUT
```

**Pros:**
- Simplest method - just comment out code
- Shows what happens WITHOUT protection
- Quick validation that locks matter

**Cons:**
- Race conditions may not appear immediately (non-deterministic)
- May need to run multiple times to see failures
- Doesn't prove locks are working, just shows they're needed
- Dangerous - could crash or corrupt data

---

#### Method 3: Add Counter to Detect Concurrent Access
**Approach:** Use a counter to detect if multiple threads enter critical section simultaneously

```cpp
std::atomic<int> inside_critical_section{0};

void update_price(Prices& price_ds, int i_value, std::atomic<bool>& continue_running){
    while(continue_running){
        price_data_access_lock.lock();
        inside_critical_section++;
        if (inside_critical_section > 1) {
            std::cout << "ERROR: Multiple threads in critical section!\n";
        }
        
        price_ds.prices[i_value] = distrib(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Make it more obvious
        
        inside_critical_section--;
        price_data_access_lock.unlock();
    }
}
```

**Pros:**
- Actively detects mutual exclusion violations
- Clear error message when locks fail
- Similar to professional invariant checking
- Can be left in code as debugging aid

**Cons:**
- Requires code changes to add counter
- Adds overhead from atomic operations
- Only catches violations if they occur during test run

---

### Professional/Industry Methods

#### Method 4: Thread Sanitizer (TSan) - Industry Standard ⭐
**Approach:** Compile with built-in race condition detector

```bash
# Compile with ThreadSanitizer
g++ -fsanitize=thread -g miniarbtirage.cpp -o miniarbtirage

# Run program - TSan reports any race conditions
./miniarbtirage
```

**What it detects:**
- Race conditions (data accessed by multiple threads without synchronization)
- Exact file names and line numbers
- Which threads are involved
- Whether it's read-write or write-write conflict

**Pros:**
- **Zero code changes needed** - just a compiler flag
- Catches races you didn't know existed
- Reports exact locations with line numbers
- Industry standard at Google, Microsoft, Meta, etc.
- Much more thorough than manual testing
- Detects subtle race conditions that don't cause obvious bugs

**Cons:**
- Not available on all compilers (limited MinGW support on Windows)
- ~5-15x slowdown when running
- Increases memory usage significantly
- May have false positives (rare)

**When to use:** Always, if available. This is the first tool pros reach for.

---

#### Method 5: Assertions & Invariant Checks
**Approach:** Add `assert()` statements to check assumptions about thread safety

```cpp
void update_price(Prices& price_ds, int i_value, std::atomic<bool>& continue_running){
    while(continue_running){
        assert(!price_data_access_lock.try_lock() && "Lock should already be held!");
        // Or check data invariants:
        // assert(price_ds.prices[0] >= 0 && "Price should never be negative");
    }
}
```

**Pros:**
- Catches violations immediately at runtime
- Can be disabled in release builds (`-DNDEBUG`)
- Documents assumptions about thread safety
- Standard practice in production code

**Cons:**
- Requires knowing what invariants to check
- Only works if violation occurs during testing
- Crashes program when assertion fails (by design)

**When to use:** Throughout development, with assertions checking data consistency and thread safety assumptions.

---

#### Method 6: Code Review
**Approach:** Have another engineer review the code looking for synchronization issues

**What reviewers check:**
- All accesses to shared data have locks
- Same mutex used for all accesses to the same data
- No lock ordering issues (potential deadlocks)
- Locks held for minimum necessary time
- Proper use of `std::lock_guard` or RAII patterns

**Pros:**
- Catches design issues, not just race conditions
- Finds logic errors that tools miss
- Knowledge transfer between team members
- No code or compiler changes needed

**Cons:**
- Human error - reviewers can miss issues
- Time-consuming
- Requires experienced reviewer
- Doesn't catch runtime-only issues

**When to use:** Always, for any multithreaded code. Required at most companies before code merges.

---

#### Method 7: Stress Testing
**Approach:** Run with many threads, high load, for extended periods

```cpp
// Instead of 2 threads, use 10+
std::thread get_price_threads[10];

// Let it run for hours/days
// On multicore system with high contention
```

**Pros:**
- Exposes rare race conditions that only occur under load
- Tests real-world scenarios
- Finds performance bottlenecks
- Validates code works under stress

**Cons:**
- Very time-consuming (hours to days)
- Race conditions may still not appear (non-deterministic)
- Requires significant compute resources
- Hard to debug issues that occur

**When to use:** Before releasing multithreaded code to production. Often run continuously in CI/CD systems.

---

### Recommendation: Which Method to Use?

**For this project (learning):**
1. **Start with Method 3** (counter) - immediate feedback, teaches you what mutexes do
2. **Try Method 2** (remove locks) - see what breaks without protection
3. **Use Method 4** (TSan) if available on your system

**For professional development:**
1. **Thread Sanitizer (Method 4)** - always, if available
2. **Code Review (Method 6)** - required for all code
3. **Assertions (Method 5)** - embedded in code throughout development
4. **Stress Testing (Method 7)** - before production release

**Golden Rule:** Never assume locks are working. Always verify with at least one method.
