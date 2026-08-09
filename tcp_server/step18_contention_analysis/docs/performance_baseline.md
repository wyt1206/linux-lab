# Performance Baseline

## Project

Linux C++ Event-driven TCP Server

---

# 1. Architecture

The server is implemented as a Linux-based C++ event-driven TCP server framework.

The current architecture includes:

* TCP socket server
* epoll-based Reactor pattern
* EventLoop abstraction
* Channel abstraction
* Acceptor for connection management
* TcpConnection lifecycle management
* Non-blocking socket handling
* ThreadPool-based asynchronous request processing
* Asynchronous response handling with write buffering

High-level architecture:

```
Client
  |
  |
TCP Socket
  |
  |
Acceptor
  |
  |
EventLoop + epoll
  |
  |
TcpConnection
  |
  |
ThreadPool
  |
  |
Async Response
```

---

# 2. Evaluation Components

To evaluate runtime behavior and performance characteristics, the following components are introduced:

## Runtime Metrics

The server collects internal runtime metrics:

* active connections
* total requests
* total responses
* ThreadPool queue depth

These metrics provide visibility into internal execution behavior during workload execution.

## Benchmark Workload Generator

A multi-client benchmark client is used to generate concurrent TCP workloads.

The benchmark measures:

* latency distribution
* throughput
* request completion rate

---

# 3. Environment

## Operating System

Linux

## CPU

TBD

## Memory

TBD

## Compiler

GCC / G++

---

# 4. Server Configuration

Server port:

```
8080
```

ThreadPool configuration:

```
Worker threads: 4
```

---

# 5. Benchmark Workload

## Test 1: Normal Workload Baseline

Configuration:

```
Clients: 10

Requests per client: 1000

Total Requests: 10000
```

Command:

```bash
./benchmark_client 10 1000
```

---

# 6. Performance Result

Example result:

```
========== Benchmark Result ==========

Requests: 10000

P50 latency(us): 222

P99 latency(us): 937

Throughput(req/s): 36118.3

======================================
```

---

# 7. Runtime Observation

In addition to external benchmark metrics, internal runtime metrics are collected.

Example:

```
========== Runtime Metrics ==========

Connections: 0

Requests: 10000

Responses: 10000

ThreadPool Queue: 0

=====================================
```

---

# 8. Baseline Analysis

The baseline workload demonstrates the current performance characteristics before resource contention analysis and low-latency optimization.

## Request Processing Correctness

The server successfully completes all benchmark requests.

Expected behavior:

```
Requests == Responses
```

This confirms that the Reactor event loop, asynchronous processing model, and connection lifecycle management operate correctly.

---

## Connection Lifecycle

After benchmark completion:

```
Connections: 0
```

indicates that client connections are properly closed and removed from the server connection manager.

---

## ThreadPool Behavior

Under normal workload:

```
ThreadPool Queue: 0
```

The worker threads are able to process incoming tasks without noticeable queue buildup.

This indicates that the current workload does not create significant thread scheduling contention.

---

# 9. Baseline Limitations

Although the server demonstrates stable performance under normal workload, the current baseline does not expose behavior under resource pressure.

Potential performance bottlenecks include:

* ThreadPool worker saturation
* task queue buildup
* increased tail latency
* CPU scheduling overhead
* resource contention between tasks

Additional experiments are required to understand how internal resource pressure affects external latency.

---

# 10. Future Optimization Targets

The following experiments will extend the baseline:

## Resource Contention Analysis

Analyze:

* ThreadPool queue growth
* worker utilization
* workload pressure impact

## Low Latency Optimization

Investigate:

* scheduling overhead
* unnecessary context switching
* event processing latency
* runtime resource management

## Future Steps

* Step 18.3: Introduce workload contention simulation
* Step 18.4: Analyze latency/resource relationship
* Step 19: Runtime-aware Resource Management
* Step 20: Hardware-aware Optimization

```
```
