# Step 18.6 Allocation Analysis

## Objective

Analyze memory allocation behavior
inside the request processing path.

---

# 18.6.1 Measure Allocation Behavior

## Runtime Metrics

Example workload:

Clients: 100
Requests/client: 200

Results:

Request Buffers: 20000
Response Buffers: 20000
Write Buffer Appends: 20000


## Observation

Each request triggers multiple buffer operations.

---

# 18.6.2 Identify Allocation Hotspots

## 1. Request Message Construction

Location:
TcpConnection::handleRead()


```cpp
std::string message(buffer, n);
````

Impact:

Creates a new string buffer for each request.

---

## 2. Task Queue Submission

```cpp
[self, message]
```

Impact:
Message data is copied into task closure.

---

## 3. Response Construction

```cpp
std::string response =
    "processed: " + message;
```

Impact:
Creates another temporary string.

---

## 4. Write Buffer Growth

```cpp
writeBuffer_ += msg;
```

Impact:
May trigger buffer reallocation.

# 18.6.3 Optimize Buffer Reuse

## Optimization 1: Reserve Write Buffer Capacity

Before:

writeBuffer_ dynamically grows during append operations.

This may trigger memory reallocation and data copying.


Change:

```cpp
writeBuffer_.reserve(8192);
```

Impact:

Pre-allocate buffer capacity to reduce
runtime reallocations during response handling.

## Optimization 2: Reduce String Copies

Before:

[self, message]

and:

[self, msg]

After:

[self, message = std::move(message)]

and:

[self, msg = std::move(msg)]

Impact:

Reduce unnecessary string data copies
during asynchronous request processing.
