# Arbitrage Detector - Full-Scale Project Plan

## Current State
- ✅ Basic multi-threaded prototype with mutex synchronization
- ✅ Continuous price updates and difference detection
- 📁 Websocket playground (for real exchange feeds)
- 📁 Empty Project folder (ready for production version)

## Lock-Free vs Mutex Decision Framework

### Lock-Free Advantages:
- No blocking, better worst-case latency
- No context switching overhead
- Better scalability under high contention
- No deadlock risk

### Lock-Free Disadvantages:
- More CPU-intensive (spinning/retrying)
- Higher complexity and easier to introduce bugs
- May perform worse under low contention
- Memory ordering overhead can be significant
- ABA problems and other subtle issues

### When Lock-Free Wins:
- High contention scenarios
- Real-time systems needing predictable latency
- Many readers, few writers
- Short critical sections with frequent access

### When Mutexes Win:
- Low to moderate contention
- Complex critical sections
- When blocking is acceptable
- Easier to reason about correctness

## Full-Scale Architecture Design

### 1. Data Flow Architecture
```
Websocket Threads (per exchange) 
    ↓
Lock-free Queues (SPSC/MPSC)
    ↓
Price Update Thread
    ↓
Shared Price State (atomic or RCU)
    ↓
Detector Threads (parallel reads)
    ↓
Alert/Order Queues
```
--------------------------------------------------------------------------
┌─────────────────────────────────────────────────────────────────┐
│                    ARBITRAGE DETECTOR SYSTEM                    │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  Binance WS  │  │ Coinbase WS  │  │  Kraken WS   │  ← Exchange APIs
│   Thread 1   │  │   Thread 2   │  │   Thread 3   │
└──────┬───────┘  └──────┬───────┘  └──────┬───────┘
       │                 │                 │
       │ Raw JSON        │ Raw JSON        │ Raw JSON
       ↓                 ↓                 ↓
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ JSON Parser  │  │ JSON Parser  │  │ JSON Parser  │
└──────┬───────┘  └──────┬───────┘  └──────┬───────┘
       │                 │                 │
       │ Price structs   │                 │
       ↓                 ↓                 ↓
┌─────────────────────────────────────────────────┐
│        Lock-Free SPSC Queues (per exchange)     │  ← Per-thread queues
└──────────────────────┬──────────────────────────┘
                       │
                       ↓
              ┌────────────────┐
              │ Price Aggregator│  ← Single consumer thread
              │     Thread      │
              └────────┬────────┘
                       │
                       ↓
              ┌─────────────────┐
              │ Shared Price Map│  ← Atomic updates or RCU
              │  (BTC/USD: {    │
              │   Binance: X,   │
              │   Coinbase: Y}) │
              └────────┬─────────┘
                       │
         ┌─────────────┼─────────────┐
         │             │             │
         ↓             ↓             ↓
  ┌──────────┐  ┌──────────┐ ┌──────────┐
  │ Detector │  │ Detector │ │ Detector │  ← Multiple reader threads
  │ Thread 1 │  │ Thread 2 │ │ Thread 3 │     (parallel analysis)
  └─────┬────┘  └─────┬────┘ └─────┬────┘
        │             │             │
        └─────────────┼─────────────┘
                      ↓
            ┌──────────────────┐
            │ Alert/Order Queue│  ← Lock-free MPSC queue
            └─────────┬─────────┘
                      ↓
              ┌───────────────┐
              │ Alert Handler │  ← Console/log/trading
              │     Thread    │
              └───────────────┘

--------------------------------------------------------------------------

**Key Principle:** Minimize lock contention by separating writes (fewer) from reads (many)

### 2. Component-Level Synchronization Strategy

#### Use Lock-Free For:
- **Price feed queues**: High-frequency updates from websockets → lock-free SPSC/MPSC queues
- **Price state**: If many detector threads reading, consider `std::atomic` or RCU patterns
- **Alert/order queues**: Lock-free for low-latency order execution

#### Use Mutexes For:
- **Order book management**: Complex state, mutex is simpler
- **Configuration updates**: Infrequent, mutex is fine
- **Logging/statistics**: Not performance-critical

