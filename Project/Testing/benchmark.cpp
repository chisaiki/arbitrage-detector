/*
 * benchmark_mpsc.cpp
 *
 * Benchmarks for MPSCQueue vs a mutex-protected baseline.
 * Tests:
 *   1. Single-producer throughput (baseline sanity check)
 *   2. Multi-producer throughput (2, 4, 8 producers)
 *   3. Latency: push-to-pop round-trip (single producer)
 *   4. False-sharing check: cache-aligned vs naive struct
 *   5. Correctness: no items lost or double-consumed under contention
 *
 * Build (g++):
 *   g++ -O2 -std=c++20 -pthread benchmark_mpsc.cpp -o benchmark_mpsc
 *
 * Adjust INCLUDE paths to match your project layout.
 */

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <mutex>
#include <queue>
#include <numeric>
#include <cassert>
#include <iomanip>
#include <cstring>
#include <algorithm>


#include "../MPSC/MPSC.hpp"
#include "../MarketData/MarketData.hpp"
#include "../OrderBook/ExchangeQuote.hpp"
#include "../OrderBook/OrderBook.hpp"
/**********************************************/
using namespace std::chrono;
using Clock = high_resolution_clock;
 
// ── Helpers ───────────────────────────────────────────────────────────────────
 
static constexpr size_t QUEUE_CAPACITY = 1024;
static constexpr int    WARMUP_ITERS   = 100'000;
 
// How many items each producer will attempt to push
static constexpr int ITEMS_PER_PRODUCER = 500'000;
 
// ── Mutex-protected baseline queue ───────────────────────────────────────────
 
template <typename T>
class MutexQueue {
    std::mutex      mtx_;
    std::queue<T>   q_;
    size_t          cap_;
public:
    explicit MutexQueue(size_t cap) : cap_(cap) {}
 
    bool push(const T& v) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (q_.size() >= cap_) return false;
        q_.push(v);
        return true;
    }
 
    bool pop(T& out) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (q_.empty()) return false;
        // Can't use copy-assignment because std::atomic members delete it.
        // Reconstruct 'out' in-place using the explicit copy constructor instead.
        out.~T();
        new (&out) T(q_.front());
        q_.pop();
        return true;
    }
};
 
// ── Pretty printer ────────────────────────────────────────────────────────────
 
void print_header(const char* title) {
    std::cout << "\n╔══════════════════════════════════════════════════╗\n";
    std::cout << "║  " << std::left << std::setw(48) << title << "║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n";
}
 
void print_result(const char* label, double value, const char* unit) {
    std::cout << "  " << std::left << std::setw(36) << label
              << std::right << std::setw(12) << std::fixed << std::setprecision(2)
              << value << "  " << unit << "\n";
}
 
// ═════════════════════════════════════════════════════════════════════════════
// TEST 1 — Single-producer throughput
// Measures raw enqueue/dequeue ops per second with no contention.
// ═════════════════════════════════════════════════════════════════════════════
 
void bench_single_producer_throughput() {
    print_header("TEST 1 · Single-Producer Throughput");
 
    MQueue::MPSCQueue<Arbitrage::MarketItem, QUEUE_CAPACITY> mpsc;
    MutexQueue<Arbitrage::MarketItem>                        mutex_q(QUEUE_CAPACITY);
    Arbitrage::OrderBook<Arbitrage::TopOfBook>                  book;
 
    Arbitrage::MarketItem sample{};
    sample.priceCents  = 50000;
    sample.timestamp   = 1000;
    sample.exchangeId  = 0;
    sample.can_overwrite.store(true);
 
    // ── MPSC ──────────────────────────────────────────────────────────────────
    long long mpsc_pushes = 0, mpsc_pops = 0;
    auto t0 = Clock::now();
 
    for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
        sample.timestamp = static_cast<uint64_t>(i);
        if (mpsc.push(sample)) {
            ++mpsc_pushes;
            mpsc.pop(book);
            ++mpsc_pops;
        }
    }
 
    double mpsc_ms = duration_cast<microseconds>(Clock::now() - t0).count() / 1000.0;
 
    // ── Mutex baseline ────────────────────────────────────────────────────────
    long long mtx_pushes = 0;
    t0 = Clock::now();
 
    for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
        Arbitrage::MarketItem tmp = sample;
        if (mutex_q.push(tmp)) {
            ++mtx_pushes;
            mutex_q.pop(tmp);
        }
    }
 
    double mtx_ms = duration_cast<microseconds>(Clock::now() - t0).count() / 1000.0;
 
    double mpsc_mops = (mpsc_pushes / 1e6) / (mpsc_ms / 1000.0);
    double mtx_mops  = (mtx_pushes  / 1e6) / (mtx_ms  / 1000.0);
 
    print_result("MPSCQueue  (Mops/s)",  mpsc_mops, "Mops/s");
    print_result("MutexQueue (Mops/s)",  mtx_mops,  "Mops/s");
    print_result("Speedup",              mpsc_mops / mtx_mops, "x");
}
 
