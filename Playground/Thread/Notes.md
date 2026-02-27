## Goal

Start with the simplest possible version:
- Write a program where two threads each randomly update a "price" in shared memory, and a third thread detects when the difference exceeds a threshold. That's the core of what you're building. It's maybe 80 lines of C++. Struggle with that first. The mutex you write there is the same concept you'll use everywhere else.

## Plan

