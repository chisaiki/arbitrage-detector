# What to look for in tests

## 1024 vs 512 Slots
When you run your benchmarks, don't just measure average pop latency. Measure tail latency (the 99th and 99.9th percentile times) under a simulated burst load. If 512 slots shows lower baseline latency but occasional massive latency spikes for producers, you'll know the extra capacity of 1024 is worth the slight L1 overflow.

I'm testing between the buffer size being either 1024 slots or 512 slots. L1 cache has 32KB (512 lines), so either I fill the entire L1 with my queue, or split with 1024

The Real Benchmark: Burst Capacity vs. Eviction Overhead
When you test 512 vs. 1024, you are primarily testing two competing trade-offs:

With 512 Slots (32 KB): You minimize L1 cache evictions. The buffer memory stays highly localized, leaving more room in your L1 cache for local stack variables or state tracking.

With 1024 Slots (64 KB): You double your headroom for market data bursts. If a sudden market event causes producers to dump 700 items in a microsecond while your consumer is processing an arbitrage opportunity, a 512-slot buffer will fill up and stall your producers.

## Never Hitting Arbitrage

I set my order book data struct to all 0's so its never going to find a price lower than 0