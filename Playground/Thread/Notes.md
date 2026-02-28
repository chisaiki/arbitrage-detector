## Goal

Start with the simplest possible version:
- Write a program where two threads each randomly update a "price" in shared memory, and a third thread detects when the difference exceeds a threshold. That's the core of what you're building. It's maybe 80 lines of C++. Struggle with that first. The mutex you write there is the same concept you'll use everywhere else.

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

## Plan

