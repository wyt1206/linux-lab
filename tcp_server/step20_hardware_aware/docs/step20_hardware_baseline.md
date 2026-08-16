# Step 20.1 — Hardware Baseline

## Objective

Establish the hardware topology and current server performance
before applying hardware-aware optimizations.

The goal is to understand the CPU, cache, and NUMA environment
before changing thread placement or memory behavior.

---

# 1. CPU Topology

Collected using:

```bash
lscpu
lscpu -e
nproc
```

## CPU Summary

```text
Architecture: aarch64
Vendor: Apple
CPUs: 10
Threads per core: 1
Cores per cluster: 10
Clusters: 1
```

The server is running inside a Docker container on an
Apple Silicon system.

CPU topology:

```text
Single CPU cluster
├── CPU 0
├── CPU 1
├── CPU 2
├── CPU 3
├── CPU 4
├── CPU 5
├── CPU 6
├── CPU 7
├── CPU 8
└── CPU 9
```

---

# 2. Cache Topology

Reported by `lscpu`:

```text
L1d: 1.3 MiB (10 instances)
L1i: 1.9 MiB (10 instances)
L2: 48 MiB (3 instances)
L3: 64 MiB (1 instance)
```

CPU/cache mapping reported by `lscpu -e`:

```text
CPU 0-3  -> L2 domain 0
CPU 4-7  -> L2 domain 1024
CPU 8-9  -> L2 domain 2048
```

This shows that multiple CPUs share L2 cache domains.

The cache topology will be relevant to later experiments involving
CPU placement and cache locality.

---

# 3. NUMA Topology

Collected using:

```bash
numactl --hardware
```

Result:

```text
available: 0 nodes ()
No distance information available.
```

The current Docker environment does not expose NUMA topology
through `numactl`.

Therefore, NUMA-specific experiments may require a different
environment where NUMA topology is visible to the container.

---

# 4. Performance Baseline

The current server configuration was kept unchanged.

ThreadPool queue capacity:

```text
100
```

Workload:

```text
Clients: 100
Requests/client: 200
Total requests: 20000
```

## Benchmark Result

```text
========== Benchmark Result ==========
Requests: 20000
P50 latency(us): 1033816
P99 latency(us): 1676816
Throughput(req/s): 93.599
```

## Runtime Metrics

```text
========== Runtime Metrics ==========
Connections: 0
Requests: 0
Responses: 20000
ThreadPool Queue Current: 0
ThreadPool Queue Max: 97
Average Queue Wait(us): 1022467
Average Execution Time(us): 42728
Request Buffers: 20000
Response Buffers: 20000
Write Buffer Appends: 20000
Rejected Requests: 0
=====================================
```

---

# 5. Observation

The workload produces significant ThreadPool queueing.

Average queue wait:

```text
~1.02 seconds
```

Average task execution time:

```text
~42.7 milliseconds
```

Queue waiting time is therefore much larger than task execution
time.

The current performance is primarily affected by queueing delay
under this workload.

The hardware topology and performance results are recorded as the
baseline for subsequent hardware-aware optimization experiments.

---

# 6. Baseline Summary

| Category          | Result                   |
| ----------------- | ------------------------ |
| Architecture      | aarch64                  |
| CPU Vendor        | Apple                    |
| CPUs              | 10                       |
| Threads/Core      | 1                        |
| Cores/Cluster     | 10                       |
| Clusters          | 1                        |
| L1d               | 1.3 MiB                  |
| L1i               | 1.9 MiB                  |
| L2                | 48 MiB                   |
| L3                | 64 MiB                   |
| NUMA              | Not exposed in container |
| P50               | 1033.816 ms              |
| P99               | 1676.816 ms              |
| Throughput        | 93.599 req/s             |
| Queue Max         | 97                       |
| Queue Wait        | 1022.467 ms              |
| Execution Time    | 42.728 ms                |
| Rejected Requests | 0                        |

---

## Conclusion

Step 20.1 establishes the hardware and performance baseline
before applying hardware-aware optimizations.

The next steps will investigate whether CPU placement, cache
locality, and thread scheduling can affect server performance.


# 7. ThreadPool Worker Placement

## ThreadPool Configuration

The server creates a ThreadPool with four worker threads:

```cpp
threadPool_ = std::make_unique<ThreadPool>(4);
````

The current environment exposes ten logical CPUs:

```text
CPU 0-9
```

Therefore, the server consists of one main/EventLoop thread and
four ThreadPool worker threads.

## Observation Method

Thread placement was observed using:

```bash
ps -T -p <PID> -o pid,tid,psr,pcpu,comm
```

CPU affinity was checked using:

```bash
taskset -pc <PID>
```

The process is allowed to run on all available CPUs:

```text
0-9
```

No explicit CPU affinity is configured.

## Observed Thread Placement

During a CPU-intensive benchmark, the following thread placement
was observed:

| Thread | Role                  | Observed CPU     | CPU Usage |
| ------ | --------------------- | ---------------- | --------: |
| 22379  | EventLoop/main thread | 5 -> 6 -> 2 -> 6 |     ~0.6% |
| 22380  | ThreadPool worker     | 4                |      ~95% |
| 22381  | ThreadPool worker     | 0 -> 1 -> 8      |      ~95% |
| 22382  | ThreadPool worker     | 9                |      ~95% |
| 22383  | ThreadPool worker     | 3                |      ~95% |

The four worker threads were heavily CPU-bound and were observed
running on different logical CPUs.

One worker thread (TID 22381) was observed migrating between
logical CPUs 0, 1, and 8 during the benchmark.

The EventLoop thread also changed its observed CPU placement,
but its CPU utilization remained low.

## Observation

The current implementation does not explicitly control CPU
placement.

The Linux scheduler therefore determines where each EventLoop
and ThreadPool worker thread executes.

Under the current CPU-intensive workload, the scheduler generally
places the four worker threads on different logical CPUs, while
the EventLoop thread consumes very little CPU.

One worker was observed to migrate between CPUs during execution.

At this stage, the impact of thread migration and CPU placement
on performance is not yet determined.
