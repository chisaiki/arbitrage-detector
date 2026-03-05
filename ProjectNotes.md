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

## Next Steps & Milestones

### Phase 1: Websocket Integration
- [ ] Set up websocket connections to real exchanges (Binance, Coinbase, etc.)
- [ ] Parse JSON price feeds
- [ ] Handle reconnection and error cases

### Phase 2: Architecture Design
- [ ] Design the production architecture
- [ ] Define data structures for price state
- [ ] Plan thread model and synchronization strategy

### Phase 3: Performance Benchmarking
- [ ] Benchmark current mutex implementation
- [ ] Implement lock-free version
- [ ] Compare performance under different loads

### Phase 4: Detection Logic
- [ ] Implement multi-exchange arbitrage detection
- [ ] Add triangular arbitrage detection
- [ ] Optimize detection algorithms

### Phase 5: Production Features
- [ ] Add logging and monitoring
- [ ] Implement alerting system
- [ ] Add configuration management
- [ ] Build order execution (if trading)

## Development Philosophy

**Rule of thumb:** Start with mutexes for correctness, then profile. Only optimize to lock-free if measurements show contention is a bottleneck.

## Questions to Resolve
- Which exchanges to support?
- Real trading or just detection/alerting?
- What arbitrage types to detect (simple, triangular, cross-exchange)?
- Latency requirements?
- Scale (how many pairs/exchanges)?
