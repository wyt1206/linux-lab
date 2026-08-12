# Step 19 Backpressure Analysis

## Objective

Analyze server behavior when request arrival rate
exceeds ThreadPool processing capacity.

---

# 19.1 Overload Behavior

## Workload

Clients: 100

Requests/client: 200

Total Requests: 20000

## Benchmark Result

P50 latency: 936480 us

P99 latency: 1349665 us

Throughput: 105.323 req/s

## Runtime Metrics

ThreadPool Queue Max: 97

Average Queue Wait: 908659 us

Average Execution Time: 37973 us

## Observation

Under high concurrency, worker threads become saturated
and tasks accumulate in the ThreadPool queue.

The average queue wait time is significantly higher than
the task execution time, indicating that queueing is a major
contributor to request latency.

The current ThreadPool uses an unbounded task queue.
Therefore, sustained overload may allow the queue to grow
continuously.

This motivates introducing a bounded task queue and a
task rejection strategy.
