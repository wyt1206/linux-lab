# linux-lab

This project is my practice project for learning Linux system programming with C++.

I started from a simple TCP echo server and gradually added more features, including multi-threading, thread pool, epoll-based I/O handling, and Reactor-style event handling.

Through this project, I learned more about:

- TCP socket programming
- Linux file descriptors
- epoll
- multi-thread programming
- event-driven server design

# Development Environment

I developed this project inside a Docker Ubuntu environment.

## Create Docker Environment

```bash
docker run -it \
--name linux-lab \
-v ~/Repos/linux-lab:/workspace \
ubuntu:24.04 \
bash
```

## Install Required Packages

Inside the container:

```bash
apt update

apt install -y \
g++ \
cmake \
make \
git
```

## Start Container

```bash
docker start -ai linux-lab
```

## Enter Running Container

From another terminal:

```bash
docker exec -it linux-lab bash
```

# Build and Run

Inside the container:

```bash
cd /workspace

mkdir build

cd build

cmake ..

make
```

Run:

```bash
./server
```

# Current Design

The server was gradually changed from a simple TCP server to an event-driven server.

Current main components:

Client
  |
TCP Socket
  |
Acceptor
  |
TcpConnection
  |
Channel
  |
EventLoop
  |
epoll

Component description:

| Component     | Description                        |
| ------------- | ---------------------------------- |
| EventLoop     | Handles epoll events               |
| Channel       | Stores fd events and callbacks     |
| Acceptor      | Accepts new client connections     |
| TcpConnection | Handles client connection and data |
| TcpServer     | Manages server                     |
| ThreadPool    | Runs tasks using worker threads    |

---

# How Event Handling Works

```
Client sends data

        |
        v

Kernel updates socket state

        |
        v

epoll_wait() returns event

        |
        v

EventLoop handles event

        |
        v

Channel calls callback

        |
        v

TcpConnection processes data
```

---

# Project Steps

## TCP Server

Implemented basic TCP server:

- Create socket
- Bind port
- Listen for connections
- Accept clients
- Send and receive data

## Multi-thread Server

Added concurrency support:

- One thread per connection
- Thread pool
- Condition variable for task scheduling

## epoll Server

Added Linux epoll support:

- Create epoll instance
- Wait for events using epoll_wait()
- Handle client read events
- Add non-blocking socket
- Add EPOLLET support
- Add write buffer with EPOLLOUT

## Reactor Refactoring

Refactored the code structure:

- Extract EventLoop from server logic
- Add Channel abstraction
- Add Acceptor
- Add TcpConnection
- Manage connection close

## Thread Pool

Added:

- Worker threads
- Task queue
- Thread synchronization

---

# Future Plans

I plan to continue improving this project:

- Add simple benchmark tests
- Measure latency and throughput
- Use Linux perf to understand performance
- Improve logging

---

# What I Learned

Through this project, I practiced:

- Linux socket programming
- TCP connection handling
- epoll-based event handling
- Reactor pattern
- Thread synchronization
- C++ programming
