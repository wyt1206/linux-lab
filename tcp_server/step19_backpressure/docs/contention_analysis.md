# Step 18.3 Workload Contention Analysis


## Objective

Introduce artificial CPU workload
to simulate resource contention
and observe runtime impact.


## Configuration

ThreadPool:
4 workers


## Results


### Test 1

Clients: 10
Requests/client:100

P50:
79 ms

P99:
161 ms

Throughput:
121 req/s


### Test 2

Clients:50
Requests/client:100

P50:
425 ms

P99:
617 ms

Throughput:
113 req/s


### Test 3

Clients:100
Requests/client:200

P50:
1.14 s

P99:
2.08 s

Throughput:
85 req/s


## Runtime Observation

Maximum ThreadPool queue depth:

97


## Analysis

Increasing workload causes:

- worker saturation
- queue buildup
- increased tail latency
- reduced throughput

The experiment demonstrates
runtime contention impact on
event-driven server performance.