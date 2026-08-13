# Step 18.5 Logging Overhead Analysis

## Objective

Evaluate the impact of synchronous request-path logging
on server latency and throughput.

## Changes

- Introduced Logger abstraction
- Replaced request-path `std::cout` with Logger
- Added ability to disable verbose logging during benchmarks

---

# Benchmark Configuration

Server:

- Linux C++ Event-driven TCP Server
- epoll Reactor model
- ThreadPool: 4 workers


---

# Experiment 1: Moderate Load

Workload:

```

Clients: 50
Requests/client: 100
Total requests: 5000

```

## Result

| Metric | Logging Enabled | Logging Disabled |
|---|---:|---:|
| P50 latency | 447848 us | 443016 us |
| P99 latency | 754844 us | 794236 us |
| Throughput | 107.4 req/s | 104.5 req/s |

Observation:

Under moderate load, disabling logging does not
significantly improve performance.

---

# Experiment 2: High Load

Workload:

```

Clients: 100
Requests/client: 200
Total requests: 20000

```

## Result

| Metric | Logging Enabled | Logging Disabled |
|---|---:|---:|
| P50 latency | 1022837 us | 883626 us |
| P99 latency | 1929177 us | 1079957 us |
| Throughput | 92.59 req/s | 112.70 req/s |

## Improvement

- P50 latency reduced by ~13.6%
- P99 latency reduced by ~44%
- Throughput improved by ~21.7%

---

# Analysis

Logging overhead is workload dependent.

- Under moderate load, logging is not the main bottleneck.
- Under high concurrency, synchronous logging increases
  contention and affects tail latency.

The result shows that request-path logging can become
a scalability bottleneck when the server is under heavy load.

---

# Conclusion

Replacing synchronous logging with a controllable Logger
abstraction improves performance under high contention,
while having limited impact under normal workload.
