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

## Plan

