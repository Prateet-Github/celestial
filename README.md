# Celestial

A lightweight, high-performance, asynchronous HTTP server engine written from scratch in low-level C.

Celestial bypasses heavyweight application frameworks to interface directly with the operating system kernel. Utilizing **OS-level event demultiplexing** (`kqueue` on macOS/BSD) and **non-blocking I/O** via the **Reactor Pattern**, Celestial achieves massive concurrency with minimal memory overhead and zero CPU idle waste.

---

## Key Features

- **Reactor Pattern Architecture:** Built on non-blocking file descriptors (`O_NONBLOCK`) dispatched through a central event loop.
- **Zero-Copy Kernel Notification:** Uses OS-native demultiplexers (`kqueue` / `kevent`) to process socket events with 0% CPU overhead while idle.
- **Bare-Metal Footprint:** Tiny memory baseline (~1 MB) compared to heavy runtimes like Node.js or JVM.
- **Modular C Design:** Clean interface separation across header (`.h`) and implementation (`.c`) files.
- **Zero Dependencies:** Pure C standard library and POSIX/BSD system calls—no external packages or third-party engines.

---

## Architecture

Instead of allocating a thread per connection (which scales poorly), Celestial uses a **Single-Threaded Non-Blocking Reactor Loop**—the exact same architectural pattern powering **Nginx**, **Redis**, and **Node.js** (`libuv`):

```text
+-------------------------------------------------------------------+
|                        CELESTIAL REACTOR                          |
|                                                                   |
|   1. Non-Blocking Sockets   --->  fcntl(fd, O_NONBLOCK)          |
|   2. Event Demultiplexer    --->  kqueue() / kevent()             |
|   3. Event Loop             --->  while (1)                       |
|   4. Handlers               --->  handle_accept() / handle_read() |
+-------------------------------------------------------------------+
                                  |
                                  v
                   +-----------------------------+
                   |          OS KERNEL          |
                   |   Monitors active sockets   |
                   +-----------------------------+