# ot-joinserver

`ot-joinserver` is a C++20 TCP server built on Boost.Asio coroutines, paired with a small in-memory data layer (tables + a table registry) intended as the storage backend for the server.

Currently the server (`joinserver`) runs a line-oriented echo protocol: it accepts newline-terminated strings over TCP and writes each one back to the same connection.

## Features

- Coroutine-based async TCP server (`TcpServer`) — one coroutine per client connection, non-blocking accept loop
- Newline-delimited string protocol: read a line, respond with a line
- Handles multiple concurrent clients
- In-memory `Table` storage: thread-safe (reader/writer lock) insert/remove/get/list by integer id
- `DbEngine`: create/drop/lookup named tables, list all tables

## Requirements

- CMake 3.20+
- C++20 compiler (GCC/Clang/MSVC)
- Boost (headers; `Boost::boost`)
- POSIX threads
- Optional: GoogleTest (for unit tests)

## Build

```bash
cmake -S . -B build
cmake --build build
```

The server executable will be available at:

```bash
build/bin/joinserver
```

### Build with tests enabled

```bash
cmake -S . -B build -DWITH_GOOGLE_TEST=ON
cmake --build build
```

Note: if `WITH_GOOGLE_TEST` is `OFF` in your existing CMake cache, tests are not added.

## Quick Start

### Run the server

```bash
./build/bin/joinserver [port]
```

`port` is optional and defaults to `9000`.

### Talk to it with `nc`

```bash
nc 127.0.0.1 9000
```

Type a line and press Enter — the server echoes it back on the same connection:

```text
hello
hello
foo bar
foo bar
```

Non-interactive one-shot test:

```bash
printf 'hello world\nsecond line\n' | nc 127.0.0.1 9000
```

Multiple clients can connect at once; each connection is handled independently.

## Architecture

Project layout:

- `src/app/join-server`: executable entry point (`main.cpp`) — wires `TcpServer` to a per-connection echo handler
- `src/lib/network/server`: `TcpServer` — Boost.Asio coroutine-based TCP acceptor and session dispatcher
- `src/lib/data/storage`: `Table`/`Record` — thread-safe in-memory key/value table (`int` id → name)
- `src/lib/data/db-engine`: `DbEngine` — owns and manages a set of named `Table`s

Key components:

- `TcpServer`
	- Accepts connections on a configured port
	- Spawns a caller-supplied coroutine (`SessionHandler`) per accepted socket
	- Runs on a caller-owned `boost::asio::io_context`

- `Table`
	- Stores records keyed by integer id under a `std::shared_mutex`
	- `insert` / `remove` / `truncate` / `get` / `getAll` / `size`
	- Exposes raw data and its mutex for callers that need custom locking (e.g. cross-table joins)

- `DbEngine`
	- Creates, drops, and looks up `Table`s by name
	- Guards its table registry with its own `std::shared_mutex`
	- `listTables()` returns all registered table names

## Testing

When built with `-DWITH_GOOGLE_TEST=ON`:

```bash
ctest --test-dir build --output-on-failure
```

## Packaging

The project includes CPack settings for generating a Debian package:

```bash
cpack --config build/CPackConfig.cmake
```

## Notes

- The echo protocol is line-based: each message must end with `\n`.
- The server runs single-threaded (`io_context::run()` on the main thread); concurrency comes from coroutines, not OS threads.