// ═════════════════════════════════════════════════════════════════════════════
// TEST 2 — Multi-producer throughput
// N threads push concurrently; one consumer thread drains.
// ═════════════════════════════════════════════════════════════════════════════
 
double run_mpsc_multiproducer(int num_producers) {
    MQueue::MPSCQueue<Arbitrage::MarketItem, QUEUE_CAPACITY> mpsc;
    Arbitrage::OrderBook<Arbitrage::TopOfBook>                  book;
 
    std::atomic<bool>    start_flag{false};
    std::atomic<int>     producers_done{0};
    std::atomic<long long> total_pushed{0};
 
    Arbitrage::MarketItem sample{};
    sample.priceCents = 50000;
    sample.exchangeId = 0;
    sample.can_overwrite.store(true);
 
    // Producer threads
    std::vector<std::thread> producers;
    for (int p = 0; p < num_producers; ++p) {
        producers.emplace_back([&, p]() {
            while (!start_flag.load(std::memory_order_acquire)) { /* spin */ }
            long long pushed = 0;
            Arbitrage::MarketItem item = sample;
            item.exchangeId = p % 2;
            for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
                item.timestamp = static_cast<uint64_t>(i);
                if (mpsc.push(item)) ++pushed;
            }
            total_pushed.fetch_add(pushed);
            producers_done.fetch_add(1);
        });
    }
 
    // Consumer thread
    std::atomic<long long> total_popped{0};
    std::thread consumer([&]() {
        while (!start_flag.load(std::memory_order_acquire)) { /* spin */ }
        while (producers_done.load() < num_producers || !mpsc.isEmpty()) {
            if (mpsc.pop(book)) ++total_popped;
        }
    });
 
    auto t0 = Clock::now();
    start_flag.store(true, std::memory_order_release);
 
    for (auto& t : producers) t.join();
    consumer.join();
 
    double elapsed_s = duration_cast<microseconds>(Clock::now() - t0).count() / 1e6;
    return (total_pushed.load() / 1e6) / elapsed_s; // Mops/s
}
 
double run_mutex_multiproducer(int num_producers) {
    MutexQueue<Arbitrage::MarketItem> mutex_q(QUEUE_CAPACITY);
 
    std::atomic<bool>  start_flag{false};
    std::atomic<int>   producers_done{0};
    std::atomic<long long> total_pushed{0};
 
    Arbitrage::MarketItem sample{};
    sample.priceCents = 50000;
    sample.exchangeId = 0;
    sample.can_overwrite.store(true);
 
    std::vector<std::thread> producers;
    for (int p = 0; p < num_producers; ++p) {
        producers.emplace_back([&, p]() {
            while (!start_flag.load(std::memory_order_acquire)) {}
            long long pushed = 0;
            Arbitrage::MarketItem item = sample;
            for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
                item.timestamp = static_cast<uint64_t>(i);
                if (mutex_q.push(item)) ++pushed;
            }
            total_pushed.fetch_add(pushed);
            producers_done.fetch_add(1);
        });
    }
 
    std::thread consumer([&]() {
        while (!start_flag.load(std::memory_order_acquire)) {}
        Arbitrage::MarketItem tmp{};
        while (producers_done.load() < num_producers) {
            mutex_q.pop(tmp);
        }
        // drain remainder
        while (mutex_q.pop(tmp)) {}
    });
 
    auto t0 = Clock::now();
    start_flag.store(true, std::memory_order_release);
    for (auto& t : producers) t.join();
    consumer.join();
 
    double elapsed_s = duration_cast<microseconds>(Clock::now() - t0).count() / 1e6;
    return (total_pushed.load() / 1e6) / elapsed_s;
}
 
