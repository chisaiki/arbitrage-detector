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

## Plan

