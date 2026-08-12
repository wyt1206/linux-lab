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

## 19.2 Add Bounded ThreadPool Queue
### Configuration
Queue capacity: 100

Benchmark

Clients: 100
Requests/client: 200
Total requests: 20000
P50 latency: 877704 us
P99 latency: 2793617 us
Throughput: 105.622 req/s

Runtime Metrics

Queue Max: 98
Average Queue Wait: 906276 us
Average Execution Time: 37866 us

Observation

The bounded queue limits the queue size, but simply bounding the
queue does not provide effective overload handling yet.

The queue still approaches its capacity and requests experience
significant queue waiting time.

## 19.3 — Handle Task Rejection
The ThreadPool now rejects new tasks when the queue reaches its
configured capacity.
For validation, the queue capacity was temporarily set to 10.
### Benchmark
```text
Clients: 100
Requests/client: 200
Total requests: 20000
P50 latency: 1078 us
P99 latency: 114782 us
Throughput: 2073.03 req/s

Runtime Metrics

Queue Max: 10
Average Queue Wait: 51080 us
Average Execution Time: 33875 us
Request Buffers: 20000
Response Buffers: 1072
Write Buffer Appends: 20000
Rejected Requests: 18928

Observation

The bounded queue prevents requests from accumulating indefinitely.

When the queue reaches capacity, new tasks are rejected and the
client receives an overload response.

The queue wait time decreased significantly compared with the
unbounded queue configuration.

The throughput improvement should not be interpreted as increased
processing capacity, because most requests were rejected during this
overload test.