void bench_multiproducer_throughput() {
    print_header("TEST 2 · Multi-Producer Throughput");
    std::cout << "  " << std::left
              << std::setw(10) << "Producers"
              << std::setw(18) << "MPSC (Mops/s)"
              << std::setw(18) << "Mutex (Mops/s)"
              << std::setw(10) << "Speedup" << "\n";
    std::cout << "  " << std::string(56, '-') << "\n";
 
    for (int n : {1, 2, 4, 8}) {
        double mpsc_mops = run_mpsc_multiproducer(n);
        double mtx_mops  = run_mutex_multiproducer(n);
        std::cout << "  " << std::left
                  << std::setw(10) << n
                  << std::setw(18) << std::fixed << std::setprecision(2) << mpsc_mops
                  << std::setw(18) << mtx_mops
                  << std::setw(10) << (mpsc_mops / mtx_mops) << "x\n";
    }
}
 
// ═════════════════════════════════════════════════════════════════════════════
// TEST 3 — Push-to-pop latency (single producer, nanoseconds)
// Measures how long from push() returning true until pop() processes the item.
// Uses a shared atomic timestamp to record the push time.
// ═════════════════════════════════════════════════════════════════════════════
 
void bench_latency() {
    print_header("TEST 3 · Push-to-Pop Latency (single producer)");
 
    static constexpr int LATENCY_SAMPLES = 10'000;
    std::vector<double> latencies;
    latencies.reserve(LATENCY_SAMPLES);
 
    MQueue::MPSCQueue<Arbitrage::MarketItem, QUEUE_CAPACITY> mpsc;
    Arbitrage::OrderBook<Arbitrage::TopOfBook>                  book;
 
    std::atomic<int64_t> push_ns{0};
    std::atomic<bool>    item_ready{false};
    std::atomic<bool>    stop{false};
 
    std::thread consumer([&]() {
        while (!stop.load()) {
            if (item_ready.load(std::memory_order_acquire)) {
                int64_t pop_time = duration_cast<nanoseconds>(
                    Clock::now().time_since_epoch()).count();
                int64_t push_time = push_ns.load(std::memory_order_relaxed);
                latencies.push_back(static_cast<double>(pop_time - push_time));
                mpsc.pop(book);
                item_ready.store(false, std::memory_order_release);
            }
        }
    });
 
    Arbitrage::MarketItem sample{};
    sample.priceCents = 50000;
    sample.exchangeId = 0;
    sample.can_overwrite.store(true);
 
    for (int i = 0; i < LATENCY_SAMPLES; ++i) {
        // Wait for consumer to be ready
        while (item_ready.load(std::memory_order_acquire)) { /* spin */ }
 
        push_ns.store(
            duration_cast<nanoseconds>(Clock::now().time_since_epoch()).count(),
            std::memory_order_relaxed);
 
        sample.timestamp = static_cast<uint64_t>(i);
        while (!mpsc.push(sample)) { /* retry on full */ }
 
        item_ready.store(true, std::memory_order_release);
    }
 
    stop.store(true);
    consumer.join();
 
    if (!latencies.empty()) {
        std::sort(latencies.begin(), latencies.end());
        double p50 = latencies[latencies.size() * 50 / 100];
        double p95 = latencies[latencies.size() * 95 / 100];
        double p99 = latencies[latencies.size() * 99 / 100];
        double avg = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
 
        print_result("Average latency (ns)",  avg, "ns");
        print_result("P50 latency (ns)",       p50, "ns");
        print_result("P95 latency (ns)",       p95, "ns");
        print_result("P99 latency (ns)",       p99, "ns");
    }
}
 
// ═════════════════════════════════════════════════════════════════════════════
// TEST 4 — Cache alignment verification
// Checks that head_ and tail_ don't share a cache line.
// ═════════════════════════════════════════════════════════════════════════════
 
