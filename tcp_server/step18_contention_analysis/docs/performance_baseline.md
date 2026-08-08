# Performance Baseline

## Project

Linux C++ Event-driven TCP Server

## Architecture

- TCP socket server
- epoll-based Reactor pattern
- EventLoop
- Channel abstraction
- TcpConnection lifecycle management
- ThreadPool based asynchronous processing
- Runtime metrics
- Benchmark workload generator

## Environment

OS:
CPU
Memory:
Compiler:

## Server Configuration

Port: 8080
ThreadPool size: 4

## Benchmark Workload

### Test 1

Clients: 10
Requests per client: 1000
Total Requests: 10000

## Result

P50 latency: 222 us
P99 latency: 937 us
Throughput: 36118 req/s

## Observation

The baseline shows the current latency and throughput
before contention analysis and low latency optimization.

Future optimization targets:

- reduce tail latency
- reduce scheduling overhead
- analyze thread contention
- improve resource utilization