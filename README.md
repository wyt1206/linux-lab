# linux-lab

This project is my practice project for learning Linux system programming with C++.

I started from a simple TCP echo server and gradually added more features, including multi-threading, thread pool, epoll-based I/O handling, and Reactor-style event handling.

Through this project, I learned more about:

- TCP socket programming
- Linux file descriptors
- epoll
- multi-thread programming
- event-driven server design
- thread pool and task scheduling

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
TcpServer

 |
TcpConnection
 |
 +----------------+
 |                |
 v                v
Channel       writeBuffer

 |
EventLoop

 |
epoll


TcpConnection

 |
 v

ThreadPool

 |
 v

Worker Threads

Component description:

| Component     | Description                             |
| ------------- | --------------------------------------- |
| EventLoop     | Handles epoll events                    |
| Channel       | Stores fd events and callbacks          |
| Acceptor      | Accepts new client connections          |
| TcpServer     | Creates and manages connections         |
| TcpConnection | Handles client connection state, read/write buffer and socket I/O |
| ThreadPool    | Runs tasks using worker threads         |

# How Event Handling Works

```
Client sends data

        |
        v

EventLoop detects event using epoll

        |
        v

Channel triggers callback

        |
        v

TcpConnection reads data

        |
        v

Data is stored in write buffer

        |
        v

EPOLLOUT event triggers write

        |
        v

TcpConnection sends data back
```

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
- Handle write events using EPOLLOUT

## Reactor Refactoring

Refactored the code structure:

- Extract EventLoop from server logic
- Add Channel abstraction
- Add Acceptor for accepting connections
- Add TcpConnection for per-client state
- Add TcpServer connection lifecycle management
- Manage connection creation and close
- Separate read and write event handling

## Reactor Architecture Improvements (Step 15)

Improved Reactor internals and lifecycle handling:

- Add explicit Channel registration state to distinguish new, added, and deleted channels
- Use `EPOLL_CTL_ADD` for new channels and `EPOLL_CTL_MOD` for event changes
- Make `TcpServer` lifecycle and service startup semantics clearer
- Ensure the Reactor keeps channel state consistent during accept, read, write, and close

## Thread Pool and Concurrency

Added:

- Worker threads
- Task queue
- Thread synchronization
- ThreadPool abstraction
- Submit tasks from TcpConnection to worker threads

## Write Buffer

Added write buffer support:

- Store pending response data inside TcpConnection
- Separate data processing from socket writing
- Prepare for EPOLLOUT based asynchronous writing

# Future Plans

I plan to continue improving this project:

- Add simple benchmark tests
- Measure latency and throughput
- Use Linux perf to understand performance
- Improve logging

# What I Learned

Through this project, I practiced:

- Linux socket programming
- TCP connection handling
- epoll-based event handling
- Reactor pattern
- Thread synchronization
- C++ programming
- Network I/O buffering