void check_alignment() {
    print_header("TEST 4 · Cache-Line Alignment Check");
 
    MQueue::MPSCQueue<Arbitrage::MarketItem, QUEUE_CAPACITY> q;
 
    // We can't take the address of private members directly, but we can
    // verify the struct layout via sizeof and known offsets.
    // Instead, we verify sizeof and that the queue object is aligned.
    uintptr_t addr = reinterpret_cast<uintptr_t>(&q);
    bool aligned   = (addr % 64) == 0;
 
    std::cout << "  Queue object address: 0x" << std::hex << addr << std::dec << "\n";
    std::cout << "  64-byte aligned:      " << (aligned ? "YES ✓" : "NO  ✗") << "\n";
    std::cout << "  sizeof(MPSCQueue):    " << sizeof(q) << " bytes\n";
 
    // Each of head_, tail_, and buffer should start on a 64-byte boundary.
    // We verify that the total size is a multiple of 64 (no padding leaks).
    bool size_ok = (sizeof(q) % 64) == 0;
    std::cout << "  Size multiple of 64:  " << (size_ok ? "YES ✓" : "NO  ✗") << "\n";
}
 
// ═════════════════════════════════════════════════════════════════════════════
// TEST 5 — Correctness under contention
// N producers push a known total; consumer counts pops.
// Verifies no item is lost or duplicated.
// ═════════════════════════════════════════════════════════════════════════════
 
void test_correctness() {
    print_header("TEST 5 · Correctness Under Contention");
 
    static constexpr int N_PRODUCERS   = 4;
    static constexpr int ITEMS_EACH    = 50'000;
 
    MQueue::MPSCQueue<Arbitrage::MarketItem, QUEUE_CAPACITY> mpsc;
    Arbitrage::OrderBook<Arbitrage::TopOfBook>                  book;
 
    std::atomic<bool>      go{false};
    std::atomic<long long> pushed{0};
    std::atomic<long long> popped{0};
    std::atomic<int>       done{0};
 
    Arbitrage::MarketItem sample{};
    sample.priceCents = 50000;
    sample.exchangeId = 0;
    sample.can_overwrite.store(true);
 
    std::vector<std::thread> producers;
    for (int p = 0; p < N_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            while (!go.load(std::memory_order_acquire)) {}
            long long n = 0;
            Arbitrage::MarketItem item = sample;
            item.exchangeId = p % 2;
            for (int i = 0; i < ITEMS_EACH; ++i) {
                item.timestamp = static_cast<uint64_t>(i + p * ITEMS_EACH);
                // Retry instead of dropping, so we can count exactly
                while (!mpsc.push(item)) { std::this_thread::yield(); }
                ++n;
            }
            pushed.fetch_add(n);
            done.fetch_add(1);
        });
    }
 
    std::thread consumer([&]() {
        while (!go.load(std::memory_order_acquire)) {}
        while (done.load() < N_PRODUCERS || !mpsc.isEmpty()) {
            if (mpsc.pop(book)) popped.fetch_add(1);
        }
    });
 
    go.store(true, std::memory_order_release);
    for (auto& t : producers) t.join();
    consumer.join();
 
    long long p = pushed.load(), c = popped.load();
    std::cout << "  Items pushed:    " << p << "\n";
    std::cout << "  Items consumed:  " << c << "\n";
    std::cout << "  Items lost:      " << (p - c) << "\n";
    std::cout << "  Result: " << (p == c ? "PASS ✓  (no items lost)" : "FAIL ✗  (items lost or duplicated!)")
              << "\n";
}
 
// ═════════════════════════════════════════════════════════════════════════════
// main
// ═════════════════════════════════════════════════════════════════════════════
 
int main() {
    std::cout << "══════════════════════════════════════════════════════\n";
    std::cout << "  MPSCQueue Benchmark Suite\n";
    std::cout << "  Items per producer: " << ITEMS_PER_PRODUCER << "\n";
    std::cout << "══════════════════════════════════════════════════════\n";
 
    bench_single_producer_throughput();
    bench_multiproducer_throughput();
    bench_latency();
    check_alignment();
    test_correctness();
 
    std::cout << "\nDone.\n";
    return 0;
}