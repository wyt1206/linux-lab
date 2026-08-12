# Step 19 Backpressure Analysis

## Objective

Analyze server behavior when request arrival rate
exceeds ThreadPool processing capacity.

---

## 19.1 Overload Behavior

### Workload

Clients: 100

Requests/client: 200

Total Requests: 20000

### Benchmark Result

P50 latency: 936480 us

P99 latency: 1349665 us

Throughput: 105.323 req/s

### Runtime Metrics

ThreadPool Queue Max: 97

Average Queue Wait: 908659 us

Average Execution Time: 37973 us

### Observation

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

### Benchmark

Clients: 100
Requests/client: 200
Total requests: 20000
P50 latency: 877704 us
P99 latency: 2793617 us
Throughput: 105.622 req/s

### Runtime Metrics

Queue Max: 98
Average Queue Wait: 906276 us
Average Execution Time: 37866 us

### Observation

The bounded queue limits the queue size, but simply bounding the
queue does not provide effective overload handling yet.

The queue still approaches its capacity and requests experience
significant queue waiting time.

## 19.3 — Handle Task Rejection
The ThreadPool now rejects new tasks when the queue reaches its
configured capacity.
For validation, the queue capacity was temporarily set to 10.

### Benchmark
Clients: 100
Requests/client: 200
Total requests: 20000
P50 latency: 1078 us
P99 latency: 114782 us
Throughput: 2073.03 req/s

### Runtime Metrics

Queue Max: 10
Average Queue Wait: 51080 us
Average Execution Time: 33875 us
Request Buffers: 20000
Response Buffers: 1072
Write Buffer Appends: 20000
Rejected Requests: 18928

### Observation

The bounded queue prevents requests from accumulating indefinitely.

When the queue reaches capacity, new tasks are rejected and the
client receives an overload response.

The queue wait time decreased significantly compared with the
unbounded queue configuration.

The throughput improvement should not be interpreted as increased
processing capacity, because most requests were rejected during this
overload test.

## 19.4 Benchmark Backpressure
The backpressure mechanism was evaluated under increasing workloads
with a ThreadPool queue capacity of 100.
### Workload 1
Clients: 50
Requests/client: 100
Requests: 5000
P50 latency(us): 431268
P99 latency(us): 646396
Throughput(req/s): 111.084
Queue Max: 47
Average Queue Wait(us): 411367
Average Execution Time(us): 35994
Rejected Requests: 0

### Workload 2
Clients: 100
Requests/client: 200
Requests: 20000
P50 latency(us): 895835
P99 latency(us): 1369853
Throughput(req/s): 108.905
Queue Max: 98
Average Queue Wait(us): 878656
Average Execution Time(us): 36725
Rejected Requests: 0

### Workload 3
Clients: 200
Requests/client: 200
Requests: 40000
P50 latency(us): 2866
P99 latency(us): 1243398
Throughput(req/s): 213.084
Queue Max: 100
Average Queue Wait(us): 895008
Average Execution Time(us): 40010
Rejected Requests: 21236

### Observation

As workload increases, the ThreadPool queue approaches its configured
capacity.

At 200 clients × 200 requests, the queue reaches its limit and
backpressure starts rejecting new tasks.

The accepted request count matches the number of response buffers:

40000 - 21236 = 18764

This confirms that the rejection path is working as expected.

The low P50 latency under the overloaded workload should not be
interpreted as increased processing capacity. A large number of
requests are rejected immediately instead of waiting in the queue.

Backpressure therefore trades request admission for bounded queue
growth and controlled overload behavior.