### 3. Critical Performance Factors
1. **Websocket parsing** - Often the bottleneck, not locks
2. **Network latency** - Between exchanges (can't optimize away)
3. **Memory allocation** - Pre-allocate buffers
4. **Cache coherency** - Keep hot data together, avoid false sharing

### 4. Detailed Data Flow Visualization

```
┌─────────────────────────────────────────────────────────────────┐
│                    ARBITRAGE DETECTOR SYSTEM                    │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│  Binance WS  │  │ Coinbase WS  │  │  Kraken WS   │  ← Exchange APIs
│   Thread 1   │  │   Thread 2   │  │   Thread 3   │
└──────┬───────┘  └──────┬───────┘  └──────┬───────┘
       │                 │                 │
       │ Raw JSON        │ Raw JSON        │ Raw JSON
       ↓                 ↓                 ↓
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ JSON Parser  │  │ JSON Parser  │  │ JSON Parser  │
└──────┬───────┘  └──────┬───────┘  └──────┬───────┘
       │                 │                 │
       │ Price structs   │                 │
       ↓                 ↓                 ↓
┌─────────────────────────────────────────────────┐
│        Lock-Free SPSC Queues (per exchange)     │  ← Per-thread queues
└──────────────────────┬──────────────────────────┘
                       │
                       ↓
              ┌────────────────┐
              │ Price Aggregator│  ← Single consumer thread
              │     Thread      │
              └────────┬────────┘
                       │
                       ↓
              ┌─────────────────┐
              │ Shared Price Map│  ← Atomic updates or RCU
              │  (BTC/USD: {    │
              │   Binance: X,   │
              │   Coinbase: Y}) │
              └────────┬─────────┘
                       │
         ┌─────────────┼─────────────┐
         │             │             │
         ↓             ↓             ↓
  ┌──────────┐  ┌──────────┐ ┌──────────┐
  │ Detector │  │ Detector │ │ Detector │  ← Multiple reader threads
  │ Thread 1 │  │ Thread 2 │ │ Thread 3 │     (parallel analysis)
  └─────┬────┘  └─────┬────┘ └─────┬────┘
        │             │             │
        └─────────────┼─────────────┘
                      ↓
            ┌──────────────────┐
            │ Alert/Order Queue│  ← Lock-free MPSC queue
            └─────────┬─────────┘
                      ↓
              ┌───────────────┐
              │ Alert Handler │  ← Console/log/trading
              │     Thread    │
              └───────────────┘
```

## Build Timeline & Milestones

**Total Realistic Timeline: 6-10 weeks** (working a few hours per day as a student/solo developer)

### Phase 1: Basic WebSocket Connection ⏱️ 3-5 days
- [ ] Install µWebSockets and dependencies (SSL libraries)
- [ ] Get a single websocket connection working
- [ ] Handle one exchange's test stream
- [ ] Print raw messages to console
- **Challenges:** Build system setup, SSL configuration, understanding callbacks
- **Deliverable:** Console app that prints live Binance prices

### Phase 2: JSON Parsing ⏱️ 2-3 days
- [ ] Add JSON library (nlohmann/json or simdjson)
- [ ] Parse exchange-specific JSON formats
- [ ] Extract price data into structs
- [ ] Handle malformed messages gracefully
- **Challenges:** Each exchange has different JSON schemas
- **Deliverable:** Structured price data printed to console

### Phase 3: Multi-Exchange Support ⏱️ 3-4 days
- [ ] Connect to 2-3 exchanges simultaneously
- [ ] Run each in separate threads
- [ ] Normalize data formats across exchanges
- [ ] Handle reconnection logic
- **Challenges:** Thread management, different exchange quirks
- **Deliverable:** App showing prices from multiple exchanges

### Phase 4: Lock-Free Queues ⏱️ 4-6 days
- [ ] Implement or integrate SPSC queue (boost::lockfree or folly)
- [ ] Connect websocket threads to aggregator
- [ ] Test queue performance and correctness
- [ ] Debug concurrency issues
- **Challenges:** This is HARD - subtle bugs, memory ordering
- **Deliverable:** Working producer-consumer pipeline with queues
- **Alternative:** Start with mutexes, saves 2-3 days

### Phase 5: Price Aggregation ⏱️ 2-3 days
- [ ] Single thread consumes all queues
- [ ] Maintain current price state per exchange
- [ ] Implement atomic updates or RCU pattern
- **Challenges:** Choosing the right data structure
- **Deliverable:** Central price map with latest prices from all exchanges

### Phase 6: Arbitrage Detection ⏱️ 3-5 days
- [ ] Implement detector threads reading shared state
- [ ] Calculate cross-exchange spreads
- [ ] Identify arbitrage opportunities
- [ ] Account for fees and slippage
- **Challenges:** Accurate profitability calculations
- **Deliverable:** Real-time arbitrage opportunity detection

### Phase 7: Alert System ⏱️ 2-3 days
- [ ] Output queue for opportunities
- [ ] Console logging or file output
- [ ] Maybe Telegram/email alerts
- **Challenges:** Not very hard if just logging
- **Deliverable:** Alert notifications for profitable opportunities

### Phase 8: Testing & Optimization ⏱️ 5-10 days
- [ ] Stress testing with high message rates
- [ ] Profile for bottlenecks
- [ ] Fix race conditions and memory leaks
- [ ] Tune performance
- **Challenges:** Finding and fixing subtle bugs
- **Deliverable:** Production-ready system

### Build Order (Critical Path)
1. **Week 1-2:** Single websocket + JSON parsing (Phases 1-2)
2. **Week 3:** Multi-exchange connections (Phase 3)
3. **Week 4-5:** Lock-free queues (Phase 4) - *expect frustration here!*
4. **Week 6:** Price aggregation + basic detection (Phases 5-6)
5. **Week 7-8:** Polish, testing, optimization (Phases 7-8)

### Time-Saving Tips
- **Start with mutexes first** - get it working in 4 weeks, then optimize
- **Use existing queue libraries** - don't write your own lock-free queue
- **Test with one exchange** before adding more
- **Use debuggers and sanitizers** (ThreadSanitizer, AddressSanitizer)
- **Profile before optimizing** - don't guess where the bottlenecks are

## Development Philosophy

**Rule of thumb:** Start with mutexes for correctness, then profile. Only optimize to lock-free if measurements show contention is a bottleneck.

## Questions to Resolve
- Which exchanges to support?
- Real trading or just detection/alerting?
- What arbitrage types to detect (simple, triangular, cross-exchange)?
- Latency requirements?
- Scale (how many pairs/exchanges)?
